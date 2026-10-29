#include <Runtime/RootReplaySceneServicesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootReplaySceneServicesRuntime::RootReplaySceneServicesRuntime(RootReplaySceneNativeDependencies d,
    SceneManagerNativeServices& manager,IEnvironmentRootMemory& memory)noexcept
    :RootReplaySceneServicesRuntime(d,manager.mode,manager.dialog,memory){}
RootReplaySceneServicesRuntime::RootReplaySceneServicesRuntime(RootReplaySceneNativeDependencies d,
    ModeSelectButtonNativeServices& publicButtons,OptionCharSelectDialogNativeDependencies& selector,
    IEnvironmentRootMemory& memory)noexcept
    :source_(d),mode_{publicButtons.button,memory},
     buttons_{mode_,d.common.environment.RootSource().LockSource(),source_,d.common.pictures,d.common.graphics,
        d.common.graphics,d.common.strings,publicButtons,selector},
     scripts_({d.common.root,d.common.environment.RootSource(),d.common.graphics,d.execution,d.errors,d.names,d.common.strings},d.resourceNames),
     scene_{source_,buttons_,scripts_,memory,d.common.graphics}{}
}
