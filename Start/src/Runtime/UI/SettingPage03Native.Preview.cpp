#include "SettingPage03Native.Internal.h"
#include <Logging/Logger.h>

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
namespace
{
int Index(std::span<const SettingPage03PreviewRow> rows, int button)
{
    for (std::size_t i = 0; i != rows.size(); ++i) if (rows[i].button == button) return static_cast<int>(i);
    return -1;
}
int ParamsIndex(const SettingPage03ResourceProfile& profile, int rowIndex, bool chars)
{
    const auto rows = chars ? profile.characterPreviews : profile.effectPreviews;
    const auto indices = chars ? profile.characterParamsIndices : profile.effectParamsIndices;
    if (indices.size() != rows.size() || indices[rowIndex] < 0)
        throw std::logic_error("Sound page preview params mapping is incomplete");
    return indices[rowIndex];
}
int MultiplyPercent(int master, int local) noexcept
{ return std::bit_cast<int>(static_cast<unsigned>(master) * static_cast<unsigned>(local)) / 100; }
void Advance(std::uint32_t* state, EnvironmentVectorStorage<RuntimeCString>* files)
{
    const auto value = Read(*state) + 1u; *state = value;
    const auto begin = reinterpret_cast<std::uintptr_t>(Read(files->begin));
    const auto end = reinterpret_cast<std::uintptr_t>(Read(files->end));
    if (std::bit_cast<int>(value) >= (std::bit_cast<int>(end - begin) >> 2)) *state = 0;
}
}
void PlayPreview(Common* p, std::int32_t button, bool chars)
{
    StopPreview(p, chars); // 539AF0/53A230即使无效button也先停止、清name、清活动ID。
    const auto& profile = CurrentSettingPage03Profile();
    const auto rows = chars ? profile.characterPreviews : profile.effectPreviews;
    const auto index = Index(rows, button); if (index < 0) return;
    const auto& masterRow = SettingSoundMasterRowFor(chars ? SettingSoundMaster::Voice : SettingSoundMaster::Effects);
    Fresh gate; Into(p).findDialogAt90(p, &gate.slot, 15);
    int enabled = 0;
    {
        RefLife life{gate.slot};
        if (Read(gate.slot.baseAt00))
        { auto* c = Resolve<Common>(gate.slot); enabled = Table(c).checkedAt98(c, masterRow.checkbox); }
        life.Normal();
    }
    if (!enabled) return;
    Fresh local; Into(p).findDialogAt90(p, &local.slot, chars ? 17 : 16); RefLife life{local.slot};
    if (Read(local.slot.baseAt00))
    {
        auto* c = Resolve<Common>(local.slot); const auto& row = rows[index];
        if (Table(c).checkedAt98(c, row.checkbox))
        {
            const auto paramsIndex = ParamsIndex(profile, index, chars);
            Text registration;
            if (chars)
            {
                InitializeEmptyRuntimeCString(registration.value, RequireSettingPage03NativeServices().strings); registration.active = true;
                SetRuntimeCStringAt437680(registration.value, profile.registration.data(), static_cast<int>(profile.registration.size()));
            }
            auto* files = chars ? Sounds(true)->RegistrationFilesAt98(paramsIndex, registration.value.GetString()) : Sounds(true)->TypeFilesAt54(paramsIndex);
            if (files)
            {
                auto* state = chars ? Sounds(true)->RegistrationStateAt94(paramsIndex, registration.value.GetString()) : Sounds(true)->TypeStateAt50(paramsIndex);
                if (state)
                {
                    const auto master = ParentValue(p, masterRow.slider);
                    c = Resolve<Common>(local.slot); const auto value = Table(c).valueAt9C(c, row.slider, 0);
                    const auto volume = MultiplyPercent(master, value);
                    auto* sound = Sounds(true);
                    // 原01:4ECBE0/4ED090及18:539AF0/53A230未检查此危险域；明确拒绝，不伪成功。
                    const auto begin = reinterpret_cast<std::uintptr_t>(Read(files->begin));
                    const auto end = reinterpret_cast<std::uintptr_t>(Read(files->end));
                    const auto fileIndex = Read(*state);
                    if (begin == end || fileIndex >= (end - begin) / sizeof(RuntimeCString))
                    {
                        Logging::WriteFormat(Logging::Level::Error, "Sound page preview files/state invalid: characters=%d, button=%d, params=%d, state=%u",
                            chars, button, paramsIndex, fileIndex);
                        throw std::out_of_range("Sound page preview has no file at the current state");
                    }
                    const auto address = begin + fileIndex * sizeof(RuntimeCString);
                    const auto* file = Field<RuntimeCString>(reinterpret_cast<void*>(address), 0).GetString();
                    Text mapped; auto* returned = sound->MapResourceAtA0(&mapped.value, file); mapped.active = true;
                    const char* held = chars ? returned->GetString() : mapped.value.GetString();
                    const auto slot = Read(chars ? RootSound()->voicesBeginAt14 : RootSound()->remainingEffectsBeginAt20);
                    auto* manager = RootSound();
                    RequireSettingPage03NativeServices().open.OpenAt42BC90(*manager, slot, held, volume, 0, 0);
                    if (chars) mapped.Normal(); // 53A230先释放输出串再推进state/发布88。
                    Advance(state, files); Field<int>(p, chars ? 0x88 : 0x84) = button;
                    if (!chars) mapped.Normal(); // 539AF0相反，保存84早于字符串释放。
                }
            }
            if (chars) registration.Normal();
        }
    }
    life.Normal();
}
void AdjustPreview(Common* p, std::int32_t button, bool chars)
{
    const auto slot = Read(chars ? RootSound()->voicesBeginAt14 : RootSound()->remainingEffectsBeginAt20);
    auto* manager = RootSound();
    const auto handle = Read(Field<int>(Read(manager->slotsBeginAt04), slot * 8u));
    auto& s = RequireSettingPage03NativeServices();
    if (!s.sound.IsPlaying(handle)) return;
    const auto current = Read(Field<int>(p, chars ? 0x88 : 0x84));
    if (!(button < 0 || button == current) || current < 0) return;
    const auto& profile = CurrentSettingPage03Profile();
    const auto rows = chars ? profile.characterPreviews : profile.effectPreviews;
    const auto index = Index(rows, current); if (index < 0) return;
    const auto& masterRow = SettingSoundMasterRowFor(chars ? SettingSoundMaster::Voice : SettingSoundMaster::Effects);
    Fresh gate; Into(p).findDialogAt90(p, &gate.slot, 15);
    int enabled = 0;
    {
        RefLife life{gate.slot};
        if (Read(gate.slot.baseAt00))
        { auto* c = Resolve<Common>(gate.slot); enabled = Table(c).checkedAt98(c, masterRow.checkbox); }
        life.Normal();
    }
    if (!enabled) return;
    Fresh local; Into(p).findDialogAt90(p, &local.slot, chars ? 17 : 16); RefLife life{local.slot};
    const auto& row = rows[index];
    if (!Read(local.slot.baseAt00)) { life.Normal(true); return; }
    auto* c = Resolve<Common>(local.slot);
    if (!Table(c).checkedAt98(c, row.checkbox)) { life.Normal(true); return; }
    const auto master = ParentValue(p, masterRow.slider);
    c = Resolve<Common>(local.slot); const auto volume = MultiplyPercent(master, Table(c).valueAt9C(c, row.slider, 0));
    const auto newSlot = Read(chars ? RootSound()->voicesBeginAt14 : RootSound()->remainingEffectsBeginAt20);
    auto* currentManager = RootSound();
    const auto newHandle = Read(Field<int>(Read(currentManager->slotsBeginAt04), newSlot * 8u));
    s.sound.SetVolume(newHandle, 100, volume); life.Normal();
}
}
