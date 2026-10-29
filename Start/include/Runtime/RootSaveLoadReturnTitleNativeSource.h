#pragma once
#include <Runtime/SaveLoadDialogNativeCommands.h>

namespace KaGuYa::Start::Runtime
{
class RootSaveLoadNativeSource;
struct GeneralConfirmDialogNativeServices;
// 所有根/环境/Info读取沿同一个已装配SaveLoad来源，不在构造时Capture。
class RootSaveLoadReturnTitleNativeSource final:public ISaveLoadDialogNativeReturnTitleSource
{
public:
    using PostMessageEntry=BOOL(WINAPI*)(HWND,UINT,WPARAM,LPARAM);
    RootSaveLoadReturnTitleNativeSource(RootSaveLoadNativeSource& root,
        GeneralConfirmDialogNativeServices& confirmation,PostMessageEntry post=&::PostMessageA) noexcept
        :root_(root),confirmation_(confirmation),post_(post){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    GuiDialogNativeBody5Ch* ConstructGeneralConfirmationAt4E3330(void*,std::uint32_t) override;
    void* CaptureInformationAfter413BE0() override;
    IModalDialogRunnerServices& ModalServices() override;
    BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM) override;
    GeneralConfirmDialogNativeServices* NativeGeneralConfirmationServices() const noexcept override;
private:
    RootSaveLoadNativeSource& root_;
    GeneralConfirmDialogNativeServices& confirmation_;
    PostMessageEntry post_;
};
}
