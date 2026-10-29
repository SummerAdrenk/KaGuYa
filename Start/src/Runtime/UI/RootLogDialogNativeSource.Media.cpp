#include <Runtime/RootLogDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>

namespace KaGuYa::Start::Runtime
{
YtlReferenceSlotRuntime* RootLogDialogNativeSource::CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{
    // 原callee不消费Draw ECX；调用方已解析固定Draw Ref，这里不再取根。
    CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;
}
bool RootLogDialogNativeSource::LoadSurfaceAt480000(void* draw,YtlReferenceSlotRuntime& out,const char* name)
{return LoadGuiControlSurfaceReferenceAt480000(out,draw,name,dependencies_.pictures,dependencies_.graphics,dependencies_.strings);}
bool RootLogDialogNativeSource::PlayVoicesAt42C4F0(SoundManagerNativeBody50h* body,const YtlReferenceSlotRuntime* names)
{return dependencies_.root.NativeSoundServices().Voice(body).PlayBatchRawAt42C4F0(names);}
bool RootLogDialogNativeSource::AnyVoiceAt4196C0(SoundManagerNativeBody50h* body)
{return dependencies_.root.NativeSoundServices().Wait(body).AnyVoiceAt4196C0();}
void RootLogDialogNativeSource::AdjustEffectsAt4C2410(SoundManagerNativeBody50h* body)
{dependencies_.root.NativeSoundServices().Text(body).volume.Adjust();}
void RootLogDialogNativeSource::RefreshSoundAt433F60(SoundManagerNativeBody50h* body)
{dependencies_.root.NativeSoundServices().Text(body).volume.Refresh();}
}
