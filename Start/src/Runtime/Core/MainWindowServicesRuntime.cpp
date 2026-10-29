#include <Runtime/MainWindowServicesRuntime.h>
#include <Runtime/DirectMovieRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/GeneralConfirmationServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
MainWindowServicesRuntime::MainWindowServicesRuntime(EnvironmentStartupRuntime& environment,RootRuntimeOwner& owner,
    GeneralConfirmationServicesRuntime& confirmations,EditWindowNativeViewportServices& viewport,
    EditWindowNativeMapServices& maps,WindowThunkNativeServices& thunks) noexcept
    :environment_(environment),owner_(owner),confirmations_(confirmations),viewport_(viewport),maps_(maps),thunks_(thunks){}
RootRuntimePayload& MainWindowServicesRuntime::CurrentRootAt413BE0() const
{owner_.EnsureCreatedAt413BE0();return owner_.RequirePublished();}
bool MainWindowServicesRuntime::QuitRequested() const
{return CurrentRootAt413BE0().ExitRequestedAt1BC();}
bool MainWindowServicesRuntime::CloseSuppressed() const
{return CurrentRootAt413BE0().ModalGateAt1B8()!=0;}
SaveDataEnvironmentArchive* MainWindowServicesRuntime::CurrentSaveAtEnvironmentSlot1() const
{return static_cast<SaveDataEnvironmentArchive*>(environment_.Archives().CaptureArchiveAt4180C0(1));}
bool MainWindowServicesRuntime::CloseConfirmationEnabled() const
{
    // 4237DF先判首次base，之后独立捕获slot1；共享消失保护仍属已登记现代边界。
    if(!CurrentSaveAtEnvironmentSlot1())return false;
    auto* save=CurrentSaveAtEnvironmentSlot1();
    if(!save)throw std::logic_error("MainWindow SaveData disappeared after close gate check");
    return save->bytesAt77[5]!=0;
}
INT_PTR MainWindowServicesRuntime::ShowCloseConfirmation()
{return confirmations_.RunCloseConfirmationAt42380D();}
WindowModeView MainWindowServicesRuntime::CurrentMode() const
{return CurrentRootAt413BE0().WindowModeAt0C();}
bool MainWindowServicesRuntime::FullscreenModeEnabled() const{return CurrentMode().ModeAt04()!=0;}
bool MainWindowServicesRuntime::MinimizeWhenDeactivated() const{return CurrentRootAt413BE0().Frame().ReadMinimizeByteAt1B2()!=0;}
void MainWindowServicesRuntime::SynchronizeSavedNormalPosition(){CurrentMode().SynchronizeSavedNormalPositionAt42116C();}
RECT MainWindowServicesRuntime::SavedNormalPosition() const{return CurrentMode().SavedMonitorRectAt1C();}
bool MainWindowServicesRuntime::HasResizeTarget() const{return CurrentRootAt413BE0().MovieBasePresentAt48();}
void MainWindowServicesRuntime::NotifyResizeTarget()
{
    const auto movie=CurrentRootAt413BE0().MovieAt48();
    if(!movie&&!movie.IsNative())throw std::logic_error("MainWindow resize requires current controlled movie");
    movie.Resize();
}
void MainWindowServicesRuntime::UpdateVisibleNativeWindowLayout()
{
    // 42ECAB先外层同步固定receiver；4875F0内部2+n次重新取根，不替代或合并。
    auto& selected=CurrentRootAt413BE0();
    UpdateEditWindowNativeViewportAt4875F0(selected.EditWindowsAt54(),nullptr,viewport_,maps_,thunks_);
}
}
