#include "MainFrameSettingsNative.Internal.h"

namespace KaGuYa::Start::Runtime::MainFrameSettingsNativeDetail
{
namespace
{
using Parameters = EnvironmentSoundParamsArchive;
Parameters* Saved(MainFrameSettingsNativeServices& services, unsigned site)
{ return Resolve<Parameters>(Field<Slot>(Registry(services, 1, site), 0x1C)); }
void Assign(RuntimeCString& destination, const RuntimeCString& source)
{ AssignRuntimeCStringAt410220(destination, source); }
}
void CaptureSnapshot(Locals& values, MainFrameSettingsNativeServices& services)
{
    auto* destination = Read(values.mappings.begin);
    for (unsigned index = 0;; ++index)
    {
        auto* const counted = Registry(services, 2, 0x428F3E, RegistryOrder::InlineBiasFirst, true);
        if (index >= Count(Field<Vector>(counted, 0x30))) break;
        auto* const source = Registry(services, 2, 0x42908D, RegistryOrder::InlineStrideFirst, true);
        auto* const element = At(Read(Field<Vector>(source, 0x30).begin), index);
        Assign(*destination, *element);
        destination = At(destination, 1);
    }
    auto* const mapped = Registry(services, 2, 0x429238, RegistryOrder::InlineBiasFirst);
    auto* const mappings = &Field<Vector>(mapped, 0x30);
    services.apply.music.StopEffectsAt413270(*RootSound(0x429288), mappings);
    services.volume.RefreshAt433F60(*RootSound(0x4292C2));
}
void RestoreSounds(Locals& values, MainFrameSettingsNativeServices& services)
{
    auto& apply = services.apply;
    static_cast<void>(apply.playback.StopAndClearAt4384D0(*RootSound(0x429729)));
    apply.music.StopEffectsAt413270(*RootSound(0x42975F), nullptr);
    if (Read(*Saved(services, 0x429792)->MusicMuteAt10()))
    {
        auto* const manager = RootSound(0x42980D);
        static_cast<void>(apply.music.StopAbsoluteAt4383B0(*manager, Read(manager->musicAt10)));
    }
    else if (Field<RuntimeCString>(Registry(services, 2, 0x429851), 0x2C).GetLength())
    {
        auto* const parameters = Saved(services, 0x4298A9);
        const auto name = Field<RuntimeCString>(Registry(services, 2, 0x42991F), 0x2C).GetString();
        auto* const manager = RootSound(0x429978);
        const auto volume = std::bit_cast<std::int32_t>(Read(*parameters->MusicVolumeAt14()));
        if (apply.music.HasSlotsAt4D5B1A(*manager))
            static_cast<void>(apply.music.ApplyActiveAt4D5B28(*manager, name, volume, 1));
    }
    auto* const parameters = Saved(services, 0x4299C0);
    auto* const manager = RootSound(0x429A3B);
    const auto volumePointer = parameters->MusicVolumeAt14();
    const auto music = Read(manager->musicAt10);
    const auto volume = std::bit_cast<std::int32_t>(Read(*volumePointer));
    auto* const slots = Read(manager->slotsBeginAt04);
    auto* const slot = reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(slots) + music * 8u);
    apply.sound.SetVolume(Read(slot->handleAt00), 100, volume);

    auto* const firstBegin = Read(values.mappings.begin);
    unsigned index = 0;
    for (;; ++index)
    {
        auto* const system = GameSystem(0x429ABC);
        if (std::bit_cast<std::int32_t>(index) >= Read(Field<std::int32_t>(system, 0x50))) break;
        const auto name = At(firstBegin, index)->GetString();
        if (!SettingDialogNativeApplyDetail::HeaderLength(name)) continue;
        static_cast<void>(apply.effects.RestoreFirstAt42C1B0(*RootSound(0x429B8A), name, index, 0));
        auto* const mapped = Registry(services, 2, 0x429BBF);
        auto* const destination = At(Read(Field<Vector>(mapped, 0x30).begin), index);
        Assign(*destination, *At(firstBegin, index));
    }
    // 429C8E..429CC1只捕获一次上界；不是51CFC0剩余映射每轮重读size的合同。
    const auto upper = std::bit_cast<std::int32_t>(Count(values.mappings));
    while (std::bit_cast<std::int32_t>(index) < upper)
    {
        const auto initial = At(Read(values.mappings.begin), index)->GetString();
        if (SettingDialogNativeApplyDetail::HeaderLength(initial))
        {
            const bool loop = Saved(services, 0x429CD5)->HasLoopEffectAt28();
            if (loop)
            {
                if (Read(*Saved(services, 0x429D50)->LoopEffectMuteAt2C()))
                {
                    auto* const receiver = Saved(services, 0x429DCA);
                    auto* const system = GameSystem(0x429E5E);
                    const auto name = At(Read(values.mappings.begin), index)->GetString();
                    auto* const selected = RootSound(0x429F3D);
                    const auto value = std::bit_cast<std::int32_t>(Read(*receiver->LoopEffectVolumeAt30()));
                    const auto relative = index - Read(Field<unsigned>(system, 0x50));
                    static_cast<void>(apply.effects.RestoreSecondAt42C070(*selected, name, relative, value));
                }
            }
            else if (Read(*Saved(services, 0x429F93)->NormalEffectMuteAt20()))
            {
                auto* const receiver = Saved(services, 0x42A00D);
                auto* const system = GameSystem(0x42A0A1);
                const auto name = At(Read(values.mappings.begin), index)->GetString();
                auto* const selected = RootSound(0x42A180);
                const auto value = std::bit_cast<std::int32_t>(Read(*receiver->NormalEffectVolumeAt24()));
                const auto relative = index - Read(Field<unsigned>(system, 0x50));
                static_cast<void>(apply.effects.RestoreSecondAt42C070(*selected, name, relative, value));
            }
            auto* const mapped = Registry(services, 2, 0x42A1D2);
            auto* const destination = At(Read(Field<Vector>(mapped, 0x30).begin), index);
            Assign(*destination, *At(Read(values.mappings.begin), index));
        }
        ++index;
    }
}
}
