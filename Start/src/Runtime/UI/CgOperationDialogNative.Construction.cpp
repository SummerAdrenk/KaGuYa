#include "CgOperationDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace CgOperationNativeDetail
{
namespace
{
struct ButtonsUnwind final
{
    Buttons& body;
    bool active = true;
    ~ButtonsUnwind() noexcept
    {
        if (!active) return;
        ReleaseYtlReference(body.surfacesAt68[1]);
        ReleaseYtlReference(body.surfacesAt68[0]);
        ReleaseYtlReference(body.eventsAt5C);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
struct PopupUnwind final
{
    Popup& body;
    bool active = true;
    ~PopupUnwind() noexcept
    {
        if (!active) return;
        DestroyGuiControlSurfaceReferencesAt43EAA0(body.pagesAt88);
        ReleaseYtlReference(body.backgroundAt7C);
        ReleaseYtlReference(body.eventsAt5C);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
struct DialogUnwind final
{
    Dialog& body;
    bool active = true;
    ~DialogUnwind() noexcept
    {
        if (!active) return;
        DestroyGuiControlSurfaceReferencesAt43EAA0(body.digitsAt8C);
        DestroyAvailability(body.availabilityAt80);
        DestroyGuiDialogIntoNativeAt4F7B80(&body.into.common);
    }
};
void CreateManager(Slot& slot)
{
    void* raw = ::operator new(0x0C);
    auto* manager = raw ? ::new(raw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(slot, manager);
}
template<class Descriptor>
void CreateTypedChild(Slot& output, Common& parent, const Descriptor& descriptor)
{
    // 5045C0保留fresh输出保护与工厂临时Ref，不跳过那次复制/释放。
    output = {};
    RefGuard outputGuard{output};
    auto& s = Services();
    Slot temporary;
    if constexpr (std::is_same_v<Descriptor, GuiStaticDescriptorNative24h>)
        CreateGuiStaticAt505800(temporary, &parent, descriptor, s.loader, s.surfaces, s.strings);
    else
        CreateGuiButtonAt5041D0(temporary, &parent, descriptor, s.loader, s.surfaces, s.strings);
    RefGuard temporaryGuard{temporary};
    AssignReference(output, temporary, false);
    temporaryGuard.active = false;
    ReleaseGuiControlReferenceAt4F6460(temporary);
    outputGuard.active = false;
}
void AddEventChild(Common& body, Slot& manager, Slot& child, RECT& bounds)
{
    auto* receiver = Resolve<GuiControlNativeBody50h>(child);
    RECT rectangle;
    const auto* source = receiver->vptrAt00->rectAt3C(receiver, &rectangle);
    ::UnionRect(&bounds, &bounds, source);
    Slot event;
    CreateToolClickedEventAt4E2F5A(event, child);
    RefGuard eventGuard{event};
    auto* captured = event.controlAt08;
    Slot binding{event.baseAt00, 0, captured};
    if (captured) ++captured->referenceCountAt04;
    BindPopupGuiControlEventAt4EA856(*Resolve<GuiControlNativeBody50h>(child), binding, event);
    Slot upcast = Copy(child);
    RefGuard upcastGuard{upcast};
    auto* upcastControl = upcast.controlAt08;
    AppendGuiControlChildReference(body.childrenAt0C, upcast);
    upcastGuard.Captured(upcastControl, true);
    Slot eventArgument{event.baseAt00, 0, captured};
    if (captured) ++captured->referenceCountAt04;
    RefGuard argumentGuard{eventArgument};
    AppendGuiClickedEventReference(*Resolve<GuiClickedEventReferences>(manager), eventArgument);
    argumentGuard.Captured(eventArgument.controlAt08, true);
    eventGuard.Captured(captured, true);
}
GuiButtonDescriptorNative2Ch NativeDescriptor(const CgUiButtonRecordNative28h& record,int event)
{
    GuiButtonDescriptorNative2Ch result{};
    result.common.eventAt00=event;
    for(int i=0;i<7;++i)result.common.namesAt04[i]=record.namesAt00[i];
    result.hitSlotAt24=7;
    result.childIdAt28=record.timelineAt24;
    return result; // 旧loader未应用record POINT；自然原点由资源组保留。
}
void CreateNativeCell(Slot& output,Common& parent,int event,POINT position,const Slot& available,const Slot& unavailable)
{
    void* raw=::operator new(sizeof(GuiButtonNativeBody5Ch));
    RawGuard rawGuard{raw,sizeof(GuiButtonNativeBody5Ch)};
    auto* button=ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw),&parent,event,Copy(available));
    rawGuard.active=false;
    struct BodyOwner
    {
        GuiButtonNativeBody5Ch* body;
        bool active=true;
        ~BodyOwner() noexcept {if(active)body->common.vptrAt00->deletingDestructorAt00(&body->common,1);}
    } owner{button};
    ConstructGuiButtonReferenceAt504298(output,button);
    owner.active=false;
    RefGuard outputGuard{output};
    Slot unavailableArgument=Copy(unavailable);
    // 该入口消费增持参数，正常与异常尾都由callee释放，调用方不再挂析构守卫。
    AppendGuiControlResourceArgumentAt4FF1F0(button->common.resourcesAt28,unavailableArgument);
    button->common.vptrAt00=&NativeOperationButtonMethods().common;
    button->common.xAt34=position.x;button->common.yAt38=position.y;
    outputGuard.active=false;
}
}

Slot* CompatibleSurfaceAt444470(Slot& output, Surface& source)
{
    auto& s = Services();
    auto* created = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
    ConstructGraphicsSurfaceReferenceAt451500(output, created, 1);
    RefGuard outputGuard{output};
    const auto width = source.widthAt08;
    const auto height = source.heightAt0C;
    const auto alpha = source.alphaAt18;
    // 44457C独立Reset再SurfaceCreate；不能拿415010替代其不同发布顺序。
    auto* destination = Resolve<Surface>(output);
    destination->ResetAt432670(s.surfaces);
    const auto handle = s.surfaces.CreateSurface(width, height, alpha, 1);
    destination->handleAt04 = handle;
    if (handle)
    {
        destination->widthAt08 = width;
        destination->heightAt0C = height;
        destination->alphaAt18 = alpha;
    }
    else
    {
        // 原4446D8可抛释放，成功才清control/base；bias保留。
        ReleaseYtlReference(output);
    }
    outputGuard.active = false;
    return &output;
}

Buttons* ConstructButtonsAt4DAC80(Buttons& body, Common* parent, std::int32_t id)
{
    const auto& profile=Profile();
    ConstructGuiDialogAt4E0BE0(&body.common, parent, id);
    body.common.vptrAt00 = &ButtonsMethods().common;
    ::new(&body.eventsAt5C) Slot;
    ::new(&body.surfacesAt68[0]) Slot;
    ::new(&body.surfacesAt68[1]) Slot;
    ButtonsUnwind unwind{body};
    CreateManager(body.eventsAt5C);
    RECT bounds{};
    const auto count=NativeVisuals()?profile.nativePrimary.size():profile.primaryButtons.size();
    for (std::size_t i = 0; i != count+1; ++i)
    {
        Slot child;
        if (!i) CreateTypedChild(child, body.common, profile.primaryFrame);
        else if(NativeVisuals())
        {
            // 旧索引0..4按已证动作归一到共享事件3..7，原输入绑定另行保留。
            const auto descriptor=NativeDescriptor(profile.nativePrimary[i-1],static_cast<int>(i)+2);
            CreateTypedChild(child,body.common,descriptor);
        }
        else CreateTypedChild(child, body.common, profile.primaryButtons[i-1]);
        RefGuard childGuard{child};
        if(i&&NativeVisuals())Resolve<GuiControlNativeBody50h>(child)->vptrAt00=&NativeOperationButtonMethods().common;
        AddEventChild(body.common, body.eventsAt5C, child, bounds);
        childGuard.Reload(true);
    }
    body.boundsAt80 = bounds;
    RECT digitBounds{};
    RECT shape = reinterpret_cast<Dialog*>(body.common.parentAt04)->digitBoundsAt98;
    const auto* points = profile.primaryPoints.data();
    for (int i = 0; i != 3; ++i)
    {
        RECT placed = shape;
        placed.right = Add(Sub(points[i].x, shape.left), shape.right);
        placed.left = points[i].x;
        placed.bottom = Add(shape.bottom, Sub(points[i].y, shape.top));
        placed.top = points[i].y;
        ::UnionRect(&digitBounds, &digitBounds, &placed);
    }
    auto& s = Services();
    auto* surface = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
    Slot first;
    ConstructGraphicsSurfaceReferenceAt451500(first, surface, 1);
    RefGuard firstGuard{first};
    auto* captured = first.controlAt08;
    if (!Resolve<Surface>(first)->CreateAt415010(s.surfaces,
        Sub(digitBounds.right, digitBounds.left), Sub(digitBounds.bottom, digitBounds.top), 1, 1)) throw 1;
    auto* receiver = Resolve<Surface>(first);
    receiver->originYAt14 = digitBounds.top;
    receiver->originXAt10 = digitBounds.left;
    AssignReference(body.surfacesAt68[0], first, false);
    Slot second;
    CompatibleSurfaceAt444470(second, *Resolve<Surface>(first));
    RefGuard secondGuard{second};
    AssignReference(body.surfacesAt68[1], second, true);
    secondGuard.Reload();
    receiver = Resolve<Surface>(body.surfacesAt68[1]);
    receiver->originXAt10 = digitBounds.left;
    receiver->originYAt14 = digitBounds.top;
    firstGuard.Captured(captured);
    RECT combined{};
    ::UnionRect(&combined, &digitBounds, &body.boundsAt80);
    body.boundsAt80 = combined;
    unwind.active = false;
    return &body;
}

Popup* ConstructPopupAt4DB860(Popup& body, Common* parent, std::int32_t id)
{
    const auto& profile=Profile();
    ConstructGuiDialogAt4E0BE0(&body.common, parent, id);
    body.common.vptrAt00 = &PopupMethods().common;
    ::new(&body.eventsAt5C) Slot;
    ::new(&body.backgroundAt7C) Slot;
    body.pagesAt88 = {};
    // 共享Reset可能先于Begin；用已有字段建立明确的未选页状态，不读取未初始化值。
    body.pageAt94=-1;body.keyboardLatchAt98=0;
    PopupUnwind unwind{body};
    CreateManager(body.eventsAt5C);
    auto& s = Services();
    Slot frame;
    CreateGuiStaticAt505800(frame, &body.common, profile.popupFrame, s.loader, s.surfaces, s.strings);
    RefGuard frameGuard{frame};
    GuiDialogBaseVtable().appendChildAt10(&body.common, Copy(frame));
    RECT scratch;
    auto* child = Resolve<GuiControlNativeBody50h>(frame);
    RECT bounds = *child->vptrAt00->rectAt3C(child, &scratch);
    if(NativeVisuals())
    {
        const auto& group=RequireGuiControlResource(*child->resourcesAt28.begin);
        const auto image=group.slotIndicesAt00[0];
        if(image<0||image>=GuiControlSurfaceReferenceCount(group.surfacesAt1C))
            throw std::logic_error("Native CG popup background is absent");
        AssignReference(body.backgroundAt7C,group.surfacesAt1C.begin[image],true);
        child->vptrAt00->writeEnabledAt4C(child,0); // 背景已合成到page，不能再叠第二遍alpha。
    }
    Slot page;
    ConstructGraphicsSurfaceReferenceAt451500(page, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings), 1);
    RefGuard pageGuard{page};
    auto* pageControl = page.controlAt08;
    if (!Resolve<Surface>(page)->CreateAt415010(s.surfaces,
        Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top), 1, 1)) throw 1;
    auto* pageBody = Resolve<Surface>(page);
    pageBody->originXAt10 = bounds.left;
    pageBody->originYAt14 = bounds.top;
    if(NativeVisuals())BlitAt4425C0(pageBody,Resolve<Surface>(body.backgroundAt7C),0,0);
    AppendSurface(body.pagesAt88, page);
    Slot group;
    ConstructGuiControlResourceReferenceAt4D76A0(group, NewGuiControlResourceAt5047DD(), 1);
    RefGuard groupGuard{group};
    auto* groupControl = group.controlAt08;
    AppendGuiControlSurfaceArgumentAt538350(*Resolve<GuiControlResourceNativeBody38h>(group), 0, Copy(page));
    Resolve<GuiControlResourceNativeBody38h>(group)->combinedRectAt28 = bounds;
    void* raw = ::operator new(0x50);
    RawGuard rawGuard{raw, 0x50};
    auto* content = raw ? ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(raw),
        &body.common, 9, Copy(group)) : nullptr;
    rawGuard.active = false;
    Slot contentReference;
    ConstructGuiStaticReferenceAt4D7940(contentReference, content, 1);
    RefGuard contentGuard{contentReference};
    GuiDialogBaseVtable().appendChildAt10(&body.common, Copy(contentReference));
    const auto* name = s.popupButtonNameAt61A660.GetString();
    const auto* header = reinterpret_cast<const ATL::CStringData*>(name) - 1;
    if (!NativeVisuals()&&header->nDataLength != 0)
    {
        auto* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        if (!body.backgroundAt7C.baseAt00)
            AdoptGraphicsSurfaceAt407A80(body.backgroundAt7C, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<Surface>(body.backgroundAt7C), name)) throw 1;
    }
    contentGuard.Reload(false, false);
    groupGuard.Captured(groupControl);
    pageGuard.Captured(pageControl);
    frameGuard.Reload();
    Slot nativeGroups[2];
    RefGuard availableGuard{nativeGroups[0]},unavailableGuard{nativeGroups[1]};
    if(NativeVisuals())for(int i=0;i<2;++i)
        LoadGuiControlResourceAt5047A0(nativeGroups[i],profile.nativeCells[i].namesAt00,s.loader,s.surfaces,s.strings);
    for (std::int32_t i = 0; i != profile.pageCapacity; ++i)
    {
        Slot item;
        const auto& points=profile.popupPoints[i];
        POINT position{Sub(points[2].x,profile.cellOffset.x),Sub(points[2].y,profile.cellOffset.y)};
        if(NativeVisuals())CreateNativeCell(item,body.common,ItemEventBase+i,position,nativeGroups[0],nativeGroups[1]);
        else
        {
            auto descriptor=profile.popupCell;
            descriptor.common.eventAt00=ItemEventBase+i;
            descriptor.common.positionAt20=&position;
            CreateGuiButtonAt5041D0(item,&body.common,descriptor,s.loader,s.surfaces,s.strings);
        }
        RefGuard itemGuard{item};
        AddEventChild(body.common, body.eventsAt5C, item, bounds);
        itemGuard.Reload(true);
    }
    for (std::int32_t i = 0; i != 2; ++i)
    {
        Slot item;
        if(NativeVisuals())CreateTypedChild(item,body.common,NativeDescriptor(profile.nativePages[i],PreviousPageEvent+i));
        else CreateTypedChild(item, body.common, profile.pageButtons[i]);
        RefGuard itemGuard{item};
        if(NativeVisuals())Resolve<GuiControlNativeBody50h>(item)->vptrAt00=&NativeOperationButtonMethods().common;
        AddEventChild(body.common, body.eventsAt5C, item, bounds);
        itemGuard.Reload(true);
    }
    body.boundsAt68 = bounds;
    for (const auto& binding:profile.navigation)
    {
        Slot navigation;
        CreateGuiNavigationAt504CE0(navigation, binding.input, binding.mappings.data());
        RefGuard navigationGuard{navigation};
        AppendGuiNavigationReferenceAt4D9D60(body.common.navigationAt24, navigation);
        navigationGuard.Reload(true);
    }
    unwind.active = false;
    return &body;
}
}

