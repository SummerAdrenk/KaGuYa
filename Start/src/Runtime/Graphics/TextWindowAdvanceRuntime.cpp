#include <Runtime/TextWindowAdvanceRuntime.h>
#include <Runtime/TextWindowEffectRestoreRuntime.h>

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/TextWindowPlaybackRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/WaitMarkRuntime.h>

#include <bit>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t ReadLe32(const std::uint8_t* bytes) noexcept
{
    return std::uint32_t{bytes[0]} | (std::uint32_t{bytes[1]} << 8) |
        (std::uint32_t{bytes[2]} << 16) | (std::uint32_t{bytes[3]} << 24);
}

std::int32_t Signed32(std::uint32_t bits) noexcept
{
    return std::bit_cast<std::int32_t>(bits);
}

class Cp932Locale final
{
public:
    Cp932Locale() : locale_(_create_locale(LC_CTYPE, ".932"))
    {
        if (!locale_) throw std::runtime_error("CTextWin advance CP932 locale unavailable");
    }
    ~Cp932Locale() { _free_locale(locale_); }
    Cp932Locale(const Cp932Locale&) = delete;
    Cp932Locale& operator=(const Cp932Locale&) = delete;
    [[nodiscard]] _locale_t Get() const noexcept { return locale_; }
private:
    _locale_t locale_;
};

const unsigned char* Bytes(const char* value) noexcept
{
    return reinterpret_cast<const unsigned char*>(value);
}

}

TextWindowAdvanceRuntime::TextWindowAdvanceRuntime(TextWindowLayerRuntime& window,
    const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
    const ClickInfoArchive& click, Platform::MouseView mouse,
    Platform::KeyboardView keyboard, ITextWindowSoundSource& sound,
    ITextWindowEffectRestoreSource& restore,
    WaitMarkRuntime& waitMark, ITextWindowPlaybackServices& playback,
    ITextWindowAdvanceServices& services) noexcept
    : window_(window), save_(save), unsave_(unsave), click_(click), mouse_(mouse),
      input_(mouse, keyboard), sound_(sound), restore_(restore),
      waitMark_(waitMark), playback_(playback), services_(services) {}

bool TextWindowAdvanceRuntime::ReadSkipAt4AA540() const
{
    if (input_.ReadHeldAtVslot00(6)) return true;
    if ((unsave_.FlagsAt14() & 3u) != 3u) return false;
    if ((save_.bytesAt34[4] & 1u) == 0) return true;
    return window_.TextState().unreadAt9A.value() == 0;
}

bool TextWindowAdvanceRuntime::CanAdvanceAt4AA3E0() const
{
    if ((unsave_.FlagsAt1C() & 2u) != 0) return false;
    if ((unsave_.FlagsAt20() & 2u) != 0) return false;
    // 5AE218/5AE224 的原 CP932 名称；两次根读取不能合并为快照。
    const Cp932Locale locale;
    if (_mbscmp_l(Bytes(services_.ReadCurrentSceneNameAt1C().c_str()),
        Bytes("\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA"), locale.Get()) == 0)
        return false;
    return _mbscmp_l(Bytes(services_.ReadCurrentSceneNameAt1C().c_str()),
        Bytes("\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"), locale.Get()) != 0;
}

std::int32_t TextWindowAdvanceRuntime::ReadWheelAt4AD740() const noexcept
{
    return mouse_.SecondaryCountdown() == 0 ? mouse_.WheelSteps() : 0;
}

bool TextWindowAdvanceRuntime::ReadAdvanceInputAt4B0550() const
{
    switch (ReadLe32(click_.bytesAt04.data() + 4))
    {
    case 1: if (input_.ReadPressedAtVslot04(5)) return true; break;
    case 2: if (input_.ReadPressedAtVslot04(0)) return true; break;
    case 3: if (input_.ReadPressedAtVslot04(8)) return true; break;
    default: break;
    }
    switch (ReadLe32(click_.bytesAt04.data()))
    {
    case 1: return mouse_.DownLatched(1) != 0;
    case 2: return mouse_.DownLatched(0) != 0;
    case 3: return mouse_.DownLatched(2) != 0;
    default: return false;
    }
}

void TextWindowAdvanceRuntime::RestoreEffectsAfterSkip()
{
    RestoreTextWindowEffectsAt4BC34B(restore_);
    if (!waitMark_.Enabled()) static_cast<void>(waitMark_.SetRequestedEnabled(true, 0));
}

void TextWindowAdvanceRuntime::MaintainVoiceAt4C0A70()
{
    // 4C0BDE..4C1181：只在所有门控成立且没有 Voice 播放时重进真实 4C2A40。
    if (save_.valueAt81 != 0 &&
        sound_.CaptureVoicesSlotAt3C()->baseAt00 != 0 &&
        VoiceAllowed() &&
        (unsave_.FlagsAt18() & 2u) == 0 &&
        !sound_.CaptureRootAfter413BE0().Resolve().effects.AnyVoiceSlotPlaying())
        static_cast<void>(playback_.PreparePlaybackAt4C2A40());
    // 4C1186：正常路径总是刷新，启动返回 false 也不能省略；异常不补调。
    sound_.CaptureRootAfter413BE0().Resolve().volume.Refresh();
}
bool TextWindowAdvanceRuntime::VoiceAllowed()
{
    const auto parameters = sound_.CaptureParametersAt4188C0();
    const auto* names = sound_.CaptureVoicesSlotAt3C();
    return parameters.Allowed(names);
}

bool TextWindowAdvanceRuntime::TickAt4BC260(const std::uint8_t inputMask)
{
    auto& state = window_.TextState();
    const auto remaining = state.autoWaitFramesAt5C.value();
    if (remaining != 0)
        state.autoWaitFramesAt5C = Signed32(static_cast<std::uint32_t>(remaining) - 1u);

    if (ReadSkipAt4AA540())
    {
        if ((save_.bytesAt34[4] & 2u) != 0) sound_.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots();
        // 原代码在 Stop 回调后再次读取 +38，不提前合并两个位判断。
        if ((save_.bytesAt34[4] & 0x20u) != 0) sound_.CaptureRootAfter413BE0().Resolve().effects.StopEffectsKeepingMappingsAt413270();
        playback_.ClearRootUpdateEnabledAt1B0After413BE0();
        return true;
    }

    if (state.skipAt60.value() != 0)
    {
        state.skipAt60 = std::uint8_t{0};
        RestoreEffectsAfterSkip();
    }
    if ((inputMask & 5u) != 0) return false;
    if ((inputMask & 2u) != 0 && (unsave_.FlagsAt18() & 2u) != 0)
    {
        if (ReadWheelAt4AD740() > 0) unsave_.FlagsAt1C() |= 2u;
        return false;
    }
    if (!CanAdvanceAt4AA3E0()) return false;

    if ((unsave_.FlagsAt18() & 2u) != 0 && state.autoWaitFramesAt5C.value() == 0)
    {
        if (sound_.CaptureVoicesSlotAt3C()->baseAt00 == 0) return true;
        if (!VoiceAllowed()) return true;
        if (!sound_.CaptureRootAfter413BE0().Resolve().effects.AnyVoiceSlotPlaying()) return true;
    }
    if (ReadWheelAt4AD740() > 0)
    {
        unsave_.FlagsAt1C() |= 2u;
        MaintainVoiceAt4C0A70();
        return false;
    }
    if (ReadAdvanceInputAt4B0550()) return true;
    if (ReadWheelAt4AD740() < 0) return true;
    MaintainVoiceAt4C0A70();
    return false;
}
}
