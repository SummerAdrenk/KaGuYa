#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/ParamsText.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 现代值夹具；导入 native 表只消费 cp932，不保存或重建 utf16。
struct ParamsThumbnail final
{
    bool operator==(const ParamsThumbnail&) const = default;
    std::array<ParamsText, 8> strings;
    std::array<std::uint32_t, 3> integers{};
};

// 工厂逐成员管理的正文：不得值初始化整块，也不得运行聚合类型的普通析构。
struct ParamsThumbnailRuntime final
{
    RuntimeCString nameAt00;
    std::array<const char*, 7> aliasesAt04;
    // 41D411：三个typed integer原值；业务用途未证，不按恒值猜坐标或索引。
    std::array<std::uint32_t, 3> integersAt20;
    EnvironmentVectorStorage<RuntimeCString> ownedStringsAt2C;
};
static_assert(sizeof(ParamsThumbnailRuntime) == 0x38);
static_assert(offsetof(ParamsThumbnailRuntime, nameAt00) == 0x00);
static_assert(offsetof(ParamsThumbnailRuntime, aliasesAt04) == 0x04);
static_assert(offsetof(ParamsThumbnailRuntime, integersAt20) == 0x20);
static_assert(offsetof(ParamsThumbnailRuntime, ownedStringsAt2C) == 0x2C);

// 仅写新输出槽，不是替换 API；41D52F 前失败没有正文/字符串/Ref 清理资格。
// 注入分配必须兼容普通 operator delete；control 不保存分配器。
void CreateParamsThumbnailAt41D463(YtlReferenceSlotRuntime& outRef,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 可抛；reader 自己提供 40DAD0 展开和 state 21h 正常释放的终止边界。
void ReleaseParamsThumbnailAt43B9D0(YtlReferenceSlotRuntime& reference);

// 借用原生索引 index=0..6。返回 reader 的 ESI chars：空串/快路径保留入口缓存，
// 仅在 vector 增长成功后重读 source。异常时不提前发布 aliases[index]。
const char* SetParamsThumbnailAliasAt41D590(ParamsThumbnailRuntime& body,
    std::uint32_t index, const RuntimeCString& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

class ParamsThumbnailTable final
{
    static ParamsThumbnailRuntime& Resolve(const YtlReferenceSlotRuntime& reference);

public:
    ParamsThumbnailTable() noexcept = default;
    ~ParamsThumbnailTable() noexcept;
    // 显式 profile 深快照便利口，不是原生 Ref 复制语义，不保存第二份值表。
    ParamsThumbnailTable(const ParamsThumbnailTable& other);
    ParamsThumbnailTable& operator=(const ParamsThumbnailTable& other);
    ParamsThumbnailTable(ParamsThumbnailTable&& other) noexcept;
    ParamsThumbnailTable& operator=(ParamsThumbnailTable&& other) noexcept;
    void push_back(const ParamsThumbnail& fixture);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    [[nodiscard]] ParamsThumbnailRuntime& At(std::uint32_t index);
    [[nodiscard]] const ParamsThumbnailRuntime& At(std::uint32_t index) const;
    [[nodiscard]] ParamsThumbnailRuntime& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsThumbnailRuntime& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsThumbnailTable& other) const;

    void clear() noexcept;
    void AppendReference(const YtlReferenceSlotRuntime& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

    template<bool Constant>
    class Iterator final
    {
        using SlotPointer = std::conditional_t<Constant,
            const YtlReferenceSlotRuntime*, YtlReferenceSlotRuntime*>;
        template<bool> friend class Iterator;
        SlotPointer slot_ = nullptr;
    public:
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using value_type = ParamsThumbnailRuntime;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Constant, const value_type&, value_type&>;
        using pointer = std::conditional_t<Constant, const value_type*, value_type*>;

        Iterator() noexcept = default;
        explicit Iterator(SlotPointer slot) noexcept : slot_(slot) {}
        template<bool Other> requires (Constant && !Other)
        Iterator(const Iterator<Other>& other) noexcept : slot_(other.slot_) {}
        [[nodiscard]] reference operator*() const { return ParamsThumbnailTable::Resolve(*slot_); }
        [[nodiscard]] pointer operator->() const { return &**this; }
        Iterator& operator++() noexcept { ++slot_; return *this; }
        Iterator operator++(int) noexcept { auto previous = *this; ++*this; return previous; }
        bool operator==(const Iterator&) const = default;
    };
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    [[nodiscard]] iterator begin() noexcept { return iterator(storage.begin); }
    [[nodiscard]] iterator end() noexcept { return iterator(storage.end); }
    [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(storage.begin); }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(storage.end); }
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return end(); }

    EnvironmentVectorStorage<YtlReferenceSlotRuntime> storage{nullptr, nullptr, nullptr};
};
static_assert(sizeof(ParamsThumbnailTable) == 12);
static_assert(offsetof(ParamsThumbnailTable, storage) == 0);
}
