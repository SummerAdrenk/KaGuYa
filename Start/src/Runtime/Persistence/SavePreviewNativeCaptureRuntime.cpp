#include <Runtime/SavePreviewNativeCaptureRuntime.h>
#include <Runtime/ParamsGameSystemRuntime.h>
#include <exception>

namespace KaGuYa::Start::Runtime
{
void SavePreviewNativeCaptureRuntime::CaptureIntoAt48DBE0(YtlReferenceSlotRuntime& targetAt1E4)
{
    GraphicsSurfaceNativeScope scope(services_.surfaces);
    const SIZE size=services_.sizes.ReadScreenSizeAt47CC00();
    auto* created=NewGraphicsSurfaceAt40C2E0(services_.surfaces,services_.strings);
    AdoptGraphicsSurfaceAt407A80(targetAt1E4,created);
    auto* target=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
    if(!target->CreateAt415010(services_.surfaces,size.cx,size.cy,0,1))throw 2;
    const auto initial=source_.CaptureDrawLayersAt413BE0();
    const auto window=initial.WindowZero();
    for(std::uint32_t index=0;;++index)
    {
        const auto* game=source_.CaptureGameSystemAt418480();
        if(index>=static_cast<std::uint32_t>(game->startupConfigBytes.size()))break;
        // 48DDDF unsigned count<index才跳过；等于count继续读物理Ref，不裁到较小数组。
        if(window.PatternLayerCount()<index)continue;
        const auto pattern=window.PatternAt4AD2A0(index);
        if(!pattern||!pattern.EnabledByteAt0C())continue;
        YtlReferenceSlotRuntime snapshot;
        SnapshotPatternPreviewNativeAt46EA70(snapshot,pattern.NativeBody(),services_);
        struct Unwind{YtlReferenceSlotRuntime& slot;bool active=true;~Unwind() noexcept{if(active)ReleaseYtlReference(slot);}} cleanup{snapshot};
        if(auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot))
        {
            // 4425C0目标句柄先读，目标0时不读源+04；参数全部为原零值。
            target=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
            const auto destination=target->handleAt04;
            if(destination)
            {
                const auto source=image->handleAt04;
                if(source)(void)services_.drawing.SurfaceBlitRaw(destination,source,0,0,nullptr,nullptr,nullptr,0);
            }
        }
        // 48DEA3在payload返回后重读当前local control，不套用46EA70缓存control的正常尾。
        cleanup.active=false;
        try{ReleaseYtlReference(snapshot);}catch(...){std::terminate();}
    }
    for(std::uint32_t index=1;;++index)
    {
        // 48DED6读数量，48DF24另取元素；回调换manager时不能共享一次捕获。
        const auto count=source_.CaptureDrawLayersAt413BE0();
        if(index>=count.OwnedLayerCount())break;
        const auto current=source_.CaptureDrawLayersAt413BE0();
        const auto layer=current.LayerAt418C10(index);
        if(!layer||!layer.EnabledByteAt0C())continue;
        target=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(targetAt1E4);
        layer.CompositeToSurface(target?GraphicsSurfaceDestination::FromNative(*target):GraphicsSurfaceDestination(0),nullptr);
    }
}
}
