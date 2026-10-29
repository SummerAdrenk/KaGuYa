#pragma once
#include <Runtime/LegacyDisplaySettings.h>

#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ScriptNamedValueNativeTables;
ScriptNamedValueNativeTables MamatomoScriptNamedValueTables() noexcept;
struct TextWindowTextDescriptor;
struct TitleResourceProfile;
struct StartupResourceProfile;
struct DateWindowResourceProfile;
struct CountDownWindowFiveFrameResourceProfile;
struct CommandSelectionResourceProfile;
struct TokimekiGaugeResourceProfile;
struct LoveHolicGaugesResourceProfile;
struct MusicResourceProfile;
struct SettingPage03ResourceProfile;
struct SettingShellResourceProfile;
struct SettingTextDisplayResourceProfile;
struct SettingOtherResourceProfile;
struct CgPageResourceProfile;
struct ReplayPageResourceProfile;
struct AppreciationSceneResourceProfile;
struct OptionCharResourceProfile;
struct CgOperationResourceProfile;
struct LogPageResourceProfile;
struct SaveLoadResourceProfile;
struct NameRegistrationResourceProfile;
struct GeneralConfirmResourceProfile;
struct ProgramResourceProfile;
struct CountResourceProfile;
struct LegacyMenuSelectionResourceProfile;
struct LegacyCharacterSelectionProfile;
struct LikabilityResourceProfile;
struct LegacyItemAcquisitionResourceProfile;
struct LegacyMapSelectionProfile;
struct CgUiButtonRecordNative28h;
std::span<const CgUiButtonRecordNative28h,12> MamatomoModeSelectButtonRecords() noexcept;
std::span<const TextWindowTextDescriptor, 2> MamatomoTextWindowNativeDescriptors() noexcept;
// 编译项只选择已审计的数据/行为合同，不复制VM、对象布局或资源解析器。
enum class TitleDialogFamily : std::uint8_t
{
    Unreviewed,
    LegacyComposedTitle,
    GuiTitleDialog,
};

enum class TitleNamePromptGate : std::uint8_t
{
    Unreviewed,
    EditableNameBits,
    SavedPreferenceOnly,
    PathSwitchAndSavedPreference,
};

// 名称标明数据归属，不代表另一套运行时实现。
enum class TitleButtonResources : std::uint8_t
{
    Unreviewed,
    Mamatomo,
    PureHolic,
    BunnysMama,
    BakuAne,
    Configured,
};

enum class TitlePhase5Recovery : std::uint8_t
{
    Unreviewed,
    ResetChildrenOnly,
    ResetChildrenAndParentInput,
};

struct TitleDialogProfile final
{
    TitleDialogFamily family;
    TitleNamePromptGate namePromptGate;
    TitleButtonResources buttonResources = TitleButtonResources::Unreviewed;
    TitlePhase5Recovery phase5Recovery = TitlePhase5Recovery::Unreviewed;
    const TitleResourceProfile* resourceProfile = nullptr;
};

