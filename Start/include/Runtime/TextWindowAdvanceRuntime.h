#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class TextWindowLayerRuntime;
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class ClickInfoArchive;
class ITextWindowSoundSource;
class ITextWindowEffectRestoreSource;
class WaitMarkRuntime;
class ITextWindowPlaybackServices;

// 4AA3E0 读取根 +30 所持对象的 +1C；每次调用都借用当前根，不能缓存场景名。
class ITextWindowAdvanceServices
{
public:
    virtual ~ITextWindowAdvanceServices() = default;
    [[nodiscard]] virtual std::string ReadCurrentSceneNameAt1C() const = 0;
};

// CTextWin 完成后的等待入口；不拥有文本状态、不推进 4B2730 外层子状态。
class TextWindowAdvanceRuntime final
{
public:
    TextWindowAdvanceRuntime(TextWindowLayerRuntime& window,
        const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
        const ClickInfoArchive& click, Platform::MouseView mouse,
        Platform::KeyboardView keyboard, ITextWindowSoundSource& sound,
        ITextWindowEffectRestoreSource& restore,
        WaitMarkRuntime& waitMark, ITextWindowPlaybackServices& playback,
        ITextWindowAdvanceServices& services) noexcept;

    // 原栈参数只读低字节；true 为完成等待，false 为继续。异常原样传播。
    [[nodiscard]] bool TickAt4BC260(std::uint8_t inputMask);

private:
    [[nodiscard]] bool ReadSkipAt4AA540() const;
    [[nodiscard]] bool CanAdvanceAt4AA3E0() const;
    [[nodiscard]] bool ReadAdvanceInputAt4B0550() const;
    [[nodiscard]] std::int32_t ReadWheelAt4AD740() const noexcept;
    void RestoreEffectsAfterSkip();
    void MaintainVoiceAt4C0A70();
    bool VoiceAllowed();

    TextWindowLayerRuntime& window_;
    const SaveDataEnvironmentArchive& save_;
    UnSaveDataRuntime& unsave_;
    const ClickInfoArchive& click_;
    Platform::MouseView mouse_;
    GameGuiInputStateRuntime input_;
    ITextWindowSoundSource& sound_;
    ITextWindowEffectRestoreSource& restore_;
    WaitMarkRuntime& waitMark_;
    ITextWindowPlaybackServices& playback_;
    ITextWindowAdvanceServices& services_;
};
}
