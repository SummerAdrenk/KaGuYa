#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
GraphicsSurfaceNativeBody& Resolve(YtlReferenceSlotRuntime& slot)
{
    auto* body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot);
    // 4ACDD0/4A8340/444470在调用点直接解原Ref；不能把原访问故障转换为logic_error。
    return *body;
}
struct ReturnUnwind final
{
    YtlReferenceSlotRuntime& slot;
    ~ReturnUnwind() noexcept { ReleaseYtlReference(slot); }
};
}

void CaptureGraphicsScreenAt4ACDD0(YtlReferenceSlotRuntime& out,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
    IGraphicsSurfaceTransitionApi& transition)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    auto* created = NewGraphicsSurfaceAt40C2E0(surfaces);
    ConstructGraphicsSurfaceReferenceAt451500(out, created, 1);
    try
    {
        const SIZE size = sizes.ReadScreenSizeAt47CC00();
        if (!Resolve(out).CreateAt415010(surfaces, size.cx, size.cy, false, false))
            ReleaseYtlReference(out);
        else
            // 4ACECE之后重新解析返回Ref，handle读取在Backup调用前。
            transition.BackupScreenToSurfaceIfLoaded(Resolve(out).handleAt04);
    }
    catch (...)
    {
        ReturnUnwind unwind{out};
        throw;
    }
}

void CompositeGraphicsFrameAt4A8340(YtlReferenceSlotRuntime& out,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
    DrawLayersView manager, std::int32_t alpha)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    auto* created = NewGraphicsSurfaceAt40C2E0(surfaces);
    ConstructGraphicsSurfaceReferenceAt451500(out, created, 1);
    try
    {
        const SIZE size = sizes.ReadScreenSizeAt47CC00();
        if (!Resolve(out).CreateAt415010(surfaces, size.cx, size.cy, alpha, false))
            ReleaseYtlReference(out);
        else
            manager.RegisteredLayers().CompositeToSurface(
                GraphicsSurfaceDestination::FromNative(Resolve(out)), nullptr);
    }
    catch (...)
    {
        ReturnUnwind unwind{out};
        throw;
    }
}

void CloneGraphicsSurfaceAt444470(YtlReferenceSlotRuntime& out,
    const GraphicsSurfaceNativeBody& source, std::uint8_t copyPixels,
    IGraphicsSurfaceApi& surfaces, IGraphicsSurfaceRawCopyApi& copy,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    auto* created = NewGraphicsSurfaceAt40C2E0(surfaces, strings);
    // 4444CE..444545：control构造失败不回收已发布正文，完成后才保护返回槽。
    ConstructGraphicsSurfaceReferenceAt451500(out, created, 1);
    try
    {
        const auto width = source.widthAt08;
        const auto height = source.heightAt0C;
        const auto alpha = source.alphaAt18;
        // 44455B只写局部低byte；Graphics:1000FE18仅检查此byte。
        // 此处不复现不确定栈字节；415010的其它完整DWORD入口不受影响。
        if (!Resolve(out).CreateAt415010(surfaces, width, height, alpha, 1))
        {
            ReleaseYtlReference(out);
        }
        else if (copyPixels)
        {
            copy.SurfaceCopyBlitRaw(Resolve(out).handleAt04, source.handleAt04,
                0, 0, nullptr, nullptr, nullptr, 0);
            // 44469E..4446D3：回调可改返回Ref或源origin，不能使用Create前的缓存。
            auto& destination = Resolve(out);
            destination.originXAt10 = source.originXAt10;
            destination.originYAt14 = source.originYAt14;
        }
    }
    catch (...)
    {
        ReturnUnwind unwind{out};
        throw;
    }
}
}
