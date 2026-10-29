#pragma once
#include <Runtime/RootCgSceneNativeSource.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
// 只装配非拥有服务；环境锁直接借唯一environment.RootSource的601160。
// Manager/CG窗口和所有逃逸Ref必须先退役，再销毁此集合及其外部依赖。
class RootCgSceneServicesRuntime final
{
public:
    RootCgSceneServicesRuntime(RootCgSceneNativeDependencies,ModeSelectButtonNativeServices&,
        OptionCharSelectDialogNativeDependencies&,IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        const RuntimeCString& popupName=ProcessCgSceneNativePopupButtonName())noexcept;
    RootCgSceneServicesRuntime(RootCgSceneNativeDependencies d,SceneManagerNativeServices& manager,
        IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        const RuntimeCString& popupName=ProcessCgSceneNativePopupButtonName()) noexcept;
    RootCgSceneServicesRuntime(const RootCgSceneServicesRuntime&)=delete;
    RootCgSceneServicesRuntime& operator=(const RootCgSceneServicesRuntime&)=delete;
    CgSceneNativeServices& Scene() noexcept{return scene_;}
    CgButtonNativeServices& Buttons() noexcept{return buttons_;}
    RootCgSceneNativeSource& Source() noexcept{return source_;}
private:
    RootCgSceneNativeSource source_;
    ModeButtonNativeServices mode_;
    CgButtonNativeServices buttons_;
    CgDisplayWindowNativeServices display_;
    CgOperationDialogNativeServices dialog_;
    CgOperationWindowNativeServices operation_;
    CgDisplayResourceNativeServices resources_;
    CgSceneNativeServices scene_;
};
}
