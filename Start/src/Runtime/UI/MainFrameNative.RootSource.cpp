#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/RootWindowObjectsNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/WindowModeManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootMainFrameNativeSource::RootMainFrameNativeSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
    GraphicsRootOwner& graphics,IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& graphicsWindows,
    EditWindowNativeViewportServices& viewport,EditWindowNativeMapServices& maps,WindowThunkNativeServices& thunks)noexcept
    :root_(root),environment_(environment),graphics_(graphics),strings_(strings),graphicsWindows_(graphicsWindows),
     viewport_(viewport),maps_(maps),thunks_(thunks){}
void RootMainFrameNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFrameNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
EnvironmentRootStorage* RootMainFrameNativeSource::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
YtlReferenceSlotRuntime& RootMainFrameNativeSource::ModeAt0C(void* root)
{return static_cast<RootRuntimePayload*>(root)->WindowModeSlotAt0C();}
RootControlNativeState& RootMainFrameNativeSource::ControlAt1AC(void* root)
{return static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().Native();}
YtlReferenceSlotRuntime& RootMainFrameNativeSource::InformationAt3C(void* root)
{return static_cast<RootRuntimePayload*>(root)->InformationDisplaySlotAt3C();}
void RootMainFrameNativeSource::SynchronizeModeAt42116C(void* root,WindowModeNative2Ch& mode)
{WindowModeView(mode,static_cast<RootRuntimePayload*>(root)->WindowModeServices()).SynchronizeSavedNormalPositionAt42116C();}
DWORD RootMainFrameNativeSource::MovieBaseAt48(void* root)
{return *static_cast<const volatile std::uintptr_t*>(&static_cast<RootRuntimePayload*>(root)->MovieSlotAt48().baseAt00);}
void RootMainFrameNativeSource::ResizeMovieAt48(void* root)
{static_cast<RootRuntimePayload*>(root)->MovieAt48().Resize();}
void RootMainFrameNativeSource::ReleaseWindowObjectsAt4826A0(void* root)
{ReleaseRootWindowObjectsNativeAt4826A0(*static_cast<RootRuntimePayload*>(root));}
void RootMainFrameNativeSource::PaintRootAt483B70(void* root,PAINTSTRUCT paint)
{PaintRootWindowNativeAt483B70(*static_cast<RootRuntimePayload*>(root),paint,graphics_,strings_,graphicsWindows_);}
void RootMainFrameNativeSource::UpdateEditViewportAt4875F0(void* root)
{UpdateEditWindowNativeViewportAt4875F0(static_cast<RootRuntimePayload*>(root)->EditWindowsAt54(),nullptr,viewport_,maps_,thunks_);}
}
