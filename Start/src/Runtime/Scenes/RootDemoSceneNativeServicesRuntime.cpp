#include <Runtime/RootDemoSceneNativeServicesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootDemoSceneNativeServicesRuntime::RootDemoSceneNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,ParamsRuntimeRootOwner& params,InputRootOwner& input,
    NativeGraphicsRuntimeLibrary& graphics,IGuiControlResourceLoadServices& pictures,
    ISpriteNativeResourceSource& sprites,IArchiveResourceSource& archives,IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)noexcept
    :ownedSounds_(std::in_place,root,environment,archives),scenes_(environment,input,root,graphics),
     source_(environment,root,pictures,sprites,*ownedSounds_,graphics,strings),
     sequences_{source_,graphics,graphics,graphics,graphics,graphics,memory,strings},params_(params),
     services_{scenes_,scenes_,params_,sequences_}{}
RootDemoSceneNativeServicesRuntime::RootDemoSceneNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment,ParamsRuntimeRootOwner& params,InputRootOwner& input,
    NativeGraphicsRuntimeLibrary& graphics,IGuiControlResourceLoadServices& pictures,
    ISpriteNativeResourceSource& sprites,IArchiveResourceSource& archives,IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)noexcept
    :RootDemoSceneNativeServicesRuntime(root,environment.RootSource(),params,input,graphics,pictures,sprites,archives,memory,strings){}
RootDemoSceneNativeServicesRuntime::RootDemoSceneNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentStartupRuntime& environment,ParamsRuntimeRootOwner& params,InputRootOwner& input,
    NativeGraphicsRuntimeLibrary& graphics,IGuiControlResourceLoadServices& pictures,
    ISpriteNativeResourceSource& sprites,IDemoSoundCommandServices& sounds,IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)noexcept
    :scenes_(environment.RootSource(),input,root,graphics),
     source_(environment.RootSource(),root,pictures,sprites,sounds,graphics,strings),
     sequences_{source_,graphics,graphics,graphics,graphics,graphics,memory,strings},params_(params),
     services_{scenes_,scenes_,params_,sequences_}{}
}
