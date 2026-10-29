#pragma once
#include <Platform/InputViews.h>

#include <Runtime/SelectWindowLayerRuntime.h>
#include <Runtime/SelectWindowDialogRuntime.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/ProgramWindowFieldReferences.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class MessageReadFlagsRuntime;
class ITextWindowSoundSource;
class IVoiceRelationLookup;
class ScrVmValueTable;
class ScrVmExecutableProfile;
class ScrVmScriptCommandState;

class SelectWindowConstructionRuntime final : public ISelectWindowConstructionServices
{
public:
    // 固定输入入口：params 由调用方保活；读取不跟随 Params 根换代。
    SelectWindowConstructionRuntime(const ParamsGameSystem& params,
        IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
        ProgramDwordReference width, ProgramDwordReference height,
        SelectWindowDialogDependencies dialog,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner()) noexcept;
    // 45CCFE/45CDDB/45CEB8：每项颜色才取当前 Params+4，不缓存或保活 child。
    // 构造不解析 root；root 和其他借用依赖须长于服务，缺 child 在读取时明确失败。
    SelectWindowConstructionRuntime(ParamsRuntimeRootOwner& paramsRoot,
        IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
        ProgramDwordReference width, ProgramDwordReference height,
        SelectWindowDialogDependencies dialog,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner()) noexcept;
    std::shared_ptr<const std::uint32_t> ReadColorOverride(std::uint32_t index) override;
    std::shared_ptr<ISelectWindowDialog> CreateDialogAt4F17E0() override;
    std::unique_ptr<RuntimeGraphicsSurface> AllocateSurface() override;
    std::int32_t ReadProgramHeightAt1C() const override;
    std::int32_t ReadProgramWidthAt18() const override;
private:
    [[nodiscard]] const ParamsGameSystem& ReadGameSystem() const;
    const ParamsGameSystem* fixedParams_ = nullptr;
    ParamsRuntimeRootOwner* paramsRoot_ = nullptr;
    IMessageWindowConfigExpressionParser& parser_;
    IGraphicsSurfaceApi& surfaces_;
    ProgramDwordReference width_;
    ProgramDwordReference height_;
    SelectWindowDialogDependencies dialog_;
    ISettingTagErrorSource& errors_;
};

struct SelectWindowStateDependencies final
{
    const IMessageReaderVersion& readerVersion;
    MessageChoiceSelectionRuntime& choices;
    MessageFormatSelectionRuntime& formats;
    MessageReadFlagsRuntime& readFlags;
    const ScrVmMenuState& menu;
    const ScrVmScriptCommandState& script;
    const ScrVmExecutableProfile& profile;
    const ScrVmValueTable& values;
    const SaveDataEnvironmentArchive& save;
    UnSaveDataRuntime& unsave;
    const std::string& rootFontFaceAt619824;
    Platform::MouseView mouse;
    ITextWindowSoundSource& sound;
};

// 全部状态借用同一生产对象；不复制 menu、脚本、Save 或已读数组为私有快照。
class SelectWindowStateServicesRuntime final : public ISelectWindowStateServices
{
public:
    explicit SelectWindowStateServicesRuntime(SelectWindowStateDependencies dependencies) noexcept;
    float ReadMessageVersion() const override;
    std::size_t MenuCountAt64() const override;
    std::int32_t MenuChoiceAt64(std::size_t ordinal) const override;
    std::int32_t ReadChoiceAt418700(std::int32_t id, MessageTextSelection& output) override;
    std::int32_t ReadLegacyAt417C40(std::int32_t id, MessageFormatSelectionTarget output) override;
    bool HasProgramChoiceFlagsAt28() const override;
    bool ReadProgramChoiceFlag(std::int32_t ordinal) const override;
    bool HighlightChosenAt27() const override;
    std::string ReadFontFaceAt619824() const override;
    std::uint32_t ReadSaveColorAt64() const override;
    std::uint8_t ReadSaveByteAt60() const override;
    std::uint8_t ReadSaveByteAt61() const override;
    std::uint32_t MouseSecondaryCountdownAt2C() const override;
    std::int32_t MouseWheelAt34() const override;
    void SetUnSaveFlagAt1C() override;
    void MarkChoiceAt4C29D0(std::int32_t id) override;
    void RefreshEffectsAt433F60() override;
private:
    SelectWindowStateDependencies dependencies_;
};
}
