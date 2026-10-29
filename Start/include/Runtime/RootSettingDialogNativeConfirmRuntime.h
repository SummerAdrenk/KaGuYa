#pragma once
#include <Runtime/RootSettingDialogNativeConfirmSource.h>

namespace KaGuYa::Start::Runtime
{
struct RootSettingDialogNativeConfirmDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    GeneralConfirmDialogNativeServices& general;
    NameRegistrationDialogNativeServices& names;
    IModalDialogRunnerServices& modal;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IStartFileApi& files=DefaultNativeSerialize18Files();
    decltype(&::PostMessageA) post=&::PostMessageA;
};
// 服务/来源均在正文外；宿主必须先退役所有84h/94h及逃逸Ref，再销毁本集合。
class RootSettingDialogNativeConfirmRuntime final
{
public:
    explicit RootSettingDialogNativeConfirmRuntime(RootSettingDialogNativeConfirmDependencies) noexcept;
    RootSettingDialogNativeConfirmRuntime(const RootSettingDialogNativeConfirmRuntime&)=delete;
    RootSettingDialogNativeConfirmRuntime& operator=(const RootSettingDialogNativeConfirmRuntime&)=delete;
    SettingDialogNativeConfirmServices& Services() noexcept{return services_;}
private:
    NativeSerialize18Services serialization_;
    RootSettingDialogNativeConfirmSource source_;
    SettingDialogNativeConfirmConstructors constructors_;
    SettingDialogNativeConfirmServices services_;
};
}
