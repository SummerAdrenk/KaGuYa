#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
const SettingTextBoolRow& CheckboxRow(Common& p)
{
    for (const auto rows : CurrentSettingTextDisplayProfile().booleanGroups)
        for (const auto& row : rows)
            if (row.id == p.idAt08 && row.checkbox) return row;
    throw std::logic_error("Checkbox settings row is not in the game profile");
}
const SettingTextChoiceRow& ChoiceRow(Common& p)
{
    const auto& profile = CurrentSettingTextDisplayProfile();
    if (profile.rightClick.id == p.idAt08) return profile.rightClick;
    for (const auto& row : profile.sceneChoices)
        if (row.id == p.idAt08) return row;
    throw std::logic_error("Settings choice is not in the game profile");
}
GuiCheckBoxNativeBody60h* Checkbox(Common& p, const SettingTextBoolRow& row, Slot& out)
{
    Read(p.vptrAt00)->findChildAt14(&p, &out, row.checkbox->eventAt00);
    if (!Read(out.baseAt00)) throw std::logic_error("Required settings checkbox is missing");
    return Resolve<GuiCheckBoxNativeBody60h>(out);
}
std::int32_t __fastcall PollCheckbox(Common* p, void*)
{
    const auto& row = CheckboxRow(*p);
    Read(p->vptrAt00)->processInputAt54(p);
    const auto event = PollChoiceManager(reinterpret_cast<Choice*>(p)->managerAt74);
    if (event < 0) return 1;
    if (event != row.checkbox->eventAt00) throw std::logic_error("Unexpected settings checkbox event");
    auto* parent = reinterpret_cast<Container*>(p->parentAt04);
    if (!parent || Read(parent->storedAt68)) throw std::logic_error("Settings checkbox is not loaded");
    Fresh found; found.slot = {}; RefLife life{found.slot};
    auto* c = Checkbox(*p, row, found.slot);
    c->checkedAt5C = Read(c->checkedAt5C) == 0;
    const bool value = (Read(c->checkedAt5C) != 0) == row.checkedValue;
    RECT rect;
    auto* bounds = Read(c->common.vptrAt00)->rectAt3C(&c->common, &rect);
    Read(p->vptrAt00)->invalidateAt64(p, *bounds);
    // 01:5006D0 / 05:4DC490在点击时提交，不推迟到End。
    if (row.commit == SettingTextCommit::Immediate) WriteSettingTextBool(row.field, value);
    life.Normal();
    return 1;
}
}
std::span<const SettingTextBoolRow> BooleanRows(SettingPageRadioContainerKind kind)
{
    const auto index = static_cast<std::size_t>(kind);
    if (index >= 4) throw std::logic_error("Not a boolean settings container");
    return CurrentSettingTextDisplayProfile().booleanGroups[index];
}
bool ReadBooleanRow(Common& p, const SettingTextBoolRow& row)
{
    if (!row.checkbox) return ReadSettingRadioChoiceNativeAt50ADE0(p) == row.trueEvent;
    Fresh found; found.slot = {}; RefLife life{found.slot};
    const bool value = (Read(Checkbox(p, row, found.slot)->checkedAt5C) != 0) == row.checkedValue;
    life.Normal(); return value;
}
void WriteBooleanRow(Common& p, const SettingTextBoolRow& row, bool value)
{
    if (!row.checkbox)
    { WriteSettingRadioChoiceNativeAt537B10(p, value ? row.trueEvent : row.falseEvent); return; }
    Fresh found; found.slot = {}; RefLife life{found.slot};
    Checkbox(p, row, found.slot)->checkedAt5C = value == row.checkedValue;
    life.Normal();
}
const SettingPageChoiceNativeVtable38& SettingCheckboxRowTable() noexcept
{
    static const auto table = [] {
        auto t = SettingFixRadioButtonsNativeVtable();
        t.common.pollAt48 = reinterpret_cast<decltype(t.common.pollAt48)>(&PollCheckbox);
        return t;
    }();
    return table;
}
void __fastcall Load02B1(Common* p, void*)
{
    const auto& row = ChoiceRow(*p);
    const auto value = ReadSettingTextChoice(row.field);
    int event = row.otherReadEvent;
    for (const auto& entry : row.values)
        if (entry.value == value) { event = entry.event; break; }
    WriteSettingRadioChoiceNativeAt537B10(*p, event);
}
void __fastcall Store02B1(Common* p, void*)
{
    const auto& row = ChoiceRow(*p);
    const auto event = ReadSettingRadioChoiceNativeAt50ADE0(*p);
    for (const auto& entry : row.values)
        if (entry.event == event) { WriteSettingTextChoice(row.field, entry.value); return; }
    // 01:4EBBE0/4EBF50遇未知选择保留旧值；右键选项则写None。
    if (!row.preserveUnknownSelection) WriteSettingTextChoice(row.field, 0);
}
void __fastcall LoadSceneChoices(Common* p, void*)
{
    for (const auto& row : CurrentSettingTextDisplayProfile().sceneChoices)
    {
        Fresh found; Into(p).findDialogAt90(p, &found.slot, row.id); RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        { auto* child = Resolve<Common>(found.slot); QTable(child).loadAt90(child); }
        life.Normal();
    }
    reinterpret_cast<Container*>(p)->storedAt68 = 0;
}
void __fastcall StoreSceneChoices(Common* p, void*)
{
    // 子选项在各自事件上提交，End不能覆盖随后写入的新值。
    reinterpret_cast<Container*>(p)->storedAt68 = 1;
}
}
