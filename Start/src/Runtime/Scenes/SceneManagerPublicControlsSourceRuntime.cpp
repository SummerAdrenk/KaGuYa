#include <Runtime/SceneManagerPublicControlsSourceRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
SceneManagerView SceneManagerPublicControlsSourceRuntime::Capture()
{
    if(!root_)return receiver_;
    root_->EnsureCreatedAt413BE0();return root_->Published()->PostWindow().BorrowSceneManagerAt30();
}
CgDisplayPublicSelectorRuntime SceneManagerPublicControlsSourceRuntime::CaptureSelectorAt478A30()
{
    const auto receiver=Capture();
    if(!receiver.IsNative())return receiver.ControlledBody()->AcquireSelectorAt478A30();
    return CgDisplayPublicSelectorRuntime(BorrowOptionCharDialogNativeAt478A30(*receiver.NativeBody()),receiver.NativeServices().dialog);
}
CgDisplayPublicModeButtonsRuntime SceneManagerPublicControlsSourceRuntime::CaptureModesAt47B7B0()
{
    const auto receiver=Capture();
    if(!receiver.IsNative())return receiver.ControlledBody()->AcquireModeButtonsAt47B7B0();
    return CgDisplayPublicModeButtonsRuntime(BorrowModeButtonsNativeAt47B7B0(*receiver.NativeBody()),receiver.NativeServices().mode);
}
}
