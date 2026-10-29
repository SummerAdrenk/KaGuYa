#pragma once
#include <optional>
#include <Runtime/RootGaugeRuntime.h>

#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/DrawGuiNativeScope.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/LayerWindowDrawStateRuntime.h>
#include <Runtime/MessageWindowNativeStateSource.h>
#include <Runtime/TextWindowNativeColors.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/GuiModalNativeAdapters.h>
#include <Runtime/OpacityWindowStateServicesRuntime.h>
#include <Runtime/NativeInputRuntime.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
class EnvironmentPostWindowStartupRuntime;

// 原459650/4B5AC5的现场来源；构造只借用，不读取尚未初始化的根。
class RootDrawWindowSources final : public ILayerWindowConstructionSource,
    public ILayerWindowEffectNativeSource
{
public:
    RootDrawWindowSources(ParamsRuntimeRootOwner&,EnvironmentStartupRuntime&,NativeGraphicsRuntimeLibrary&) noexcept;
    PatternLayerScreenSize ReadScreenSizeAt47CC00() override;
    std::int32_t ReadStartupConfigCountAt45983C() override;
    std::uint8_t ReadStartupConfigByteAt45998B(std::size_t index) override;
    void ClearScreenIfLoadedAt4182F0() override;
    std::int32_t ReadProgramHeightAt4180C0() override;
    std::int32_t ReadProgramWidthAt4180C0() override;
private:
    ParamsRuntimeRootOwner& params_;
    EnvironmentStartupRuntime& environment_;
    NativeGraphicsRuntimeLibrary& graphics_;
};
class RootMessageWindowNativeSource final : public IMessageWindowNativeRootSource
{
public:
    explicit RootMessageWindowNativeSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    MessageWindowNativeRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};

struct RootDrawServiceDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    ScriptNamedValueNativeTables namedValues;
    ISettingTagErrorSource& settingErrors;
    IMessageWindowNativeConfigExpressions& expressions;
    IScrVmCommandErrorSink& scriptErrors;
    IGuiControlResourceLoadServices& pictures;
    ISpriteNativeResourceSource& sprites;
    const IScrResourceNameRootSource& resourceNames;
    const ToolDialogNativeProfile& toolProfile;
    const ToolDialogNativeRelation& toolRelations;
    const SelectDialogNativeProfile& selectProfile;
    const CountDownWindowResourceProfile& countdownProfile;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    const CountDownWindowNativeResourceProfile* nativeCountdownProfile = nullptr;
    const WaitMarkNativeProfile& waitProfile = MamatomoWaitMarkNativeProfile();
    const OpacityDialogNativeProfile& opacityProfile = MamatomoOpacityDialogNativeProfile();
    const DateWindowResourceProfile* dateProfile = nullptr;
    const CountDownWindowFiveFrameResourceProfile* fiveFrameCountdownProfile = nullptr;
    const CommandSelectionResourceProfile* commandProfile = nullptr;
    const TokimekiGaugeResourceProfile* tokimekiProfile = nullptr;
    const LoveHolicGaugesResourceProfile* loveHolicProfile = nullptr;
};

// 仅拥有宿主适配器和引用集合，不拥有任何原生Draw/Surface/Input/Environment正文。
// 外部依赖必须比本集合长寿；已绑定Post的图层必须先退役再销毁此集合。
// 默认Root八层沿同一套来源运行，资源/表达式算法仍由显式依赖提供，不暗建第二套owner。
class RootDrawServicesRuntime final
{
public:
    explicit RootDrawServicesRuntime(RootDrawServiceDependencies dependencies) noexcept;
    RootDrawServicesRuntime(const RootDrawServicesRuntime&)=delete;
    RootDrawServicesRuntime& operator=(const RootDrawServicesRuntime&)=delete;
    void Bind(EnvironmentPostWindowStartupRuntime& post) noexcept;
    IDrawManagerNativeRootSource& DrawSource() noexcept { return draws_; }
    PostWindowNativeLayerServices& Layers() noexcept { return layers_; }
    LayerWindowNativeServices& LayerRuntime() noexcept { return layerRuntime_; }
    DrawLayersNativeWindowServices& WindowEffects() noexcept { return windowEffects_; }
    DrawGuiNativeServices& Gui() noexcept { return gui_; }
    ToolWindowNativeInteractionServices& ToolInteraction() noexcept { return toolInteraction_; }
    IOpacityWindowInputSource& OpacityInput() noexcept { return opacityInput_; }
    IMessageWindowNativeStateSource& MessageState() noexcept { return messageState_; }
private:
    RootDrawManagerNativeSource draws_;
    RootDrawWindowSources windowSource_;
    RootMessageWindowNativeSource messageRoot_;
    MessageWindowNativeStateSource messageState_;
    MessageWindowNativeParamsSource messageParams_;
    MessageWindowNativeConfigServices config_;
    TextWindowNativeColorSource colors_;
    ColorTransWindowNativeClear clear_;
    ColorTransWindowNativeServices colorTrans_;
    MessageWindowNativePaneServices panes_;
    MessageWindowLayerNativeServices message_;
    EnvironmentToolWindowReplayNativeSource replay_;
    ToolWindowNativeServices tool_;
    TextWindowLayerNativeServices text_;
    EnvironmentWaitMarkNativeStateSource waitState_;
    WaitMarkNativeServices wait_;
    RootSelectDialogNativeSource selectionState_;
    SelectWindowNativeServices selection_;
    EnvironmentCountDownWindowNativeStateSource countdownState_;
    CountDownWindowNativeServices countdown_;
    RootDateWindowStateSource dateState_;
    std::optional<DateWindowNativeServices> date_;
    RootCommandSelectionSource commandSource_;
    std::optional<CommandSelectionNativeServices> command_;
    RootGaugeValueSource gaugeValues_;
    std::optional<TokimekiGaugeNativeServices> tokimeki_;
    std::optional<LoveHolicGaugesNativeServices> loveHolic_;
    LayerWindowDrawStateRuntime drawingState_;
    PatternLayerNativeRenderServices pattern_;
    LayerWindowNativeServices layerRuntime_;
    DrawLayersNativeWindowServices windowEffects_;
    EnvironmentToolWindowNativeInteractionSource toolState_;
    ToolWindowNativeInteractionServices toolInteraction_;
    GuiModalInputSource guiInput_;
    OpacityWindowStateServicesRuntime opacity_;
    OpacityWindowInputAggregateSource opacityInput_;
    DrawGuiNativeServices gui_;
    PostWindowNativeLayerServices layers_;
};
}
