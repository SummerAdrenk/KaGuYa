#pragma once

#include <Runtime/MainFrameApplicationNative.h>
#include <Runtime/MainFrameTerminalNativeRuntime.h>
#include <Runtime/MainFrameCommandNativeRuntime.h>
#include <Runtime/MainFramePopupNativeRuntime.h>
#include <Runtime/RootMainFrameMenuNativeRuntime.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootApplicationNativeServicesRuntime;
class NativeInputRuntime;
struct RootMainFrameNativeServiceDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeSoundRuntimeLibrary& sound;
    RootApplicationNativeServicesRuntime& application;
    IMainFrameSettingsCommandNative& settings;
    WindowClassModuleNativeBody38h& nativeBase;
    DialogEndpointNativeServices endpoint;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};

// 真实父表/终端/成员菜单/Popup及4342A0消息泵的唯一冷集合。
// 只借长期Root/Scene/Draw；不创建窗口、注册类或提前执行Pre/Post。
class RootMainFrameNativeServicesRuntime final : public IWinMainNativeApplication
{
public:
    explicit RootMainFrameNativeServicesRuntime(RootMainFrameNativeServiceDependencies) noexcept;
    RootMainFrameNativeServicesRuntime(const RootMainFrameNativeServicesRuntime&) = delete;
    RootMainFrameNativeServicesRuntime& operator=(const RootMainFrameNativeServicesRuntime&) = delete;
    int RunApplicationAt4342A0(LPSTR, int) override;
    MainFrameApplicationNativeServices& Application() noexcept { return application_; }
    MainFrameMenuNativeServices& Menu() noexcept { return menu_; }
    MainFramePopupPositionNativeServices& PopupPosition() noexcept { return position_; }
    RootLegacyDisplaySettingsRuntime& LegacyDisplay() noexcept { return legacy_; }
private:
    RootApplicationNativeServicesRuntime& engine_;
    DialogEndpointNativeServices endpoint_;
    Win32LegacyDisplayDeviceApi legacyModes_;
    Win32WindowModePlatformApi legacyPlatform_;
    RootLegacyDisplaySettingsRuntime legacy_;
    RootLegacyDisplayWindowApi legacyWindows_;
    RootMainFrameNativeSource roots_;
    MainFrameNativeModules modules_;
    GeneralConfirmMainFrameCloseNativeDialogs close_;
    RootMainFrameContextMenuNativeSource context_;
    MainFramePopupNativeRuntime popup_;
    RootSoundDeviceRebuildNativeRuntime rebuild_;
    RootMainFrameSoundDeviceNativeSource soundDevice_;
    MainFrameCommandNativeRuntime commands_;
    MainFrameTerminalNativeRuntime terminals_;
    RootMainFramePopupPositionNativeSource positionSource_;
    MainFramePopupPositionNativeServices position_;
    RootMainFrameMenuActionsNativeSource actions_;
    RootScriptRecoveryRuntime recovery_;
    RootAboutDialogNativeSource aboutSource_;
    AboutDialogNativeServices about_;
    RootMainFrameMenuNativeSource menuSource_;
    RootMainFrameMenuNativeCalls menuCalls_;
    MainFrameMenuNativeServices menu_;
    MainFrameNativeDispatchServices dispatch_;
    MainFrameWindowNativeServices window_;
    MainFrameWindowClassNativeInfo54h windowClass_;
    MainFrameWindowClassNativeServices classes_;
    Win32ModalDialogMessageApi messages_;
    MainFrameApplicationNativeServices application_;
};
}
