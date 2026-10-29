#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
struct ConstructionFrame final
{
    Body& body;
    bool active = true;
    ~ConstructionFrame() noexcept
    {
        if (!active) return;
        // 5FB4B4 state4->3->2->1->0；失败不重新安装Main表。
        ReleaseReload(body.calendarAt80, true);
        ReleaseReload(body.partsAt74, true);
        ReleaseReload(body.unusedReferenceAt68, true);
        ReleaseReload(body.eventsAt5C, true);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
template<class Factory> void AppendDescriptor(Body& b, Factory&& factory, bool events)
{
    Slot child;
    RefFrame childFrame{child};
    Slot made;
    factory(made);
    RefFrame madeFrame{made};
    // 4FA010：两个槽独立，factory完成后才取得fresh输出展开资格。
    ReleaseReload(child, true);
    child = Copy(made);
    madeFrame.Reload();
    auto* capturedChild = Read(child.controlAt08);
    if (events && Read(Resolve<GuiControlNativeBody50h>(child)->typeAt08) != 1)
    {
        Slot event;
        CreateToolClickedEventAt4E2F5A(event, child);
        RefFrame eventFrame{event};
        auto* capturedEvent = Read(event.controlAt08);
        Slot binding = Copy(event);
        RefFrame bindingFrame{binding};
        auto* control = Resolve<GuiControlNativeBody50h>(child);
        if (&control->eventAt3C != &binding)
        {
            ReleaseReload(control->eventAt3C, true);
            control->eventAt3C = Copy(event);
        }
        control->eventEnabledAt48 = 1;
        bindingFrame.Reload();
        Slot argument{Read(event.baseAt00), 0, capturedEvent};
        if (capturedEvent) ++capturedEvent->referenceCountAt04;
        RefFrame argumentFrame{argument};
        AppendGuiClickedEventReference(*Resolve<GuiClickedEventReferences>(b.eventsAt5C), argument);
        argumentFrame.Reload();
        eventFrame.Reload();
    }
    Slot argument = Copy(child);
    RefFrame argumentFrame{argument};
    auto* capturedArgument = Read(argument.controlAt08);
    if (Read(b.common.childrenAt0C.end) == Read(b.common.childrenAt0C.capacity))
    {
        GrowGuiControlChildrenAt4D93B0(b.common.childrenAt0C, Read(b.common.childrenAt0C.end), argument);
        capturedArgument = Read(argument.controlAt08);
    }
    else AppendGuiControlChildReference(b.common.childrenAt0C, argument);
    if (events) argumentFrame.Reload();
    else argumentFrame.CapturedReloadUnchecked(capturedArgument);
    // 菜单65/53CB70为正常尾终止态；初始三项24使用缓存control。
    if (events) childFrame.Reload();
    else childFrame.Captured(capturedChild);
}
GuiControlNativeBody50h* ConstructInlineStaticAt4DD4CE(void* raw, Body& b,
    std::int32_t event, Slot& group, Control* capturedGroupControl)
{
    auto* child = static_cast<GuiControlNativeBody50h*>(raw);
    Slot auxiliary{Read(group.baseAt00), 0, capturedGroupControl};
    if (capturedGroupControl) ++capturedGroupControl->referenceCountAt04;
    RefFrame auxiliaryFrame{auxiliary};
    child->childIdAt0C = -1; child->unknownAt10 = 0;
    child->stateAt14 = 1; child->enabledAt18 = 1;
    child->hoverAt1C = 0; child->focusAt20 = 0; child->unknownAt24 = 0;
    child->resourcesAt28 = {}; child->eventAt3C = {};
    child->eventEnabledAt48 = 1; child->parentAt4C = &b.common;
    child->yAt38 = 0; child->xAt34 = 0;
    child->vptrAt00 = &GuiStaticVtable(); child->eventAt04 = event; child->typeAt08 = 1;
    struct BaseFrame final
    {
        GuiControlNativeBody50h* child;
        bool active = true;
        ~BaseFrame() noexcept { if (active) GuiControlBaseVtable().deletingDestructorAt00(child, 0); }
    } baseFrame{child};
    Slot argument{Read(auxiliary.baseAt00), 0, capturedGroupControl};
    if (capturedGroupControl) ++capturedGroupControl->referenceCountAt04;
    AppendGuiControlResourceArgumentAt4FF1F0(child->resourcesAt28, argument);
    auto* selected = reinterpret_cast<GuiControlResourceNativeBody38h*>(Read(group.baseAt00));
    child->xAt34 = Read(selected->combinedRectAt28.left);
    child->yAt38 = Read(selected->combinedRectAt28.top);
    baseFrame.active = false;
    // 4DD5AD..5D0内联正常尾固定EDI；4F4050的原参数尾则重读control，不能混用。
    auxiliaryFrame.Captured(capturedGroupControl);
    return child;
}
void AppendTrainingCanvas(Body& b, std::int32_t index)
{
    Slot group;
    ConstructGuiControlResourceReferenceAt4D76A0(group, NewGuiControlResourceAt5047DD(), 1);
    RefFrame groupFrame{group};
    auto* groupControl = Read(group.controlAt08);
    Slot surface = Copy(Read(Part(b).canvasesAt00.begin)[index]);
    RefFrame surfaceFrame{surface};
    auto* surfaceControl = Read(surface.controlAt08);
    auto* selectedGroup = Resolve<GuiControlResourceNativeBody38h>(group);
    const auto surfaceIndex = Count(selectedGroup->surfacesAt1C);
    AppendGuiControlSurfaceReference(selectedGroup->surfacesAt1C, surface);
    Resolve<GuiControlResourceNativeBody38h>(group)->slotIndicesAt00[0] = surfaceIndex;
    surfaceFrame.Captured(surfaceControl);
    const auto bounds = SurfaceRect(Read(Part(b).canvasesAt00.begin)[index]);
    ::CopyRect(&Resolve<GuiControlResourceNativeBody38h>(group)->combinedRectAt28, &bounds);
    void* raw = ::operator new(0x50);
    GuiControlNativeBody50h* control = nullptr;
    try { if (raw) control = ConstructInlineStaticAt4DD4CE(raw, b, index + 6, group, groupControl); }
    catch (...) { ::operator delete(raw, std::size_t{0x50}); throw; }
    Slot child;
    ConstructGuiStaticReferenceAt4D7940(child, control, 1);
    RefFrame childFrame{child};
    auto* childControl = Read(child.controlAt08);
    Slot argument = Copy(child);
    RefFrame argumentFrame{argument};
    auto* argumentControl = Read(argument.controlAt08);
    if (Read(b.common.childrenAt0C.end) == Read(b.common.childrenAt0C.capacity))
    {
        GrowGuiControlChildrenAt4D93B0(b.common.childrenAt0C, Read(b.common.childrenAt0C.end), argument);
        argumentControl = Read(argument.controlAt08);
    }
    else AppendGuiControlChildReference(b.common.childrenAt0C, argument);
    argumentFrame.Captured(argumentControl);
    auto* archive = BorrowInlineEnvironment(0x4DD6F0, 0x4DD7D5);
    auto* root = Root(0x4DD831, 0x4DD83C);
    const auto weight = ReadNamed(root, archive, Sample18::params[index * 6], 0x4DD851, 0x4DD859, 0x4DD860);
    archive = BorrowEnvironment(0x4DD86B);
    root = Root(0x4DD8C2, 0x4DD8CD);
    const auto stress = ReadNamed(root, archive, Sample18::params[index * 6 + 1], 0x4DD8D9, 0x4DD8E1, 0x4DD8E8);
    archive = BorrowEnvironment(0x4DD8F3);
    root = Root(0x4DD94A, 0x4DD955);
    const auto level = ReadNamed(root, archive, Sample18::params[index * 6 + 2], 0x4DD961, 0x4DD969, 0x4DD970);
    archive = BorrowEnvironment(0x4DD97B);
    root = Root(0x4DD9D2, 0x4DD9DD);
    const auto unusedCount = ReadNamed(root, archive, Sample18::params[index * 6 + 3], 0x4DD9E9, 0x4DD9F1, 0x4DD9F8);
    RenderProgramMain00DialogNativeAt5279C0(b, index, std::uint32_t(stress), level, weight, unusedCount, 0, 0, 0, 0);
    childFrame.Captured(childControl);
    groupFrame.Captured(groupControl);
}
}
ProgramMain00DialogNativeBodyC0h* ConstructProgramMain00DialogNativeAt4DCEB0(void* rawC0,
    GuiDialogNativeBody5Ch* parent, std::int32_t id)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    auto* b = ::new(rawC0) Body;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->common.vptrAt00 = &ProgramMain00DialogNativeVtable();
    ::new(&b->eventsAt5C) Slot;
    ::new(&b->unusedReferenceAt68) Slot;
    ::new(&b->partsAt74) Slot;
    ::new(&b->calendarAt80) Slot;
    ConstructionFrame members{*b};
    void* raw = ::operator new(0x0C);
    auto* manager = raw ? ::new(raw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(b->eventsAt5C, manager);
    auto& s = leaves.services;
    for (const auto& descriptor : Sample18::initial)
        AppendDescriptor(*b, [&](Slot& out) { CreateGuiStaticAt505800(out, &b->common, descriptor, s.loader, s.shared.surfaces, s.shared.strings); }, false);
    raw = ::operator new(0x74);
    Parts* parts = nullptr;
    try { if (raw) parts = ConstructProgramTrainingPartsNativeAt4F55A0(raw); }
    catch (...) { ::operator delete(raw, std::size_t{0x74}); throw; }
    AdoptProgramTrainingPartsNativeAt4DD0E0(b->partsAt74, parts);
    for (std::int32_t index = 0; index < Count(Part(*b).canvasesAt00); ++index) AppendTrainingCanvas(*b, index);
    CalendarProgramMain00DialogNativeAt505920(*b);
    // 608A0C两组的type/count/pointer经PE核对：1/8/604B90，0/31/604710。
    for (const auto& descriptor : Sample18::buttons)
        AppendDescriptor(*b, [&](Slot& out) { CreateGuiButtonAt5041D0(out, &b->common, descriptor, s.loader, s.shared.surfaces, s.shared.strings); }, true);
    for (const auto& descriptor : Sample18::statics)
        AppendDescriptor(*b, [&](Slot& out) { CreateGuiStaticAt505800(out, &b->common, descriptor, s.loader, s.shared.surfaces, s.shared.strings); }, true);
    const GuiDialogNavigationMapping* maps[]{Sample18::navigation0, Sample18::navigation1};
    for (std::int32_t i = 0; i != 2; ++i)
    {
        Slot navigation;
        CreateGuiNavigationAt504CE0(navigation, i + 3, maps[i]);
        RefFrame frame{navigation};
        AppendGuiNavigationReferenceAt4D9D60(b->common.navigationAt24, navigation);
        frame.Reload();
    }
    // 原ctor不写8C/90/94/98..BC，Begin才启动训练状态。
    members.active = false;
    return b;
}
ProgramMain00DialogNativeBodyC0h* ConstructProgramMain00DialogNativeAt4DCEB0(void* rawC0,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramMain00DialogNativeServices& s)
{
    ProgramMain00DialogNativeScope scope{s};
    return ConstructProgramMain00DialogNativeAt4DCEB0(rawC0, parent, id);
}
void DestroyProgramMain00DialogNativeAt4F75B0(ProgramMain00DialogNativeBodyC0h& b) noexcept
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    b.common.vptrAt00 = &ProgramMain00DialogNativeVtable();
    ReleaseReload(b.calendarAt80, true);
    ReleaseReload(b.partsAt74, true);
    ReleaseReload(b.unusedReferenceAt68, true);
    ReleaseReload(b.eventsAt5C, true);
    DestroyGuiDialogAt4F7AC0(&b.common);
}
ProgramMain00DialogNativeBodyC0h* DeleteProgramMain00DialogNativeAt4FC110(
    ProgramMain00DialogNativeBodyC0h* b, std::uint32_t flags) noexcept
{
    DestroyProgramMain00DialogNativeAt4F75B0(*b);
    if (flags & 1u) ::operator delete(b, std::size_t{0xC0});
    return b;
}
}
