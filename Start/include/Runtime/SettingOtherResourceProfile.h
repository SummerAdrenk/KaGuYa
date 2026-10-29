#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/GameNamePolicy.h>
#include <array>
#include <cstdint>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class SettingOtherBlock { Startup, Name, Effects, VersionInfo, SceneChoices, Countdown };
enum class SettingOtherBool { SkipStartup, SkipNewGameName, EffectsEnabled, CountdownHalfAlpha };
enum class SettingOtherWord { InsertionChoice, OralChoice, CountdownPosition, CountdownSize, CountdownColor };
enum class SettingOtherCommit { Immediate, OnBlockEnd };
enum class SettingOtherNameEntry { None, LocalResource112, ParentResult };
enum class SettingOtherNameAccept { ClearSkip, ClearSkipThenDeferredRadioCommit, PreserveSkip };
enum class SettingOtherNameRefresh { RebuildName, ReloadPage };
enum class SettingOtherCapability { AlwaysAvailable, EnvironmentNameChange };
enum class SettingOtherPlacement { CenterXY, AbsoluteTopLeft, RightAligned };
enum class SettingOtherChildKind { Static, Checkbox, Radio, Button, Slider };
struct SettingOtherRadio final
{
    int dialogId;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
};
struct SettingOtherBoolBinding final
{
    SettingOtherBool field;
    int controlOrRadioId;
    bool checkboxStoredValueWhenChecked;
    int trueEvent, falseEvent;
    SettingOtherCommit commit;
};
struct SettingOtherChoice final { int event; std::uint16_t value; };
struct SettingOtherWordBinding final
{
    SettingOtherWord field;
    int radioId;
    std::span<const SettingOtherChoice> values;
    int otherReadEvent;
    bool preserveUnknownSelection;
};
struct SettingOtherChild final
{
    SettingOtherChildKind kind;
    unsigned index;
    // 倒计时块的静态标签和checkbox原本不参与包围矩形。
    bool includeInBounds = true;
};
struct SettingOtherBackendIndicator final { int control; std::uint32_t value; };
struct SettingOtherBlockProfile final
{
    SettingOtherBlock kind;
    int dialogId;
    std::span<const SettingOtherChild> creationOrder;
    std::span<const GuiStaticDescriptorNative24h> statics;
    std::span<const SaveLoadMessageBoxCheckBoxDescriptorNative48h> checks;
    std::span<const SettingOtherRadio> radios;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const GuiSliderDescriptorNative6Ch> sliders;
    std::span<const SettingOtherBoolBinding> booleans;
    std::span<const SettingOtherWordBinding> words;
    std::span<const int> gateRadios, gateControls;
    std::span<const SettingOtherBackendIndicator> backendIndicators;
    // Used only by blocks with sliders; the original effect speed is an index.
    std::array<int, 2> sliderRange{};
};
struct SettingOtherTextStyle final
{
    std::string_view fontCp932;
    int height, weight;
    std::uint8_t renderMode, characterSpacing, lineSpacing;
    bool vertical;
    unsigned normalColor, disabledColor, noShadowColor;
    POINT shadowOffset;
    // 只作原证据记录；当前现代Windows支持域不重造旧OS<5分支。
    bool legacyOsWidthPlus12 = false;
};
struct SettingOtherNameProfile final
{
    SettingOtherNameEntry entry = SettingOtherNameEntry::None;
    int buttonEvent = -1, parentResult = -1;
    SettingOtherNameAccept accept = SettingOtherNameAccept::PreserveSkip;
    SettingOtherCapability capability = SettingOtherCapability::AlwaysAvailable;
    GameNameByteLimits limits;
    SettingOtherTextStyle style;
    POINT center;
    int textStateControl = -1;
    // Radio ids disable the nested dialog; buttonEvent hides the button;
    // other ids set the direct label state to zero, at the original load/begin point.
    std::span<const int> disabledControlsAtLoad, disabledControlsAtBegin;
    SettingOtherNameRefresh refresh = SettingOtherNameRefresh::RebuildName;
};
struct SettingOtherVersionProfile final
{
    bool present;
    SettingOtherTextStyle style;
    SettingOtherPlacement placement;
    std::array<POINT,2> anchors; // 产品版本、文件版本，不能与姓名字段混用。
};
struct SettingOtherResourceProfile final
{
    std::span<const SettingOtherBlockProfile> blocks;
    const GuiStaticDescriptorNative24h* background; // 01 None，无虚构背景。
    SettingOtherNameProfile name;
    SettingOtherVersionProfile versions;
};
const SettingOtherResourceProfile& CurrentSettingOtherProfile();
const SettingOtherBlockProfile& SettingOtherBlockFor(SettingOtherBlock);
}
