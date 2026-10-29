#pragma once
#include <Runtime/RootDemoSceneNativeServicesRuntime.h>
#include <Runtime/DemoSceneNativeProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class NativeGraphicsRuntimeLibrary;
// 长期根宿主集合：不Capture或建立另一个Root/Draw；所有依赖须长于逃逸场景。
class RootDemoSceneProductionRuntime final
{
public:
    RootDemoSceneProductionRuntime(ISceneConstructionServices&,RootRuntimeOwner&,
        EnvironmentStartupRuntime&,ParamsRuntimeRootOwner&,InputRootOwner&,NativeGraphicsRuntimeLibrary&,
        IGuiControlResourceLoadServices&,ISpriteNativeResourceSource&,IDemoSoundCommandServices&,
        IEnvironmentRootMemory& = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    ISceneConstructionServices& Construction() noexcept{return construction_;}
    DemoSequenceNativeServices& SequenceServices() noexcept{return native_.SequenceServices();}
    RootDemoSceneRuntimeServices& SceneServices() noexcept{return native_.SceneServices();}
    DemoSceneNativeServices& NativeSceneServices() noexcept{return native_.Services();}
private:
    RootDemoSceneNativeServicesRuntime native_;
    DemoSceneNativeConstructionServices construction_;
};
}
