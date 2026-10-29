#include <Runtime/ParamsRegistCgTable.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Storage = EnvironmentVectorStorage<Slot>;

class RetiredRegistCgControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredRegistCgControl() noexcept : EnvironmentObjectReferenceControl(nullptr) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 原 5ADD18 -> 412460：退化后再调用删除槽，尺寸为 14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredRegistCgControl) == 0x18);

void RetireRegistCgControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 411950/411A40：flags=0 也退化虚表；+04..17（含未写填充）全部保持。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredRegistCgControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class RegistCgGroupControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RegistCgGroupControl(ParamsRegistCgGroupRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsRegistCgGroupRuntime*>(payloadAt14);
        if (!body) return;
        // 5AE468 -> 43CCC0：items 析构 40E3E0 整体 noexcept，随后 name Free 可外传。
        // 不运行 GroupRuntime 隐式析构，避免再销毁一次 items/name。
        DestroyEnvironmentArchiveReferencesAt497080(body->itemsAt04.storage);
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x10});
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireRegistCgControl(this, flags);
    }
};
static_assert(sizeof(RegistCgGroupControl) == 0x18);

class RegistCgItemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RegistCgItemControl(ParamsRegistCgItemRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsRegistCgItemRuntime*>(payloadAt14);
        if (!body) return;
        // 5AE480 -> 43CE00：只释放首 CString，Free 抛出时不补删正文/control。
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x10});
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireRegistCgControl(this, flags);
    }
};
static_assert(sizeof(RegistCgItemControl) == 0x18);

// 仅限显式 fixture/profile 深快照，不代表 41ADE0 的局部 EH 资格。
// 原生工厂内部仍不回滚；生产 reader 必须在 25h/26h 等原状态点建立清理资格。
struct SnapshotReference final
{
    Slot slot;
    ~SnapshotReference() noexcept { ReleaseYtlReference(slot); }
};

void SnapshotString(RuntimeCString& destination, const char* chars, std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params registCg snapshot exceeds CString length");
    SetRuntimeCStringAt437680(destination, chars, static_cast<int>(length));
}

std::size_t TableSize(const Storage& storage) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.end) -
        reinterpret_cast<std::uintptr_t>(storage.begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

const Slot& ReferenceAt(const Storage& storage, std::uint32_t index) noexcept
{
    // 原 DWORD 索引回绕后取 Ref；不添加 vector/count 范围校正。
    const auto address = reinterpret_cast<std::uintptr_t>(storage.begin) + index * 12u;
    return *reinterpret_cast<const Slot*>(address);
}
}

ParamsRegistCgGroupRuntime* CreateParamsRegistCgGroupAt41D8A6(Slot& outRef, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* body = static_cast<ParamsRegistCgGroupRuntime*>(memory.AllocateBody(0x10));
    if (body)
    {
        InitializeEmptyRuntimeCString(body->nameAt00, strings);
        // 41D8BD..41D8CB 在 GetNil 之后；只开始 items 表的生命周期，不构造整个 group。
        ::new (static_cast<void*>(&body->itemsAt04)) ParamsRegistCgItemTable;
    }
    // 41D8D8..41D8F1：此前失败不改输出槽，也没有正文/name 的清理资格。
    outRef = {};
    if (!body) return nullptr;
    outRef.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) RegistCgGroupControl(body) : nullptr;
    // 41D92D 不检查空 control；不得将它变成成功的空 Ref 或补正文回滚。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    outRef.controlAt08 = control;
    control->countAt10 = 1;
    return body; // 41D934：ESI原正文，不重读var_90。
}

ParamsRegistCgItemRuntime* CreateParamsRegistCgItemAt41D9C2(Slot& outRef, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* body = static_cast<ParamsRegistCgItemRuntime*>(memory.AllocateBody(0x10));
    if (body) InitializeEmptyRuntimeCString(body->nameAt00, strings);
    // 41D9DF..41D9F3：不写 x/y/value；GetNil 失败时也尚未触碰输出槽。
    outRef = {};
    if (!body) return nullptr;
    outRef.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) RegistCgItemControl(body) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    outRef.controlAt08 = control;
    control->countAt10 = 1;
    return body; // EBX跨41DA0B分配保持，追加才读取var_9C。
}

void ReleaseParamsRegistCgGroupAt43B950(Slot& reference)
{
    ReleaseYtlReference(reference);
}

void ReleaseParamsRegistCgItemAt43BA10(Slot& reference)
{
    ReleaseYtlReference(reference);
}

ParamsRegistCgItemTable::~ParamsRegistCgItemTable() noexcept
{
    // 40E3E0 / 5DFCB4 flags=5，与 497080 同序；删除存储后才清三指针。
    DestroyEnvironmentArchiveReferencesAt497080(storage);
}

