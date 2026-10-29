#pragma once
#include <Runtime/SettingPage04Native.h>
#include <Runtime/SettingDialogNativeGlobals.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
// 两份全局CString由宿主持有；每次getter重新取字符槽，不缓存face副本。
class RootSettingPage04NativeSource final : public ISettingPage04NativeSource
{
public:
    explicit RootSettingPage04NativeSource(EnvironmentRootOwner& environment) noexcept
        :RootSettingPage04NativeSource(environment,ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::NameFace),
            ProcessSettingDialogNativeGlobal(SettingDialogGlobalId::SystemFace)){}
    RootSettingPage04NativeSource(EnvironmentRootOwner& environment,
        const RuntimeCString& nameFace, const RuntimeCString& systemFace) noexcept
        : environment_(environment), nameFace_(nameFace), systemFace_(systemFace) {}
    RootSettingPage04NativeSource(EnvironmentRootOwner& environment,
        const RuntimeCString& nameFace, const RuntimeCString& systemFace,
        RootRuntimeOwner& root, ParamsRuntimeRootOwner& params) noexcept
        : RootSettingPage04NativeSource(environment,nameFace,systemFace) { root_=&root;params_=&params; }
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    const char* FontFaceAt61A64C() override;
    const char* FontFaceAt61A650() override;
    GameResourceNamePromptResult RunLocalNamePrompt() override;
private:
    EnvironmentRootOwner& environment_;
    const RuntimeCString& nameFace_;
    const RuntimeCString& systemFace_;
    RootRuntimeOwner* root_ = nullptr;
    ParamsRuntimeRootOwner* params_ = nullptr;
};
// 406BC0/407430的原CString构造段；进程atexit由SettingDialogNativeGlobals登记。
void ConstructSettingNameFontFaceAt406BC0(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ConstructSettingSystemFontFaceAt407430(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
