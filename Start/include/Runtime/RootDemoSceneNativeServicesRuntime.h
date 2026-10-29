#pragma once
#include <Runtime/RootDemoSceneRuntimeServices.h>
#include <Runtime/RootDemoSequenceNativeSource.h>
#include <Runtime/DemoSceneNative.h>
#include <Runtime/ParamsDemoLookupRuntime.h>
#include <Runtime/RootDemoSoundCommandServices.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class NativeGraphicsRuntimeLibrary;
// 原生Logo/Ending服务owner，无旧现代工厂上游；只借用，不Capture/预建任何场景。
class RootDemoSceneNativeServicesRuntime final
{
public:
    // 生产入口直接借真实VFS并原位拥有SOUND adapter，不要求旧环境代理或测试声音叶。
    RootDemoSceneNativeServicesRuntime(RootRuntimeOwner&,EnvironmentRootOwner&,ParamsRuntimeRootOwner&,
        InputRootOwner&,NativeGraphicsRuntimeLibrary&,IGuiControlResourceLoadServices&,ISpriteNativeResourceSource&,
        IArchiveResourceSource&,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource())noexcept;
    RootDemoSceneNativeServicesRuntime(RootRuntimeOwner&,EnvironmentStartupRuntime&,ParamsRuntimeRootOwner&,
        InputRootOwner&,NativeGraphicsRuntimeLibrary&,IGuiControlResourceLoadServices&,ISpriteNativeResourceSource&,
        IArchiveResourceSource&,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource())noexcept;
    RootDemoSceneNativeServicesRuntime(RootRuntimeOwner&,EnvironmentStartupRuntime&,ParamsRuntimeRootOwner&,
        InputRootOwner&,NativeGraphicsRuntimeLibrary&,IGuiControlResourceLoadServices&,ISpriteNativeResourceSource&,
        IDemoSoundCommandServices&,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource())noexcept;
    RootDemoSceneNativeServicesRuntime(const RootDemoSceneNativeServicesRuntime&)=delete;
    RootDemoSceneNativeServicesRuntime& operator=(const RootDemoSceneNativeServicesRuntime&)=delete;
    RootDemoSceneRuntimeServices& SceneServices()noexcept{return scenes_;}
    DemoSequenceNativeServices& SequenceServices()noexcept{return sequences_;}
    DemoSceneNativeServices& Services()noexcept{return services_;}
private:
    std::optional<RootDemoSoundCommandServices> ownedSounds_;
    RootDemoSceneRuntimeServices scenes_;
    RootDemoSequenceNativeSource source_;
    DemoSequenceNativeServices sequences_;
    ParamsDemoRootSource params_;
    DemoSceneNativeServices services_;
};
}
