#pragma once

#include <Runtime/ScrVmArithmeticHandlers.h>
#include <Runtime/ScrVmCallHandlers.h>
#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScrVmDatePlaceHandler.h>
#include <Runtime/ScrVmEndHandler.h>
#include <Runtime/ScrVmFileCallHandlers.h>
#include <Runtime/ScrVmFollowHandler.h>
#include <Runtime/ScrVmMenuHandler.h>
#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScrVmProgramHandler.h>
#include <Runtime/ScrVmSaveHandler.h>
#include <Runtime/ScrVmSceneHandler.h>
#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScrVmTextHandler.h>
#include <Runtime/ScrVmTitleHandler.h>
#include <Runtime/ScrVmUpdateHandler.h>
#include <Runtime/ScrVmVoiceHandler.h>
#include <Runtime/ScrVmWaitHandler.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
struct ScrVmRuntimeStateBundle final
{
private:
    std::unique_ptr<ScriptNativeBody> fixtureBody_;
    ScriptNativePublication fixturePublication_;
    ScriptNativeBody& body_;
    ScriptNativePublication& publication_;
public:
    explicit ScrVmRuntimeStateBundle(RootRuntimePayload& root,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 独立工具拥有明确的prefix-only正文；生产根使用下面的borrowed构造。
    explicit ScrVmRuntimeStateBundle(ScrVmParamsGenerationBinding binding = CreateScrVmParamsGenerationSlot())
        : fixtureBody_(std::make_unique<ScriptNativeBody>(ScriptNativeBody::PrefixOnlyTag{})),
          body_(*fixtureBody_), publication_(fixturePublication_), paramsGeneration(std::move(binding)),
          scriptStorage(body_, publication_), scriptCommand(body_, publication_) {}
    explicit ScrVmRuntimeStateBundle(ScriptNativeBody& body, ScriptNativePublication& publication,
        ScrVmParamsGenerationBinding binding,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource())
        : body_(body), publication_(publication), paramsGeneration(std::move(binding)),
          scriptStorage(body_, publication_, memory, strings), scriptCommand(body_, publication_) {}
    ScrVmRuntimeStateBundle(const ScrVmRuntimeStateBundle&) = delete;
    ScrVmRuntimeStateBundle& operator=(const ScrVmRuntimeStateBundle&) = delete;
    ScrVmRuntimeStateBundle(ScrVmRuntimeStateBundle&&) = delete;
    ScrVmRuntimeStateBundle& operator=(ScrVmRuntimeStateBundle&&) = delete;
    // 仅验证装配身份，不触发Params getter、分配或Ref增持。
    void RequireScriptView(const ScrScriptRuntimeState& runtime) const;
    [[nodiscard]] ScriptNativeBody& NativeBody() noexcept { return body_; }
    [[nodiscard]] ScriptNativePublication& Publication() noexcept { return publication_; }
    [[nodiscard]] ScrScriptRuntimeState CreateScriptView()
        { return ScrScriptRuntimeState(body_, publication_, paramsGeneration); }
    // 根装配须以{Borrowed(rootSlot)}构造；事后改槽不会重绑定已经创建的四个view。
    // 默认入口仍明确创建独立owning槽，不能用它代替root+1C0。
    ScrVmParamsGenerationBinding paramsGeneration =
        CreateScrVmParamsGenerationSlot();
    ScrVmValueTable values{paramsGeneration};
    ScrVmCallStack localCallStack{paramsGeneration};
    ScrVmFileCallStack fileCallStack{paramsGeneration};
    // 4C8D23..45只清内部Ref；29个handler登记后才由CScript构造边界发布正文。
    ScrVmScriptStorage scriptStorage;
    ScrVmScriptCommandState scriptCommand;
    ScrVmTextState text;
    ScrVmMenuState menu{paramsGeneration};
};

struct ScrVmRuntimeBoundaries final
{
    IScrVmScriptSwitchBoundary& scriptSwitch;
    IScrVmUpdateRuntimeBoundary& update;
    IScrVmTextRuntimeBoundary& text;
    IScrVmMenuLayerBoundary& menu;
    IScrVmSoundOpenGate& sound;
    IScrVmProgramRuntimeBoundary& program;
    IScrVmMusicNameWriter& musicName;
    IScrVmUpdateValueSource& updateValue;
    IScriptDateCommandSource& date;
    IScriptFileCommandSource& fileCommands;
    IScriptSoundGroupSource& soundGroups;
    IScriptSoundLeafSource& soundLeaves;
    IScriptMusicSource& musicSource;
    IScriptVoiceSource& voiceSource;
    IScriptMenuSource& menuSource;
    IScriptProgramMessageSource& programMessages;
};

struct ScrVmRuntimeErrorSinks final
{
    IScrVmCommandErrorSink& command;
    IScrVmCommandErrorSink& assign;
    IScrVmCommandErrorSink& compare;
    IScrVmUpdateCommandErrorSink& update;
    IScrVmSceneCommandErrorSink& scene;
};

// 五类来源均由装配者显式提供；不允许生产组合层退回各handler的独立夹具绑定。
// save必须借同一正文的command/runtime，所有来源须短于其实际root和view的生存期。
struct ScrVmRuntimeSources final
{
    IScriptExecutionRootSource& execution;
    IScriptControlCommandSource& control;
    IScriptArithmeticCommandSource& arithmetic;
    IScriptTitleCommandSource& title;
    IScriptSaveCommandSource& save;
};

class ScrVmRuntimeComposition final
{
public:
    // memory仅转发Core，strings仅转发invalid；不将Params依赖解释为全VM注入。
    ScrVmRuntimeComposition(
        ScrVmRuntimeStateBundle& state,
        ScrVmRuntimeSources sources,
        const IScrResourceNameSource& resourceNames,
        const IScrVmVoiceNameSource& voiceNames,
        IScrVmSceneSource& sceneSource,
        IScrVmSceneReplaySource& sceneReplay,
        ScrVmRuntimeBoundaries boundaries,
        ScrVmRuntimeErrorSinks errorSinks,
        IScrVmProgramRandSource& randSource,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

    // 兼容旧签名；全原生组合的SAVE始终使用sources.save，不再消费profile。
    ScrVmRuntimeComposition(
        ScrVmRuntimeStateBundle& state,
        ScrVmRuntimeSources sources,
        const IScrResourceNameSource& resourceNames,
        const IScrVmVoiceNameSource& voiceNames,
        const ScrVmExecutableProfile& executableProfile,
        IScrVmSceneSource& sceneSource,
        IScrVmSceneReplaySource& sceneReplay,
        ScrVmRuntimeBoundaries boundaries,
        ScrVmRuntimeErrorSinks errorSinks,
        IScrVmProgramRandSource& randSource,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

    [[nodiscard]] const ScrVmCore& Core() const noexcept;

private:
    ScrInvalidOpcodeHandler invalid_;
    ScrAssignInstructionHandler assign_;
    ScrSetFlagInstructionHandler setFlag_;
    ScrClearFlagInstructionHandler clearFlag_;
    ScrEndInstructionHandler end_;
    ScrWaitInstructionHandler wait_;
    ScrUpdateInstructionHandler update_;
    ScrMusicInstructionHandler music_;
    ScrTextInstructionHandler text_;
    ScrMenuInstructionHandler menu_;
    ScrSoundInstructionHandler sound_;
    ScrJumpInstructionHandler jump_;
    ScrFileJumpInstructionHandler fileJump_;
    ScrCompareInstructionHandler compare_;
    ScrIfTrueInstructionHandler ifTrue_;
    ScrIfFalseInstructionHandler ifFalse_;
    ScrFileCallInstructionHandler fileCall_;
    ScrFileReturnInstructionHandler fileReturn_;
    ScrCallInstructionHandler call_;
    ScrReturnInstructionHandler return_;
    ScrProgramInstructionHandler program_;
    ScrTitleInstructionHandler title_;
    ScrSceneInstructionHandler scene_;
    ScrDateWindowInstructionHandler dateWindow_;
    ScrDatePlaceResetInstructionHandler datePlaceReset_;
    ScrSaveInstructionHandler save_;
    ScrFollowJumpInstructionHandler follow_;
    ScrVoiceInstructionHandler voice_;
    ScrNopInstructionHandler nop_;
    ScrVmCore core_;
};
}
