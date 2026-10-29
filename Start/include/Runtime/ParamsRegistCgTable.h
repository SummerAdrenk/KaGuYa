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
// 现代值夹具；导入原生表只消费 cp932，不保存第二份值表或重建 utf16。
struct ParamsRegistCgItem final
{
    bool operator==(const ParamsRegistCgItem&) const = default;
    ParamsText name;
    std::uint32_t x = 0;
    std::uint32_t y = 0;
    std::uint32_t value = 0;
};

struct ParamsRegistCgGroup final
{
    bool operator==(const ParamsRegistCgGroup&) const = default;
    ParamsText name;
    std::vector<ParamsRegistCgItem> items;
};

// 工厂只构造 name；三个 DWORD 在 reader 写入前保持未初始化。
// 正文由 control 显式销毁，不得另跑聚合类型的普通析构。
struct ParamsRegistCgItemRuntime final
{
    RuntimeCString nameAt00;
    std::uint32_t xAt04;
    std::uint32_t yAt08;
    std::uint32_t valueAt0C;
};
static_assert(sizeof(ParamsRegistCgItemRuntime) == 0x10);
static_assert(offsetof(ParamsRegistCgItemRuntime, nameAt00) == 0x00);
static_assert(offsetof(ParamsRegistCgItemRuntime, xAt04) == 0x04);
static_assert(offsetof(ParamsRegistCgItemRuntime, yAt08) == 0x08);
static_assert(offsetof(ParamsRegistCgItemRuntime, valueAt0C) == 0x0C);

class ParamsRegistCgItemTable final
{
    static ParamsRegistCgItemRuntime& Resolve(const YtlReferenceSlotRuntime& reference);

public:
    ParamsRegistCgItemTable() noexcept = default;
    ~ParamsRegistCgItemTable() noexcept;

    // 显式 profile 深快照；原生 Ref 复制只能使用 AppendReference。
    ParamsRegistCgItemTable(const ParamsRegistCgItemTable& other);
    ParamsRegistCgItemTable& operator=(const ParamsRegistCgItemTable& other);
    ParamsRegistCgItemTable(ParamsRegistCgItemTable&& other) noexcept;
    ParamsRegistCgItemTable& operator=(ParamsRegistCgItemTable&& other) noexcept;
    void push_back(const ParamsRegistCgItem& fixture);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    // 不按 count 校正索引；最终 null 解址与 thumbnail 表一样显式抛 logic_error。
    [[nodiscard]] ParamsRegistCgItemRuntime& At(std::uint32_t index);
    [[nodiscard]] const ParamsRegistCgItemRuntime& At(std::uint32_t index) const;
    [[nodiscard]] ParamsRegistCgItemRuntime& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsRegistCgItemRuntime& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsRegistCgItemTable& other) const;

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
        using value_type = ParamsRegistCgItemRuntime;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Constant, const value_type&, value_type&>;
        using pointer = std::conditional_t<Constant, const value_type*, value_type*>;

        Iterator() noexcept = default;
        explicit Iterator(SlotPointer slot) noexcept : slot_(slot) {}
        template<bool Other> requires (Constant && !Other)
        Iterator(const Iterator<Other>& other) noexcept : slot_(other.slot_) {}
        [[nodiscard]] reference operator*() const { return ParamsRegistCgItemTable::Resolve(*slot_); }
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
static_assert(sizeof(ParamsRegistCgItemTable) == 12);
static_assert(offsetof(ParamsRegistCgItemTable, storage) == 0);

// group 正文按 name、空 items 的顺序构造；control 删除时先 items、后 name。
struct ParamsRegistCgGroupRuntime final
{
    RuntimeCString nameAt00;
    ParamsRegistCgItemTable itemsAt04;
};
static_assert(sizeof(ParamsRegistCgGroupRuntime) == 0x10);
static_assert(offsetof(ParamsRegistCgGroupRuntime, nameAt00) == 0x00);
static_assert(offsetof(ParamsRegistCgGroupRuntime, itemsAt04) == 0x04);

// 只接收新输出槽，不是替换 API。body/GetNil 失败时输出槽尚未初始化；
// control 分配失败保留已发布 base，不补正文/name 回滚。注入分配须兼容普通 delete。
// 返回跨control分配保持的正文寄存器；outRef.base可能已被分配回调改写。
ParamsRegistCgGroupRuntime* CreateParamsRegistCgGroupAt41D8A6(YtlReferenceSlotRuntime& outRef,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
ParamsRegistCgItemRuntime* CreateParamsRegistCgItemAt41D9C2(YtlReferenceSlotRuntime& outRef,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 两个原始 release 均可抛，payload 回调后重读槽 control；无 control 不清 base/bias。
// reader 自己提供 40D9F0/40DB40 展开及 state 28h 正常 item 释放的终止边界。
void ReleaseParamsRegistCgGroupAt43B950(YtlReferenceSlotRuntime& reference);
void ReleaseParamsRegistCgItemAt43BA10(YtlReferenceSlotRuntime& reference);

class ParamsRegistCgTable final
{
    static ParamsRegistCgGroupRuntime& Resolve(const YtlReferenceSlotRuntime& reference);

public:
    ParamsRegistCgTable() noexcept = default;
    ~ParamsRegistCgTable() noexcept;

    // 深复制两层正文及 CString；不共享 group/item control，也不保存现代影子表。
    ParamsRegistCgTable(const ParamsRegistCgTable& other);
    ParamsRegistCgTable& operator=(const ParamsRegistCgTable& other);
    ParamsRegistCgTable(ParamsRegistCgTable&& other) noexcept;
    ParamsRegistCgTable& operator=(ParamsRegistCgTable&& other) noexcept;
    void push_back(const ParamsRegistCgGroup& fixture);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept { return storage.begin == storage.end; }
    // 按 Ref 的 base+bias*stride 借用正文，空 control 的 stride 为 0。
    // 不检查索引范围；最终 null 解址显式抛 logic_error。
    [[nodiscard]] ParamsRegistCgGroupRuntime& At(std::uint32_t index);
    [[nodiscard]] const ParamsRegistCgGroupRuntime& At(std::uint32_t index) const;
    [[nodiscard]] ParamsRegistCgGroupRuntime& operator[](std::size_t index)
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] const ParamsRegistCgGroupRuntime& operator[](std::size_t index) const
        { return At(static_cast<std::uint32_t>(index)); }
    [[nodiscard]] bool operator==(const ParamsRegistCgTable& other) const;

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
        using value_type = ParamsRegistCgGroupRuntime;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional_t<Constant, const value_type&, value_type&>;
        using pointer = std::conditional_t<Constant, const value_type*, value_type*>;

        Iterator() noexcept = default;
        explicit Iterator(SlotPointer slot) noexcept : slot_(slot) {}
        template<bool Other> requires (Constant && !Other)
        Iterator(const Iterator<Other>& other) noexcept : slot_(other.slot_) {}
        [[nodiscard]] reference operator*() const { return ParamsRegistCgTable::Resolve(*slot_); }
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
static_assert(sizeof(ParamsRegistCgTable) == 12);
static_assert(offsetof(ParamsRegistCgTable, storage) == 0);
}
