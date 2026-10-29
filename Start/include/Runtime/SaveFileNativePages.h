#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// UI只借用原Ref表，不物化值数组；每次size/end取当前三指针。
template<class T>
class SaveFileReferenceView final
{
public:
    explicit SaveFileReferenceView(const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& storage) noexcept
        : storage_(&storage) {}
    struct Iterator final
    {
        using value_type = std::remove_const_t<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::forward_iterator_tag;
        const YtlReferenceSlotRuntime* slot;
        T& operator*() const noexcept { return *ResolveEnvironmentObjectReference<T>(*slot); }
        T* operator->() const noexcept { return ResolveEnvironmentObjectReference<T>(*slot); }
        Iterator& operator++() noexcept { ++slot; return *this; }
        Iterator operator++(int) noexcept { const auto previous = *this; ++*this; return previous; }
        bool operator==(const Iterator&) const = default;
    };
    [[nodiscard]] std::size_t size() const noexcept
    {
        return (reinterpret_cast<std::uintptr_t>(storage_->end) -
            reinterpret_cast<std::uintptr_t>(storage_->begin)) / sizeof(YtlReferenceSlotRuntime);
    }
    [[nodiscard]] bool empty() const noexcept { return storage_->begin == storage_->end; }
    [[nodiscard]] T& operator[](std::size_t index) const noexcept
        { return *ResolveEnvironmentObjectReference<T>(storage_->begin[index]); }
    [[nodiscard]] T& at(std::size_t index) const
    {
        if (index >= size()) throw std::out_of_range("save file reference index");
        return (*this)[index];
    }
    [[nodiscard]] T& back() const noexcept { return (*this)[size() - 1]; }
    [[nodiscard]] Iterator begin() const noexcept { return {storage_->begin}; }
    [[nodiscard]] Iterator end() const noexcept { return {storage_->end}; }
private:
    const EnvironmentVectorStorage<YtlReferenceSlotRuntime>* storage_;
};
// 470A40/47155B/471B34：CTime正文是两个DWORD；不增加宿主的8-byte对齐要求。
struct SaveFileNativeTime final
{
    std::uint32_t lowAt00;
    std::uint32_t highAt04;
};

// 470977..4709A5：仅时间Ref为空和一次raw GetNil；没有独立available字段。
struct SaveFileNativeItem final
{
    YtlReferenceSlotRuntime timeAt00;
    union { RuntimeCString labelAt0C; };
    SaveFileNativeItem() noexcept {}
    ~SaveFileNativeItem() noexcept {}
    [[nodiscard]] bool Available() const noexcept { return timeAt00.baseAt00 != 0; }
    [[nodiscard]] std::uint64_t LastWriteTime() const noexcept
    {
        const auto* time = ResolveEnvironmentObjectReference<SaveFileNativeTime>(timeAt00);
        return (std::uint64_t{time->highAt04} << 32u) | time->lowAt00;
    }
};

// 4708D7/4713D0/4719AD：+00未初始化；普通页另于470952写0，quick/auto不写。
struct SaveFileNativePage final
{
    std::uint32_t availabilityAt00;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> itemsAt04;
    // 目录内存字段，不参与任何Serialize：稀疏页的显示下标不能充当文件页号。
    std::int32_t physicalPageNumber = 0;
    [[nodiscard]] SaveFileReferenceView<SaveFileNativeItem> Items() noexcept { return SaveFileReferenceView<SaveFileNativeItem>(itemsAt04); }
    [[nodiscard]] SaveFileReferenceView<const SaveFileNativeItem> Items() const noexcept { return SaveFileReferenceView<const SaveFileNativeItem>(itemsAt04); }
};
static_assert(sizeof(SaveFileNativeTime) == 8 && alignof(SaveFileNativeTime) == 4);
static_assert(sizeof(SaveFileNativeItem) == 0x10 && offsetof(SaveFileNativeItem, labelAt0C) == 0x0C);
static_assert(sizeof(SaveFileNativePage) == 0x14 && offsetof(SaveFileNativePage, itemsAt04) == 4);

[[nodiscard]] SaveFileNativePage* CreateSaveFilePageAt4708C7(
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] SaveFileNativeItem* CreateSaveFileItemAt470961(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 在分配之后读取source，保留原470A54读取临时CParams的顺序。
[[nodiscard]] SaveFileNativeTime* CreateSaveFileTimeAt470A40(
    const SaveFileNativeTime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// fresh清空后先发布base，再分配control；control失败不能补回收正文。
void AdoptSaveFilePageAt4708F0(YtlReferenceSlotRuntime& fresh, SaveFileNativePage* page,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AdoptSaveFileItemAt4709B5(YtlReferenceSlotRuntime& fresh, SaveFileNativeItem* item,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReplaceSaveFilePageAt451C60(YtlReferenceSlotRuntime& destination, SaveFileNativePage* page,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReplaceSaveFileTimeAt470A6E(YtlReferenceSlotRuntime& destination, SaveFileNativeTime* time,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

void DestroySaveFileItemAt4653A0(SaveFileNativeItem& item) noexcept;
// 删除业务只清时间Ref及字符串，不销毁条目；此调用链的异常允许外传。
void ClearSaveFileItemAt5024C0(SaveFileNativeItem& item);
void ClearSaveFilePageAt502510(SaveFileNativePage& page);
void UpdateSaveFilePageCountAt48EFE0(SaveFileNativePage& page) noexcept;
void DestroySaveFileItemsAt462090(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& items,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
void DestroySaveFilePagesAt4623A0(EnvironmentVectorStorage<YtlReferenceSlotRuntime>& pages,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

// fast分支用调用方缓存的control和bias=0；慢分支回调返回后才重读local.control。
[[nodiscard]] YtlReferenceControlRuntime* AppendSaveFileReferenceAt470D1A(
    EnvironmentVectorStorage<YtlReferenceSlotRuntime>& target,
    const YtlReferenceSlotRuntime& local, YtlReferenceControlRuntime* cachedControl,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void ReleaseSaveFileCachedReferenceAt470D50(YtlReferenceSlotRuntime& local,
    YtlReferenceControlRuntime* cachedControl) noexcept;

// 调用方已为按值参数增持一次；此函数消费该引用，不再额外AddRef。
// 48D19A的高DWORD是有符号比较；相等时低DWORD无符号比较，零/负时间不入选。
[[nodiscard]] std::int32_t FindLatestSaveFileItemAt48D090(YtlReferenceSlotRuntime ownedPage) noexcept;
}
