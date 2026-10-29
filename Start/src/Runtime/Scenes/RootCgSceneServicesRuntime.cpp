#include <Runtime/RootCgSceneServicesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootCgSceneServicesRuntime::RootCgSceneServicesRuntime(RootCgSceneNativeDependencies d,
    SceneManagerNativeServices& manager,IEnvironmentRootMemory& memory,const RuntimeCString& popupName) noexcept
    :RootCgSceneServicesRuntime(d,manager.mode,manager.dialog,memory,popupName){}
RootCgSceneServicesRuntime::RootCgSceneServicesRuntime(RootCgSceneNativeDependencies d,
    ModeSelectButtonNativeServices& publicButtons,OptionCharSelectDialogNativeDependencies& selector,
    IEnvironmentRootMemory& memory,const RuntimeCString& popupName)noexcept
    :source_(d),mode_{publicButtons.button,memory},
     buttons_{mode_,d.environment.RootSource().LockSource(),source_,d.pictures,d.graphics,d.graphics,d.strings,publicButtons,selector},
     display_{d.input.RootSource(),d.input.NativeServices(),d.environment.RootSource(),d.input.NativeServices().cursor,d.graphics},
     dialog_{d.pictures,d.graphics,d.graphics,d.input.RootSource(),d.strings,popupName},
     operation_{dialog_,source_,d.input.RootSource(),d.input.NativeServices()},resources_{source_,d.strings,d.graphics,dialog_},
     scene_{source_,buttons_,display_,operation_,resources_,memory,d.graphics}{}
}
