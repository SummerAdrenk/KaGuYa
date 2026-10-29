#pragma once

#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/GuiDialogNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct MusicNavigationRecord final
{
    std::int32_t input;
    const GuiDialogNavigationMapping* pairs;
    std::int32_t first;
    std::int32_t repeat;
};

// 各游戏的原表只提供数据；GUI、选择状态、音乐播放与页面转场共用同一实现。
struct MusicResourceProfile final
{
    std::span<const char* const> backgrounds;
    SaveLoadMessageBoxCheckBoxDescriptorNative48h checkbox;
    std::span<const POINT> positions;
    std::span<const MusicNavigationRecord> navigation;
    std::span<const char* const> songs;
    const char* exitCue = nullptr;
    enum class Presentation : std::uint8_t { TiledCheckboxes, IndividualStateImages };
    Presentation presentation = Presentation::TiledCheckboxes;
    std::span<const SaveLoadMessageBoxCheckBoxDescriptorNative48h> trackButtons;
};

const MusicResourceProfile& MamatomoMusicResourceProfile() noexcept;
const MusicResourceProfile& GameMusicResourceProfile() noexcept;
}
