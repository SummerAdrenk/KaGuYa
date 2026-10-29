#include <Runtime/RootSaveLoadReturnTitleNativeSource.h>
#include <Runtime/RootSaveLoadNativeSource.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSaveLoadReturnTitleNativeSource::CaptureEnvironmentAt4180C0()
{return root_.CaptureEnvironmentAt4180C0();}
GuiDialogNativeBody5Ch* RootSaveLoadReturnTitleNativeSource::ConstructGeneralConfirmationAt4E3330(
    void* raw,std::uint32_t mode)
{
    GeneralConfirmDialogNativeScope scope(confirmation_);
    return ConstructGeneralConfirmDialogNativeAt4E3330(raw,std::bit_cast<std::int32_t>(mode));
}
void* RootSaveLoadReturnTitleNativeSource::CaptureInformationAfter413BE0()
{return root_.CaptureInformationAfter413BE0();}
IModalDialogRunnerServices& RootSaveLoadReturnTitleNativeSource::ModalServices()
{return root_.ModalServices();}
BOOL RootSaveLoadReturnTitleNativeSource::PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)
{return post_(window,message,w,l);}
GeneralConfirmDialogNativeServices* RootSaveLoadReturnTitleNativeSource::NativeGeneralConfirmationServices() const noexcept
{return &confirmation_;}
}
