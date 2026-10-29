#include "NameRegistrationDialogNative.Internal.h"
#include <Runtime/NameRegistrationResourceProfile.h>
#include <new>
#include <initializer_list>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace NameRegistrationDialogNativeDetail;
// RadioButton与Button的5Ch/29槽同形，只有+54响应选择消息；不是CheckBox60h。
void __fastcall RadioSelect(Child* p, void*, std::int32_t address)
{
    auto* message = reinterpret_cast<const std::int32_t*>(address);
    if (!message || message[0] || !p->vptrAt00->readEnabledAt50(p)) return;
    const auto* selected = reinterpret_cast<const std::int32_t*>(message[1]);
    p->vptrAt00->writeStateAt44(p, *selected != p->eventAt04);
}
const GuiButtonNativeVtable29& RadioTable() noexcept
{
    static const auto t = []
    {
        auto value = GuiButtonVtable();
        value.common.nullAt54 = reinterpret_cast<decltype(value.common.nullAt54)>(&RadioSelect);
        return value;
    }();
    return t;
}
class RadioControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RadioControl(Child* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (auto* p = static_cast<Child*>(payloadAt14)) p->vptrAt00->deletingDestructorAt00(p, 1); }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
static_assert(sizeof(RadioControl) == 0x18);
void CreateRadio(Slot& out, Common* parent, const GuiButtonDescriptorNative2Ch& descriptor,
    NameRegistrationDialogNativeServices& s)
{
    // 505090：group成功前out未写；control失败无已完成5Ch正文回收资格。
    LocalReference result{out, false};
    UnwrittenReference group;
    LoadGuiControlResourceAt5047A0(group.slot, descriptor.common.namesAt04.data(), s.loader, s.surfaces, s.strings);
    LocalReference groupLifetime{group.slot};
    const auto base = group.slot.baseAt00;
    if (!base) throw 1;
    auto* captured = group.slot.controlAt08;
    if (auto* point = descriptor.common.positionAt20)
        MoveGuiControlResourceAt5086C0(Copy(group.slot), point->x, point->y);
    void* raw = ::operator new(0x5C);
    struct Raw final
    { void* p; bool active = true; ~Raw() noexcept { if (active) ::operator delete(p, std::size_t{0x5C}); } } allocation{raw};
    Child* child = nullptr;
    if (raw)
    {
        Slot extra = Copy(group.slot);
        LocalReference extraLifetime{extra};
        Slot argument{base, group.slot.biasAt04, captured};
        if (captured) ++captured->referenceCountAt04;
        child = &ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw), parent,
            descriptor.common.eventAt00, argument)->common;
        child->vptrAt00 = &RadioTable().common;
        child->typeAt08 = 3;
        extraLifetime.Captured(captured, false);
    }
    allocation.active = false;
    out = {};
    if (child)
    {
        out.baseAt00 = reinterpret_cast<std::uintptr_t>(child);
        void* r = ::operator new(0x18);
        auto* control = r ? ::new(r) RadioControl(child) : nullptr;
        out.controlAt08 = control;
        control->referenceCountAt04 = 1;
        out.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x5C;
        static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
    }
    result.active = true;
    ResolveGuiControl(out)->childIdAt0C = descriptor.childIdAt28;
    child = ResolveGuiControl(out);
    reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setHitSlotAt60(child, descriptor.hitSlotAt24);
    groupLifetime.Captured(captured, false);
    result.active = false;
}
void CreateChild(Slot& output, Common* parent, int type, int index, NameRegistrationDialogNativeServices& s)
{
    // 5045C0清fresh返回槽；typed工厂临时Ref仅在成功返回后取得展开资格。
    output = {};
    LocalReference result{output};
    UnwrittenReference temporary;
    Slot* returned = &temporary.slot;
    const auto& profile=CurrentNameRegistrationResourceProfile();
    switch (type)
    {
    case 0: CreateGuiStaticAt505800(temporary.slot, parent, profile.statics[index], s.loader, s.surfaces, s.strings); break;
    case 7: returned = s.source.CreatePanelAt504E90(&temporary.slot, parent, &profile.panel); break;
    case 6: CreateRadio(temporary.slot, parent, profile.radios[index], s); break;
    case 1: CreateGuiButtonAt5041D0(temporary.slot, parent, profile.buttons[index], s.loader, s.surfaces, s.strings); break;
    default: throw 1;
    }
    LocalReference typed{temporary.slot};
    ReleaseYtlReference(output);
    CopyGuiClickedEventReference(output, *returned);
    typed.Reload();
    result.active = false;
}
void Populate(Body& body, int type, int index, NameRegistrationDialogNativeServices& s)
{
    UnwrittenReference child;
    CreateChild(child.slot, &body.common, type, index, s);
    LocalReference childLifetime{child.slot}; // state1 -> state0。
    UnwrittenReference event;
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    LocalReference eventLifetime{event.slot}; // state2：new18失败时尚无此资格。
    auto* eventControl = event.slot.controlAt08;
    Slot binding{event.slot.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    BindPopupGuiControlEventAt4EA856(*ResolveGuiControl(child.slot), binding, event.slot);
    Slot upcast = Copy(child.slot);
    LocalReference upcastLifetime{upcast}; // state5 -> state2；注意与event binding不是同槽。
    auto* control = upcast.controlAt08;
    if (body.common.childrenAt0C.end == body.common.childrenAt0C.capacity)
    {
        GrowGuiControlChildrenAt4D93B0(body.common.childrenAt0C, body.common.childrenAt0C.end, upcast);
        control = upcast.controlAt08;
    }
    else AppendGuiControlChildReference(body.common.childrenAt0C, upcast);
    upcastLifetime.Captured(control);
    // 4D8550和既有4D9620同Ref12/增长/复制发布算法；快路bias=0，增长路回读control。
    AppendPopupClickedEventReferenceAt4EA97F(body.eventsAt74, event.slot, eventControl);
    eventLifetime.Captured(eventControl);
    childLifetime.Reload();
}
using PanelAddId = void (__thiscall*)(Child*, std::int32_t);
using PanelAddNavigation = void (__thiscall*)(Child*, Slot);
template<class T> T PanelSlot(Child* p, std::size_t offset)
{ return Field<T>(const_cast<GuiControlNativeVtable23*>(p->vptrAt00), offset); }
}
Body* ConstructNameRegistrationDialogNativeAt4E4340(void* storage)
{
    (void)CurrentNameRegistrationResourceProfile();
    auto& s = RequireNameRegistrationDialogNativeServices();
    auto* body = static_cast<Body*>(storage);
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->backgroundAt5C = {};
    body->eventsAt74 = {};
    body->common.vptrAt00 = &NameRegistrationDialogNativeVtable().common;
    struct CtorEH final
    { Body* p; bool active = true; ~CtorEH() noexcept { if (active) DestroyNameRegistrationDialogNativeAt4F80D0(p); } } unwind{body};
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    constexpr int types[]{0, 7, 6, 1}, counts[]{4, 1, 2, 3};
    for (int group = 0; group != 4; ++group)
        for (int index = 0; index != counts[group]; ++index) Populate(*body, types[group], index, s);
    UnwrittenReference panel;
    GuiDialogBaseVtable().findChildAt14(&body->common, &panel.slot, 5); // 原直接509D20，不经可覆写14。
    LocalReference selected{panel.slot}; // state9；最后正常尾只用构造后捕获EDI。
    auto* captured = panel.slot.controlAt08;
    if (panel.slot.baseAt00)
    {
        for (int id : {9, 10})
        { auto* child = ResolveGuiControl(panel.slot); PanelSlot<PanelAddId>(child, 0x5C)(child, id); }
        for (int input : {3, 4})
        {
            auto* child = ResolveGuiControl(panel.slot); // 必须在navigation工厂之前固定receiver。
            UnwrittenReference navigation;
            CreateGuiNavigationAt504CE0(navigation.slot, input, NameRegistrationDialogNativeRadioNavigationSample18());
            PanelSlot<PanelAddNavigation>(child, 0x60)(child, navigation.slot); // 按值消费，无caller展开资格。
        }
    }
    selected.Captured(captured, false);
    body->radioEnabledAt80 = 1;
    unwind.active = false;
    return body;
}
}
