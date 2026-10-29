#include <Runtime/ParamsSceneLabelNative.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsRuntimeReadCursor.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <cwchar>
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
using Entry = ParamsSceneLabelEntryRuntime;

class RetiredSceneLabelControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredSceneLabelControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役后再次删除的实参是 14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredSceneLabelControl) == 0x18);

void RetireSceneLabelControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 411830/411350：flags=0 仍换虚表，+04..17 原字节不变；首次删除传 18h。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredSceneLabelControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

class SceneLabelOwnerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SceneLabelOwnerControl(ParamsSceneLabelNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5AE28C -> 43CBA0：缓存正文，40DD60 完成后才删同一 10h 正文。
        auto* const body = static_cast<ParamsSceneLabelNative*>(payloadAt14);
        if (!body) return;
        DestroyEnvironmentArchiveReferencesAt497080(body->labelsAt04);
        ::operator delete(body, std::size_t{0x10});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireSceneLabelControl(this, flags); }
};
static_assert(sizeof(SceneLabelOwnerControl) == 0x18);

class SceneLabelEntryControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SceneLabelEntryControl(Entry* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5AE528 -> 43C7E0 无局部 EH：raw name Free 可抛，不能换 ATL noexcept 析构。
        auto* const body = static_cast<Entry*>(payloadAt14);
        if (!body) return;
        DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, std::size_t{0x0C});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireSceneLabelControl(this, flags); }
};
static_assert(sizeof(SceneLabelEntryControl) == 0x18);

struct CurrentReferenceCleanup final
{
    Slot& slot;
    bool armed = true;
    ~CurrentReferenceCleanup() noexcept
    { if (armed) ReleaseParamsSceneLabelAt40CFE0(slot); }
};

void ReleaseCachedReference(Slot& slot, YtlReferenceControlRuntime* captured) noexcept
{
    // 41FC08/41FC39 的 state6/7 -> 541E55；payload 后仍使用缓存 control。
    if (!captured) return;
    if (--captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

void AppendSceneLabelReference(Storage& storage, std::uintptr_t base,
    YtlReferenceControlRuntime* captured, IEnvironmentRootMemory& memory)
{
    // 41FBBA：第二个局部 Ref 是独立持有；其展开资格先于 append。
    Slot temporary{base, 0, captured};
    CurrentReferenceCleanup cleanup{temporary};
    if (captured) ++captured->referenceCountAt04;
    auto* const end = storage.end;
    if (end != storage.capacity)
    {
        end->baseAt00 = base;
        end->biasAt04 = 0;
        end->controlAt08 = captured;
        if (captured) ++captured->referenceCountAt04;
        storage.end = reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(storage.end) + 12u);
    }
    else
    {
        // 408900 与共享增长逐 helper 对照：分配后读源、copy 旧 Ref、1.5 倍和 EH 一致。
        AppendEnvironmentArchiveReferenceAt49852D(storage, temporary, memory);
        captured = temporary.controlAt08; // 41FC05 仅慢路径重读第二局部。
    }
    cleanup.armed = false;
    ReleaseCachedReference(temporary, captured);
}

void ReadSceneLabelText8At41F9E3(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination,
    const std::uint8_t& key, IRuntimeStringManagerSource& strings)
{
    const auto start = cursor.Offset();
    const auto length = std::to_integer<std::uint8_t>(cursor.Slice(start, 1)[0]);
    if (length)
    {
        // 41FA10..41FA20：显式可写输入逐字节原地XOR；失败留下已改前缀，不复制或回滚。
        for (std::uint32_t index = 0; index < length; ++index)
            cursor.XorByteAt(start + 1u + index, key);
        // 41FA2A..41FA38：417360之前固定源地址，40B490完成后才有CString清理资格。
        const auto source = cursor.CaptureSourceAt(start + 1u);
        struct Temporary final
        {
            union { RuntimeCString text; };
            bool armed = false;
            Temporary() noexcept {}
            ~Temporary() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&text); }
        } temporary;
        auto* const manager = strings.CaptureAt417360();
        ConstructRuntimeCStringAt40B490(&temporary.text,
            reinterpret_cast<const char*>(source.data), length, manager);
        temporary.armed = true;
        if (!destination)
            throw std::logic_error("SceneLabel narrow destination is null (safe boundary)");
        AssignRuntimeCStringAt410220(*destination, temporary.text);
        // 41FA4D..41FA69：先缓存header再撤state2，正常Free抛出不能重试。
        auto* const header = reinterpret_cast<ATL::CStringData*>(
            const_cast<char*>(temporary.text.GetString())) - 1;
        temporary.armed = false;
        ReleaseRuntimeCStringHeader(header);
    }
    // length=0只消耗1，既不解密也不清目标；正常临时释放以后才提交。
    cursor.CommitOffsetAfter419EB0(static_cast<std::uint32_t>(start + length + 1u));
}

