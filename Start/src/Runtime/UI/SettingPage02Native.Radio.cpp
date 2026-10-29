#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage02NativeDetail;
std::int32_t ReadSettingRadioChoiceNativeAt50ADE0(Common& p)
{
    for (auto* position = Read(p.childrenAt0C.begin); position != Read(p.childrenAt0C.end); ++position)
    {
        auto* child = Resolve<GuiControlNativeBody50h>(*position);
        if (!Read(child->vptrAt00)->readStateAt48(child))
        { child = Resolve<GuiControlNativeBody50h>(*position); return Read(child->eventAt04); }
    }
    return -1;
}
void WriteSettingRadioChoiceNativeAt537B10(Common& p, std::int32_t value)
{
    for (auto* position = Read(p.childrenAt0C.begin); position != Read(p.childrenAt0C.end); ++position)
    {
        auto* child = Resolve<GuiControlNativeBody50h>(*position);
        const auto before = Read(child->vptrAt00)->readStateAt48(child);
        child = Resolve<GuiControlNativeBody50h>(*position);
        if (Read(child->eventAt04) == value)
        {
            child = Resolve<GuiControlNativeBody50h>(*position); Read(child->vptrAt00)->resetAt08(child);
            child = Resolve<GuiControlNativeBody50h>(*position); Read(child->vptrAt00)->writeStateAt44(child, 0);
        }
        else { child = Resolve<GuiControlNativeBody50h>(*position); Read(child->vptrAt00)->writeStateAt44(child, 1); }
        child = Resolve<GuiControlNativeBody50h>(*position);
        if (before != Read(child->vptrAt00)->readStateAt48(child))
        {
            child = Resolve<GuiControlNativeBody50h>(*position);
            const auto* table = Read(p.vptrAt00); // 原先固定父表，再调用子rect。
            RECT rect; const auto* returned = Read(child->vptrAt00)->rectAt3C(child, &rect);
            table->invalidateAt64(&p, *returned);
        }
    }
}
namespace SettingPage02NativeDetail
{
std::int32_t PollChoiceManager(Slot& slot)
{
    auto* manager = Resolve<GuiClickedEventReferences>(slot);
    auto* position = Read(manager->begin); const auto* end = Read(manager->end);
    for (; position != end; ++position)
    {
        const auto base = Read(position->baseAt00);
        if (!Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->eventValueAt08);
        auto* clicked = ResolveBase<GuiClickedEventNativeBody>(*position, base);
        Read(clicked->vptrAt00)->clearAt04(clicked); return value;
    }
    return -1;
}
void __fastcall BeginContainer(Common* p, void*)
{
    GuiDialogIntoBaseVtable().common.beginAt44(p); // 直接50C900之后才虚98读取配置。
    CTable(p).loadAt98(p);
}
std::int32_t __fastcall EndContainer(Common* p, void*) { CTable(p).storeAt9C(p); return 1; }
RECT* __fastcall ContainerBounds(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x6C); return out; }
void __fastcall BeginChoice(Common* p, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p, 0x74)));
    Read(p->vptrAt00)->resetChildrenAt68(p); QTable(p).loadAt90(p);
}
std::int32_t __fastcall PollChoice(Common* p, void*)
{
    Read(p->vptrAt00)->processInputAt54(p);
    const auto value = PollChoiceManager(Field<Slot>(p, 0x74));
    if (value >= 0) { WriteSettingRadioChoiceNativeAt537B10(*p, value); QTable(p).storeAt94(p); }
    return 1;
}
std::int32_t __fastcall EndChoice(Common*, void*) { return 1; } // 508BD0/509100原真返回1。
RECT* __fastcall ChoiceBounds(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x64); return out; }
}
}
