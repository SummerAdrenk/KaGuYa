#include "PatternPreviewNative.Internal.h"
#include <Runtime/GraphicsSurfaceReference.h>
#include <exception>

namespace KaGuYa::Start::Runtime
{
void ReleasePatternPreviewSavedControlMayThrow(YtlReferenceControlRuntime* control)
{
    if(control&&--control->referenceCountAt04==0)
    {if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}
}
void ReleasePatternPreviewSavedControl(YtlReferenceControlRuntime* control) noexcept
{
    try
    {
        ReleasePatternPreviewSavedControlMayThrow(control);
    }
    catch(...){std::terminate();}
}
void SnapshotPatternPreviewNativeAt46EA70(YtlReferenceSlotRuntime& output,
    PatternLayerNativeBody80h& pattern,PatternPreviewNativeServices& services)
{
    GraphicsSurfaceNativeScope surfaces(services.surfaces);
    YtlReferenceSlotRuntime snapshot;
    auto* created=NewGraphicsSurfaceAt40C2E0(services.surfaces,services.strings);
    ConstructGraphicsSurfaceReferenceAt451500(snapshot,created,1);
    YtlReferenceControlRuntime* saved=nullptr;
    struct Unwind{YtlReferenceSlotRuntime& slot;bool active=true;~Unwind() noexcept{if(active)ReleaseYtlReference(slot);}} cleanup{snapshot};
    try
    {
        const SIZE size=services.sizes.ReadScreenSizeAt47CC00();
        auto* main=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(pattern.mainAt18);
        // 46EB1A只定义alpha低byte；其它栈高位没有来源，沿415010的实际byte判定。
        const auto alpha=main->alphaAt18;
        auto* destination=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(snapshot);
        if(!destination->CreateAt415010(services.surfaces,size.cx,size.cy,alpha,1))throw 1;
        RenderPatternPreviewSpritesNative(pattern,snapshot,services);
        saved=FilterPatternPreviewNative(pattern,snapshot,services);
    }
    catch(...){ReleaseYtlReference(snapshot);saved=snapshot.controlAt08;}
    // 46F5DF/46F63F使用跨Filter析构保留的EDI；仅catch/no-filter路径在46F62E回读。
    output.baseAt00=snapshot.baseAt00;output.biasAt04=snapshot.biasAt04;output.controlAt08=saved;
    if(saved)++saved->referenceCountAt04;
    cleanup.active=false;ReleasePatternPreviewSavedControl(saved);
}
}
