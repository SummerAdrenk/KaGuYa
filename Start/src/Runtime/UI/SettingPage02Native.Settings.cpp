#include "SettingPage02Native.Internal.h"
#include <vector>

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
void Load(Common* p, SettingPageRadioContainerKind kind)
{
    for (const auto& row : BooleanRows(kind))
    {
        Fresh found; Into(p).findDialogAt90(p, &found.slot, row.id); RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            auto* fixed = Resolve<Common>(found.slot);
            WriteBooleanRow(*fixed, row, ReadSettingTextBool(row.field));
        }
        life.Normal();
    }
    reinterpret_cast<Container*>(p)->storedAt68 = 0;
}
void Store(Common* p, SettingPageRadioContainerKind kind)
{
    auto& body = *reinterpret_cast<Container*>(p);
    if (!Read(body.storedAt68))
    {
        for (const auto& row : BooleanRows(kind))
        {
            // Immediate controls have already committed. Do not overwrite newer shared state.
            if (row.commit == SettingTextCommit::Immediate) continue;
            Fresh found; Into(p).findDialogAt90(p, &found.slot, row.id); RefLife life{found.slot};
            if (Read(found.slot.baseAt00))
            {
                const bool value = ReadBooleanRow(*Resolve<Common>(found.slot), row);
                WriteSettingTextBool(row.field, value);
            }
            life.Normal();
        }
    }
    body.storedAt68 = 1;
}
int NestedChoice(Common* p, int id)
{
    Fresh found; Into(p).findDialogAt90(p, &found.slot, id); RefLife life{found.slot};
    const auto value = Read(found.slot.baseAt00) ? ReadSettingRadioChoiceNativeAt50ADE0(*Resolve<Common>(found.slot)) : -1;
    life.Normal(); return value;
}
}
void __fastcall Load01B0(Common* p, void*) { Load(p, SettingPageRadioContainerKind::Page01Block00); }
void __fastcall Store01B0(Common* p, void*) { Store(p, SettingPageRadioContainerKind::Page01Block00); }
void __fastcall Load01B1(Common* p, void*) { Load(p, SettingPageRadioContainerKind::Page01Block01); }
void __fastcall Store01B1(Common* p, void*) { Store(p, SettingPageRadioContainerKind::Page01Block01); }
void __fastcall Load02B0(Common* p, void*) { Load(p, SettingPageRadioContainerKind::Page02Block00); }
void __fastcall Store02B0(Common* p, void*) { Store(p, SettingPageRadioContainerKind::Page02Block00); }
void __fastcall Load02B3(Common* p, void*) { Load(p, SettingPageRadioContainerKind::Page02Block03); }
void __fastcall Store02B3(Common* p, void*) { Store(p, SettingPageRadioContainerKind::Page02Block03); }
std::int32_t __fastcall Poll02B0(Common* p, void*)
{
    std::vector<std::pair<const SettingTextBoolRow*,int>> before;
    for (const auto& row : BooleanRows(SettingPageRadioContainerKind::Page02Block00))
        if (!row.checkbox && row.commit == SettingTextCommit::Immediate)
            before.emplace_back(&row, NestedChoice(p, row.id));
    GuiDialogIntoBaseVtable().common.pollAt48(p);
    // 09:4EE600 and 18:52E370: exit/defaults act on their new value before the outer command.
    for (const auto& [row, previous] : before)
    {
        Fresh found; Into(p).findDialogAt90(p, &found.slot, row->id); RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            auto* fixed = Resolve<Common>(found.slot);
            if (ReadSettingRadioChoiceNativeAt50ADE0(*fixed) != previous)
                WriteSettingTextBool(row->field, ReadBooleanRow(*Resolve<Common>(found.slot), *row));
        }
        life.Normal();
    }
    return 1;
}
}