struct GameBuildProfile final
{
    const char* slug;
    TitleDialogProfile title;
    enum class SoundSettings : std::uint8_t { Grouped, Categorized };
    SoundSettings soundSettings = SoundSettings::Grouped;
    enum class StartupResources : std::uint8_t { Mamatomo, BakuAne, Configured };
    StartupResources startupResources = StartupResources::Mamatomo;
    using ScriptNamesSource = ScriptNamedValueNativeTables (*)() noexcept;
    ScriptNamesSource scriptNames = &MamatomoScriptNamedValueTables;
    bool countdownEnabled = true;
    bool waitSkipResultEnabled = true;
    bool restartPointEnabled = true;
    using TextLayoutSource = std::span<const TextWindowTextDescriptor, 2> (*)() noexcept;
    TextLayoutSource textLayout = &MamatomoTextWindowNativeDescriptors;
    const StartupResourceProfile* startupProfile = nullptr;
    const DateWindowResourceProfile* dateWindow = nullptr;
    const CountDownWindowFiveFrameResourceProfile* fiveFrameCountdown = nullptr;
    const CommandSelectionResourceProfile* commandSelection = nullptr;
    const TokimekiGaugeResourceProfile* tokimekiGauge = nullptr;
    const LoveHolicGaugesResourceProfile* loveHolicGauges = nullptr;
    const MusicResourceProfile* musicResources = nullptr;
    bool envReadColorBytePresent = true;
    bool startupFullscreenControl = true;
    using ModeSelectDataSource = std::span<const CgUiButtonRecordNative28h,12> (*)() noexcept;
    ModeSelectDataSource modeSelectRecords = &MamatomoModeSelectButtonRecords;
    bool modeSelectExplicitPosition = true;
    const SettingPage03ResourceProfile* soundPage = nullptr;
    const SettingShellResourceProfile* settingsShell = nullptr;
    const SettingTextDisplayResourceProfile* settingsTextDisplay = nullptr;
    bool topMenuRequiresFrameLeft = true;
    LegacyDisplaySettingsFamily legacyDisplaySettings = LegacyDisplaySettingsFamily::Disabled;
    const SettingOtherResourceProfile* settingsOther = nullptr;
    const CgPageResourceProfile* cgPage = nullptr;
    const ReplayPageResourceProfile* replayPage = nullptr;
    const AppreciationSceneResourceProfile* appreciationScene = nullptr;
    const OptionCharResourceProfile* optionCharacters = nullptr;
    const CgOperationResourceProfile* cgOperation = nullptr;
    const LogPageResourceProfile* logPage = nullptr;
    const SaveLoadResourceProfile* saveLoad = nullptr;
    const NameRegistrationResourceProfile* nativeName = nullptr;
    const GeneralConfirmResourceProfile* generalConfirm = nullptr;
    const ProgramResourceProfile* programResources = nullptr;
    const CountResourceProfile* countResources = nullptr;
    const LegacyMenuSelectionResourceProfile* legacyMenuSelection = nullptr;
    const LegacyCharacterSelectionProfile* legacyCharacterSelection = nullptr;
    const LikabilityResourceProfile* likability = nullptr;
    const LegacyItemAcquisitionResourceProfile* legacyItemAcquisition = nullptr;
    const LegacyMapSelectionProfile* legacyMapSelection = nullptr;
};

// 每个EXE链接自己的只读定义；共享静态库不按编号重编，不运行时猜版本。
const GameBuildProfile& CompiledGameProfile() noexcept;
inline bool HasPrimaryGauge() noexcept
{ return CompiledGameProfile().tokimekiGauge || CompiledGameProfile().loveHolicGauges; }
inline std::uint32_t LastStartupDrawLayerIndex() noexcept
{ return (CompiledGameProfile().countdownEnabled ? 7u : 6u) + (CompiledGameProfile().dateWindow ? 1u : 0u)
    + (CompiledGameProfile().commandSelection ? 1u : 0u) + (HasPrimaryGauge() ? 1u : 0u)
    + (CompiledGameProfile().loveHolicGauges ? 1u : 0u); }
inline std::uint32_t StartupSelectionLayerIndex() noexcept
{ return CompiledGameProfile().dateWindow ? 6u : 5u; }
inline std::uint32_t StartupOpacityLayerIndex() noexcept
{ return StartupSelectionLayerIndex() + 1u; }
inline std::uint32_t StartupPrimaryGaugeLayerIndex() noexcept
{ return StartupOpacityLayerIndex() + 1u; }
inline std::uint32_t StartupCommandSelectionLayerIndex() noexcept
{ return StartupPrimaryGaugeLayerIndex() + (HasPrimaryGauge() ? 1u : 0u); }
inline std::uint32_t StartupMasoGaugeLayerIndex() noexcept
{ return StartupCommandSelectionLayerIndex() + (CompiledGameProfile().commandSelection ? 1u : 0u); }

struct StartupLayerRequest final { std::uint8_t requested; std::int32_t argument; };
inline StartupLayerRequest StartupPrepareRequest(std::uint32_t index, bool show, std::int32_t mode) noexcept
{
    // Ero Zemi 4444B0/444850：日期插在Wait之后，隐藏时独立消费mode。
    if (CompiledGameProfile().dateWindow && index == 5u)
        return {static_cast<std::uint8_t>(show), show ? 0 : mode};
    if (!show) return {0, 0};
    if (HasPrimaryGauge() && index == StartupPrimaryGaugeLayerIndex()) return {1, 0};
    if (CompiledGameProfile().loveHolicGauges && index == StartupMasoGaugeLayerIndex()) return {1, 0};
    if (CompiledGameProfile().commandSelection && index == StartupCommandSelectionLayerIndex())
        return {static_cast<std::uint8_t>(mode == 2), 0};
    return {static_cast<std::uint8_t>(index <= 3u
        || (index == StartupSelectionLayerIndex() && mode == 1)
        || (CompiledGameProfile().countdownEnabled && index == LastStartupDrawLayerIndex())), 0};
}
}
