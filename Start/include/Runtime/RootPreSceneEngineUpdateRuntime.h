#pragma once
#include <Platform/InputViews.h>
#include <Runtime/EnvironmentPreSceneUpdateState.h>
#include <Runtime/RootToolWindowServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 46D420完整生产组合；不拥有第二份输入/Environment/声音/图层，只持长寿根的调用视图。
// 本对象须长于使用ToolQueries作为replay来源的工具图层。
class RootPreSceneEngineUpdateRuntime final
{
public:
    RootPreSceneEngineUpdateRuntime(EnvironmentStartupRuntime& environment, ParamsRuntimeRootOwner& params,
        RootRuntimeOwner& root, Platform::IInputRuntime& input, ScriptNamedValueNativeTables tables,
        decltype(&::MessageBoxA) messageBox,
        ToolWindowNativeInteractionServices* nativeTool = nullptr,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    void Update();
    RootToolWindowServicesRuntime& ToolQueries() noexcept { return tools_; }
private:
    EnvironmentPreSceneUpdateState state_;
    RootToolWindowServicesRuntime tools_;
    EnvironmentSoundMappingRestoreSource soundSource_;
    SoundMappingRestoreRuntime soundRestore_;
    PreSceneEngineUpdateServicesRuntime services_;
    PreSceneEngineUpdateRuntime update_;
};
}
