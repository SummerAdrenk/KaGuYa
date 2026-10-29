#include <Runtime/RootDrawServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
RootDrawWindowSources::RootDrawWindowSources(ParamsRuntimeRootOwner& params,
    EnvironmentStartupRuntime& environment,NativeGraphicsRuntimeLibrary& graphics) noexcept
    :params_(params),environment_(environment),graphics_(graphics){}
PatternLayerScreenSize RootDrawWindowSources::ReadScreenSizeAt47CC00()
{
    const auto size=graphics_.ReadScreenSizeAt47CC00();return {size.cx,size.cy};
}
std::int32_t RootDrawWindowSources::ReadStartupConfigCountAt45983C()
{
    // 459812..459846：每轮重新解GS，计数来自control+10，不减bias、不要求读取完成。
    auto* game=params_.CaptureAt418480()->GameSystem().get();
    return std::bit_cast<std::int32_t>(game->startupConfigBytes.storage.Size());
}
std::uint8_t RootDrawWindowSources::ReadStartupConfigByteAt45998B(std::size_t index)
{
    // 459937..45998B：先bias+index，再乘stride；保持空base访问，不走有界operator[]。
    auto* game=params_.CaptureAt418480()->GameSystem().get();
    return *game->startupConfigBytes.storage.DataAt(static_cast<std::uint32_t>(index));
}
void RootDrawWindowSources::ClearScreenIfLoadedAt4182F0(){graphics_.ClearScreenIfLoaded(nullptr);}
std::int32_t RootDrawWindowSources::ReadProgramHeightAt4180C0()
{return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().heightAt1C);}
std::int32_t RootDrawWindowSources::ReadProgramWidthAt4180C0()
{return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().widthAt18);}

MessageWindowNativeRootReceiver RootMessageWindowNativeSource::CaptureAfterEnsureAt413BE0()
{
    // 4870F2跨498020固定root，随后才解析该root的CParams；不在Capture时提前创建CParams。
    root_.EnsureCreatedAt413BE0();
    return {root_.Published(),
        [](void* p){return &static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0();},
        [](void* p){return static_cast<RootRuntimePayload*>(p)->PostWindow().DrawManagerAt18();}};
}
RootDrawServicesRuntime::RootDrawServicesRuntime(RootDrawServiceDependencies d) noexcept
    :draws_(d.root),windowSource_(d.params,d.environment,d.graphics),messageRoot_(d.root),
      messageState_(messageRoot_,d.environment.RootSource(),d.namedValues,d.strings),
      messageParams_(d.params),config_{messageParams_,d.settingErrors,d.expressions,d.memory},
      colors_(d.params,d.settingErrors,d.expressions),clear_(d.graphics.SurfaceCalls()),
      colorTrans_{d.graphics,d.graphics,d.pictures,clear_,d.strings},panes_{colorTrans_,d.memory},
      message_{panes_,config_,messageState_,d.scriptErrors},
      replay_(d.environment.RootSource(),d.root,d.namedValues,d.scriptErrors,d.strings),
      tool_{d.toolProfile,d.toolRelations,d.pictures,replay_,d.graphics,d.strings,d.memory},
      text_{d.graphics,d.graphics,d.environment.RootSource(),messageState_,colors_,d.strings,d.memory},
      waitState_(d.environment.RootSource(),d.input.RootSource()),
      wait_{d.sprites,waitState_,d.graphics,d.graphics,d.strings,d.memory,d.waitProfile},selectionState_(d.root),
      selection_{d.selectProfile,selectionState_,d.pictures,colors_,d.environment.RootSource(),
        d.graphics,d.graphics,d.strings,d.memory},
      countdownState_(d.environment.RootSource(),d.root,d.namedValues,d.scriptErrors,d.strings),
      countdown_{d.sprites,countdownState_,d.countdownProfile,d.graphics,d.graphics,d.strings,d.memory,
          d.nativeCountdownProfile,d.fiveFrameCountdownProfile},
      dateState_(d.root,d.namedValues,d.scriptErrors,d.strings),
      date_(d.dateProfile ? std::optional<DateWindowNativeServices>{
          {d.pictures,dateState_,d.graphics,d.graphics,d.strings,*d.dateProfile}} : std::nullopt),
      commandSource_(d.root,d.environment.RootSource(),d.namedValues,d.scriptErrors,d.strings),
      command_(d.commandProfile ? std::optional<CommandSelectionNativeServices>{
          {*d.commandProfile,commandSource_,d.pictures,d.graphics,d.strings}} : std::nullopt),
      gaugeValues_(d.root,d.namedValues,d.scriptErrors,d.strings),
      tokimeki_(d.tokimekiProfile ? std::optional<TokimekiGaugeNativeServices>{
          {d.pictures,gaugeValues_,d.graphics,d.graphics,d.strings,*d.tokimekiProfile}} : std::nullopt),
      loveHolic_(d.loveHolicProfile ? std::optional<LoveHolicGaugesNativeServices>{
          {d.pictures,gaugeValues_,d.graphics,d.graphics,d.strings,*d.loveHolicProfile}} : std::nullopt),
      drawingState_(d.input,d.environment),pattern_{d.graphics,d.graphics,d.graphics,d.resourceNames,d.pictures,d.strings},
      layerRuntime_{pattern_,drawingState_},windowEffects_{drawingState_,windowSource_},
      toolState_(d.environment.RootSource(),d.input.RootSource(),d.root,draws_,d.namedValues,d.scriptErrors,d.strings),
      toolInteraction_{tool_,toolState_},guiInput_(d.input),opacity_(d.root,d.environment),opacityInput_(d.input),
      gui_{d.graphics.GuiCalls(),guiInput_,opacity_,opacity_},
      layers_{windowSource_,d.graphics,d.strings,d.memory,message_,tool_,text_,wait_,selection_,d.pictures,countdown_,d.opacityProfile,
          date_ ? &*date_ : nullptr,command_ ? &*command_ : nullptr,
          tokimeki_ ? &*tokimeki_ : nullptr,loveHolic_ ? &*loveHolic_ : nullptr}
{}
void RootDrawServicesRuntime::Bind(EnvironmentPostWindowStartupRuntime& post) noexcept
{post.BindNativeDrawServices(layers_,layerRuntime_,windowEffects_,gui_);}
}
