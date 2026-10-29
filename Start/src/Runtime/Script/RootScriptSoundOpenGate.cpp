#include <Runtime/RootScriptSoundOpenGate.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/TextWindowLayerNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class BorrowedTextSoundGateSource final : public ITextSoundGateNativeSource
{
public:
    BorrowedTextSoundGateSource(InputRootOwner& input, EnvironmentRootOwner& environment) noexcept
        : input_(input), environment_(environment) {}
    InputRootNativeBody24h* CaptureInputAt418210() override
    { return input_.CaptureAt418210(); }
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override
    { return environment_.CaptureAt4180C0(); }
private:
    InputRootOwner& input_;
    EnvironmentRootOwner& environment_;
};

std::uint8_t ReadSaveByteAt38(const EnvironmentRootStorage& environment)
{
    auto* const save = ResolveEnvironmentArchiveAt417990(environment, 1);
    // 4D3C54/4D6776/4D6453/4D19BB均为BYTE；空槽仍在+38处故障。
    return *reinterpret_cast<const volatile std::uint8_t*>(
        reinterpret_cast<std::uintptr_t>(save) + 0x38u);
}
}

bool ReadTextSoundGateNativeAt4D3B20(
    const TextWindowLayerNativeBodyE8h* text, ITextSoundGateNativeSource& source)
{
    auto* const input = source.CaptureInputAt418210();
    auto* const keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(
        input->keyboardAt18);
    // 4D3C13..4D3C2C固定end，非空后才读+10；不是逐项重读end的4198D0。
    if (ReadKeyboardSixHeldAt4AA573(*keyboard)) return true;

    auto* const firstEnvironment = source.CaptureEnvironmentAt4180C0(0x4D3C30u);
    auto* const unsave = ResolveEnvironmentArchiveAt417990(*firstEnvironment, 2);
    // 4D3C3C先读整个DWORD再&3，不能改成低BYTE或把空正文转成false。
    const auto flags = *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(unsave) + 0x14u);
    if ((flags & 3u) != 3u) return false;

    auto* const secondEnvironment = source.CaptureEnvironmentAt4180C0(0x4D3C48u);
    if ((ReadSaveByteAt38(*secondEnvironment) & 1u) == 0u) return true;
    // 4D3C5A只在上述两级条件成立时读固定receiver；早期null可合法短路。
    return static_cast<const volatile TextWindowLayerNativeBodyE8h*>(text)->unreadAt9A == 0;
}

RootScriptSoundOpenGate::RootScriptSoundOpenGate(IDrawManagerNativeRootSource& draws,
    InputRootOwner& input, EnvironmentRootOwner& environment) noexcept
    : draws_(draws), input_(input), environment_(environment) {}

bool RootScriptSoundOpenGate::ShouldSkipOpening(ScrVmSoundSlotGroup, std::size_t) const
{
    // 原4D3B20没有group/index参数；适配器不增加槽范围或分组业务分支。
    return ShouldSkipCurrentOpening();
}

bool RootScriptSoundOpenGate::ShouldSkipRemainingOpeningAt4D196A() const
{
    return ShouldSkipCurrentOpening();
}

bool RootScriptSoundOpenGate::ShouldSkipCurrentOpening() const
{
    auto* const draw = draws_.CaptureDrawManagerAfter413BE0();
    auto* const text = reinterpret_cast<const TextWindowLayerNativeBodyE8h*>(
        DrawLayerAt418C10(*draw, 3));
    BorrowedTextSoundGateSource source(input_, environment_);
    if (!ReadTextSoundGateNativeAt4D3B20(text, source)) return false;
    // 4D676A/4D6447/4D19AD：仅true后独立晚取Env，不沿用4D3C48的receiver。
    return (ReadSaveByteAt38(*environment_.CaptureAt4180C0()) & 0x20u) != 0u;
}
}
