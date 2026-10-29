#include <Runtime/ParamsThumbnailTable.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RuntimeCStringVector.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <mbstring.h>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Storage = EnvironmentVectorStorage<Slot>;

class RetiredThumbnailControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredThumbnailControl() noexcept : EnvironmentObjectReferenceControl(nullptr) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退化后的基类删除尺寸为 14h，不再是 18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredThumbnailControl) == 0x18);

class ThumbnailControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ThumbnailControl(ParamsThumbnailRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsThumbnailRuntime*>(payloadAt14);
        if (!body) return;
        // 43CD8D state 0 -> 541E55；43CD9E 在 name Free 前取消该终止边界。
        DestroyRuntimeCStringVectorAt40DD20(body->ownedStringsAt2C);
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x38});
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 4119EA：flags=0 也退化虚表，其余二十字节全部保持。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredThumbnailControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(ThumbnailControl) == 0x18);

// 仅用于现代 fixture/profile 深快照：Create 前即有析构资格，不是 41ADE0 EH。
// 生产 reader 禁用此守卫，必须按 41D52F/41D57F/41D6AA 分阶段管理临时对象。
struct SnapshotReference final
{
    Slot slot;
    ~SnapshotReference() noexcept { ReleaseParamsThumbnailAt43B9D0(slot); }
};

void SnapshotString(RuntimeCString& destination, const char* chars, std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params thumbnail snapshot exceeds CString length");
    SetRuntimeCStringAt437680(destination, chars, static_cast<int>(length));
}

void CopyBodySnapshot(ParamsThumbnailRuntime& destination, const ParamsThumbnailRuntime& source)
{
    SnapshotString(destination.nameAt00, source.nameAt00.GetString(), source.nameAt00.GetLength());
    destination.integersAt20 = source.integersAt20;
    for (auto* item = source.ownedStringsAt2C.begin; item != source.ownedStringsAt2C.end; ++item)
    {
        RuntimeCString copy(DefaultRuntimeStringManagerSource().CaptureAt417360());
        SnapshotString(copy, item->GetString(), item->GetLength());
        AppendRuntimeCStringAt41D5E9(destination.ownedStringsAt2C, copy);
    }
    for (std::uint32_t index = 0; index < 7; ++index)
    {
        const auto* alias = source.aliasesAt04[index];
        destination.aliasesAt04[index] = nullptr;
        if (!alias) continue;
        bool relocated = false;
        auto* next = destination.ownedStringsAt2C.begin;
        for (auto* item = source.ownedStringsAt2C.begin; item != source.ownedStringsAt2C.end; ++item, ++next)
        {
            const auto offset = reinterpret_cast<std::uintptr_t>(alias) -
                reinterpret_cast<std::uintptr_t>(item->GetString());
            if (offset <= static_cast<std::uint32_t>(item->GetLength()))
            {
                destination.aliasesAt04[index] = next->GetString() + offset;
                relocated = true;
                break;
            }
        }
        if (!relocated)
        {
            // 现代可变视图可能提供外部借用；快照也要独立持有这部分字符。
            RuntimeCString copy(DefaultRuntimeStringManagerSource().CaptureAt417360());
            SnapshotString(copy, alias, std::strlen(alias));
            AppendRuntimeCStringAt41D5E9(destination.ownedStringsAt2C, copy);
            destination.aliasesAt04[index] = (destination.ownedStringsAt2C.end - 1)->GetString();
        }
    }
}
}

void CreateParamsThumbnailAt41D463(Slot& outRef, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* body = static_cast<ParamsThumbnailRuntime*>(memory.AllocateBody(0x38));
    if (body)
    {
        InitializeEmptyRuntimeCString(body->nameAt00, strings);
        body->ownedStringsAt2C = {nullptr, nullptr, nullptr};
    }
    // 41D4B3..41D4C2 在 GetNil 之后；此前没有正文/name 的展开清理 state。
    outRef = {};
    if (!body) return;
    outRef.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) ThumbnailControl(body) : nullptr;
    // 41D503 没有 null 检查：空 control 应故障，不能当作成功的空 Ref。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x38;
    outRef.controlAt08 = control;
    control->countAt10 = 1;
}

void ReleaseParamsThumbnailAt43B9D0(Slot& reference)
{
    ReleaseYtlReference(reference);
}

