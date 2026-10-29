#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScrSaveParams2Native.h>
#include <Runtime/NativeArchiveRuntime.h>

#include <new>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Body>
class ParamsControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ParamsControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        if constexpr (std::is_same_v<Body, ScriptParamsNativeBody>)
        {
            // 492CA0不查vptr：CParams没有虚表。
            if (body)
            {
                body->~ScriptParamsNativeBody();
                ::operator delete(body, std::size_t{0x70});
            }
        }
        else
        {
            // 492D00/492CF0按子正文当前IArchive虚表的+04槽释放。
            if (body) DeleteNativeArchiveAtVslot04(body, 1);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ParamsControl<ScriptParamsNativeBody>) == 0x18);
static_assert(sizeof(ParamsControl<ScrSaveParamsRuntimeState>) == 0x18);
static_assert(sizeof(ParamsControl<ScrSaveParams2Native>) == 0x18);

template<class Body>
void Publish(YtlReferenceSlotRuntime& slot, Body* body, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) ParamsControl<Body>(body) : nullptr;
    slot.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    // 原版在每次字段写入前都重新读control，不用缓存快照代替。
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = sizeof(Body);
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

template<class Body>
struct FailedConstruction final
{
    IEnvironmentRootMemory& memory;
    void* raw;
    ~FailedConstruction() noexcept { memory.DeleteBody(raw, sizeof(Body)); }
};

ScriptParamsNativeBody* Construct(IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    void* raw = memory.AllocateBody(0x70);
    try { return raw ? ::new (raw) ScriptParamsNativeBody(memory, strings) : nullptr; }
    catch (...)
    {
        FailedConstruction<ScriptParamsNativeBody> cleanup{memory, raw};
        throw;
    }
}
}

ScriptParamsNativeBody::ScriptParamsNativeBody(IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
    : timestampLowAt00(0), timestampHighAt04(0), fileCallsAt18(), localCallsAt24(),
      historyAt3C(), valuesAt48(), menuCandidatesAt64()
{
    // 45B020先建立全部空Ref/vector，最后46CE30；+14/+60在Reset尾部才写。
    try { ResetAt46CE30(memory, strings); }
    catch (...)
    {
        // 原八段展开不修复无control孤立正文，也不调用虚析构。
        // 5E65A0只有unwind、没有catch；成员清理必须发生在重抛展开期间。
        struct Cleanup final
        {
            ScriptParamsNativeBody& body;
            ~Cleanup() noexcept { body.DestroyMembers(); }
        } cleanup{*this};
        throw;
    }
}

void ScriptParamsNativeBody::DestroyMembers() noexcept
{
    // 463490与构造展开均为64 -> 54 -> 48 -> 3C -> 30 -> 24 -> 18 -> 08。
    menuCandidatesAt64.~ScriptDwordVectorStorage();
    ReleaseYtlReference(referenceAt54);
    valuesAt48.~ScriptDwordVectorStorage();
    historyAt3C.~ScriptDwordVectorStorage();
    ReleaseYtlReference(saveParams2At30);
    localCallsAt24.~ScriptDwordVectorStorage();
    fileCallsAt18.~ScriptFileCallStorage();
    ReleaseYtlReference(saveParamsAt08);
}

ScriptParamsNativeBody::~ScriptParamsNativeBody() noexcept { DestroyMembers(); }

void ScriptParamsNativeBody::ResetAt46CE30(IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
{
    void* raw = memory.AllocateBody(0x0C);
    ScrSaveParamsRuntimeState* first = nullptr;
    try { if (raw) first = ::new (raw) ScrSaveParamsRuntimeState(strings); }
    catch (...)
    {
        FailedConstruction<ScrSaveParamsRuntimeState> cleanup{memory, raw};
        throw;
    }
    // 46CEDC已撤销构造回收资格；发布或旧释放失败不得替调用方补收新正文。
    Publish(saveParamsAt08, first, memory);
    raw = memory.AllocateBody(0x80C);
    auto* second = raw ? ::new (raw) ScrSaveParams2Native : nullptr;
    Publish(saveParams2At30, second, memory);
    fileCallsAt18.clear();
    localCallsAt24.clear();
    programCounter = 0;
    messageFormatIndexAt60 = -1;
}

ScrSaveParamsRuntimeState* ScriptParamsNativeBody::ResolveSaveParamsAt08() const noexcept
    { return ResolveEnvironmentObjectReference<ScrSaveParamsRuntimeState>(saveParamsAt08); }
ScrSaveParams2Native* ScriptParamsNativeBody::ResolveSaveParams2At30() const noexcept
    { return ResolveEnvironmentObjectReference<ScrSaveParams2Native>(saveParams2At30); }

std::uint64_t ScriptParamsNativeBody::GetSaveTimestamp() const noexcept
{
    return static_cast<std::uint64_t>(timestampLowAt00) |
        (static_cast<std::uint64_t>(timestampHighAt04) << 32u);
}

void ScriptParamsNativeBody::SetSaveTimestamp(std::uint64_t value) noexcept
{
    timestampLowAt00 = static_cast<std::uint32_t>(value);
    timestampHighAt04 = static_cast<std::uint32_t>(value >> 32u);
}

ScriptParamsNativeBody* GetParamsAt47BDB0(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    if (!slot.baseAt00) Publish(slot, Construct(memory, strings), memory);
    return ResolveEnvironmentObjectReference<ScriptParamsNativeBody>(slot);
}

void ReplaceParamsAt481720(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    Publish(slot, Construct(memory, strings), memory);
}
}
