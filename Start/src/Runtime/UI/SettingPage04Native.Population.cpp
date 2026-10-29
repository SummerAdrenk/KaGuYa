#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
void NewManager(Slot& slot)
{auto* raw=::operator new(0x0C);AdoptGuiClickedEventManagerAt4D7B90(slot,raw?::new(raw) GuiClickedEventReferences{}:nullptr);}
void CreateGeneric(Slot& out,Common* parent,unsigned kind,const void* d)
{
    auto& s=RequireSettingPage04NativeServices();out={};RefLife result{out};Fresh typed;
    switch(kind)
    {
    case 0:CreateGuiStaticAt505800(typed.slot,parent,*static_cast<const GuiStaticDescriptorNative24h*>(d),s.loader,s.surfaces,s.strings);break;
    case 1:CreateGuiButtonAt5041D0(typed.slot,parent,*static_cast<const GuiButtonDescriptorNative2Ch*>(d),s.loader,s.surfaces,s.strings);break;
    case 2:{SaveLoadMessageBoxNativeChildFactoriesRuntime factory({s.loader,s.surfaces,s.strings});factory.CreateAt5043B0(&typed.slot,parent,d);break;}
    case 3:CreateGuiSliderAt5052C0(typed.slot,parent,*static_cast<const GuiSliderDescriptorNative6Ch*>(d),s.loader,s.surfaces,s.strings);break;
    default:throw 1;
    }
    RefLife life{typed.slot};ReleaseYtlReference(out);CopyGuiClickedEventReference(out,typed.slot);life.Normal(true);result.active=false;
}
void AppendEventControl(Common& p,Slot& manager,Slot& child,RECT* bounds,bool direct)
{
    RefLife childLife{child};auto* selected=Resolve<Control>(child);
    if(bounds){RECT out;auto* rectangle=ControlTable(selected).rectAt3C(selected,&out);::UnionRect(bounds,bounds,rectangle);}
    Fresh event;
    if(direct)
    {
        // 4E8900/4E9B90先new0C才解child+04，随后调用真实4D7780。
        auto& memory=RequireSettingPage04NativeServices().memory;auto* raw=memory.AllocateBody(0x0C);
        auto* body=raw?::new(raw) GuiClickedEventNativeBody{&GuiClickedEventVtable(),0,Read(Resolve<Control>(child)->eventAt04)}:nullptr;
        ConstructGuiClickedEventReferenceAt4D7780(event.slot,body,1,memory);
    }
    else CreateToolClickedEventAt4E2F5A(event.slot,child);
    RefLife eventLife{event.slot};auto* captured=Read(event.slot.controlAt08);
    Slot argument=Copy(event.slot);
    {
        RefLife life{argument};selected=Resolve<Control>(child);auto& destination=selected->eventAt3C;
        if(&destination!=&argument){ReleaseOld(destination);CopyGuiClickedEventReference(destination,argument);}
        selected->eventEnabledAt48=1;
        if(direct)life.Normal();else life.Captured(captured); // callee536DB0与原内联缓存尾分开。
    }
    if(direct)GuiDialogBaseVtable().appendChildAt10(&p,Copy(child));
    else
    {
        Slot copied=Copy(child);RefLife life{copied};auto* chosen=Read(copied.controlAt08);
        if(p.childrenAt0C.end==p.childrenAt0C.capacity)
        {GrowGuiControlChildrenAt4D93B0(p.childrenAt0C,p.childrenAt0C.end,copied);chosen=Read(copied.controlAt08);}
        else AppendGuiControlChildReference(p.childrenAt0C,copied);
        life.Captured(chosen,true);
    }
    Slot owned{Read(event.slot.baseAt00),Read(event.slot.biasAt04),captured};
    if(captured)++captured->referenceCountAt04; // 原ESI跨child append保存，不重选event control。
    {
        RefLife life{owned};auto* current=captured;
        if(direct){AppendGuiClickedEventReference(*Resolve<GuiClickedEventReferences>(manager),owned);life.Normal();}
        else {AppendPopupClickedEventReferenceAt4EA97F(*Resolve<GuiClickedEventReferences>(manager),owned,current);life.Captured(current,true);}
    }
    eventLife.Captured(captured,!direct);childLife.Normal(!direct);
}
void AppendStaticInline(Common& p,const GuiStaticDescriptorNative24h& d,RECT& bounds)
{
    auto& s=RequireSettingPage04NativeServices();Fresh group;
    LoadGuiControlResourceAt5047A0(group.slot,d.namesAt04.data(),s.loader,s.surfaces,s.strings);RefLife groupLife{group.slot};
    if(!Read(group.slot.baseAt00))throw 1;
    auto* captured=Read(group.slot.controlAt08);
    auto copyGroup=[&]{Slot argument{Read(group.slot.baseAt00),Read(group.slot.biasAt04),captured};if(captured)++captured->referenceCountAt04;return argument;};
    if(auto* point=Read(d.positionAt20))
    {const auto y=Read(point->y),x=Read(point->x);MoveGuiControlResourceAt5086C0(copyGroup(),x,y);}
    auto* raw=s.memory.AllocateBody(0x50);
    struct Raw final {void* p;IEnvironmentRootMemory& m;bool active=true;~Raw() noexcept {if(active)m.DeleteBody(p,0x50);}} cleanup{raw,s.memory};
    auto* body=raw?ConstructGuiStaticAt4F4050(static_cast<Control*>(raw),&p,Read(d.eventAt00),copyGroup()):nullptr;
    cleanup.active=false;Fresh child;ConstructGuiStaticReferenceAt4D7940(child.slot,body,1);RefLife life{child.slot};
    // 与505800不同：inline在rect/append之前释放资源局部Ref，正常尾用旧ESI。
    groupLife.Captured(captured,true);
    auto* selected=Resolve<Control>(child.slot);RECT out;auto* rect=ControlTable(selected).rectAt3C(selected,&out);::UnionRect(&bounds,&bounds,rect);
    Slot copied=Copy(child.slot);
    {
        RefLife copiedLife{copied};auto* c=Read(copied.controlAt08);
        if(p.childrenAt0C.end==p.childrenAt0C.capacity){GrowGuiControlChildrenAt4D93B0(p.childrenAt0C,p.childrenAt0C.end,copied);c=Read(copied.controlAt08);}
        else AppendGuiControlChildReference(p.childrenAt0C,copied);
        copiedLife.Captured(c,true);
    }
    life.Normal(true);
}
int PollManager(Slot& slot)
{
    auto* manager=Resolve<GuiClickedEventReferences>(slot);auto* pos=Read(manager->begin);const auto* end=Read(manager->end);
    for(;pos!=end;++pos)
    {
        const auto base=Read(pos->baseAt00);if(!Read(ResolveBase<GuiClickedEventNativeBody>(*pos,base)->clickedFlagAt04))continue;
        const auto id=Read(ResolveBase<GuiClickedEventNativeBody>(*pos,base)->eventValueAt08);
        auto* e=ResolveBase<GuiClickedEventNativeBody>(*pos,base);Read(e->vptrAt00)->clearAt04(e);return id;
    }
    return -1;
}
int ReadRadio(Common* p)
{
    for(auto* pos=Read(p->childrenAt0C.begin);pos!=Read(p->childrenAt0C.end);++pos)
    {auto* c=Resolve<Control>(*pos);if(!ControlTable(c).readStateAt48(c))return Read(Resolve<Control>(*pos)->eventAt04);}
    return -1;
}
void WriteRadio(Common* p,int id)
{
    for(auto* pos=Read(p->childrenAt0C.begin);pos!=Read(p->childrenAt0C.end);++pos)
    {
        auto* c=Resolve<Control>(*pos);const auto old=ControlTable(c).readStateAt48(c);c=Resolve<Control>(*pos);
        if(Read(c->eventAt04)==id){c=Resolve<Control>(*pos);ControlTable(c).resetAt08(c);c=Resolve<Control>(*pos);ControlTable(c).writeStateAt44(c,0);}
        else {c=Resolve<Control>(*pos);ControlTable(c).writeStateAt44(c,1);}
        c=Resolve<Control>(*pos);
        if(old!=ControlTable(c).readStateAt48(c))
        {c=Resolve<Control>(*pos);auto* fixed=Read(p->vptrAt00);RECT out;auto* r=ControlTable(c).rectAt3C(c,&out);fixed->invalidateAt64(p,*r);}
    }
}
void __fastcall RadioBegin(Common* p,void*)
{ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p,0x74)));Read(p->vptrAt00)->resetChildrenAt68(p);RTable(p).loadAt90(p);}
int __fastcall RadioPoll(Common* p,void*)
{Read(p->vptrAt00)->processInputAt54(p);const auto id=PollManager(Field<Slot>(p,0x74));if(id>=0){WriteRadio(p,id);RTable(p).storeAt94(p);}return 1;}
RECT* __fastcall RadioBounds(Common* p,void*,RECT* out){*out=Field<RECT>(p,0x64);return out;}
Radio* ConstructRadio(void* raw,Common* parent,int id,std::span<const GuiButtonDescriptorNative2Ch> descriptors)
{
    auto* p=::new(raw) Radio;ConstructGuiDialogAt4E0BE0(&p->common,parent,id);p->fieldAt5C=1;p->managerAt74={};p->common.vptrAt00=&RadioTable().common;
    struct Unwind final {Common* p;bool active=true;~Unwind() noexcept {if(active)DestroyRadio(p);}} unwind{&p->common};
    NewManager(p->managerAt74);RECT bounds{};auto& s=RequireSettingPage04NativeServices();
    for(const auto& d:descriptors){Fresh child;CreateGuiButtonAt5041D0(child.slot,&p->common,d,s.loader,s.surfaces,s.strings);AppendEventControl(p->common,p->managerAt74,child.slot,&bounds,false);}
    p->boundsAt64=bounds;unwind.active=false;return p;
}
void AppendCheckbox(Container& parent,const SaveLoadMessageBoxCheckBoxDescriptorNative48h& descriptor,RECT& bounds)
{
    auto& memory=RequireSettingPage04NativeServices().memory;
    auto* raw=memory.AllocateBody(sizeof(Radio));
    if(!raw)throw std::bad_alloc();
    Radio* body=nullptr;
    try
    {
        body=ConstructRadio(raw,&parent.common,descriptor.eventAt00,{});
        body->common.vptrAt00=&CheckboxTable().common;
        Fresh check;CreateGeneric(check.slot,&body->common,2,&descriptor);
        RECT rectangle{};
        AppendEventControl(body->common,body->managerAt74,check.slot,&rectangle,false);
        body->boundsAt64=rectangle;
    }
    catch(...)
    {
        if(body)DestroyRadio(&body->common);
        memory.DeleteBody(raw,sizeof(Radio));throw;
    }
    Fresh child;ConstructDialogRef(child.slot,&body->common,6,sizeof(Radio));RefLife life{child.slot};
    GuiDialogIntoBaseVtable().appendDialogAt8C(&parent.common,Copy(child.slot));
    ::UnionRect(&bounds,&bounds,&body->boundsAt64);
}

