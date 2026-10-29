#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootGeneralConfirmDialogNativeSource::RootGeneralConfirmDialogNativeSource(RootRuntimeOwner& root,
    NativeGraphicsRuntimeLibrary& graphics, const Platform::IInputRuntime& input,
    IGuiControlResourceLoadServices& pictures, INativeScreenCaptureServices& capture,
    IModalDialogRunnerServices& modal, IRuntimeStringManagerSource& strings) noexcept
    : screen_(graphics), input_(input), root_(root, graphics, capture),
      services_{pictures, graphics, graphics, graphics.GuiCalls(), input_, screen_, root_, modal, strings} {}
void RootGeneralConfirmDialogNativeSource::Screen::InvalidateRectAt52603A(const RECT* rectangle)
{ graphics_.InvalidatedRectIfLoaded(rectangle); }
}
