#include <Runtime/RootDemoSceneProductionRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootDemoSceneProductionRuntime::RootDemoSceneProductionRuntime(ISceneConstructionServices& upstream,
    RootRuntimeOwner& root,EnvironmentStartupRuntime& environment,ParamsRuntimeRootOwner& params,
    InputRootOwner& input,NativeGraphicsRuntimeLibrary& graphics,IGuiControlResourceLoadServices& pictures,
    ISpriteNativeResourceSource& sprites,IDemoSoundCommandServices& sounds,
    IEnvironmentRootMemory& memory,IRuntimeStringManagerSource& strings) noexcept
    :native_(root,environment,params,input,graphics,pictures,sprites,sounds,memory,strings),
     construction_(upstream,native_.Services()){}
}
