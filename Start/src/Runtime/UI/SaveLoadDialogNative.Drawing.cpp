#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
void DrawSaveLoadDialogNativeAt524800(SaveLoadDialogNativeBody184h& body,const RECT* clip,
    ISaveLoadDialogNativeDrawingSource& source)
{
    if(body.common.enabledAt38)
        source.DrawCapturedAt441980(ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.capturedAt88),clip);
    // 524848即使enabled=0也直接进523370；由该函数自己执行原门控。
    GuiDialogIntoBaseVtable().common.drawAt60(&body.common,clip);
}
void CompositeSaveLoadDialogNativeAt524860(SaveLoadDialogNativeBody184h& body,YtlReferenceSlotRuntime argument,
    const RECT* clip,ISaveLoadDialogNativeDrawingSource& source)
{
    struct Argument
    {
        YtlReferenceSlotRuntime& slot;bool armed=true;
        ~Argument() noexcept{if(armed)ReleaseYtlReference(slot);}
        void Finish() noexcept{armed=false;SaveLoadDialogNativeDetail::ReleaseWithoutClearing(slot);}
    } cleanup{argument};
    if(body.common.enabledAt38)
    {
        // 524892先解析captured，后解析目标；4425C0再先门控目标handle，不能提前读取源handle。
        auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.capturedAt88);
        auto* target=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(argument);
        source.BlitCapturedAt4425C0(target,image,clip);
    }
    YtlReferenceSlotRuntime copied;CopyGraphicsSurfaceReferenceAt47D740(copied,argument);
    GuiDialogIntoBaseVtable().common.compositeAt5C(&body.common,copied,clip);
    cleanup.Finish();
}
void FlushSaveLoadDialogDirtyNativeAt525FB0(SaveLoadDialogNativeBody184h& body,IGuiDialogNativeScreenSource& screen)
{
    auto& common=body.common;
    auto* dirty=common.vptrAt00->dirtyAt00(&common);dirty->vptrAt00->coalesceAt04(dirty);
    dirty=common.vptrAt00->dirtyAt00(&common);
    const auto count=std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if(!count)return;
    dirty=common.vptrAt00->dirtyAt00(&common);auto* rectangles=dirty->vptrAt00->rectsAt1C(dirty);
    for(int i=0;i<count;++i)
    {
        const auto* selected=rectangles+i;
        common.vptrAt00->drawAt60(&common,selected);
        screen.InvalidateRectAt52603A(selected);
    }
    dirty=common.vptrAt00->dirtyAt00(&common);dirty->vptrAt00->clearAt00(dirty);
}
}
