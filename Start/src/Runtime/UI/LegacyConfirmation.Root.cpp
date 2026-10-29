#include <Runtime/LegacyConfirmation.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
void CaptureLegacyConfirmationScreen(YtlReferenceSlotRuntime& output, IGraphicsSurfaceApi& surfaces,
    IGraphicsScreenSizeApi& sizes, IGraphicsSurfaceTransitionApi& transition,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    YtlReferenceSlotRuntime local;
    auto* created = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
    ConstructGraphicsSurfaceReferenceAt451500(local, created, 1);
    struct Lifetime final
    {
        YtlReferenceSlotRuntime& slot;
        bool armed = true;
        ~Lifetime() noexcept { if (armed) ReleaseYtlReference(slot); }
    } lifetime{local};
    const auto size = sizes.ReadScreenSizeAt47CC00();
    auto* surface = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(local);
    // 4C9190/4161A0：alpha=0,clear=0；失败空返回，不改成捕获上一帧。
    if (surface->CreateAt415010(surfaces, size.cx, size.cy, 0, 0))
        transition.BackupScreenToSurfaceIfLoaded(
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(local)->handleAt04);
    else ReleaseYtlReference(local);
    CopyGraphicsSurfaceReferenceAt47D740(output, local);
    lifetime.armed = false;
    try { ReleaseYtlReference(local); } // 4C928C：返回槽发布后才释放本地Ref。
    catch (...)
    {
        // FuncInfo559C54 state0/5205B3：正常本地退役抛出时，返回槽仍有展开资格。
        Lifetime resultUnwind{output};
        throw;
    }
}

LegacyConfirmationRootSource::LegacyConfirmationRootSource(RootRuntimeOwner& root,
    NativeGraphicsRuntimeLibrary& graphics, const Platform::IInputRuntime& input,
    IGuiControlResourceLoadServices& pictures, IQuickOperationsNativeSource& shared,
    IRuntimeStringManagerSource& strings) noexcept
    : root_(root), graphics_(graphics), input_(input),
      services_{pictures, graphics, graphics, graphics.GuiCalls(), input_, *this, *this, shared, strings} {}

GraphicsSurfaceReference LegacyConfirmationRootSource::CaptureScreenNativeAt4ACDD0()
{
    root_.EnsureCreatedAt413BE0(); // Baku 416AD0 -> 416940，当次根的DrawManager。
    static_cast<void>(ResolveEnvironmentObjectReference<void>(root_.RequirePublished().DrawManagerSlotAt18()));
    // 4C9190不读receiver字段，不能提前拒绝空DrawManager。
    GraphicsSurfaceReference result(graphics_);
    CaptureLegacyConfirmationScreen(result.Slot(), graphics_, graphics_, graphics_, services_.strings);
    return result;
}
void LegacyConfirmationRootSource::InvalidateRectAt52603A(const RECT* rectangle)
{ graphics_.InvalidatedRectIfLoaded(rectangle); } // Baku 415870 -> 43B910。
}
