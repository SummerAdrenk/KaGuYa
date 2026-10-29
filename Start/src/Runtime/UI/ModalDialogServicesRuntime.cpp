#include <Runtime/ModalDialogServicesRuntime.h>
#include <Runtime/GraphicsPresentNativeApi.h>
#include <Runtime/RootFrameControlRuntime.h>
#include <Runtime/RootLogicalRectangleRuntime.h>
#include <Runtime/WindowInputUpdateRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
bool Win32ModalDialogMessageApi::Peek(MSG& message)
{
    return ::PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE) != FALSE;
}
int Win32ModalDialogMessageApi::Get(MSG& message)
{
    return ::GetMessageA(&message, nullptr, 0, 0);
}
void Win32ModalDialogMessageApi::Translate(const MSG& message)
{
    static_cast<void>(::TranslateMessage(&message));
}
void Win32ModalDialogMessageApi::Dispatch(const MSG& message)
{
    static_cast<void>(::DispatchMessageA(&message));
}

ModalDialogServicesRuntime::ModalDialogServicesRuntime(IGraphicsPresentNativeApi& graphics,
    RootFrameControlRuntime& frame, IRootLogicalRectangleSynchronization& root,
    WindowInputUpdateRuntime& input, IModalDialogMessageApi& messages) noexcept
    : graphics_(graphics), frame_(&frame), root_(root), input_(input), messages_(messages)
{
}
ModalDialogServicesRuntime::ModalDialogServicesRuntime(IGraphicsPresentNativeApi& graphics,
    RootRuntimeOwner& root, WindowInputUpdateRuntime& input, IModalDialogMessageApi& messages) noexcept
    : graphics_(graphics), owner_(&root), root_(root), input_(input), messages_(messages)
{
}
RootFrameControlRuntime& ModalDialogServicesRuntime::Frame() const
{
    return owner_ ? owner_->RequirePublished().Frame() : *frame_;
}
void ModalDialogServicesRuntime::RenderScreenIfGraphicsLoaded()
{
    // 5072E3为空模块即跳过；不能要求Graphics::Initialized或触发新的初始化。
    graphics_.RenderScreenIfLoaded();
}
void ModalDialogServicesRuntime::SynchronizeRuntimeAt413BE0()
{
    root_.SynchronizeRuntimeAt413BE0();
}
bool ModalDialogServicesRuntime::RootOwnerUpdateEnabledAt1B0() const
{
    return Frame().FrameWaitEnabledAt1B0();
}
void ModalDialogServicesRuntime::UpdateRootOwnerAt6CAtVslot18()
{
    Frame().WaitFrameAt43FA60();
}
bool ModalDialogServicesRuntime::PeekMessageNoRemove(MSG& message)
{
    return messages_.Peek(message);
}
int ModalDialogServicesRuntime::GetMessageAt5073B0(MSG& message)
{
    return messages_.Get(message);
}
void ModalDialogServicesRuntime::TranslateMessageAt5073C9(const MSG& message)
{
    messages_.Translate(message);
}
void ModalDialogServicesRuntime::DispatchMessageAt5073D3(const MSG& message)
{
    messages_.Dispatch(message);
}
bool ModalDialogServicesRuntime::ExitRequestedAt1BC() const
{
    return owner_ ? owner_->RequirePublished().ExitRequestedAt1BC() : frame_->ExitRequestedAt1BC();
}
void ModalDialogServicesRuntime::UpdateInputAt450AA0()
{
    input_.UpdateAt450AA0();
}
}
