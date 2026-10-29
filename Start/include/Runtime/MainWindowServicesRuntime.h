#pragma once
#include <Platform/MainWindow.h>
#include <Runtime/EditWindowNativeViewport.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class RootRuntimePayload;
class SaveDataEnvironmentArchive;
class GeneralConfirmationServicesRuntime;
class WindowModeView;

// 生产只借同一根和原54h窗口向量；现代固定载体适配器已隔离到tests。
class MainWindowServicesRuntime final:public Platform::IMainWindowRuntime
{
public:
    MainWindowServicesRuntime(EnvironmentStartupRuntime&,RootRuntimeOwner&,GeneralConfirmationServicesRuntime&,
        EditWindowNativeViewportServices&,EditWindowNativeMapServices&,WindowThunkNativeServices&) noexcept;
    bool QuitRequested() const override;
    bool CloseSuppressed() const override;
    bool CloseConfirmationEnabled() const override;
    INT_PTR ShowCloseConfirmation() override;
    bool FullscreenModeEnabled() const override;
    bool MinimizeWhenDeactivated() const override;
    void SynchronizeSavedNormalPosition() override;
    RECT SavedNormalPosition() const override;
    bool HasResizeTarget() const override;
    void NotifyResizeTarget() override;
    void UpdateVisibleNativeWindowLayout() override;
private:
    RootRuntimePayload& CurrentRootAt413BE0() const;
    SaveDataEnvironmentArchive* CurrentSaveAtEnvironmentSlot1() const;
    WindowModeView CurrentMode() const;
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& owner_;
    GeneralConfirmationServicesRuntime& confirmations_;
    EditWindowNativeViewportServices& viewport_;
    EditWindowNativeMapServices& maps_;
    WindowThunkNativeServices& thunks_;
};
}
