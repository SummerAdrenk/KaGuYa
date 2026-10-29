#include <Runtime/SelectWindowServicesRuntime.h>
#include <Runtime/MessageReadFlagsRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmMenuHandler.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <array>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SelectWindowConstructionRuntime::SelectWindowConstructionRuntime(const ParamsGameSystem& params,
    IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
    ProgramDwordReference width, ProgramDwordReference height, SelectWindowDialogDependencies dialog,
    ISettingTagErrorSource& errors) noexcept
    : fixedParams_(&params), parser_(parser), surfaces_(surfaces), width_(width), height_(height),
      dialog_(dialog), errors_(errors) {}
SelectWindowConstructionRuntime::SelectWindowConstructionRuntime(ParamsRuntimeRootOwner& paramsRoot,
    IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
    ProgramDwordReference width, ProgramDwordReference height, SelectWindowDialogDependencies dialog,
    ISettingTagErrorSource& errors) noexcept
    : paramsRoot_(&paramsRoot), parser_(parser), surfaces_(surfaces), width_(width), height_(height),
      dialog_(dialog), errors_(errors) {}
const ParamsGameSystem& SelectWindowConstructionRuntime::ReadGameSystem() const
{
    if (!paramsRoot_) return *fixedParams_;
    const auto& current = paramsRoot_->GetOrCreateAt418480().GameSystem();
    if (!current) throw std::logic_error("SelectWindow requires the current params GameSystem");
    return *current;
}
std::shared_ptr<const std::uint32_t> SelectWindowConstructionRuntime::ReadColorOverride(std::uint32_t index)
{
    // 5B2D28/38/48 的原 CP932 配置键：選択肢文字色、既選択文字色、選択肢飾り色。
    constexpr std::array<const char*, 3> names{
        "\x91\x49\x91\xF0\x8E\x88\x95\xB6\x8E\x9A\x90\x46",
        "\x8A\xF9\x91\x49\x91\xF0\x95\xB6\x8E\x9A\x90\x46",
        "\x91\x49\x91\xF0\x8E\x88\x8F\xFC\x82\xE8\x90\x46"};
    const auto* name = names.at(index);
    const auto& params = ReadGameSystem();
    auto* errorCString = errors_.CaptureAt47BCF0(); // 45CD24/45CE01/45CEDE
    std::uint32_t color;
    {
        const auto definition = CaptureSettingChildAt477E90(errorCString, params.settingTags[1].Native(), name);
        color = ReadSettingNodeRgbColorAt4797C0(errorCString, definition.Native(), "Value", parser_);
    }
    return std::make_shared<const std::uint32_t>(color);
}
std::shared_ptr<ISelectWindowDialog> SelectWindowConstructionRuntime::CreateDialogAt4F17E0()
    { return std::make_shared<SelectWindowDialogRuntime>(dialog_); }
std::unique_ptr<RuntimeGraphicsSurface> SelectWindowConstructionRuntime::AllocateSurface()
    { return std::make_unique<RuntimeGraphicsSurface>(surfaces_); }
std::int32_t SelectWindowConstructionRuntime::ReadProgramHeightAt1C() const { return height_.Read(); }
std::int32_t SelectWindowConstructionRuntime::ReadProgramWidthAt18() const { return width_.Read(); }

SelectWindowStateServicesRuntime::SelectWindowStateServicesRuntime(SelectWindowStateDependencies dependencies) noexcept
    : dependencies_(dependencies) {}
float SelectWindowStateServicesRuntime::ReadMessageVersion() const
    { return dependencies_.readerVersion.ReadVersionAt47DDE0(); }
std::size_t SelectWindowStateServicesRuntime::MenuCountAt64() const
    { return dependencies_.menu.CandidateCount(); }
std::int32_t SelectWindowStateServicesRuntime::MenuChoiceAt64(std::size_t ordinal) const
    { return dependencies_.menu.CandidateAt(ordinal); }
std::int32_t SelectWindowStateServicesRuntime::ReadChoiceAt418700(std::int32_t id, MessageTextSelection& output)
    { return dependencies_.choices.ReadChoiceAt418700(id, output, 0); }
std::int32_t SelectWindowStateServicesRuntime::ReadLegacyAt417C40(std::int32_t id, MessageFormatSelectionTarget output)
    { return dependencies_.formats.ReadFormatAt417C40(id, output, 0, false); }
bool SelectWindowStateServicesRuntime::HasProgramChoiceFlagsAt28() const
    { return dependencies_.script.ReadStateByteAt28() != 0; }
bool SelectWindowStateServicesRuntime::ReadProgramChoiceFlag(std::int32_t ordinal) const
{
    constexpr std::array<const char*, 4> names{"prog_param00", "prog_param01", "prog_param02", "prog_param03"};
    return dependencies_.values.ReadSigned(dependencies_.profile.ResolveNamedValue(
        names.at(static_cast<std::size_t>(ordinal)))) != 0;
}
bool SelectWindowStateServicesRuntime::HighlightChosenAt27() const
    { return dependencies_.script.ReadStateByteAt27() != 0; }
std::string SelectWindowStateServicesRuntime::ReadFontFaceAt619824() const
    { return dependencies_.rootFontFaceAt619824; }
std::uint32_t SelectWindowStateServicesRuntime::ReadSaveColorAt64() const
{
    const auto& bytes = dependencies_.save.bytesAt60;
    return std::uint32_t(bytes[4]) | (std::uint32_t(bytes[5]) << 8) |
        (std::uint32_t(bytes[6]) << 16) | (std::uint32_t(bytes[7]) << 24);
}
std::uint8_t SelectWindowStateServicesRuntime::ReadSaveByteAt60() const { return dependencies_.save.bytesAt60[0]; }
std::uint8_t SelectWindowStateServicesRuntime::ReadSaveByteAt61() const { return dependencies_.save.bytesAt60[1]; }
std::uint32_t SelectWindowStateServicesRuntime::MouseSecondaryCountdownAt2C() const
    { return dependencies_.mouse.SecondaryCountdown(); }
std::int32_t SelectWindowStateServicesRuntime::MouseWheelAt34() const { return dependencies_.mouse.WheelSteps(); }
void SelectWindowStateServicesRuntime::SetUnSaveFlagAt1C() { dependencies_.unsave.FlagsAt1C() |= 2u; }
void SelectWindowStateServicesRuntime::MarkChoiceAt4C29D0(std::int32_t id)
    { static_cast<void>(dependencies_.readFlags.MarkChoiceAt4C29D0(id, 1)); }
void SelectWindowStateServicesRuntime::RefreshEffectsAt433F60()
    { dependencies_.sound.CaptureRootAfter413BE0().Resolve().volume.Refresh(); }
}
