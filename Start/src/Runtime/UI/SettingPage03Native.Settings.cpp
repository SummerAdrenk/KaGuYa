#include "SettingPage03Native.Internal.h"
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
void* Registry1(bool inlineCapture)
{
    auto& s = RequireSettingPage03NativeServices().source;
    auto* root = inlineCapture ? s.CaptureEnvironmentInlineAt6198C8() : s.CaptureEnvironmentAt4180C0();
    return ResolveEnvironmentArchiveAt417990(*root, 1);
}
EnvironmentSoundParamsArchive* Sounds(bool inlineCapture)
{ return Resolve<EnvironmentSoundParamsArchive>(Field<Slot>(Registry1(inlineCapture), 0x1C)); }
SoundManagerNativeBody50h* RootSound()
{
    auto& s = RequireSettingPage03NativeServices().source;
    s.EnsureRootAt413BE0(); auto* fixedRoot = s.RootAt6198C4();
    return Resolve<SoundManagerNativeBody50h>(s.RootSoundSlotAt24(fixedRoot));
}
BOOL __fastcall Checked(Common* p, void*, std::int32_t id)
{
    Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, id); RefLife life{f.slot};
    const auto value = Read(f.slot.baseAt00) ? Read(Field<std::int32_t>(Resolve<Control>(f.slot), 0x5C)) == 0 : FALSE;
    life.Normal(); return value;
}
std::int32_t __fastcall Value(Common* p, void*, std::int32_t id, std::uint8_t force)
{
    Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, id); RefLife life{f.slot};
    int result = 0;
    if (Read(f.slot.baseAt00) && (force || CTable(Resolve<Control>(f.slot)).readStateAt48(Resolve<Control>(f.slot))))
    { auto* c = Resolve<Control>(f.slot); result = Slider(c).valueAt60(c); }
    life.Normal(); return result;
}
void __fastcall SetChecked(Common* p, void*, std::int32_t id, std::int32_t checked, std::int32_t redraw)
{
    for (const auto& row : CurrentSettingPage03Profile().muteRows)
    {
        if (row.checkbox != id) continue;
        Fresh f; Read(p->vptrAt00)->findChildAt14(p, &f.slot, id);
        {
            RefLife life{f.slot};
            if (Read(f.slot.baseAt00))
            {
                auto* c = Resolve<Control>(f.slot); CTable(c).resetAt08(c);
                Field<std::int32_t>(Resolve<Control>(f.slot), 0x5C) = checked;
                if (redraw)
                {
                    c = Resolve<Control>(f.slot); auto* fixedTable = Read(p->vptrAt00);
                    RECT out; auto* returned = CTable(c).rectAt3C(c, &out); fixedTable->invalidateAt64(p, *returned);
                }
            }
            life.Normal(true);
        }
        for (int dependent : row.dependents)
        {
            Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, dependent); RefLife life{child.slot};
            if (Read(child.slot.baseAt00))
            {
                auto* c = Resolve<Control>(child.slot); CTable(c).resetAt08(c);
                c = Resolve<Control>(child.slot); CTable(c).writeStateAt44(c, checked == 0);
                if (redraw)
                {
                    c = Resolve<Control>(child.slot); auto* fixedTable = Read(p->vptrAt00);
                    RECT out; auto* returned = CTable(c).rectAt3C(c, &out); fixedTable->invalidateAt64(p, *returned);
                }
            }
            life.Normal(true);
        }
    }
}
std::int32_t __fastcall PreviewForSlider(Common*, void*, std::int32_t id)
{
    const auto& profile = CurrentSettingPage03Profile();
    for (const auto& row : profile.characterPreviews) if (row.slider == id) return row.button;
    for (const auto& row : profile.effectPreviews) if (row.slider == id) return row.button;
    return -1;
}
namespace
{
using DwordGetter = std::uint32_t* (EnvironmentSoundParamsArchive::*)();
using ByteGetter = std::uint8_t* (EnvironmentSoundParamsArchive::*)();
struct MasterAccess final { DwordGetter volume; ByteGetter mute; };
MasterAccess AccessFor(SettingSoundMaster role)
{
    switch (role)
    {
    case SettingSoundMaster::Music:
        return {&EnvironmentSoundParamsArchive::MusicVolumeAt14, &EnvironmentSoundParamsArchive::MusicMuteAt10};
    case SettingSoundMaster::Effects:
        return {&EnvironmentSoundParamsArchive::NormalEffectVolumeAt24, &EnvironmentSoundParamsArchive::NormalEffectMuteAt20};
    case SettingSoundMaster::Voice:
        return {&EnvironmentSoundParamsArchive::VoiceVolumeAt6C, &EnvironmentSoundParamsArchive::VoiceMuteAt68};
    }
    throw std::logic_error("Sound page master role is invalid");
}
bool Equal(const RuntimeCString& name, const char* rowName)
{
    if (!rowName) throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(name.GetString()), reinterpret_cast<const unsigned char*>(rowName)) == 0;
}
void Registration(Text& text)
{
    InitializeEmptyRuntimeCString(text.value, RequireSettingPage03NativeServices().strings); text.active = true;
    const auto name = CurrentSettingPage03Profile().registration;
    SetRuntimeCStringAt437680(text.value, name.data(), static_cast<int>(name.size()));
}
}
void __fastcall Load00(Common* p, void*)
{
    const auto& profile = CurrentSettingPage03Profile();
    for (const auto& row : profile.masters)
    {
        const auto access = AccessFor(row.role);
        Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, row.slider); RefLife life{child.slot};
        if (Read(child.slot.baseAt00))
        {
            auto* sound = Sounds(); auto* c = Resolve<Control>(child.slot); const auto* fixed = &Slider(c);
            const auto value = Read(*(sound->*access.volume)()); fixed->setValueAt64(c, std::bit_cast<std::int32_t>(value));
        }
        auto* sound = Sounds(); const auto* fixed = &Table(p);
        const auto mute = Read(*(sound->*access.mute)()); fixed->setCheckedAtA0(p, row.checkbox, mute, 0);
        life.Normal();
    }
    if (profile.loop.kind == SettingSoundLoopKind::CheckBox)
    {
        Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, profile.loop.checkbox); RefLife life{child.slot};
        if (Read(child.slot.baseAt00))
        {
            // 02:4F4AE4/509CD0直接保存checked；共享SaveData字段为+81，不走mute反转。
            const bool checked = Read(Field<std::uint8_t>(Registry1(), 0x81)) != 0;
            Field<int>(Resolve<Control>(child.slot), 0x5C) = checked;
        }
        life.Normal();
    }
    Field<int>(p, 0x5C) = 0;
}
void __fastcall Store00(Common* p, void*)
{
    if (!Read(Field<int>(p, 0x5C)))
    {
        const auto& profile = CurrentSettingPage03Profile();
        for (const auto& row : profile.masters)
        {
            const auto access = AccessFor(row.role);
            Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, row.slider); RefLife life{child.slot};
            if (Read(child.slot.baseAt00))
            {
                auto* c = Resolve<Control>(child.slot); auto* sound = Sounds(); const auto* fixed = &Slider(c);
                auto* destination = (sound->*access.volume)(); *destination = static_cast<std::uint32_t>(fixed->valueAt60(c));
            }
            auto* sound = Sounds(); const auto checked = Table(p).checkedAt98(p, row.checkbox);
            *(sound->*access.mute)() = static_cast<std::uint8_t>(checked == 0); life.Normal();
        }
        if (profile.loop.kind == SettingSoundLoopKind::CheckBox)
        {
            Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, profile.loop.checkbox); RefLife life{child.slot};
            if (Read(child.slot.baseAt00))
            {
                // 02:4F4DB5/4F4DD0读取原checkbox state并写checked，不调用Checked(mute)。
                const bool checked = Read(Field<int>(Resolve<Control>(child.slot), 0x5C)) != 0;
                Field<std::uint8_t>(Registry1(), 0x81) = static_cast<std::uint8_t>(checked);
            }
            life.Normal();
        }
    }
    Field<int>(p, 0x5C) = 1;
}
void __fastcall Load01(Common* p, void*)
{
    const auto& profile = CurrentSettingPage03Profile();
    if (Sounds(true)->HasTypeGroupAt34())
    {
        for (const auto& row : profile.effects)
        {
            int mute = 1;
            for (int i = 0; i < Sounds(true)->TypeCountAt3C(); ++i)
            {
                Text name; Sounds(true)->TypeNameAt44(&name.value, i); name.active = true;
                if (!Equal(name.value, row.name)) { name.Normal(); continue; }
                const auto volume = Read(*Sounds(true)->TypeVolumeAt4C(i));
                Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, row.slider);
                {
                    RefLife life{child.slot};
                    const bool hasSlider = Read(child.slot.baseAt00) != 0;
                    if (hasSlider)
                    { auto* c = Resolve<Control>(child.slot); Slider(c).setValueAt64(c, std::bit_cast<std::int32_t>(volume)); }
                    // 01:4EED30仅在slider存在时读mute；02+仍按共享原流程读取。
                    if (hasSlider || !profile.categorized) mute = Read(*Sounds(true)->TypeMuteAt48(i)) != 0;
                    life.Normal(true);
                }
                name.Normal(); break;
            }
            Table(p).setCheckedAtA0(p, row.checkbox, mute, 0);
        }
    }
    Field<int>(p, 0x5C) = 0;
}
void __fastcall Store01(Common* p, void*)
{
    // 516C90先查HasTypeGroup，即使stored==1也必须捕获外部环境。
    if (Sounds(true)->HasTypeGroupAt34() && !Read(Field<int>(p, 0x5C)))
    {
        for (const auto& row : CurrentSettingPage03Profile().effects)
            for (int i = 0; i < Sounds(true)->TypeCountAt3C(); ++i)
            {
                Text name; Sounds(true)->TypeNameAt44(&name.value, i); name.active = true;
                if (!Equal(name.value, row.name)) { name.Normal(); continue; }
                auto* sound = Sounds(true);
                const bool mute = Table(p).checkedAt98(p, row.checkbox) == 0;
                *sound->TypeMuteAt48(i) = static_cast<std::uint8_t>(mute);
                sound = Sounds(true); const auto* fixed = &Table(p); auto* destination = sound->TypeVolumeAt4C(i);
                *destination = static_cast<std::uint32_t>(fixed->valueAt9C(p, row.slider, 1));
                name.Normal(); break;
            }
    }
    Field<int>(p, 0x5C) = 1;
}
void __fastcall Load02(Common* p, void*)
{
    const auto& profile = CurrentSettingPage03Profile();
    // 01:4EF4D0无现代group门控；02+保留Enabled/Using的原短路次序。
    if (profile.categorized || (Sounds(true)->CharacterGroupEnabledAt70() && Read(*Sounds(true)->CharacterGroupUsingAt74())))
    {
        for (const auto& row : profile.characters)
        {
            int mute = 1;
            for (int i = 0; i < Sounds(true)->CharacterCountAt78(); ++i)
            {
                Text name; Sounds(true)->CharacterNameAt80(&name.value, i); name.active = true;
                if (!Equal(name.value, row.name)) { name.Normal(); continue; }
                Text registration; Registration(registration);
                const auto volume = Read(*Sounds(true)->RegistrationVolumeAt8C(i, registration.value.GetString()));
                Text master; Sounds(true)->RegistrationMasterAt9C(&master.value, i, registration.value.GetString()); master.active = true;
                Fresh child; Read(p->vptrAt00)->findChildAt14(p, &child.slot, row.slider);
                {
                    RefLife life{child.slot};
                    const bool hasSlider = Read(child.slot.baseAt00) != 0;
                    if (hasSlider)
                    { auto* c = Resolve<Control>(child.slot); Slider(c).setValueAt64(c, std::bit_cast<std::int32_t>(volume)); }
                    // 01:4EF4D0缺slider时保持初始mute，不把配置mute当成已加载控件。
                    if (hasSlider || !profile.categorized)
                        mute = Read(*Sounds(true)->RegistrationMuteAt90(i, registration.value.GetString())) != 0;
                    life.Normal(true);
                }
                master.Normal(); registration.Normal(); name.Normal(); break;
            }
            Table(p).setCheckedAtA0(p, row.checkbox, mute, 0);
        }
    }
    Field<int>(p, 0x5C) = 0;
}
void __fastcall Store02(Common* p, void*)
{
    const auto& profile = CurrentSettingPage03Profile();
    if ((profile.categorized || (Sounds(true)->CharacterGroupEnabledAt70() && Read(*Sounds(true)->CharacterGroupUsingAt74()))) && !Read(Field<int>(p, 0x5C)))
    {
        for (const auto& row : profile.characters)
            for (int i = 0; i < Sounds(true)->CharacterCountAt78(); ++i)
            {
                Text name; Sounds(true)->CharacterNameAt80(&name.value, i); name.active = true;
                if (!Equal(name.value, row.name)) { name.Normal(); continue; }
                Text registration; Registration(registration);
                auto* sound = Sounds(true); const auto checked = Table(p).checkedAt98(p, row.checkbox);
                *sound->RegistrationMuteAt90(i, registration.value.GetString()) = static_cast<std::uint8_t>(checked == 0);
                sound = Sounds(true); const auto* fixed = &Table(p);
                auto* destination = sound->RegistrationVolumeAt8C(i, registration.value.GetString());
                *destination = static_cast<std::uint32_t>(fixed->valueAt9C(p, row.slider, 1));
                registration.Normal(); name.Normal(); break;
            }
    }
    Field<int>(p, 0x5C) = 1;
}
void __fastcall Load04(Common* p, void*)
{
    Field<int>(p, 0x80) = -1; p->enabledAt38 = 0;
    if (CurrentSettingPage03Profile().pager.enabled)
    {
        // 11:4CFC40、15:4B9820先选page0，再启用分页。
        SelectPageIndicatorAt503320(p, 0, 0); p->enabledAt38 = 1;
    }
    Field<int>(p, 0x5C) = 0;
}
void __fastcall Store04(Common* p, void*) { Field<int>(p, 0x5C) = 1; }
std::int32_t __fastcall Poll04(Common* p, void*)
{
    const auto& pager = CurrentSettingPage03Profile().pager;
    if (!pager.enabled || !Read(p->enabledAt38)) return 1;
    Read(p->vptrAt00)->processInputAt54(p);
    const auto id = PollManager(Field<Slot>(p, 0x74));
    if (id != pager.buttons[0] && id != pager.buttons[1]) return 1;
    const int old = Read(Field<int>(p, 0x80));
    if (old >= 0)
    {
        const auto plus = std::bit_cast<int>(static_cast<unsigned>(old) + 1u);
        const int next = id == pager.buttons[0] ? (static_cast<unsigned>(plus) < 2u ? plus : 1) : (old - 1 >= 0 ? old - 1 : 0);
        if (old != next) SelectPageIndicatorAt503320(p, next, 1);
    }
    Read(p->vptrAt00)->resetChildrenAt68(p); return 1;
}
}
