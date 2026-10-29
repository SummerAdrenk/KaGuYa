#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
namespace
{
int Sub(int x,int y) noexcept {return std::bit_cast<int>(static_cast<unsigned>(x)-static_cast<unsigned>(y));}
void DrawSurface(Surface* source,int x,int y,const RECT* clip)
{
    const int handle=Read(source->handleAt04);
    if(handle)static_cast<void>(RequireSettingPage04NativeServices().drawing.SurfaceDrawRaw(handle,x,y,nullptr,nullptr,clip,0));
}
void BlitSurface(Surface* target,Surface* source,int x,int y,const RECT* clip)
{
    const int destination=Read(target->handleAt04);if(!destination)return;
    const int from=Read(source->handleAt04);
    if(from)static_cast<void>(RequireSettingPage04NativeServices().drawing.SurfaceBlitRaw(destination,from,x,y,nullptr,nullptr,clip,0));
}
}
void __fastcall CombinedDraw(Common* p,void*,const RECT* clip)
{GuiDialogIntoBaseVtable().common.drawAt60(p,clip);GuiDialogBaseVtable().drawAt60(p,clip);}
void __fastcall CombinedComposite(Common* p,void*,Slot owned,const RECT* clip)
{
    RefLife life{owned};
    GuiDialogIntoBaseVtable().common.compositeAt5C(p,Copy(owned),clip);
    GuiDialogBaseVtable().compositeAt5C(p,Copy(owned),clip);life.Normal();
}
void __fastcall NameDraw(Common* p,void*,const RECT* clip)
{
    CombinedDraw(p,nullptr,clip);if(!Read(p->enabledAt38))return;
    auto& slot=Field<Slot>(p,0x7C);const auto base=Read(slot.baseAt00);if(!base)return;
    auto* surface=ResolveBase<Surface>(slot,base);const auto anchor=CurrentSettingOtherProfile().name.center;
    const int x=Sub(anchor.x,Read(surface->widthAt08)>>1),y=Sub(anchor.y,Read(surface->heightAt0C)>>1);
    DrawSurface(Resolve<Surface>(slot),x,y,clip);
}
void __fastcall NameComposite(Common* p,void*,Slot owned,const RECT* clip)
{
    RefLife life{owned};
    GuiDialogIntoBaseVtable().common.compositeAt5C(p,Copy(owned),clip);
    GuiDialogBaseVtable().compositeAt5C(p,Copy(owned),clip);
    if(Read(p->enabledAt38))
    {
        auto& slot=Field<Slot>(p,0x7C);const auto base=Read(slot.baseAt00);
        if(base)
        {
            auto* surface=ResolveBase<Surface>(slot,base);const auto anchor=CurrentSettingOtherProfile().name.center;
            const int x=Sub(anchor.x,Read(surface->widthAt08)>>1),y=Sub(anchor.y,Read(surface->heightAt0C)>>1);
            BlitSurface(Resolve<Surface>(owned),ResolveBase<Surface>(slot,base),x,y,clip);
        }
    }
    life.Normal(); // 52413C/52415A：下游返回后及payload销毁后分别重读control。
}
void __fastcall TextDraw(Common* p,void*,const RECT* clip)
{
    GuiDialogBaseVtable().drawAt60(p,clip);if(!Read(p->enabledAt38))return;
    auto& surfaces=Field<GuiControlSurfaceReferences>(p,0x80);
    for(auto* pos=Read(surfaces.begin);pos!=Read(surfaces.end);++pos)
    {
        auto* current=Resolve<Surface>(*pos);const int x=Read(current->originXAt10),y=Read(current->originYAt14);
        DrawSurface(Resolve<Surface>(*pos),x,y,clip);
    }
}
void __fastcall TextComposite(Common* p,void*,Slot owned,const RECT* clip)
{
    RefLife life{owned};GuiDialogBaseVtable().compositeAt5C(p,Copy(owned),clip);
    if(Read(p->enabledAt38))
    {
        auto& surfaces=Field<GuiControlSurfaceReferences>(p,0x80);
        for(auto* pos=Read(surfaces.begin);pos!=Read(surfaces.end);++pos)
        {
            const auto base=Read(pos->baseAt00);auto* current=ResolveBase<Surface>(*pos,base);
            const int x=Read(current->originXAt10),y=Read(current->originYAt14);
            BlitSurface(Resolve<Surface>(owned),ResolveBase<Surface>(*pos,base),x,y,clip);
        }
    }
    life.Normal(); // 524410正常尾从按值参数槽读control，EH也读槽，不能缓存替代。
}
void __fastcall ParentDraw(Common* p,void*,const RECT* clip)
{
    if(Read(p->enabledAt38)){auto* footer=Resolve<Control>(reinterpret_cast<Parent*>(p)->footerAt78);ControlTable(footer).drawAt0C(footer,clip);}
    GuiDialogIntoBaseVtable().common.drawAt60(p,clip);
}
void __fastcall ParentComposite(Common* p,void*,Slot owned,const RECT* clip)
{
    RefLife life{owned};
    if(Read(p->enabledAt38)){auto* footer=Resolve<Control>(reinterpret_cast<Parent*>(p)->footerAt78);ControlTable(footer).compositeAt10(footer,Copy(owned),clip);}
    GuiDialogIntoBaseVtable().common.compositeAt5C(p,Copy(owned),clip);life.Normal();
}
}
