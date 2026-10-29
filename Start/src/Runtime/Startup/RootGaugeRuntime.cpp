#include <Runtime/RootGaugeRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootGaugeValueSource::RootGaugeValueSource(RootRuntimeOwner& root, ScriptNamedValueNativeTables names,
    IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings) noexcept
    : root_(root), names_(names), errors_(errors), strings_(strings) {}
std::int32_t RootGaugeValueSource::ReadGaugeValue(const char* name)
{
    root_.EnsureCreatedAt413BE0();
    auto& root = root_.RequirePublished();
    const auto index = ResolveScriptNamedValueNativeAt498020(name, names_, strings_);
    return ReadScriptValueAt4C73A0(root.ParamsAt47BDB0(), index, errors_);
}
namespace
{
LayerItemNativeBody14h* CaptureGauge(IDrawManagerNativeRootSource& source, bool secondary)
{
    const auto slot = secondary ? StartupMasoGaugeLayerIndex() : StartupPrimaryGaugeLayerIndex();
    return DrawLayerAt418C10(*source.CaptureDrawManagerAfter413BE0(), slot);
}
}
void RequestGaugeLayerNative(IDrawManagerNativeRootSource& source, bool secondary, bool requested)
{
    if (secondary ? !CompiledGameProfile().loveHolicGauges : !HasPrimaryGauge())
        throw std::logic_error("Requested gauge is absent from the compiled game profile");
    auto* target = CaptureGauge(source, secondary);
    const auto* methods = reinterpret_cast<const LayerItemNativeFrameVtable*>(target->vptrAt00);
    (void)methods->requestAt10(target, static_cast<std::uint8_t>(requested), 0);
    // 原07 4CF344、08 4D2636/4D2716：request返回0也必须重新取层并标dirty。
    target = CaptureGauge(source, secondary);
    methods = reinterpret_cast<const LayerItemNativeFrameVtable*>(target->vptrAt00);
    RECT bounds;
    const auto* result = methods->boundsAt0C(target, &bounds);
    target->parentAt10->vptrAt00->addDirtyAt20(target->parentAt10, result);
}
void RestorePrimaryGaugeLayerNative(IDrawManagerNativeRootSource& source, TokimekiGaugeNativeServices* tokimeki,
    LoveHolicGaugesNativeServices* loveHolic)
{
    if (!HasPrimaryGauge()) return;
    if (CompiledGameProfile().tokimekiGauge ? !tokimeki : !loveHolic)
        throw std::logic_error("Primary gauge restoration services are absent");
    const auto value = CompiledGameProfile().tokimekiGauge
        ? tokimeki->state.ReadGaugeValue("gague_disp")
        : loveHolic->state.ReadGaugeValue("gague_disp");
    if (!value) return;
    auto* target = CaptureGauge(source, false);
    // 恢复路径直接Update再启用；不能改成request增加一次disp读取或对Maso补对称操作。
    if (CompiledGameProfile().tokimekiGauge)
        UpdateTokimekiGaugeNative(*reinterpret_cast<TokimekiGaugeNativeBody*>(target));
    else UpdateLoveHolicGaugeNative(*reinterpret_cast<LoveHolicGaugeNativeBody*>(target));
    target->enabledAt0C = 1;
}
void RootProgramGaugeRequests::RequestGauge(bool secondary, bool requested)
{
    RootDrawManagerNativeSource source(root_);
    RequestGaugeLayerNative(source, secondary, requested);
}
void RootProgramGaugeRequests::RestoreVisiblePrimaryGauge()
{
    if (!HasPrimaryGauge()) return;
    root_.EnsureCreatedAt413BE0();
    auto& services = root_.RequirePublished().PostWindow().NativeLayerServices();
    RootDrawManagerNativeSource source(root_);
    RestorePrimaryGaugeLayerNative(source, services.tokimeki, services.loveHolic);
}
}
