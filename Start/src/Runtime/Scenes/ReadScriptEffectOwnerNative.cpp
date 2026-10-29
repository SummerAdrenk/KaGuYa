#include <Runtime/ReadScriptEffectOwnerNative.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class EffectControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit EffectControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4C5FF0/4C6010/4C6040/4C6050：当前payload的vptr+0C，flags=1。
        if (payloadAt14) DeleteReadScriptEffectNative(payloadAt14, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(EffectControl) == 0x18);
}

void InitializeReadScriptEffectSlotsAt4A14D0(ReadScriptEffectSlotsNative30h& slots) noexcept
{ slots = {}; }

void ReleaseReadScriptEffectSlotsAt4C0990(ReadScriptEffectSlotsNative30h& slots)
{
    ReleaseYtlReference(slots.effectAt00);
    ReleaseYtlReference(slots.sourceAt0C);
    ReleaseYtlReference(slots.previousAt18);
    ReleaseYtlReference(slots.auxiliaryAt24);
}

void DestroyReadScriptEffectSlotMembersAt4A4920(ReadScriptEffectSlotsNative30h& slots) noexcept
{
    // 4A4976..4A4A57及59CCD9..59CCB8：不预先缓存四个control。
    // 4A33F0/40D2F0同样在payload后重读control，二者FuncInfo EHFlags均为5。
    ReleaseYtlReference(slots.auxiliaryAt24);
    ReleaseYtlReference(slots.previousAt18);
    ReleaseYtlReference(slots.sourceAt0C);
    ReleaseYtlReference(slots.effectAt00);
}
void UnwindReadScriptEffectSlotsAt4A14D0(ReadScriptEffectSlotsNative30h& slots) noexcept
{ DestroyReadScriptEffectSlotMembersAt4A4920(slots); }

void InstallReadScriptEffectNative(YtlReferenceSlotRuntime& slot, void* created,
    std::uint32_t originalStride)
{
    // 释放旧payload之前，候选构造分配的EH state已恢复为-1。
    ReleaseYtlReference(slot);
    if (!created) return;
    // 4A7E18/498933/4989D3已清缓存ECX；后续test ECX不再次读slot.control。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* storage = ::operator new(0x18);
    auto* control = storage ? ::new (storage) EffectControl(created) : nullptr;
    slot.controlAt08 = control;
    // 保留原null-control写入故障，且不初始化+09..0B字节。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = originalStride;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

bool ReadScriptEffectOwnerNative::HasEffectAt1C() const { return slots_.effectAt00.baseAt00 != 0; }
bool ReadScriptEffectOwnerNative::TickEffectAtVslot00()
{ return TickReadScriptEffectNative(ResolveEnvironmentObjectReference<void>(slots_.effectAt00)) != 0; }
void ReadScriptEffectOwnerNative::DrawEffectAtVslot04()
{ DrawReadScriptEffectNative(ResolveEnvironmentObjectReference<void>(slots_.effectAt00)); }
std::uint8_t ReadScriptEffectOwnerNative::ReadyEffectAtVslot08() const
{ return ReadyReadScriptEffectNative(ResolveEnvironmentObjectReference<void>(slots_.effectAt00)); }
void ReadScriptEffectOwnerNative::ReleaseResourcesAt4C0990()
{ ReleaseReadScriptEffectSlotsAt4C0990(slots_); }
void ReadScriptEffectOwnerNative::InstallEffect(void* created, std::uint32_t originalStride)
{ InstallReadScriptEffectNative(slots_.effectAt00, created, originalStride); }
GraphicsSurfaceNativeBody* ReadScriptEffectOwnerNative::SourceBodyAt28() const noexcept
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slots_.sourceAt0C); }
}
