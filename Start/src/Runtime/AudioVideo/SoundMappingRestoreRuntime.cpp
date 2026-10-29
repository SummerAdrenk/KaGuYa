#include <Runtime/SoundMappingRestoreRuntime.h>
#include <Runtime/ParamsGameSystemRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SoundMappingRestoreRuntime::SoundMappingRestoreRuntime(ISoundMappingRestoreSource& source) noexcept
    : source_(source)
{
}

void SoundMappingRestoreRuntime::Restore(
    const bool soundMappingRestoreEnabledAt38Bit20)
{
    if (!soundMappingRestoreEnabledAt38Bit20)
        return;

    const auto countAt50 = [](const ParamsGameSystem* game) {
        if (!game) throw std::logic_error("46D420 reads absent GameSystem (modern safety boundary)");
        return game->soundSlotCounts[3];
    };
    // 46D781/46D911均为有符号比较；每次循环重新捕获slot2，不能缓存表长度或分组数。
    for (std::uint32_t port = 0;
        std::bit_cast<std::int32_t>(port) < source_.MappingCountAt46D778(); ++port)
    {
        if (source_.MappingEmptyAt46D8E1(port))
            continue;

        if (std::bit_cast<std::int32_t>(port) <
            std::bit_cast<std::int32_t>(countAt50(source_.CaptureGameSystemAt418480())))
        {
            // 46D91A..46D9A4：两个manager独立捕获，名字重读后才执行Voice扫描。
            const auto voice = source_.CaptureSoundManagerAt413BE0();
            const char* name = source_.CaptureMappingName(port);
            const auto restore = source_.CaptureSoundManagerAt413BE0();
            const bool anyVoice = voice.AnyVoiceSlotPlaying();
            restore.RestoreFirst(port, name, anyVoice);
            continue;
        }

        const bool loop = source_.CaptureParametersAt1C().HasLoopEffectType();
        const auto parameters = source_.CaptureParametersAt1C();
        const auto* game = source_.CaptureGameSystemAt418480();
        const char* name = source_.CaptureMappingName(port);
        const auto restore = source_.CaptureSoundManagerAt413BE0();
        const auto volume = loop ? parameters.LoopEffectValueOrThrow()
                                 : parameters.NormalEffectValueOrThrow();
        // 46DB54先解volume，再从已捕获的GS读+50；DWORD减法不得改成饱和或提前读取。
        restore.RestoreSecond(port - countAt50(game), name, volume);
    }
}
}