void Populate(Container& parent,const SettingOtherBlockProfile& block,Slot* manager,RECT& bounds)
{
    for(const auto& item:block.creationOrder)
    {
        RECT ignored{};auto& target=item.includeInBounds?bounds:ignored;
        switch(item.kind)
        {
        case SettingOtherChildKind::Radio:
        {
            const auto& group=block.radios[item.index];
            AppendRadio(parent,{group.dialogId,group.buttons},target,true);break;
        }
        case SettingOtherChildKind::Checkbox:
            // Old checks use the existing 80h owner; the modern countdown check stays direct.
            if(block.radios.empty()){AppendCheckbox(parent,block.checks[item.index],target);break;}
            [[fallthrough]];
        case SettingOtherChildKind::Button:
        case SettingOtherChildKind::Slider:
        {
            if(!manager)throw std::logic_error("Other event manager is absent");
            Fresh child;
            if(item.kind==SettingOtherChildKind::Checkbox)CreateGeneric(child.slot,&parent.common,2,&block.checks[item.index]);
            else if(item.kind==SettingOtherChildKind::Button)CreateGeneric(child.slot,&parent.common,1,&block.buttons[item.index]);
            else CreateGeneric(child.slot,&parent.common,3,&block.sliders[item.index]);
            AppendEventControl(parent.common,*manager,child.slot,item.includeInBounds?&bounds:nullptr,
                item.kind!=SettingOtherChildKind::Slider);break;
        }
        case SettingOtherChildKind::Static:
            if(block.kind==SettingOtherBlock::Effects)
            {
                if(!manager)throw std::logic_error("Other effect manager is absent");
                Fresh child;CreateGeneric(child.slot,&parent.common,0,&block.statics[item.index]);
                AppendEventControl(parent.common,*manager,child.slot,item.includeInBounds?&bounds:nullptr,false);
            }
            else AppendStaticInline(parent.common,block.statics[item.index],target);
            break;
        }
    }
}
void AppendRadio(Container& parent,const SettingPage04RadioGroup& group,RECT& bounds,bool virtualAppend)
{
    auto& m=RequireSettingPage04NativeServices().memory;auto* raw=m.AllocateBody(0x80);Radio* radio=nullptr;
    {
        struct Raw final{void* p;IEnvironmentRootMemory& m;bool active=true;~Raw() noexcept{if(active)m.DeleteBody(p,0x80);}} cleanup{raw,m};
        if(raw)radio=ConstructRadio(raw,&parent.common,group.id,group.buttons);cleanup.active=false;
    }
    Fresh child;ConstructDialogRef(child.slot,radio?&radio->common:nullptr,6,0x80);RefLife life{child.slot};auto* cached=Read(child.slot.controlAt08);
    if(virtualAppend)GuiDialogIntoBaseVtable().appendDialogAt8C(&parent.common,Copy(child.slot));
    else
    {
        Slot copied=Copy(child.slot);RefLife copiedLife{copied};auto* c=Read(copied.controlAt08);
        const bool grow=parent.dialogsAt5C.end==parent.dialogsAt5C.capacity;
        AppendNestedGuiDialogReference(parent.dialogsAt5C,copied);if(grow)c=Read(copied.controlAt08);copiedLife.Captured(c,true);
    }
    RECT out;auto* selected=virtualAppend?Resolve<Common>(child.slot):&radio->common;
    auto* rectangle=RTable(selected).boundsAt8C(selected,&out);::UnionRect(&bounds,&bounds,rectangle);life.Captured(cached,!virtualAppend);
}
}
