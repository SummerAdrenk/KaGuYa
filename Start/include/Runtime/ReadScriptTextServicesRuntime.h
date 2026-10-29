#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class TextWindowFormatRuntime;
class TextWindowPlaybackRuntime;
class TextWindowAdvanceRuntime;
class ITextWindowSoundSource;
class WaitMarkRuntime;
class ScrScriptRuntimeState;
class IScrResourceNameSource;
// 4B4397..4B43B3同一调用核，独立于字形/声音/窗口服务；负ID也保留名字源捕获。
[[nodiscard]] const char* ReadCurrentScriptNameForErrorAt4B4397(
    const ScrScriptRuntimeState&,const IScrResourceNameSource&);

class IReadScriptTextServices
{
public:
    virtual ~IReadScriptTextServices() = default;
    virtual void SelectFormatAt4C2740(std::int32_t index, bool initialize) = 0;
    [[nodiscard]] virtual std::int32_t TickTextAt4BB530(bool suppressAdvance) = 0;
    [[nodiscard]] virtual bool AdvanceTextAt4BC260(std::uint8_t inputMask) = 0;
    [[nodiscard]] virtual bool VoiceArrayAllowedAtSoundParams60() = 0;
    [[nodiscard]] virtual bool AnyVoicePlayingAt4196C0() = 0;
    virtual void StopVoiceAt4384D0() = 0;
    virtual void StopEffectsAt413270() = 0;
    [[nodiscard]] virtual bool ReplayVoiceAt42C4F0() = 0;
    virtual void AdjustEffectsAt4C2410() = 0;
    virtual void RefreshEffectsAt433F60() = 0;
    [[nodiscard]] virtual std::uint8_t ReadWaitMarkEnabledAt0C() = 0;
    virtual void WriteWaitMarkEnabledAt0C(std::uint8_t value) = 0;
    [[nodiscard]] virtual bool PollKeyboardAt47F4E0(std::uint32_t action) = 0;
    [[nodiscard]] virtual bool PollMouseAt4B0740() = 0;
    // 4B43B8借用nullable名称至417130两遍格式化结束；接口不复制或保活来源正文。
    [[nodiscard]] virtual const char* ReadCurrentScriptNameForError() = 0;
};

// 4B2730 的文字/声音/输入生产绑定；工具条、场景与根模态 owner 是另一组必需依赖。
class ReadScriptTextServicesRuntime final : public IReadScriptTextServices
{
public:
    ReadScriptTextServicesRuntime(TextWindowFormatRuntime& format,
        TextWindowPlaybackRuntime& playback, TextWindowAdvanceRuntime& advance,
        ITextWindowSoundSource& sound, WaitMarkRuntime& waitMark,
        Platform::KeyboardView keyboard, Platform::MouseView mouse,
        const ScrScriptRuntimeState& script, const IScrResourceNameSource& names) noexcept;

    void SelectFormatAt4C2740(std::int32_t index, bool initialize) override;
    [[nodiscard]] std::int32_t TickTextAt4BB530(bool suppressAdvance) override;
    [[nodiscard]] bool AdvanceTextAt4BC260(std::uint8_t inputMask) override;
    [[nodiscard]] bool VoiceArrayAllowedAtSoundParams60() override;
    [[nodiscard]] bool AnyVoicePlayingAt4196C0() override;
    void StopVoiceAt4384D0() override;
    void StopEffectsAt413270() override;
    [[nodiscard]] bool ReplayVoiceAt42C4F0() override;
    void AdjustEffectsAt4C2410() override;
    void RefreshEffectsAt433F60() override;
    [[nodiscard]] std::uint8_t ReadWaitMarkEnabledAt0C() override;
    void WriteWaitMarkEnabledAt0C(std::uint8_t value) override;
    [[nodiscard]] bool PollKeyboardAt47F4E0(std::uint32_t action) override;
    [[nodiscard]] bool PollMouseAt4B0740() override;
    [[nodiscard]] const char* ReadCurrentScriptNameForError() override;

private:
    TextWindowFormatRuntime& format_;
    TextWindowPlaybackRuntime& playback_;
    TextWindowAdvanceRuntime& advance_;
    ITextWindowSoundSource& sound_;
    WaitMarkRuntime& waitMark_;
    GameGuiInputStateRuntime input_;
    Platform::MouseView mouse_;
    const ScrScriptRuntimeState& script_;
    const IScrResourceNameSource& names_;
};
}