void ReadSceneLabelTextAt41FA74(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination)
{
    auto next = cursor;
    const auto start = next.Offset();
    const auto length = static_cast<std::uint32_t>(next.U16());
    if (length)
    {
        ByteArrayReferenceRuntime temporary;
        temporary.AllocateAt412700(length + 2u);
        auto* control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
        const auto count = control ? control->countAt10 : 0u;
        const auto base = temporary.RawSlot().baseAt00;
        const auto address = [&]() noexcept
        {
            return base ? reinterpret_cast<std::uint8_t*>(base +
                temporary.RawSlot().biasAt04 * (control ? control->strideAt0C : 0u)) : nullptr;
        };
        std::memset(address(), 0, count);
        auto* const target = address();
        // 41FB25：临时数组分配以后才重取输入 begin；不预先保存字符 span。
        const auto input = next.Slice(next.Offset(), length);
        std::memmove(target, input.data(), length);
        // 此时输入已验证且无中间回调。之后只复制游标状态，不在 Free 后再次 Seek。
        next.Seek(start + length + 2u);
        // 41FB38：wide 解址与正常释放改取当前 control，但 base 仍是旧 EBX。
        control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
        const auto* const wide = reinterpret_cast<const wchar_t*>(address());
        // 明确现代安全域：不为奇数字节补造额外空间，也不越界扫描 WCHAR 零。
        if (wide && !std::wmemchr(wide, L'\0', (length + 2u) / sizeof(wchar_t)))
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUtf16, start + 2u,
                "SceneLabel WCHAR text has no complete NUL within its byte allocation (safe boundary)");
        AssignRuntimeCStringWideAt4102D0(destination, wide);
        // state4 是正常 cached terminate；更早异常由 ByteArray 的 current 40CF00 清理。
        ReleaseCachedReference(temporary.RawSlot(), control);
        // cached=null 时原局部不再有清理资格，不释放回调可能写入的新 control。
        temporary.RawSlot().controlAt08 = nullptr;
    }
    cursor = next;
}

Entry& RequireEntry(Entry* entry)
{
    if (!entry) throw std::logic_error("SceneLabel reference resolves to null (safe boundary)");
    return *entry;
}

const Slot& SlotAt(const Storage& storage, std::uint32_t index) noexcept
{
    return *reinterpret_cast<const Slot*>(reinterpret_cast<std::uintptr_t>(storage.begin) + index * 12u);
}

