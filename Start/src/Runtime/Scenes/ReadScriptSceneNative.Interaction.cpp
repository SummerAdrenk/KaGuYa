#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/FreeScrollNativeRuntime.h>
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
{
bool InteractionLayers::PrepareLayersAt46AD20(std::int32_t mode)
{ return PrepareDrawLayersViewAt46AD20(source_.DrawLayers(), mode) != 0; }
bool InteractionLayers::PrepareLayersAt46A980()
{ return PrepareDrawLayersViewAt46A980(source_.DrawLayers()) != 0; }
void InteractionLayers::SetLayerVisible(std::uint32_t slot, std::uint8_t requested, std::int32_t argument)
{
    // 共享交互层保留逻辑槽号，实体日期层插入后只在该边界映射Select/Opacity。
    if (slot == 5) slot = StartupSelectionLayerIndex();
    else if (slot == 6) slot = StartupOpacityLayerIndex();
    static_cast<void>(source_.DrawLayers().CaptureDrawLayersAt413BE0()
        .RequestOwnedLayerAtVslot10(slot, requested, argument));
}
void InteractionLayers::ResetOpacityAt47EE40()
{
    ReadScriptLayerServicesRuntime layers(source_.DrawLayers(), *this, source_.ScrollServices().opacity);
    layers.ResetOpacityAt47EE40();
}
bool InteractionLayers::UpdateLayerSixAt4BE530()
{
    ReadScriptLayerServicesRuntime layers(source_.DrawLayers(), *this, source_.ScrollServices().opacity);
    return layers.UpdateLayerSixAt4BE530();
}
bool InteractionLayers::BeginAt4ADF70()
{
    auto services = source_.ScrollServices();
    return BeginFreeScrollNativeAt4ADF70(body_.scrollAt60, services.draw, services.params,
        services.graphics, services.memory);
}
bool InteractionLayers::UpdateAt4BDD00()
{
    auto services = source_.ScrollServices();
    return UpdateFreeScrollNativeAt4BDD00(body_.scrollAt60, services.input, services.graphics,
        services.registry, services.screen);
}
void InteractionLayers::EndAt4AC970()
{
    auto services = source_.ScrollServices();
    EndFreeScrollNativeAt4AC970(body_.scrollAt60, services.draw, services.params,
        services.patterns, services.input);
}
void InteractionLayers::BeginHiddenWindowAt4ADF70() { static_cast<void>(BeginAt4ADF70()); }
bool InteractionLayers::UpdateHiddenWindowAt4BDD00() { return UpdateAt4BDD00(); }
void InteractionLayers::EndHiddenWindowAt4AC970() { EndAt4AC970(); }
}
