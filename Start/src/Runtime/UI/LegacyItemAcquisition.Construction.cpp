#include "LegacyItemAcquisition.Internal.h"
namespace KaGuYa::Start::Runtime::LegacyItemAcquisitionDetail
{
namespace
{
class PanelControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit PanelControl(Panel* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<Panel*>(payloadAt14);if(p)p->common.vptrAt00->deletingDestructorAt04(&p->common,1);}
    void DeleteControlAtVslot04(unsigned f) override {RetireEnvironmentObjectControl(this,f);}
};
void Load(Slot& out,Top& b,const char* name)
{
    auto& s=*b.services;
    AdoptGraphicsSurfaceAt407A80(out,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
    if(!s.resources.LoadIntoAt4800A0(s.resources.CaptureLoadReceiverAt413BE0(),
        *ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(out),name))throw 1;
}
}
GuiControlNativeBody50h& Child(Base& base,int id)
{
    Ref child;base.vptrAt00->findChildAt14(&base,&child.slot,id);
    auto* c=ResolveGuiControl(child.slot);if(!c)throw std::logic_error("Required item acquisition control is missing");return *c;
}
void AppendControl(Base& parent,GuiClickedEventReferences* manager,Ref& child)
{
    auto* c=ResolveGuiControl(child.slot);if(!c)throw std::logic_error("Item control constructor returned null");
    if(manager)
    {
        Ref event;CreatePopupClickedEventAt4EA7B3(event.slot,c->eventAt04);
        BindPopupGuiControlEventAt4EA856(*c,Copy(event.slot),event.slot);
        AppendGuiClickedEventReference(*manager,event.slot);
    }
    AppendGuiControlChildReference(parent.childrenAt0C,child.slot);
}
void AppendChecks(Panel& p,std::span<const LegacyItemAcquisitionCheck> rows)
{
    auto& s=Services(p);
    for(const auto& row:rows)
    {
        Ref normal,checked,child;
        LoadGuiControlResourceAt5047A0(normal.slot,row.normal.data(),s.resources,s.surfaces,s.strings);
        LoadGuiControlResourceAt5047A0(checked.slot,row.checked.data(),s.resources,s.surfaces,s.strings);
        if(!normal.slot.baseAt00||!checked.slot.baseAt00)throw 1;
        if(row.position)
        {MoveGuiControlResourceAt5086C0(Copy(normal.slot),row.position->x,row.position->y);MoveGuiControlResourceAt5086C0(Copy(checked.slot),row.position->x,row.position->y);}
        struct Owner final
        {
            GuiCheckBoxNativeBody60h* raw=new GuiCheckBoxNativeBody60h;
            bool ready=false;
            ~Owner() noexcept
            {if(raw){if(ready)raw->common.vptrAt00->deletingDestructorAt00(&raw->common,1);else ::operator delete(raw,sizeof(*raw));}}
        } owner;
        auto* c=ConstructGuiCheckBoxAt4E0520(owner.raw,&p.common,row.event,Copy(normal.slot),Copy(checked.slot));
        owner.ready=true;c->common.childIdAt0C=row.child;
        GuiCheckBoxVtable().setHitSlotAt60(&c->common,row.hit);
        ConstructGuiCheckBoxReferenceAt4D7710(child.slot,c,1);owner.raw=nullptr;
        AppendControl(p.common,&p.events,child);
    }
}
void NewPanel(Slot& out,Top& top,Role role,int id)
{
    auto& s=*top.services;Leaves leaves(s);auto p=std::make_unique<Panel>();p->owner=&top;p->role=role;
    ConstructGuiDialogAt4E0BE0(&p->common,&Common(top),id);p->common.vptrAt00=&PanelMethods();
    try
    {
        auto buttons=[&](std::span<const GuiButtonDescriptorNative2Ch> rows){for(const auto& row:rows){Ref child;
            CreateGuiButtonAt5041D0(child.slot,&p->common,row,s.resources,s.surfaces,s.strings);AppendControl(p->common,&p->events,child);}};
        const auto& data=Profile(top);
        if(role==Role::Select)
        {
            buttons(data.items);
            for(const auto& n:data.navigation)CreateAndAppendGuiNavigation(p->common.navigationAt24,n.action,n.pairs);
            AdoptGuiInputAt536EA0(p->common.inputAt18,NewGameGuiInputAt4EAAED());
        }
        else if(role==Role::System)
        {buttons(data.systemBefore);AppendChecks(*p,data.checks);buttons(data.systemAfter);}
        else
        {
            // 原0x50A6A0/0x50AB60：直接借既有Maso/Date业务渲染，不再造计量条或日期UI。
            const bool maso=role==Role::Gauge;const auto rect=s.source.SnapshotBounds(maso);
            AdoptGraphicsSurfaceAt407A80(p->canvas,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
            auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(p->canvas);
            if(!image->CreateAt415010(s.surfaces,rect.right-rect.left,rect.bottom-rect.top,1,1))throw 1;
            image->originXAt10=rect.left;image->originYAt14=rect.top;s.source.SnapshotLayer(maso,*image);
            Ref child;CreateGuiStaticFromSurfaceAt505570(child.slot,&p->common,maso?14:59,Copy(p->canvas),s.surfaces);
            AppendControl(p->common,nullptr,child);
        }
        for(auto* at=p->common.childrenAt0C.begin;at!=p->common.childrenAt0C.end;++at)
        {auto* c=ResolveGuiControl(*at);RECT rect;c->vptrAt00->rectAt3C(c,&rect);::UnionRect(&p->bounds,&p->bounds,&rect);}
        auto control=std::make_unique<PanelControl>(p.get());control->referenceCountAt04=1;control->owningAt08=1;
        control->strideAt0C=sizeof(Panel);control->countAt10=1;
        out={reinterpret_cast<std::uintptr_t>(p.get()),0,control.release()};p.release();
    }
    catch(...){DestroyPanel(*p);throw;}
}
void DestroyPanel(Panel& p) noexcept
{Leaves leaves(Services(p));ReleaseYtlReference(p.canvas);DestroyGuiClickedEventReferencesAt4F6D10(p.events);DestroyGuiDialogAt4F7AC0(&p.common);}
void DestroyTop(Top& b) noexcept
{
    Leaves leaves(*b.services);ReleaseYtlReference(b.canvas);ReleaseYtlReference(b.black);
    DestroyGuiClickedEventReferencesAt4F6D10(b.events);DestroyGuiDialogIntoNativeAt4F7B80(&Common(b));
}
}
namespace KaGuYa::Start::Runtime
{
std::size_t LegacyItemAcquisitionDialogStorageSize() noexcept {return sizeof(LegacyItemAcquisitionDetail::Top);}
GuiDialogNativeBody5Ch* ConstructLegacyItemAcquisitionDialog(void* raw,LegacyItemAcquisitionServices& s)
{
    using namespace LegacyItemAcquisitionDetail;const auto& data=RequireLegacyItemAcquisitionProfile(s.profile);Leaves leaves(s);
    auto* b=::new(raw) Top;b->services=&s;ConstructGuiDialogIntoNativeAt4E0BE0(&b->into,nullptr,-1);
    Common(*b).vptrAt00=&TopMethods().common;
    try
    {
        Ref background,close;
        // 原0x5101A0：time恰为0取昼，其余值均夜，不按18的三档时间clamp。
        CreateGuiStaticAt505800(background.slot,&Common(*b),data.backgrounds[s.source.Read("time")!=0],s.resources,s.surfaces,s.strings);
        AppendControl(Common(*b),&b->events,background);
        CreateGuiButtonAt5041D0(close.slot,&Common(*b),data.close,s.resources,s.surfaces,s.strings);AppendControl(Common(*b),&b->events,close);
        for(auto role:{Role::Select,Role::Gauge,Role::System,Role::Date})
        {
            Ref panel;const int id=role==Role::Select?1:role==Role::Gauge?2:role==Role::System?0:3;
            NewPanel(panel.slot,*b,role,id);auto* p=ResolveEnvironmentObjectReference<Panel>(panel.slot);
            TopMethods().appendDialogAt8C(&Common(*b),Copy(panel.slot));
            if(role==Role::Select)b->select=p;else if(role==Role::Gauge)b->gauge=p;else if(role==Role::System)b->system=p;else b->date=p;
        }
        const auto size=s.source.CanvasSize();b->bounds={0,0,size.cx,size.cy};Load(b->black,*b,data.black);
        AdoptGraphicsSurfaceAt407A80(b->canvas,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
        if(!ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(b->canvas)->CreateAt415010(s.surfaces,size.cx,size.cy,0,1))throw 1;
        AdoptGuiInputAt536EA0(Common(*b).inputAt18,NewGameGuiInputAt4EAAED());return &Common(*b);
    }
    catch(...){DestroyTop(*b);throw;}
}
}
