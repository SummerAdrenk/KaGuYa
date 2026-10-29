#include <Runtime/CgOperationWindowNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=CgOperationWindowNativeBody30h;
using Base=LayerItemNativeBody14h;
Body& Derived(Base* body) noexcept{return *reinterpret_cast<Body*>(body);}
void __fastcall Draw(Base* b,void*,const RECT* clip){DrawCgOperationWindowNativeAt4B54F0(Derived(b),clip);}
void __fastcall Composite(Base* b,void*,GraphicsSurfaceNativeBody* target,const RECT* clip)
{CompositeCgOperationWindowNativeAt4B5240(Derived(b),target,clip);}
Base* __fastcall Delete(Base* b,void*,std::uint32_t flags) noexcept
{DeleteCgOperationWindowNativeAt4A60D0(Derived(b),flags);return b;}
RECT* __fastcall Bounds(Base* b,void*,RECT* output){*output=Derived(b).window.boundsAt14;return output;}
std::uint8_t __fastcall Request(Base*,void*,std::uint8_t,std::int32_t){return 0;} // 4C1250 xor al,al/ret8。
void __fastcall Advance(Base*,void*){} // 原46B230是C3，不推进Dialog或动画。
void __fastcall Reset(Base* b,void*){b->enabledAt0C=0;} // 4AF1F0原五字节仅写0C。
void __fastcall Invalidate(Base* b,void*,RECT rectangle)
{if(!::IsRectEmpty(&rectangle))b->parentAt10->vptrAt00->addDirtyAt20(b->parentAt10,&rectangle);}
const CgOperationWindowNativeVtable8 table{
    {{reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
      reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
      reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
      reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
      reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
      reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
      reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)},
    reinterpret_cast<decltype(CgOperationWindowNativeVtable8::invalidateAt1C)>(&Invalidate)};
GuiDirtyRectNativeBody80h* Dirty(Body& body)
{
    auto* child=ResolveCgOperationDialogNative(body.dialogAt24);
    auto* common=&child->into.common;
    return common->vptrAt00->dirtyAt00(common);
}
}
const CgOperationWindowNativeVtable8& CgOperationWindowNativeMethods() noexcept{return table;}
void DrawCgOperationWindowNativeAt4B54F0(Body& body,const RECT* clip)
{
    auto* child=ResolveCgOperationDialogNative(body.dialogAt24);
    auto* common=&child->into.common;common->vptrAt00->drawAt60(common,clip);
}
void CompositeCgOperationWindowNativeAt4B5240(Body& body,GraphicsSurfaceNativeBody* target,const RECT* clip)
{
    // ESI receiver在451500分配之前固定；由被调用GUI消费按值Ref，caller不再次释放。
    auto* child=ResolveCgOperationDialogNative(body.dialogAt24);
    auto* common=&child->into.common;
    YtlReferenceSlotRuntime argument;ConstructGraphicsSurfaceReferenceAt451500(argument,target,0);
    common->vptrAt00->compositeAt5C(common,argument,clip);
}
void SynchronizeCgOperationWindowDirtyNativeAt4AB980(Body& body)
{
    auto* dirty=Dirty(body);dirty->vptrAt00->coalesceAt04(dirty);
    dirty=Dirty(body);const auto count=std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if(!count)return;
    dirty=Dirty(body);auto* rectangle=dirty->vptrAt00->rectsAt1C(dirty);
    for(std::int32_t i=0;i<count;++i,++rectangle)
    {
        auto* parent=body.window.layer.parentAt10;
        parent->vptrAt00->addDirtyAt20(parent,rectangle);
    }
    dirty=Dirty(body);dirty->vptrAt00->clearAt00(dirty);
}
void SetCgOperationWindowVisibilityNativeAt4C22B0(Body& body,std::uint8_t value)
{
    body.window.layer.enabledAt0C=value;
    RECT rectangle;
    auto* frame=reinterpret_cast<const LayerItemNativeFrameVtable*>(body.window.layer.vptrAt00);
    frame->boundsAt0C(&body.window.layer,&rectangle);
    auto* parent=body.window.layer.parentAt10;parent->vptrAt00->addDirtyAt20(parent,&rectangle);
}
}
