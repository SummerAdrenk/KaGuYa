#include "SettingPage03Native.SliderSnapshot.h"

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
namespace
{
bool HasRow(std::span<const SettingPage03NamedRow> rows, int id, bool checkbox)
{
    for (const auto& row : rows) if ((checkbox ? row.checkbox : row.slider) == id) return true;
    return false;
}
void StopSlot(SoundManagerNativeBody50h* manager, std::uint32_t slot)
{
    auto* fixedSlot = reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(Read(manager->slotsBeginAt04)) + slot * 8u);
    const auto handle = Read(fixedSlot->handleAt00);
    RequireSettingPage03NativeServices().sound.StopAt4385F1(handle);
    // 4383B0在外部SoundStop之后重读slotsBegin，而不是使用旧slot地址。
    auto* destination = reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(Read(manager->slotsBeginAt04)) + slot * 8u);
    SetRuntimeCStringAt437680(destination->nameAt04, "", 0);
}
void Music(Common* p)
{
    auto& s = RequireSettingPage03NativeServices();
    auto* unsaved = ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 2);
    if (!Field<RuntimeCString>(unsaved, 0x2C).GetLength()) return;
    Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, SettingSoundMasterRowFor(SettingSoundMaster::Music).slider); RefLife life{f.slot};
    if (Read(f.slot.baseAt00))
    {
        auto* c = Resolve<Control>(f.slot); const int value = Slider(c).valueAt60(c);
        auto* current = ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 2);
        const auto* held = Field<RuntimeCString>(current, 0x2C).GetString();
        const auto slot = Read(RootSound()->musicAt10);
        auto* manager = RootSound(); s.open.OpenAt42BC90(*manager, slot, held, value, 1, 0);
    }
    life.Normal();
}
void AdjustMusic(Common* p)
{
    Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, SettingSoundMasterRowFor(SettingSoundMaster::Music).slider); RefLife life{f.slot};
    if (Read(f.slot.baseAt00))
    {
        auto* c = Resolve<Control>(f.slot); const auto value = Slider(c).valueAt60(c);
        const auto slot = Read(RootSound()->musicAt10); auto* manager = RootSound();
        const auto handle = Read(Field<std::int32_t>(Read(manager->slotsBeginAt04), slot * 8u));
        RequireSettingPage03NativeServices().sound.SetVolume(handle, 100, value);
    }
    life.Normal(true);
}
}
void SyncMusicAt532740(Common* p)
{
    if (Table(p).checkedAt98(p, SettingSoundMasterRowFor(SettingSoundMaster::Music).checkbox)) Music(p);
    else { auto* manager = RootSound(); StopSlot(manager, Read(manager->musicAt10)); }
}
std::int32_t __fastcall PollBlock(Common* p, void*)
{
    const auto& profile = CurrentSettingPage03Profile();
    const auto& music = SettingSoundMasterRowFor(SettingSoundMaster::Music);
    const auto& effects = SettingSoundMasterRowFor(SettingSoundMaster::Effects);
    const auto& voice = SettingSoundMasterRowFor(SettingSoundMaster::Voice);
    SliderSnapshot before;
    for (auto* pos = Read(p->childrenAt0C.begin); pos != Read(p->childrenAt0C.end); ++pos)
    {
        auto* c = Resolve<Control>(*pos);
        if (Read(c->typeAt08) != 5) continue;
        c = Resolve<Control>(*pos); const auto value = Slider(c).valueAt60(c);
        c = Resolve<Control>(*pos); before.Insert(Read(c->eventAt04), value);
    }
    Read(p->vptrAt00)->processInputAt54(p);
    const int clicked = PollManager(Field<Slot>(p, 0x74));
    bool muteEntry = false;
    for (const auto& row : profile.muteRows) if (row.checkbox == clicked) { muteEntry = true; break; }
    if (muteEntry)
    {
        const auto* fixed = &Table(p);
        const auto value = fixed->checkedAt98(p, clicked);
        fixed->setCheckedAtA0(p, clicked, value, 1);
        if (!Table(p).checkedAt98(p, clicked))
        {
            if (clicked == music.checkbox) { auto* manager = RootSound(); StopSlot(manager, Read(manager->musicAt10)); }
            else if (clicked == voice.checkbox || HasRow(profile.characters, clicked, true)) StopPreview(Read(p->parentAt04), true);
            else if (clicked == effects.checkbox || HasRow(profile.effects, clicked, true)) StopPreview(Read(p->parentAt04), false);
        }
        else if (clicked == music.checkbox) Music(p);
    }
    else if (profile.loop.kind == SettingSoundLoopKind::CheckBox && clicked == profile.loop.checkbox)
    {
        Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, profile.loop.checkbox); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        {
            const auto old = Read(Field<int>(Resolve<Control>(f.slot), 0x5C));
            auto* c = Resolve<Control>(f.slot); CTable(c).resetAt08(c);
            Field<int>(Resolve<Control>(f.slot), 0x5C) = old == 0;
            c = Resolve<Control>(f.slot); auto* fixed = Read(p->vptrAt00);
            RECT out; auto* returned = CTable(c).rectAt3C(c, &out); fixed->invalidateAt64(p, *returned);
        }
        life.Normal();
    }
    for (const auto& row : profile.effectPreviews) if (row.button == clicked) PlayPreview(Read(p->parentAt04), clicked, false);
    for (const auto& row : profile.characterPreviews) if (row.button == clicked) PlayPreview(Read(p->parentAt04), clicked, true);
    for (auto* node = before.Begin(); node != before.End(); node = SliderSnapshot::Next(node))
    {
        const auto id = Read(node->key);
        if (id == music.slider) { AdjustMusic(p); continue; } // 原每次poll提交BGM音量，不要求变动。
        const bool type = id == effects.slider || HasRow(profile.effects, id, false);
        const bool chars = id == voice.slider || HasRow(profile.characters, id, false);
        if (!type && !chars) continue;
        Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, id); RefLife life{f.slot};
        if (Read(f.slot.baseAt00))
        {
            auto* c = Resolve<Control>(f.slot);
            if (Read(node->value) != Slider(c).valueAt60(c))
            {
                if (id == effects.slider || id == voice.slider) AdjustPreview(Read(p->parentAt04), -1, chars);
                else
                {
                    const auto button = Table(p).previewForSliderAtA4(p, id);
                    if (button >= 0) AdjustPreview(Read(p->parentAt04), button, chars);
                }
            }
        }
        life.Normal(true);
    }
    // poll只做GUI联动/即时声音副作用，不提交00/01/02配置、不更改stored资格。
    return 1;
}
void StopPreview(Common* p, bool chars)
{
    const auto slot = Read(chars ? RootSound()->voicesBeginAt14 : RootSound()->remainingEffectsBeginAt20);
    StopSlot(RootSound(), slot);
    Field<int>(p, chars ? 0x88 : 0x84) = -1;
}
}
