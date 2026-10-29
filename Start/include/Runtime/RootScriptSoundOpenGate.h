#pragma once

#include <Runtime/ScrVmSoundHandler.h>

namespace KaGuYa::Start::Runtime
{
class IDrawManagerNativeRootSource;
class InputRootOwner;
class EnvironmentRootOwner;
struct InputRootNativeBody24h;
struct EnvironmentRootStorage;
struct TextWindowLayerNativeBodyE8h;

// 4D3B20只借当前原生根；caller标记取源地址，不参与业务判定。
class ITextSoundGateNativeSource
{
public:
    virtual ~ITextSoundGateNativeSource() = default;
    [[nodiscard]] virtual InputRootNativeBody24h* CaptureInputAt418210() = 0;
    [[nodiscard]] virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(
        std::uint32_t caller) = 0;
};

[[nodiscard]] bool ReadTextSoundGateNativeAt4D3B20(
    const TextWindowLayerNativeBodyE8h* text, ITextSoundGateNativeSource& source);

// 4D6758/4D6435/4D199D共用同一门控，不拥有Draw/Input/Environment或其正文。
class RootScriptSoundOpenGate final : public IScrVmSoundOpenGate
{
public:
    RootScriptSoundOpenGate(IDrawManagerNativeRootSource& draws, InputRootOwner& input,
        EnvironmentRootOwner& environment) noexcept;
    [[nodiscard]] bool ShouldSkipOpening(ScrVmSoundSlotGroup group,
        std::size_t groupIndex) const override;
    [[nodiscard]] bool ShouldSkipRemainingOpeningAt4D196A() const override;

private:
    [[nodiscard]] bool ShouldSkipCurrentOpening() const;
    IDrawManagerNativeRootSource& draws_;
    InputRootOwner& input_;
    EnvironmentRootOwner& environment_;
};
}
