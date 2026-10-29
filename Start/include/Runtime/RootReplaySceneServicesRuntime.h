#pragma once
#include <Runtime/RootReplaySceneNativeSource.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
// 无业务正文；依赖和作用域必须活过Scene/按钮/Script所有逃逸Ref。
class RootReplaySceneServicesRuntime final
{
public:
    RootReplaySceneServicesRuntime(RootReplaySceneNativeDependencies,ModeSelectButtonNativeServices&,
        OptionCharSelectDialogNativeDependencies&,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory())noexcept;
    RootReplaySceneServicesRuntime(RootReplaySceneNativeDependencies,SceneManagerNativeServices&,
        IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory())noexcept;
    RootReplaySceneServicesRuntime(const RootReplaySceneServicesRuntime&)=delete;
    RootReplaySceneServicesRuntime& operator=(const RootReplaySceneServicesRuntime&)=delete;
    ReplaySceneNativeServices& Scene()noexcept{return scene_;}
    ReplayButtonNativeServices& Buttons()noexcept{return buttons_;}
    RootReplaySceneNativeSource& Source()noexcept{return source_;}
    RootReplayScriptNativeServices& Script()noexcept{return scripts_;}
private:
    RootReplaySceneNativeSource source_;
    ModeButtonNativeServices mode_;
    ReplayButtonNativeServices buttons_;
    RootReplayScriptNativeServices scripts_;
    ReplaySceneNativeServices scene_;
};
}