ParamsRegistCgItemTable::ParamsRegistCgItemTable(ParamsRegistCgItemTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsRegistCgItemTable& ParamsRegistCgItemTable::operator=(ParamsRegistCgItemTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsRegistCgItemTable::ParamsRegistCgItemTable(const ParamsRegistCgItemTable& other)
{
    ParamsRegistCgItemTable snapshot;
    for (const auto& source : other)
    {
        SnapshotReference fresh;
        CreateParamsRegistCgItemAt41D9C2(fresh.slot);
        auto& body = Resolve(fresh.slot);
        SnapshotString(body.nameAt00, source.nameAt00.GetString(), source.nameAt00.GetLength());
        body.xAt04 = source.xAt04;
        body.yAt08 = source.yAt08;
        body.valueAt0C = source.valueAt0C;
        snapshot.AppendReference(fresh.slot);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsRegistCgItemTable& ParamsRegistCgItemTable::operator=(const ParamsRegistCgItemTable& other)
{
    if (this != &other)
    {
        ParamsRegistCgItemTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

void ParamsRegistCgItemTable::push_back(const ParamsRegistCgItem& fixture)
{
    SnapshotReference fresh;
    CreateParamsRegistCgItemAt41D9C2(fresh.slot);
    auto& body = Resolve(fresh.slot);
    const auto& name = fixture.name.cp932;
    SnapshotString(body.nameAt00, name.data(), name.size());
    body.xAt04 = fixture.x;
    body.yAt08 = fixture.y;
    body.valueAt0C = fixture.value;
    AppendReference(fresh.slot);
}

std::size_t ParamsRegistCgItemTable::size() const noexcept { return TableSize(storage); }

ParamsRegistCgItemRuntime& ParamsRegistCgItemTable::Resolve(const Slot& reference)
{
    auto* body = ResolveEnvironmentObjectReference<ParamsRegistCgItemRuntime>(reference);
    if (!body) throw std::logic_error("Params registCg item reference resolves to null");
    return *body;
}

ParamsRegistCgItemRuntime& ParamsRegistCgItemTable::At(std::uint32_t index)
{
    return const_cast<ParamsRegistCgItemRuntime&>(std::as_const(*this).At(index));
}

const ParamsRegistCgItemRuntime& ParamsRegistCgItemTable::At(std::uint32_t index) const
{
    return Resolve(ReferenceAt(storage, index));
}

void ParamsRegistCgItemTable::clear() noexcept
{
    // 4082C0 的活动范围释放异常终止；重读 begin 设置 end，保留容量。
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void ParamsRegistCgItemTable::AppendReference(const Slot& source, IEnvironmentRootMemory& memory)
{
    // 41DB16..41DB31 快路径/409EF0 增长：已核 4082C0、43AF00、43A5C0、43B650 及 EH。
    AppendEnvironmentArchiveReferenceAt49852D(storage, source, memory);
}

bool ParamsRegistCgItemTable::operator==(const ParamsRegistCgItemTable& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            left.xAt04 != right.xAt04 || left.yAt08 != right.yAt08 ||
            left.valueAt0C != right.valueAt0C) return false;
    }
    return true;
}

ParamsRegistCgTable::~ParamsRegistCgTable() noexcept
{
    // 40E1D0 / 5DFD28 flags=5；group payload 内部先释放 items，再释放 name。
    DestroyEnvironmentArchiveReferencesAt497080(storage);
}

ParamsRegistCgTable::ParamsRegistCgTable(ParamsRegistCgTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsRegistCgTable& ParamsRegistCgTable::operator=(ParamsRegistCgTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsRegistCgTable::ParamsRegistCgTable(const ParamsRegistCgTable& other)
{
    ParamsRegistCgTable snapshot;
    for (const auto& source : other)
    {
        SnapshotReference fresh;
        CreateParamsRegistCgGroupAt41D8A6(fresh.slot);
        auto& body = Resolve(fresh.slot);
        SnapshotString(body.nameAt00, source.nameAt00.GetString(), source.nameAt00.GetLength());
        body.itemsAt04 = source.itemsAt04;
        snapshot.AppendReference(fresh.slot);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsRegistCgTable& ParamsRegistCgTable::operator=(const ParamsRegistCgTable& other)
{
    if (this != &other)
    {
        ParamsRegistCgTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

void ParamsRegistCgTable::push_back(const ParamsRegistCgGroup& fixture)
{
    SnapshotReference fresh;
    CreateParamsRegistCgGroupAt41D8A6(fresh.slot);
    auto& body = Resolve(fresh.slot);
    const auto& name = fixture.name.cp932;
    SnapshotString(body.nameAt00, name.data(), name.size());
    for (const auto& item : fixture.items) body.itemsAt04.push_back(item);
    AppendReference(fresh.slot);
}

std::size_t ParamsRegistCgTable::size() const noexcept { return TableSize(storage); }

ParamsRegistCgGroupRuntime& ParamsRegistCgTable::Resolve(const Slot& reference)
{
    auto* body = ResolveEnvironmentObjectReference<ParamsRegistCgGroupRuntime>(reference);
    if (!body) throw std::logic_error("Params registCg group reference resolves to null");
    return *body;
}

ParamsRegistCgGroupRuntime& ParamsRegistCgTable::At(std::uint32_t index)
{
    return const_cast<ParamsRegistCgGroupRuntime&>(std::as_const(*this).At(index));
}

const ParamsRegistCgGroupRuntime& ParamsRegistCgTable::At(std::uint32_t index) const
{
    return Resolve(ReferenceAt(storage, index));
}

void ParamsRegistCgTable::clear() noexcept
{
    // 41D86A/408110：清活动 Ref，回调结束后 end=begin，不释放容量。
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void ParamsRegistCgTable::AppendReference(const Slot& source, IEnvironmentRootMemory& memory)
{
    // 43D390 快路径/4097A0 增长：已核 408110、43AD50、43A560、43B590 及 EH。
    AppendEnvironmentArchiveReferenceAt49852D(storage, source, memory);
}

bool ParamsRegistCgTable::operator==(const ParamsRegistCgTable& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            !(left.itemsAt04 == right.itemsAt04)) return false;
    }
    return true;
}
}
