#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
std::int32_t ReadRadio(Common* p)
{
    for (auto* pos = Read(p->childrenAt0C.begin); pos != Read(p->childrenAt0C.end); ++pos)
    {
        auto* c = Resolve<Control>(*pos);
        if (!CTable(c).readStateAt48(c)) return Read(Resolve<Control>(*pos)->eventAt04);
    }
    return -1;
}
void WriteRadio(Common* p, std::int32_t id)
{
    for (auto* pos = Read(p->childrenAt0C.begin); pos != Read(p->childrenAt0C.end); ++pos)
    {
        auto* c = Resolve<Control>(*pos); const auto before = CTable(c).readStateAt48(c);
        c = Resolve<Control>(*pos);
        if (Read(c->eventAt04) == id)
        {
            c = Resolve<Control>(*pos); CTable(c).resetAt08(c);
            c = Resolve<Control>(*pos); CTable(c).writeStateAt44(c, 0);
        }
        else { c = Resolve<Control>(*pos); CTable(c).writeStateAt44(c, 1); }
        c = Resolve<Control>(*pos);
        if (before != CTable(c).readStateAt48(c))
        {
            c = Resolve<Control>(*pos); auto* fixed = Read(p->vptrAt00);
            RECT out; auto* returned = CTable(c).rectAt3C(c, &out); fixed->invalidateAt64(p, *returned);
        }
    }
}
void __fastcall Begin03(Common* p, void*)
{ GuiDialogIntoBaseVtable().common.beginAt44(p); ContainerTable(p).loadAt98(p); }
std::int32_t __fastcall End03(Common* p, void*) { ContainerTable(p).storeAt9C(p); return 1; }
RECT* __fastcall Bounds03(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x6C); return out; }
void __fastcall Load03(Common* p, void*)
{
    const auto& controls = CurrentSettingPage03Profile().loop;
    Fresh f; Into(p).findDialogAt90(p, &f.slot, controls.radioGroup); RefLife life{f.slot};
    if (Read(f.slot.baseAt00))
    {
        auto* selected = Resolve<Common>(f.slot); const bool loop = Read(Field<std::uint8_t>(Registry1(), 0x81)) != 0;
        WriteRadio(selected, loop ? controls.yes : controls.no);
    }
    life.Normal(); Field<int>(p, 0x68) = 0;
}
void __fastcall Store03(Common* p, void*)
{
    if (!Read(Field<int>(p, 0x68)))
    {
        const auto& controls = CurrentSettingPage03Profile().loop;
        Fresh f; Into(p).findDialogAt90(p, &f.slot, controls.radioGroup); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        {
            const bool loop = ReadRadio(Resolve<Common>(f.slot)) == controls.yes;
            Field<std::uint8_t>(Registry1(), 0x81) = static_cast<std::uint8_t>(loop);
        }
        life.Normal();
    }
    Field<int>(p, 0x68) = 1;
}
void __fastcall Gate03(Common* p, void*, std::int32_t enabled, std::int32_t redraw)
{
    const auto& controls = CurrentSettingPage03Profile().loop;
    Fresh radio; Into(p).findDialogAt90(p, &radio.slot, controls.radioGroup);
    {
        RefLife life{radio.slot};
        if (Read(radio.slot.baseAt00))
        {
            auto* selected = Resolve<Common>(radio.slot); Read(selected->vptrAt00)->writeEnabledAt24(selected, enabled != 0);
            if (redraw)
            {
                selected = Resolve<Common>(radio.slot); auto* fixed = Read(p->vptrAt00);
                RECT out; auto* returned = Table(selected).boundsAt8C(selected, &out); fixed->invalidateAt64(p, *returned);
            }
        }
        life.Normal();
    }
    Fresh label; Read(p->vptrAt00)->findChildAt14(p, &label.slot, controls.label->eventAt00); RefLife life{label.slot};
    if (Read(label.slot.baseAt00))
    {
        auto* c = Resolve<Control>(label.slot); CTable(c).resetAt08(c);
        c = Resolve<Control>(label.slot); CTable(c).writeStateAt44(c, enabled != 0);
        if (redraw)
        {
            c = Resolve<Control>(label.slot); auto* fixed = Read(p->vptrAt00);
            RECT out; auto* returned = CTable(c).rectAt3C(c, &out); fixed->invalidateAt64(p, *returned);
        }
    }
    life.Normal();
}
void __fastcall Draw03(Common* p, void*, const RECT* clip)
{ GuiDialogBaseVtable().drawAt60(p, clip); GuiDialogIntoBaseVtable().common.drawAt60(p, clip); }
void __fastcall Composite03(Common* p, void*, Slot owned, const RECT* clip)
{
    RefLife life{owned};
    GuiDialogBaseVtable().compositeAt5C(p, Copy(owned), clip);
    GuiDialogIntoBaseVtable().common.compositeAt5C(p, Copy(owned), clip);
    life.Normal(); // 523E6B/523E89：两次重读参数control，不能使用入口缓存。
}
namespace
{
void Visible(Common* p, int id, bool visible)
{
    Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, id); RefLife life{f.slot};
    if (Read(f.slot.baseAt00))
    { auto* c = Resolve<Control>(f.slot); CTable(c).writeEnabledAt4C(c, visible); }
    life.Normal(true);
}
}
void SelectCharacterPageAt503090(Common* p, std::int32_t page, std::int32_t redraw)
{
    // Mamatomo仅9项；11/15原表有第二页，使用本款完整四元组而非截取前9项。
    const auto& pager = CurrentSettingPage03Profile().pager;
    const auto rows = pager.rows;
    for (std::size_t i = 0; i != rows.size(); ++i)
    {
        const bool visible = page == 0 ? i < static_cast<std::size_t>(pager.split)
            : page == 1 ? i >= static_cast<std::size_t>(pager.split) : true;
        for (int id : rows[i]) Visible(p, id, visible);
    }
    if (redraw) Read(p->vptrAt00)->invalidateAt64(p, Field<RECT>(p, 0x64));
}
void SelectPageIndicatorAt503320(Common* p, std::int32_t page, std::int32_t redraw)
{
    const auto& pager = CurrentSettingPage03Profile().pager;
    Visible(p, pager.indicators[0], page == 0); Visible(p, pager.indicators[1], page == 1);
    Visible(p, pager.buttons[0], page == 0); Visible(p, pager.buttons[1], page == 1);
    if (redraw) Read(p->vptrAt00)->invalidateAt64(p, Field<RECT>(p, 0x64));
    Field<int>(p, 0x80) = page;
}
}