void SnapshotEntry(Storage& destination, const char* chars, std::size_t length,
    std::uint32_t value1, std::uint32_t value2)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("SceneLabel snapshot exceeds CString length");
    // 工具事务独立于原 reader：分配失败可以回收工具新正文，不替代原工厂失败资格。
    auto* body = static_cast<Entry*>(::operator new(sizeof(Entry)));
    bool nameComplete = false;
    SceneLabelEntryControl* control = nullptr;
    try
    {
        InitializeEmptyRuntimeCString(body->nameAt00);
        nameComplete = true;
        SetRuntimeCStringAt437680(body->nameAt00, chars, static_cast<int>(length));
        body->value1At04 = value1;
        body->value2At08 = value2;
        void* raw = ::operator new(sizeof(SceneLabelEntryControl));
        control = ::new (raw) SceneLabelEntryControl(body);
    }
    catch (...)
    {
        if (nameComplete) DestroyRuntimeCStringAt40CD40(&body->nameAt00);
        ::operator delete(body, sizeof(Entry));
        throw;
    }
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x0C;
    control->countAt10 = 1;
    Slot reference{reinterpret_cast<std::uintptr_t>(body), 0, control};
    CurrentReferenceCleanup cleanup{reference};
    AppendEnvironmentArchiveReferenceAt49852D(destination, reference);
}
}

void RebuildParamsSceneLabelAt434ECE(Slot& out, IEnvironmentRootMemory& memory)
{
    // 434E9B 普通释放可抛；payload 后由共享 helper 重读当前槽。
    ReleaseYtlReference(out);
    void* rawBody = memory.AllocateBody(0x10);
    auto* const body = rawBody ? ::new (rawBody) ParamsSceneLabelNative : nullptr;
    // 原处于 state=-1；control 分配失败不得回滚正文，亦无构造失败回收动作。
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* rawControl = memory.AllocateBody(0x18);
    auto* const control = rawControl ? ::new (rawControl) SceneLabelOwnerControl(body) : nullptr;
    out.controlAt08 = control; // 434F2D 先发布，再写 refs。
    // 原 null+4 故障的显式安全边界，保留部分发布，不增加 nil 或回滚。
    if (!control) throw std::bad_alloc{};
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x10;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}

Entry* CreateParamsSceneLabelEntryAt41F934(Slot& out, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* const body = static_cast<Entry*>(memory.AllocateBody(0x0C));
    if (body) InitializeEmptyRuntimeCString(body->nameAt00, strings);
    // 41F961 才清输出；GetNil 失败没有正文回收、没有旧槽释放，也不写后两个 DWORD。
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    if (!body) return nullptr;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* const control = raw ? ::new (raw) SceneLabelEntryControl(body) : nullptr;
    // 41F9A9 原空 control 在发布前故障；安全抛错也不替调用者清理 body。
    if (!control) throw std::bad_alloc{};
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x0C;
    out.controlAt08 = control; // 41F9BD 与 owner 的发布顺序不同。
    control->countAt10 = 1;
    return body;
}

ParamsSceneLabelNative* ResolveParamsSceneLabelOwner(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<ParamsSceneLabelNative>(slot); }
Entry* ResolveParamsSceneLabelEntry(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<Entry>(slot); }
void ReleaseParamsSceneLabelAt40CFE0(Slot& slot) noexcept
{
    // 5E0858：maxState=0, flags=5，current Ref payload/delete 任一异常终止。
    ReleaseYtlReference(slot);
}

