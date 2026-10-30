#include <Runtime/ParamsSettingTagRuntime.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsRuntimeProfile.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Pair = ParamsSettingPairRuntime;
using PairStorage = EnvironmentVectorStorage<Pair>;
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumPairs = 0x1FFFFFFFu;
static_assert(sizeof(void*) == 4);

std::uintptr_t Address(const void* value) noexcept
{
    return reinterpret_cast<std::uintptr_t>(value);
}

Pair* Advance(Pair* value, std::uint32_t count) noexcept
{
    return reinterpret_cast<Pair*>(Address(value) + count * 8u);
}

std::uint32_t PairCount(const Pair* begin, const Pair* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    // 40AA5A/40AA65：DWORD 差值 SAR 3，不用无符号除法替代负距离。
    return (bytes >> 3u) | ((bytes & 0x80000000u) ? 0xE0000000u : 0u);
}

ATL::CStringData* Header(const RuntimeCString& text) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()) - 16);
}

void Publish(RuntimeCString& text, ATL::CStringData* header) noexcept
{
    const auto chars = reinterpret_cast<const char*>(Address(header) + 16u);
    // 只发布Clone取得的原生指针，字段前缀清理仍由调用方控制。
    std::memcpy(static_cast<void*>(&text), &chars, sizeof(chars));
}

// 对应部分 pair 的 40CD40 展开动作；只在第二字段尚未完成时持有 first。
struct FirstFieldGuard final
{
    RuntimeCString* first;
    ~FirstFieldGuard() noexcept { if (first) DestroyRuntimeCStringAt40CD40(first); }
};

void CopyPair(Pair& destination, const Pair& source)
{
    Publish(destination.keyAt00, CloneRuntimeCStringDataAt414540(Header(source.keyAt00)));
    FirstFieldGuard partial{&destination.keyAt00};
    // 40AB45/43A87B：第一次 Clone 回调之后才读取 source.second。
    Publish(destination.valueAt04, CloneRuntimeCStringDataAt414540(Header(source.valueAt04)));
    partial.first = nullptr;
}

void ReleasePairRangeAt407CD0(Pair* begin, Pair* end)
{
    // 正序 pair，内部逆序字段；原范围函数没有 noexcept/自动重试。
    for (; begin != end; begin = Advance(begin, 1))
        ReleaseParamsSettingPairAt420324(*begin);
}

struct PrefixGuardAt40CDD0 final
{
    Pair* begin;
    Pair* end;
    ~PrefixGuardAt40CDD0() noexcept { ReleasePairRangeAt407CD0(begin, end); }
};

void CopyPairRangeAt43A820(Pair* begin, Pair* end, Pair* destination)
{
    PrefixGuardAt40CDD0 prefix{destination, destination};
    for (; begin != end; begin = Advance(begin, 1))
    {
        CopyPair(*prefix.end, *begin);
        prefix.end = Advance(prefix.end, 1);
    }
    // 43A8AE 的清理范围是 [end,end)，成功时不释放已经复制的元素。
    prefix.begin = prefix.end;
}

Pair* AllocatePairsAt40AAA2(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const std::uint32_t bytes = capacity > MaximumPairs ? 0xFFFFFFFFu : capacity * 8u;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Pair*>(memory.AllocateBody(bytes)) : nullptr;
    const std::uint32_t extended = bytes + 0x23u;
    void* const raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Pair*>(aligned);
}

