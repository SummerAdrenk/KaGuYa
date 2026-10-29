#include <Runtime/NativeLayerWindowScrVmUpdateRuntime.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/LayerWindowNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
LayerItemNativeBody14h* NativeLayerAt(const DrawLayerReferenceArray& layers, std::uint32_t index)
{
    // 418C10和4AD2A0的Ref12读取核相同：signed差/12后无符号比较，等于end不拒绝。
    const auto count = static_cast<std::uint32_t>(DrawLayerReferenceCount(layers));
    if (index > count) return nullptr;
    const auto* slot = reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(layers.begin) + index * 12u);
    return ResolveDrawLayerReference(*slot);
}

LayerWindowNativeBody4Ch* CaptureFirstWindow(IDrawManagerNativeRootSource& source)
{
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    return reinterpret_cast<LayerWindowNativeBody4Ch*>(NativeLayerAt(manager->ownedLayersAt14, 0));
}
}

NativeLayerWindowScrVmUpdateRuntime::NativeLayerWindowScrVmUpdateRuntime(ScriptNativeBody& receiver,
    ScriptResultLayerNativeBindingServices& results, ILayerWindowEffectNativeSource& effects) noexcept
    : receiver_(receiver), results_(results), effects_(effects)
{
}

void NativeLayerWindowScrVmUpdateRuntime::ApplyResultLayerList(std::int32_t listIndex)
{
    // 4D294B固定调用者CScript，不通过LoadedScript状态门控或转换原异常类别。
    ApplyScriptResultLayersNativeAt4CB030(receiver_, listIndex, results_);
}

void NativeLayerWindowScrVmUpdateRuntime::SelectLayerEffect(std::int32_t selector)
{
    auto* const manager = results_.draw.CaptureDrawManagerAfter413BE0();
    // 4D6C43首轮直接读取首槽，未访问end；不同selector才第二次取根走418C10。
    auto* const window = ResolveEnvironmentObjectReference<LayerWindowNativeBody4Ch>(
        *manager->ownedLayersAt14.begin);
    if (window->selectorAt3C == selector) return;
    auto* const current = CaptureFirstWindow(results_.draw);
    ReplaceNativeLayerWindowEffectAt4D6860(*current, selector, effects_);
}

bool NativeLayerWindowScrVmUpdateRuntime::ReadActiveResultPair(std::size_t index,
    ScrVmUpdatePair& destination) const
{
    // 4D2B8E每对坐标重新取根；4AD2A0返回null才交外层抛int1，目标pair保持原值。
    auto* const window = CaptureFirstWindow(results_.draw);
    auto* const pattern = NativeLayerAt(window->patternsAt30, static_cast<std::uint32_t>(index));
    if (!pattern) return false;
    const auto x = pattern->valueAt04;
    const auto y = pattern->valueAt08;
    destination = {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y)};
    return true;
}
}
