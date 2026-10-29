#include "LogDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
void DrawSurface(GraphicsSurfaceNativeBody* source,GraphicsSurfaceNativeBody* destination,int x,int y,const RECT* clip,bool composite)
{
    auto& api=RequireLogDialogNativeServices().drawing;
    if(composite)
    {
        const auto targetHandle=Read(destination->handleAt04);
        if(!targetHandle)return;
        const auto sourceHandle=Read(source->handleAt04);
        if(sourceHandle)static_cast<void>(api.SurfaceBlitRaw(targetHandle,sourceHandle,x,y,nullptr,nullptr,clip,0));
    }
    else{const auto handle=Read(source->handleAt04);if(handle)static_cast<void>(api.SurfaceDrawRaw(handle,x,y,nullptr,nullptr,clip,0));}
}
void __fastcall DrawText(Common* p,void*,const RECT* clip)
{
    auto& b=*reinterpret_cast<Text*>(p);
    if(Read(p->enabledAt38)&&Read(p->stateAt34))
    {
        for(auto* at=Read(b.rowsAt68.begin);at!=Read(b.rowsAt68.end);++at)
        {
            if(Read(Resolve<Row>(*at)->formatAt10)<0)continue;
            const auto shift=FrameShift();const auto y=Add(Read(Resolve<Row>(*at)->yAt08),shift.y);
            const auto x=Add(Read(Resolve<Row>(*at)->xAt04),shift.x);
            DrawSurface(Resolve<GraphicsSurfaceNativeBody>(b.frameAt94),nullptr,x,y,clip);
            const auto textY=Read(Resolve<Row>(*at)->yAt08),textX=Read(Resolve<Row>(*at)->xAt04);
            DrawSurface(Resolve<GraphicsSurfaceNativeBody>(Resolve<Row>(*at)->surfaceAt24),nullptr,textX,textY,clip);
        }
    }
    GuiDialogBaseVtable().drawAt60(p,clip);
}
void __fastcall CompositeText(Common* p,void*,Slot owned,const RECT* clip)
{
    RefLife life{owned};auto& b=*reinterpret_cast<Text*>(p);
    if(Read(p->enabledAt38)&&Read(p->stateAt34))
    {
        for(auto* at=Read(b.rowsAt68.begin);at!=Read(b.rowsAt68.end);++at)
        {
            if(Read(Resolve<Row>(*at)->formatAt10)<0)continue;
            const auto shift=FrameShift();const auto y=Add(Read(Resolve<Row>(*at)->yAt08),shift.y);
            const auto x=Add(Read(Resolve<Row>(*at)->xAt04),shift.x);
            auto* frame=Resolve<GraphicsSurfaceNativeBody>(b.frameAt94);
            auto* destination=Resolve<GraphicsSurfaceNativeBody>(owned);
            DrawSurface(frame,destination,x,y,clip,true);
            const auto textY=Read(Resolve<Row>(*at)->yAt08),textX=Read(Resolve<Row>(*at)->xAt04);
            auto* surface=Resolve<GraphicsSurfaceNativeBody>(Resolve<Row>(*at)->surfaceAt24);
            destination=Resolve<GraphicsSurfaceNativeBody>(owned);DrawSurface(surface,destination,textX,textY,clip,true);
        }
    }
    GuiDialogBaseVtable().compositeAt5C(p,Copy(owned),clip);life.TerminateNormal(false);
}
}