void DeletePairStorageAt43B350(Pair* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(begin) - 4u), sizeof(raw));
        bytes += 0x23u;
        if (Address(begin) - Address(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}

void ReleaseCurrentPairs(PairStorage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    ReleasePairRangeAt407CD0(storage.begin, storage.end);
    // 40AC1A/40DCC5：元素回调之后重读 begin/capacity，不沿用已销毁范围的地址。
    auto* const begin = storage.begin;
    const std::uint32_t bytes = (Address(storage.capacity) - Address(begin)) & 0xFFFFFFF8u;
    DeletePairStorageAt43B350(begin, bytes, memory);
}

void GrowPairsAt40AA20(PairStorage& storage, Pair* position, const Pair& source,
    IEnvironmentRootMemory& memory)
{
    auto* const oldBegin = storage.begin;
    const auto index = PairCount(oldBegin, position);
    const auto oldSize = PairCount(oldBegin, storage.end);
    if (oldSize == MaximumPairs) throw std::length_error("vector<T> too long");
    const auto required = oldSize + 1u;
    const auto capacity = PairCount(oldBegin, storage.capacity);
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumPairs - half ? required : capacity + half;
    const auto nextCapacity = grown < required ? required : grown;
    auto* const next = AllocatePairsAt40AAA2(nextCapacity, memory);
    auto* const inserted = Advance(next, index);
    auto* const afterInserted = Advance(inserted, 1);
    auto* cleanupBegin = afterInserted;
    try
    {
        // 插入项先复制，可别名旧范围；第一字段成功而第二字段失败由 CopyPair 展开。
        CopyPair(*inserted, source);
        cleanupBegin = inserted;
        auto* const currentEnd = storage.end;
        if (position == currentEnd)
        {
            CopyPairRangeAt43A820(storage.begin, currentEnd, next);
        }
        else
        {
            // 分配/Clone 回调能改变 end；不能因入口尾插而删去原非尾插分支。
            CopyPairRangeAt43A820(storage.begin, position, next);
            cleanupBegin = next;
            CopyPairRangeAt43A820(position, storage.end, afterInserted);
        }
    }
    catch (...)
    {
        // 40AC75：外层 catch 清插入项/已完成前段；此范围释放可抛，不套前缀 guard。
        ReleasePairRangeAt407CD0(cleanupBegin, afterInserted);
        DeletePairStorageAt43B350(next, nextCapacity * 8u, memory);
        throw;
    }
    // 40AC05 已撤回新块回滚资格；旧 Free/delete 抛出时不得回收新块或提前发布。
    ReleaseCurrentPairs(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, required);
    storage.capacity = Advance(next, nextCapacity);
}

class RetiredSettingTagControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredSettingTagControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 的后续 slot04=412460，尺寸为 14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredSettingTagControl) == 0x18);

void RetireSettingTagControlAt411860(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredSettingTagControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    // flags=0 也退役虚表；本次派生 slot04 的删除尺寸仍为 18h。
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class SettingTagControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SettingTagControl(ParamsSettingTagRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<ParamsSettingTagRuntime*>(payloadAt14);
        if (!body) return;
        // 5AE394.slot00=43CBC0 无 EH：两 vector 各自终止，name Free 却可外传。
        // 不调用整体 flags5 的 40FAF0，更不能 delete 聚合后再跑一遍 CString 析构。
        DestroyEnvironmentArchiveReferencesAt497080(body->childrenAt10);
        DestroyParamsSettingPairsAt40DC90(body->pairsAt04);
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x1C});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireSettingTagControlAt411860(this, flags);
    }
};
static_assert(sizeof(SettingTagControl) == 0x18);

