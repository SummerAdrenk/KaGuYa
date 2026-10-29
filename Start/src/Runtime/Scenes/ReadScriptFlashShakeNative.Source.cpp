#include <Runtime/ReadScriptFlashShakeNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptFlashShakeNativeSource::ReadScriptFlashShakeNativeSource(
    IGuiControlResourceLoadServices& loader, IDrawManagerNativeRootSource& draw,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
    IGraphicsSurfaceTransitionApi& capture, IRuntimeStringManagerSource& strings) noexcept
    : loader_(loader), draw_(draw), surfaces_(surfaces), sizes_(sizes), capture_(capture), strings_(strings) {}

bool ReadScriptFlashShakeNativeSource::LoadFlashAt480000(YtlReferenceSlotRuntime& output, const char* name)
{
    GraphicsSurfaceNativeScope scope(surfaces_);
    // 49BD2B..49BD5B：先捕获receiver，480000内部惰建24h/Ref，再由同一receiver加载。
    // 沿用共享480000的空正文ATL拒绝；此现代保护不等同原null解引用的SEH类型。
    void* receiver = loader_.CaptureLoadReceiverAt413BE0();
    return LoadGuiControlSurfaceReferenceAt480000(output, receiver, name, loader_, surfaces_, strings_);
}

YtlReferenceSlotRuntime* ReadScriptFlashShakeNativeSource::CaptureScreenAt4ACDD0(YtlReferenceSlotRuntime& output)
{
    // 4ACDD0不读ECX，但49BDE3/49D568的413BE0及root+18 Ref解析仍须逐次发生。
    (void)draw_.CaptureDrawManagerAfter413BE0();
    CaptureGraphicsScreenAt4ACDD0(output, surfaces_, sizes_, capture_);
    return &output;
}
}
