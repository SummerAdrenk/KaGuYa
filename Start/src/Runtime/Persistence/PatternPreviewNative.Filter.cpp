#include "PatternPreviewNative.Internal.h"
#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ScrResourceNameSource.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct RefCleanup
{YtlReferenceSlotRuntime& slot;bool active=true;~RefCleanup() noexcept{if(active)ReleaseYtlReference(slot);}};
}
YtlReferenceControlRuntime* FilterPatternPreviewNative(PatternLayerNativeBody80h& pattern,YtlReferenceSlotRuntime& snapshot,
    PatternPreviewNativeServices& services)
{
    const auto mode=std::bit_cast<std::int32_t>(pattern.filterAt40.modeAt04);
    if(mode<=0||mode==10)return snapshot.controlAt08;
    alignas(PatternFilterNativeBody20h) std::byte storage[sizeof(PatternFilterNativeBody20h)];
    auto* filter=CopyConstructPatternFilterNativeAt46F245(storage,pattern.filterAt40,services.memory);
    struct FilterCleanup{PatternFilterNativeBody20h& filter;IEnvironmentRootMemory& memory;~FilterCleanup() noexcept{DestroyPatternFilterNativeAt462FC0(filter,memory);}} filterCleanup{*filter,services.memory};
    YtlReferenceSlotRuntime clone;RefCleanup cloneCleanup{clone};
    {
        YtlReferenceSlotRuntime result;
        CloneGraphicsSurfaceAt444470(result,*ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot),
            mode!=3,services.surfaces,services.filters,services.strings);
        RefCleanup resultCleanup{result};
        CopyAssignGraphicsSurfaceReferenceAt465870(clone,result);
    }
    YtlReferenceControlRuntime* savedClone=nullptr;
    if(mode==6)
    {
        const auto root=services.names.CaptureParamsForNames();
        const auto names=root.CapturePatternNames();
        const auto id=std::bit_cast<std::int32_t>(*filter->argumentsAt08.begin);
        auto name=ArchiveResourceNameAt40B530(names.FindNameAt4AD1B0(id),services.strings);
        const char* chars=name.Value().GetString();
        void* receiver=services.resources.CaptureLoadReceiverAt413BE0();
        const bool loaded=LoadGuiControlSurfaceReferenceAt480000(filter->temporaryAt14,receiver,chars,
            services.resources,services.surfaces,services.strings);
        savedClone=clone.controlAt08; // 46F4A1在判AL、Apply和name Free前固定ESI。
        if(loaded)
            (void)ApplyPatternPixelFilterNativeAt442900(services.filters,*filter,
                ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(clone),nullptr,nullptr);
        name.ReleaseNormalCached(chars);
    }
    else
    {
        savedClone=clone.controlAt08; // 46F326 / 46F587在像素执行器之前固定ESI。
        (void)ApplyPatternPixelFilterNativeAt442900(services.filters,*filter,
            ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(clone),
            mode==3?ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot):nullptr,nullptr);
    }
    // 46F5CF/46F5D7仍在state15：旧快照释放异常展开clone/filter，再进入46F620业务catch。
    ReleasePatternPreviewSavedControlMayThrow(snapshot.controlAt08);
    snapshot.baseAt00=clone.baseAt00;snapshot.biasAt04=clone.biasAt04;snapshot.controlAt08=savedClone;
    if(savedClone)++savedClone->referenceCountAt04;
    // 46F5F0切state23后才是terminate；不能与上面的可抛提交释放共用合同。
    cloneCleanup.active=false;ReleasePatternPreviewSavedControl(savedClone);
    return savedClone;
}
}