void SnapshotString(RuntimeCString& destination, std::string_view source)
{
    if (source.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params SettingTag snapshot exceeds CString length");
    SetRuntimeCStringAt437680(destination, source.data(), static_cast<int>(source.size()));
}

// 仅现代快照的临时拥有者；生产 reader 使用已缓存 header 的 Clone helper。
class SnapshotPair final
{
public:
    SnapshotPair()
    {
        InitializeEmptyRuntimeCString(cell.value.keyAt00);
        FirstFieldGuard partial{&cell.value.keyAt00};
        InitializeEmptyRuntimeCString(cell.value.valueAt04);
        partial.first = nullptr;
    }
    ~SnapshotPair() noexcept { DestroyParamsSettingPairAt40CE90(cell.value); }
    SnapshotPair(const SnapshotPair&) = delete;
    SnapshotPair& operator=(const SnapshotPair&) = delete;
    Pair& Value() noexcept { return cell.value; }
private:
    // 构造失败时 Cell 析构为空；first 的唯一清理资格属于上面的显式 guard。
    union Cell
    {
        Cell() noexcept {}
        ~Cell() noexcept {}
        Pair value;
    } cell;
};

void AppendSnapshotPair(PairStorage& destination, std::string_view key, std::string_view value)
{
    SnapshotPair pair;
    SnapshotString(pair.Value().keyAt00, key);
    SnapshotString(pair.Value().valueAt04, value);
    AppendParamsSettingPairAt4202CF(destination, pair.Value());
}

bool EqualNodes(const ParamsSettingTagRuntime* left, const ParamsSettingTagRuntime* right)
{
    if (!left || !right) return left == right;
    if (RuntimeStringView(left->nameAt00) != RuntimeStringView(right->nameAt00)) return false;
    auto* lp = left->pairsAt04.begin;
    auto* rp = right->pairsAt04.begin;
    for (; lp != left->pairsAt04.end && rp != right->pairsAt04.end; ++lp, ++rp)
        if (RuntimeStringView(lp->keyAt00) != RuntimeStringView(rp->keyAt00) ||
            RuntimeStringView(lp->valueAt04) != RuntimeStringView(rp->valueAt04)) return false;
    if (lp != left->pairsAt04.end || rp != right->pairsAt04.end) return false;
    auto* lc = left->childrenAt10.begin;
    auto* rc = right->childrenAt10.begin;
    for (; lc != left->childrenAt10.end && rc != right->childrenAt10.end; ++lc, ++rc)
        if (!EqualNodes(ResolveParamsSettingTagAt410560(*lc), ResolveParamsSettingTagAt410560(*rc)))
            return false;
    return lc == left->childrenAt10.end && rc == right->childrenAt10.end;
}
}

void RebuildParamsSettingTagAt41FE80(Slot& out, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    ReleaseParamsSettingTagAt41FEAE(out);
    auto* const body = static_cast<ParamsSettingTagRuntime*>(memory.AllocateBody(0x1C));
    if (body)
    {
        // 41FF07 先写 name，随后才清六指针；原 state=-1，GetNil 失败不回滚/补写。
        InitializeEmptyRuntimeCString(body->nameAt00, strings);
        body->pairsAt04.begin = nullptr;
        body->pairsAt04.end = nullptr;
        body->pairsAt04.capacity = nullptr;
        body->childrenAt10.begin = nullptr;
        body->childrenAt10.end = nullptr;
        body->childrenAt10.capacity = nullptr;
    }
    // 41FF37 的第二次检查只删当前 control，不减持，也不调用 payload。
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    out.biasAt04 = 0;
    out.baseAt00 = Address(body);
    void* const raw = memory.AllocateBody(0x18);
    auto* const control = raw ? ::new (raw) SettingTagControl(body) : nullptr;
    // payload 来自跨分配保持的 body；先发布 control，再写 refs，后三字段每次重读槽。
    out.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x1C;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}

ParamsSettingTagRuntime* ResolveParamsSettingTagAt410560(const Slot& slot) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = slot.biasAt04 * stride;
    return reinterpret_cast<ParamsSettingTagRuntime*>(slot.baseAt00 + offset);
}

void ReleaseParamsSettingTagAt41FEAE(Slot& slot)
{
    ReleaseYtlReference(slot);
}

void CloneParamsSettingPairAt42029C(Pair& destination, const Pair& source)
{
    // 420293/4202A7 的 EDI/ESI 已由 reader 固定；与 vector CopyPair 的 second 晚读不同。
    auto* const keyHeader = Header(source.keyAt00);
    auto* const valueHeader = Header(source.valueAt04);
    Publish(destination.keyAt00, CloneRuntimeCStringDataAt414540(keyHeader));
    FirstFieldGuard partial{&destination.keyAt00};
    Publish(destination.valueAt04, CloneRuntimeCStringDataAt414540(valueHeader));
    partial.first = nullptr;
}

void ReleaseParamsSettingPairAt420324(Pair& pair)
{
    DestroyRuntimeCStringAt40CD40(&pair.valueAt04);
    DestroyRuntimeCStringAt40CD40(&pair.keyAt00);
}

void DestroyParamsSettingPairAt40CE90(Pair& pair) noexcept
{
    ReleaseParamsSettingPairAt420324(pair);
}

