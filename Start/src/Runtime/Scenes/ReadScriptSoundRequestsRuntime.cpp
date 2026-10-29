#include <Runtime/ReadScriptSoundRequestsRuntime.h>

#include <Runtime/ParamsGameSystemRuntime.h>
#include <Runtime/RuntimeCString.h>

#include <bit>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t FirstCount(const ParamsGameSystem* game)
{ return *static_cast<const volatile std::uint32_t*>(&game->soundSlotCounts[3]); }
}
ReadScriptSoundRequestsRuntime::ReadScriptSoundRequestsRuntime(IReadScriptSoundRequestsSource& source) noexcept
    : source_(source) {}
bool ReadScriptSoundRequestsRuntime::SkipRequestedAt4AA540(ReadScriptUnreadReceiver text)
{
    if (source_.ReadKeyboardSixHeldAt4AA573()) return true;
    if ((source_.ReadUnsaveFlagsAt14Via417990() & 3u) != 3u) return false;
    if ((source_.ReadSaveFlagsAt38Via417990() & 1u) == 0) return true;
    return text.Read() == 0;
}

void ReadScriptSoundRequestsRuntime::ProcessSoundRequestsAt4A9090()
{
    for (std::uint32_t port = 0; std::bit_cast<std::int32_t>(port) < source_.MappingCountInlineAt4A9202(); ++port)
    {
        if (source_.MappingEmptyInlineAt4A9371(port))
        {
            const auto query = source_.CaptureManagerAfter413BE0();
            if (!query.slots.IsPlayingAt419370(query.slots.FirstEffectsBeginAt18() + port)) continue;
            // 4A9505固定CString地址，9516固定新manager，954B清名后才读同一manager+18。
            auto* const mapping = source_.CaptureMappingSlotInlineAt4A9505(port);
            const auto stop = source_.CaptureManagerAfter413BE0();
            SetRuntimeCStringAt437680(*mapping, "", 0);
            stop.slots.StopAt4383B0(stop.slots.FirstEffectsBeginAt18() + port);
            continue;
        }
        const auto query = source_.CaptureManagerAfter413BE0();
        if (query.slots.IsPlayingAt419370(query.slots.FirstEffectsBeginAt18() + port)) continue;
        const auto text = source_.CaptureTextAfter413BE0At4A95A2();
        if (SkipRequestedAt4AA540(text) && (source_.ReadSaveFlagsAt38Via417990() & 0x20u) != 0) continue;
        if (std::bit_cast<std::int32_t>(port) < std::bit_cast<std::int32_t>(FirstCount(source_.CaptureGameSystemAt418480())))
        {
            const auto voice = source_.CaptureManagerAfter413BE0();
            const auto* name = source_.CaptureMappingNameVia417990(port);
            const auto restore = source_.CaptureManagerAfter413BE0();
            const bool anyVoice = voice.effects.AnyVoiceSlotPlaying();
            restore.effects.RestoreFirstMappedEffectRaw(port, name, anyVoice);
        }
        else
        {
            const bool loop = source_.CaptureParametersVia417990().HasLoopEffectType();
            const auto parameters = source_.CaptureParametersVia417990();
            const auto* game = source_.CaptureGameSystemAt418480();
            const auto* name = source_.CaptureMappingNameVia417990(port);
            const auto restore = source_.CaptureManagerAfter413BE0();
            const auto volume = loop ? parameters.LoopEffectValueOrThrow() : parameters.NormalEffectValueOrThrow();
            // 4A98A6先读volume，4A98AD再从此前捕获GS读+50；不重取GS或提前缓存计数。
            restore.effects.RestoreSecondMappedEffectRaw(port - FirstCount(game), name, volume);
        }
        // 两组恢复的AL均被原caller忽略；普通异常保留之前的副作用前缀。
    }
}
}
