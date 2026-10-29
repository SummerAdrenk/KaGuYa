#pragma once

#include <Runtime/MainFrameSettingsNative.h>
#include <Runtime/RootGamePropertyNativeRuntime.h>
#include <Runtime/RootSettingDialogNativeApplyRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct RootMainFrameSettingsServicesDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    NativeSoundRuntimeLibrary& sound;
    RootDrawServicesRuntime& draws;
    IScrVmCommandErrorSink& errors;
    WindowClassModuleNativeBody38h& module;
    DialogEndpointNativeServices endpoint;
    IStartFileApi& messageFiles;
    // 借真实ATL.instanceAt04槽，不与resourceInstanceAt08合并。
    const HMODULE& bitmapResourceModule;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};

// 800D六页属性表的冷服务；不创建Root/AC/E8/33Ch，依赖须覆盖末次使用。
// 调用方与Draw共用来源，并在消息泵期间保持DrawGuiNativeScope。
class RootMainFrameSettingsServicesRuntime final
{
public:
    explicit RootMainFrameSettingsServicesRuntime(RootMainFrameSettingsServicesDependencies) noexcept;
    RootMainFrameSettingsServicesRuntime(const RootMainFrameSettingsServicesRuntime&) = delete;
    RootMainFrameSettingsServicesRuntime& operator=(const RootMainFrameSettingsServicesRuntime&) = delete;
    MainFrameSettingsCommandNativeRuntime& Command() noexcept { return command_; }
    MainFrameSettingsNativeServices& Services() noexcept { return services_; }
private:
    RootSettingDialogNativeApplyRuntime apply_;
    RootGamePropertyNativeRuntime property_;
    SoundManagerNativeVolume volume_;
    RootMainFrameSettingsNativeAccess access_;
    EnvironmentTextWindowSoundSource selectionSound_;
    RootSelectWindowNativeWorkflowSource choices_;
    SelectWindowNativeWorkflowServices selection_;
    MainFrameSettingsNativeServices services_;
    MainFrameSettingsCommandNativeRuntime command_;
};
}
