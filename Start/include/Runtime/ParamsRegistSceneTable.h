#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/ParamsText.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 现代值夹具；导入只消费 cp932，不在原生表旁保留值表或重建 utf16。
struct ParamsRegistSceneItem final
{
    bool operator==(const ParamsRegistSceneItem&) const = default;
    ParamsText name;
    std::vector<ParamsText> scenes;
};

struct ParamsRegistSceneGroup final
{
    bool operator==(const ParamsRegistSceneGroup&) const = default;
    ParamsText name;
    std::vector<ParamsRegistSceneItem> items;
};

// 40CAC0 先清三指针，再构造 name；与 CG 的 name@0 和三个 DWORD 不同。
// 正文由 control 分阶段销毁，不得另跑聚合类型的普通析构。
struct ParamsRegistSceneItemRuntime final
{
    EnvironmentVectorStorage<RuntimeCString> scenesAt00;
    RuntimeCString nameAt0C;
};
static_assert(sizeof(ParamsRegistSceneItemRuntime) == 0x10);
static_assert(offsetof(ParamsRegistSceneItemRuntime, scenesAt00) == 0x00);
static_assert(offsetof(ParamsRegistSceneItemRuntime, nameAt0C) == 0x0C);

class ParamsRegistSceneItemTable final
{
    static ParamsRegistSceneItemRuntime& Resolve(const YtlReferenceSlotRuntime& reference);

public:
    ParamsRegistSceneItemTable() noexcept = default;
    ~ParamsRegistSceneItemTable() noexcept;

    // 显式 profile 深快照；生产 Ref 复制只能使用 AppendReference。
    ParamsRegistSceneItemTable(const ParamsRegistSceneItemTable& other);
    ParamsRegistSceneItemTable& operator=(const ParamsRegistSceneItemTable& other);
    ParamsRegistSceneItemTable(ParamsRegistSceneItemTable&& other) noexcept;
    ParamsRegistSceneItemTable& operator=(ParamsRegistSceneItemTable&& other) noexcept;
    void push_back(const ParamsRegistSceneItem& fixture);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    // 不校正索引/count；最终 null 解址显式抛 logic_error，与其他原生表一致。
    [[nodiscard]] ParamsRegistSceneItemRuntime& At(std::uint32_t index);
    [[nodiscard]] const ParamsRegistSceneItemRuntime& At(std::uint32_t index) const;
    [[nodiscard]] ParamsRegistSceneItemRuntime& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsRegistSceneItemRuntime& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsRegistSceneItemTable& other) const;

    void clear() noexcept;
    // 40B220：额外局部 Ref 持有；正常释放缓存 control，展开释放当前 slot。
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
        using value_type = ParamsRegistSceneItemRuntime;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Constant, const value_type&, value_type&>;
        using pointer = std::conditional_t<Constant, const value_type*, value_type*>;

        Iterator() noexcept = default;
        explicit Iterator(SlotPointer slot) noexcept : slot_(slot) {}
        template<bool Other> requires (Constant && !Other)
        Iterator(const Iterator<Other>& other) noexcept : slot_(other.slot_) {}
        [[nodiscard]] reference operator*() const { return ParamsRegistSceneItemTable::Resolve(*slot_); }
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
static_assert(sizeof(ParamsRegistSceneItemTable) == 12);
static_assert(offsetof(ParamsRegistSceneItemTable, storage) == 0);

// 40C990 按 name、空 items 顺序构造；43CD00 按 items、name 顺序释放。
struct ParamsRegistSceneGroupRuntime final
{
    RuntimeCString nameAt00;
    ParamsRegistSceneItemTable itemsAt04;
};
static_assert(sizeof(ParamsRegistSceneGroupRuntime) == 0x10);
static_assert(offsetof(ParamsRegistSceneGroupRuntime, nameAt00) == 0x00);
static_assert(offsetof(ParamsRegistSceneGroupRuntime, itemsAt04) == 0x04);

// 只接收新输出槽。独立正文 ctor 整体 noexcept；body/control 分配可抛。
// 407930/4079A0 先发布 out.control 再写 refs，随后逐次重读 control 写其余字段。
// 工厂失败不补正文回滚；注入分配必须兼容普通 delete。reader 成功后重新 Resolve。
void CreateParamsRegistSceneGroupAt41DC40(YtlReferenceSlotRuntime& outRef,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void CreateParamsRegistSceneItemAt41DCB0(YtlReferenceSlotRuntime& outRef,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// base 为空返回 nullptr；否则按当前 base+bias*stride 解址，无 control 时 stride=0。
[[nodiscard]] ParamsRegistSceneGroupRuntime* ResolveParamsRegistSceneGroupAt4105C0(
    const YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] ParamsRegistSceneItemRuntime* ResolveParamsRegistSceneItemAt4105F0(
    const YtlReferenceSlotRuntime& reference) noexcept;

// 两个原入口 EHFlags=5：正常/展开释放均终止边界；payload 后重读 slot.control。
void ReleaseParamsRegistSceneGroupAt40DA60(YtlReferenceSlotRuntime& reference) noexcept;
void ReleaseParamsRegistSceneItemAt40DBB0(YtlReferenceSlotRuntime& reference) noexcept;

class ParamsRegistSceneTable final
{
    static ParamsRegistSceneGroupRuntime& Resolve(const YtlReferenceSlotRuntime& reference);

public:
    ParamsRegistSceneTable() noexcept = default;
    ~ParamsRegistSceneTable() noexcept;

    // 两层正文及 CString 深快照，不共享 group/item control，不保存生产镜像。
    ParamsRegistSceneTable(const ParamsRegistSceneTable& other);
    ParamsRegistSceneTable& operator=(const ParamsRegistSceneTable& other);
    ParamsRegistSceneTable(ParamsRegistSceneTable&& other) noexcept;
    ParamsRegistSceneTable& operator=(ParamsRegistSceneTable&& other) noexcept;
    void push_back(const ParamsRegistSceneGroup& fixture);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    // 不校正索引/count；最终 null 解址显式抛 logic_error。
    [[nodiscard]] ParamsRegistSceneGroupRuntime& At(std::uint32_t index);
    [[nodiscard]] const ParamsRegistSceneGroupRuntime& At(std::uint32_t index) const;
    [[nodiscard]] ParamsRegistSceneGroupRuntime& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsRegistSceneGroupRuntime& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsRegistSceneTable& other) const;

    void clear() noexcept;
    // 40B150：不是裸 vector append，额外局部持有及两条释放路径均保留。
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
        using value_type = ParamsRegistSceneGroupRuntime;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Constant, const value_type&, value_type&>;
        using pointer = std::conditional_t<Constant, const value_type*, value_type*>;

        Iterator() noexcept = default;
        explicit Iterator(SlotPointer slot) noexcept : slot_(slot) {}
        template<bool Other> requires (Constant && !Other)
        Iterator(const Iterator<Other>& other) noexcept : slot_(other.slot_) {}
        [[nodiscard]] reference operator*() const { return ParamsRegistSceneTable::Resolve(*slot_); }
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
static_assert(sizeof(ParamsRegistSceneTable) == 12);
static_assert(offsetof(ParamsRegistSceneTable, storage) == 0);
}