GuiDialogNativeBody5Ch* ConstructCgOperationDialogNativeAt4DB3D0(void* raw)
{
    auto* body = ::new(raw) CgOperationDialogNativeBodyA8h;
    return &ConstructCgOperationDialogNativeAt4DB3D0(*body)->into.common;
}
CgOperationDialogNativeBodyA8h* ConstructCgOperationDialogNativeAt4DB3D0(CgOperationDialogNativeBodyA8h& body)
{
    using namespace CgOperationNativeDetail;
    const auto& profile=Profile();
    ValidateCgOperationResourceProfile(profile);
    ConstructGuiDialogIntoNativeAt4E0BE0(&body.into, nullptr, -1);
    body.into.common.vptrAt00 = &CgOperationDialogNativeMethods().common;
    body.availabilityAt80 = {};
    body.digitsAt8C = {};
    DialogUnwind unwind{body};
    RECT digitBounds{};
    for (std::size_t i = 0; i != profile.digitResources.size(); ++i)
    {
        Slot digit;
        RefGuard digitGuard{digit};
        auto& s = Services();
        auto* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        auto* created = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
        AdoptGraphicsSurfaceAt407A80(digit, created);
        auto* captured = digit.controlAt08;
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<Surface>(digit), profile.digitResources[i])) throw 1;
        auto* surface = Resolve<Surface>(digit);
        RECT rectangle{0, 0, surface->widthAt08, surface->heightAt0C};
        bool primary=false;
        for(int use=0;use<2;++use)for(int slot:profile.digitSlots[use])if(slot==static_cast<int>(i))primary=true;
        if(primary)::UnionRect(&digitBounds,&digitBounds,&rectangle);
        AppendSurface(body.digitsAt8C, digit);
        digitGuard.Captured(captured, true);
    }
    body.digitBoundsAt98 = digitBounds;
    RECT bounds{};
    {
        void* raw = ::operator new(0x94);
        RawGuard rawGuard{raw, 0x94};
        auto* buttons = raw ? ConstructButtonsAt4DAC80(*::new(raw) Buttons, &body.into.common, 0) : nullptr;
        rawGuard.active = false;
        Slot reference;
        ConstructButtonsReference(reference, buttons);
        RefGuard referenceGuard{reference};
        auto* captured = reference.controlAt08;
        GuiDialogIntoBaseVtable().appendDialogAt8C(&body.into.common, Copy(reference));
        RECT rectangle;
        auto* base = &buttons->common;
        const auto* table = reinterpret_cast<const Vtable36*>(base->vptrAt00);
        ::UnionRect(&bounds, &bounds, table->boundsAt8C(base, &rectangle));
        referenceGuard.Captured(captured);
    }
    {
        void* raw = ::operator new(0x9C);
        RawGuard rawGuard{raw, 0x9C};
        auto* popup = raw ? ConstructPopupAt4DB860(*::new(raw) Popup, &body.into.common, 1) : nullptr;
        rawGuard.active = false;
        Slot reference;
        ConstructPopupReference(reference, popup);
        RefGuard referenceGuard{reference};
        auto* captured = reference.controlAt08;
        GuiDialogIntoBaseVtable().appendDialogAt8C(&body.into.common, Copy(reference));
        RECT rectangle;
        auto* base = &popup->common;
        const auto* table = reinterpret_cast<const Vtable36*>(base->vptrAt00);
        ::UnionRect(&bounds, &bounds, table->boundsAt8C(base, &rectangle));
        referenceGuard.Captured(captured);
    }
    body.boundsAt68 = bounds;
    AdoptGuiInputAt536EA0(body.into.common.inputAt18, NewGameGuiInputAt4EAAED());
    unwind.active = false;
    return &body;
}
}