void ReadParamsSceneLabelsNativeAt41F8A0(ParamsRuntimeReadCursor& cursor,
    ParamsSceneLabelNative& native, double version, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    // 41F8A0没有二次版本门禁，版本来自434940；仅按5.0比较选择wire。
    auto local = cursor;
    // 41F8DF..41F8EC：旧key在清表前发布；>=5不碰legacy byte及padding。
    if (version < 5.0) native.legacyByteAt00 = local.U8();
    native.clear(); // state0；先清旧表并保留容量，随后才读取 count。
    const auto count = std::bit_cast<std::int32_t>(local.U32());
    for (std::int32_t index = 0; index < count; ++index)
    {
        // named union 不自动初始化槽，亦不自动清理尚未达到 state1 的工厂失败。
        union Cell
        {
            Slot value;
            Cell() noexcept {}
            ~Cell() noexcept {}
        } first;
        auto* const created = CreateParamsSceneLabelEntryAt41F934(first.value, strings, memory);
        auto* const cachedControl = first.value.controlAt08;
        CurrentReferenceCleanup cleanup{first.value};
        if (version < 5.0)
            ReadSceneLabelText8At41F9E3(local, created ? &created->nameAt00 : nullptr,
                native.legacyByteAt00, strings);
        else ReadSceneLabelTextAt41FA74(local, created ? &created->nameAt00 : nullptr);
        // 41FB9E 在 name/byteRef 完成后取当前 base；两数都写同一个缓存 EDX。
        auto* const scalarTarget = reinterpret_cast<Entry*>(first.value.baseAt00);
        const auto value1 = local.U32();
        if (!scalarTarget)
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUnitCount, local.Offset() - 4u,
                "SceneLabel scalar destination is null (safe boundary)");
        scalarTarget->value1At04 = value1;
        const auto value2 = local.U32();
        scalarTarget->value2At08 = value2;
        AppendSceneLabelReference(native.labelsAt04, reinterpret_cast<std::uintptr_t>(scalarTarget),
            cachedControl, memory);
        cleanup.armed = false;
        ReleaseCachedReference(first.value, cachedControl);
    }
    // 原只返回消费量；不得在最后一次释放回调后再次验证输入范围。
    cursor = local;
}

ParamsSceneLabelNative::ParamsSceneLabelNative() noexcept : labelsAt04{nullptr, nullptr, nullptr} {}
ParamsSceneLabelNative::~ParamsSceneLabelNative() noexcept
{
    // 40DD60 / 5E07A4 flags=5：begin 为空不改其余两指针。
    DestroyEnvironmentArchiveReferencesAt497080(labelsAt04);
}
ParamsSceneLabelNative::ParamsSceneLabelNative(const ParamsSceneLabelNative& other)
    : ParamsSceneLabelNative()
{
    ParamsSceneLabelNative snapshot;
    for (std::uint32_t index = 0; index < other.size(); ++index)
    {
        const auto& source = other[index];
        const auto name = RuntimeStringView(source.nameAt00);
        SnapshotEntry(snapshot.labelsAt04, name.data(), name.size(), source.value1At04, source.value2At08);
    }
    labelsAt04 = std::exchange(snapshot.labelsAt04, Storage{});
}
ParamsSceneLabelNative& ParamsSceneLabelNative::operator=(const ParamsSceneLabelNative& other)
{
    if (this != &other)
    {
        ParamsSceneLabelNative snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}
ParamsSceneLabelNative::ParamsSceneLabelNative(ParamsSceneLabelNative&& other) noexcept
    : labelsAt04(std::exchange(other.labelsAt04, Storage{})) {}
ParamsSceneLabelNative& ParamsSceneLabelNative::operator=(ParamsSceneLabelNative&& other) noexcept
{
    if (this != &other)
    {
        DestroyEnvironmentArchiveReferencesAt497080(labelsAt04);
        labelsAt04 = std::exchange(other.labelsAt04, Storage{});
    }
    return *this;
}
std::size_t ParamsSceneLabelNative::size() const noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(labelsAt04.end) -
        reinterpret_cast<std::uintptr_t>(labelsAt04.begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}
Entry* ParamsSceneLabelNative::Resolve(std::uint32_t index) noexcept
{ return ResolveParamsSceneLabelEntry(SlotAt(labelsAt04, index)); }
const Entry* ParamsSceneLabelNative::Resolve(std::uint32_t index) const noexcept
{ return ResolveParamsSceneLabelEntry(SlotAt(labelsAt04, index)); }
Entry& ParamsSceneLabelNative::operator[](std::uint32_t index)
{ return RequireEntry(Resolve(index)); }
const Entry& ParamsSceneLabelNative::operator[](std::uint32_t index) const
{ return RequireEntry(ResolveParamsSceneLabelEntry(SlotAt(labelsAt04, index))); }
Entry& ParamsSceneLabelNative::at(std::size_t index)
{ return const_cast<Entry&>(std::as_const(*this).at(index)); }
const Entry& ParamsSceneLabelNative::at(std::size_t index) const
{
    if (index >= size()) throw std::out_of_range("SceneLabel index exceeds its native table");
    return (*this)[static_cast<std::uint32_t>(index)];
}
void ParamsSceneLabelNative::clear() noexcept
{ ClearEnvironmentArchiveReferencesAt498312(labelsAt04); }
void ParamsSceneLabelNative::AssignSnapshot(std::span<const ParamsSceneLabel> source)
{
    ParamsSceneLabelNative snapshot;
    for (const auto& entry : source)
        SnapshotEntry(snapshot.labelsAt04, entry.name.cp932.data(), entry.name.cp932.size(),
            entry.value1, entry.value2);
    *this = std::move(snapshot);
}
bool ParamsSceneLabelNative::operator==(const ParamsSceneLabelNative& other) const
{
    if (size() != other.size()) return false;
    for (std::uint32_t index = 0; index < size(); ++index)
    {
        const auto& left = (*this)[index];
        const auto& right = other[index];
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00) ||
            left.value1At04 != right.value1At04 || left.value2At08 != right.value2At08) return false;
    }
    return true;
}