void AppendParamsSettingPairAt4202CF(PairStorage& storage, const Pair& source,
    IEnvironmentRootMemory& memory)
{
    auto* const end = storage.end;
    if (end != storage.capacity)
    {
        CopyPair(*end, source);
        // 42030C 对回调后的当前 end 加八，不用缓存目的地址覆盖 end。
        storage.end = Advance(storage.end, 1);
        return;
    }
    GrowPairsAt40AA20(storage, end, source, memory);
}

void DestroyParamsSettingPairsAt40DC90(PairStorage& storage, IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.begin) return;
    ReleaseCurrentPairs(storage, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

void DestroyParamsSettingTagAt40FAF0(ParamsSettingTagRuntime& body, IEnvironmentRootMemory& memory) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(body.childrenAt10, memory);
    DestroyParamsSettingPairsAt40DC90(body.pairsAt04, memory);
    DestroyRuntimeCStringAt40CD40(&body.nameAt00);
}

ParamsSettingTagReference::~ParamsSettingTagReference() noexcept { Reset(); }
void ParamsSettingTagReference::Reset() noexcept { ReleaseYtlReference(slot); }
ParamsSettingTagRuntime* ParamsSettingTagReference::Get() noexcept
{
    return ResolveParamsSettingTagAt410560(slot);
}
const ParamsSettingTagRuntime* ParamsSettingTagReference::Get() const noexcept
{
    return ResolveParamsSettingTagAt410560(slot);
}

void ParamsSettingTagReference::AssignSnapshot(const ParamsSettingTag& source)
{
    ParamsSettingTagReference snapshot;
    RebuildParamsSettingTagAt41FE80(snapshot.slot);
    auto* const body = snapshot.Get();
    if (!body) throw std::logic_error("Params SettingTag snapshot body is null");
    SnapshotString(body->nameAt00, source.name.cp932);
    for (const auto& pair : source.pairs)
        AppendSnapshotPair(body->pairsAt04, pair.key.cp932, pair.value.cp932);
    for (const auto& item : source.children)
    {
        ParamsSettingTagReference child;
        child.AssignSnapshot(item);
        AppendEnvironmentArchiveReferenceAt49852D(body->childrenAt10, child.slot);
    }
    *this = std::move(snapshot);
}

void ParamsSettingTagReference::AssignSnapshot(const ParamsSettingTagRuntime& source)
{
    ParamsSettingTagReference snapshot;
    RebuildParamsSettingTagAt41FE80(snapshot.slot);
    auto* const body = snapshot.Get();
    if (!body) throw std::logic_error("Params SettingTag snapshot body is null");
    SnapshotString(body->nameAt00, RuntimeStringView(source.nameAt00));
    for (auto* pair = source.pairsAt04.begin; pair != source.pairsAt04.end; ++pair)
        AppendSnapshotPair(body->pairsAt04, RuntimeStringView(pair->keyAt00), RuntimeStringView(pair->valueAt04));
    for (auto* item = source.childrenAt10.begin; item != source.childrenAt10.end; ++item)
    {
        ParamsSettingTagReference child;
        if (const auto* node = ResolveParamsSettingTagAt410560(*item)) child.AssignSnapshot(*node);
        AppendEnvironmentArchiveReferenceAt49852D(body->childrenAt10, child.slot);
    }
    *this = std::move(snapshot);
}

ParamsSettingTagReference::ParamsSettingTagReference(const ParamsSettingTagReference& other)
{
    if (const auto* source = other.Get()) AssignSnapshot(*source);
}

ParamsSettingTagReference& ParamsSettingTagReference::operator=(const ParamsSettingTagReference& other)
{
    if (this != &other)
    {
        ParamsSettingTagReference snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsSettingTagReference::ParamsSettingTagReference(ParamsSettingTagReference&& other) noexcept
    : slot(std::exchange(other.slot, Slot{})) {}

ParamsSettingTagReference& ParamsSettingTagReference::operator=(ParamsSettingTagReference&& other) noexcept
{
    if (this != &other)
    {
        Reset();
        slot = std::exchange(other.slot, Slot{});
    }
    return *this;
}

bool ParamsSettingTagReference::operator==(const ParamsSettingTagReference& other) const
{
    return EqualNodes(Get(), other.Get());
}
}
