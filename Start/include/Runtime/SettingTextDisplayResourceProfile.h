#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <array>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class SettingTextDisplayBlock
{ TextOptions, SkipOptions, Preview, ConfirmOptions, RightClick, DisplayMode, SaveOptions, SceneChoices };
enum class SettingTextBoolField
{
    Shadow, Antialias, HideMessageDuringVoice, StopVoiceOnAdvance, ReadColor,
    SkipUnread, ContinueSkip, SkipVoice, SkipEffectSound, ScreenEffects,
    ConfirmSave, ConfirmLoad, ConfirmQuickSave, ConfirmQuickLoad, ConfirmOverwrite,
    ConfirmExit, ConfirmTitle, ConfirmDefaults, ConfirmDeleteSave, ConfirmDeleteAll,
    SaveThumbnail, SaveHistory, AutoSaveChoice
};
enum class SettingTextCommit { Deferred, Immediate };
struct SettingTextBoolRow final
{
    SettingTextBoolField field;
    int id;
    const SaveLoadMessageBoxCheckBoxDescriptorNative48h* checkbox;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    bool checkedValue;
    int trueEvent, falseEvent;
    SettingTextCommit commit;
};
enum class SettingTextChoiceField { RightClick, Insertion, Oral };
struct SettingTextChoiceValue final { int event, value; };
struct SettingTextChoiceRow final
{
    SettingTextChoiceField field;
    int id;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const SettingTextChoiceValue> values;
    int otherReadEvent;
    bool preserveUnknownSelection;
};
struct SettingTextDisplayChild final { SettingTextDisplayBlock block; int id; };
struct SettingTextModeCommand final
{
    int event;
    unsigned command;
    bool updateModeButtons;
    bool fullscreen;
};
struct SettingTextDisplayMode final
{
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    const GuiStaticDescriptorNative24h* label;
    std::span<const SettingTextModeCommand> commands;
    int windowEvent, fullscreenEvent;
};
struct SettingTextPreview final
{
    std::array<GuiSliderDescriptorNative6Ch,3> sliders;
    std::array<GuiButtonDescriptorNative2Ch,2> buttons;
    std::array<const char*,2> images;
    std::array<int,3> minimum, maximum, step;
    std::string_view fontFace, sample;
    int fontHeight, fontWeight;
    unsigned textColor, outlineColor;
    unsigned char outlineWidth;
    std::array<unsigned char,2> renderModes;
    unsigned shadowColor, noShadowColor;
    POINT shadowOffset, noShadowOffset, textPosition;
    unsigned char characterSpacing, lineSpacing;
    bool vertical;
    // 仅记录来源，不控制解码器；只有18保留已验基线中的GBK定制示例。
    bool baselineSampleOverride;
};
struct SettingTextDisplayResourceProfile final
{
    std::array<std::span<const SettingTextDisplayChild>,2> pages;
    // 固定语义顺序：文本、跳过、确认、保存选项。
    std::array<std::span<const SettingTextBoolRow>,4> booleanGroups;
    SettingTextChoiceRow rightClick;
    std::span<const SettingTextChoiceRow> sceneChoices;
    SettingTextDisplayMode mode;
    SettingTextPreview preview;
    std::array<const GuiStaticDescriptorNative24h*,2> footers;
};
const SettingTextDisplayResourceProfile& CurrentSettingTextDisplayProfile();
}