std::vector<ParamsSceneLabel> ExportParamsSceneLabelSnapshot(const ParamsSceneLabelNative& native)
{
    // 显式现代快照，结果独立拥有；不放入 native/root，也不冒充原 wire Unicode。
    std::vector<ParamsSceneLabel> result;
    const auto count = native.size();
    result.reserve(count);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto& source = native[index];
        ParamsSceneLabel item;
        item.name.cp932.assign(RuntimeStringView(source.nameAt00));
        if (!item.name.cp932.empty())
        {
            if (item.name.cp932.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
                throw std::length_error("SceneLabel snapshot exceeds Win32 text length");
            const auto bytes = static_cast<int>(item.name.cp932.size());
            const auto characters = ::MultiByteToWideChar(CP_THREAD_ACP, 0,
                item.name.cp932.data(), bytes, nullptr, 0);
            if (characters <= 0) throw std::runtime_error("SceneLabel snapshot text conversion failed");
            item.name.utf16.resize(static_cast<std::size_t>(characters));
            static_assert(sizeof(wchar_t) == sizeof(char16_t));
            if (::MultiByteToWideChar(CP_THREAD_ACP, 0, item.name.cp932.data(), bytes,
                reinterpret_cast<wchar_t*>(item.name.utf16.data()), characters) != characters)
                throw std::runtime_error("SceneLabel snapshot text conversion changed length");
        }
        item.value1 = source.value1At04;
        item.value2 = source.value2At08;
        result.push_back(std::move(item));
    }
    return result;
}

ParamsSceneLabelReference::~ParamsSceneLabelReference() noexcept { reset(); }
ParamsSceneLabelReference::ParamsSceneLabelReference(const ParamsSceneLabelReference& other) noexcept
    : slot_(other.slot_)
{ if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04; }
ParamsSceneLabelReference& ParamsSceneLabelReference::operator=(const ParamsSceneLabelReference& other)
{
    if (this != &other)
    {
        ParamsSceneLabelReference retained(other);
        *this = std::move(retained);
    }
    return *this;
}
ParamsSceneLabelReference::ParamsSceneLabelReference(ParamsSceneLabelReference&& other) noexcept
    : slot_(std::exchange(other.slot_, Slot{})) {}
ParamsSceneLabelReference& ParamsSceneLabelReference::operator=(ParamsSceneLabelReference&& other)
{
    if (this != &other)
    {
        reset();
        slot_ = std::exchange(other.slot_, Slot{});
    }
    return *this;
}
void ParamsSceneLabelReference::reset() { ReleaseYtlReference(slot_); }
}