const char* SetParamsThumbnailAliasAt41D590(ParamsThumbnailRuntime& body,
    std::uint32_t index, const RuntimeCString& source, IEnvironmentRootMemory& memory)
{
    const char* cached = source.GetString();
    if (_mbscmp(reinterpret_cast<const unsigned char*>(cached),
            reinterpret_cast<const unsigned char*>("")) == 0)
    {
        body.aliasesAt04[index] = nullptr;
        return cached;
    }
    const bool growth = body.ownedStringsAt2C.end == body.ownedStringsAt2C.capacity;
    AppendRuntimeCStringAt41D5E9(body.ownedStringsAt2C, source, memory);
    if (growth) cached = source.GetString(); // 仅 41D61B 重读临时 TEXT。
    body.aliasesAt04[index] = (body.ownedStringsAt2C.end - 1)->GetString();
    return cached;
}

ParamsThumbnailTable::~ParamsThumbnailTable() noexcept
{
    // 40E330 与 497080 指令同构，包含回调后重读字段的时机。
    DestroyEnvironmentArchiveReferencesAt497080(storage);
}

ParamsThumbnailTable::ParamsThumbnailTable(ParamsThumbnailTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsThumbnailTable& ParamsThumbnailTable::operator=(ParamsThumbnailTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(storage);
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

ParamsThumbnailTable::ParamsThumbnailTable(const ParamsThumbnailTable& other)
{
    ParamsThumbnailTable snapshot;
    for (const auto& source : other)
    {
        SnapshotReference fresh;
        CreateParamsThumbnailAt41D463(fresh.slot);
        CopyBodySnapshot(Resolve(fresh.slot), source);
        snapshot.AppendReference(fresh.slot);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsThumbnailTable& ParamsThumbnailTable::operator=(const ParamsThumbnailTable& other)
{
    if (this != &other)
    {
        ParamsThumbnailTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

void ParamsThumbnailTable::push_back(const ParamsThumbnail& fixture)
{
    SnapshotReference fresh;
    CreateParamsThumbnailAt41D463(fresh.slot);
    auto& body = Resolve(fresh.slot);
    const auto& name = fixture.strings[0].cp932;
    SnapshotString(body.nameAt00, name.data(), name.size());
    for (std::uint32_t index = 0; index < 7; ++index)
    {
        RuntimeCString temporary(DefaultRuntimeStringManagerSource().CaptureAt417360());
        const auto& bytes = fixture.strings[index + 1].cp932;
        SnapshotString(temporary, bytes.data(), bytes.size());
        SetParamsThumbnailAliasAt41D590(body, index, temporary);
    }
    body.integersAt20 = fixture.integers;
    AppendReference(fresh.slot);
}

std::size_t ParamsThumbnailTable::size() const noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.end) -
        reinterpret_cast<std::uintptr_t>(storage.begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

ParamsThumbnailRuntime& ParamsThumbnailTable::Resolve(const Slot& reference)
{
    auto* body = ResolveEnvironmentObjectReference<ParamsThumbnailRuntime>(reference);
    if (!body) throw std::logic_error("Params thumbnail reference resolves to null");
    return *body;
}

ParamsThumbnailRuntime& ParamsThumbnailTable::At(std::uint32_t index)
{
    return const_cast<ParamsThumbnailRuntime&>(std::as_const(*this).At(index));
}

const ParamsThumbnailRuntime& ParamsThumbnailTable::At(std::uint32_t index) const
{
    // 原生 DWORD 表索引，再按 Ref base+bias*stride 寻址；不增加 count 边界检查。
    const auto address = reinterpret_cast<std::uintptr_t>(storage.begin) + index * 12u;
    return Resolve(*reinterpret_cast<const Slot*>(address));
}

void ParamsThumbnailTable::clear() noexcept
{
    ClearEnvironmentArchiveReferencesAt498312(storage);
}

void ParamsThumbnailTable::AppendReference(const Slot& source, IEnvironmentRootMemory& memory)
{
    // 已核对 409C80/454310 及 408230/4523B0、43AE70/4902A0、43B610/491580 同构。
    AppendEnvironmentArchiveReferenceAt49852D(storage, source, memory);
}

bool ParamsThumbnailTable::operator==(const ParamsThumbnailTable& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            left.integersAt20 != right.integersAt20) return false;
        for (std::uint32_t alias = 0; alias < 7; ++alias)
        {
            const auto* a = left.aliasesAt04[alias];
            const auto* b = right.aliasesAt04[alias];
            if ((a == nullptr) != (b == nullptr) || (a && std::strcmp(a, b) != 0)) return false;
        }
    }
    return true;
}
}
