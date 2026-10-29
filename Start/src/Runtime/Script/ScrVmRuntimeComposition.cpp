#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/RootRuntimeOwner.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
ScriptNativeBody& RequireScript(RootRuntimePayload& root)
{
    auto* body = root.ScriptAt47CD00();
    if (!body) throw std::logic_error("root CScript allocation returned null (modern safety boundary)");
    return *body;
}
}

ScrVmRuntimeStateBundle::ScrVmRuntimeStateBundle(RootRuntimePayload& root,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
    : ScrVmRuntimeStateBundle(RequireScript(root), root.ScriptPublication(),
          ScrVmParamsGenerationBinding::Borrowed(root.ParamsSlotAt1C0()), memory, strings)
{
}

void ScrVmRuntimeStateBundle::RequireScriptView(const ScrScriptRuntimeState& runtime) const
{
    if (&runtime.NativeBody() != &body_ || &runtime.Publication() != &publication_ ||
        !runtime.UsesParamsGenerationSlot(paramsGeneration))
        throw std::invalid_argument("CScript views must borrow the same body, publication and params binding");
}

ScrVmRuntimeComposition::ScrVmRuntimeComposition(
    ScrVmRuntimeStateBundle& state,
    const ScrVmRuntimeSources sources,
    const IScrResourceNameSource& resourceNames,
    const IScrVmVoiceNameSource& voiceNames,
    IScrVmSceneSource& sceneSource,
    IScrVmSceneReplaySource& sceneReplay,
    const ScrVmRuntimeBoundaries boundaries,
    const ScrVmRuntimeErrorSinks errorSinks,
    IScrVmProgramRandSource& randSource,
    IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
    : invalid_(errorSinks.command, &sources.execution, strings),
      assign_(state.values, errorSinks.assign, &sources.arithmetic),
      setFlag_(state.values, &sources.control),
      clearFlag_(state.values, &sources.control),
      end_(boundaries.fileCommands, boundaries.scriptSwitch, errorSinks.command),
      wait_(state.values, state.scriptStorage, &sources.control),
      update_(boundaries.updateValue, resourceNames, boundaries.update, state.scriptStorage,
          errorSinks.update, boundaries.fileCommands, errorSinks.command),
      music_(resourceNames, boundaries.musicName, boundaries.fileCommands, boundaries.musicSource),
      text_(state.scriptCommand,music_,boundaries.text,
          {boundaries.fileCommands, errorSinks.command, resourceNames,
              boundaries.soundGroups, boundaries.soundLeaves, boundaries.sound}),
      menu_(state.scriptCommand, music_, boundaries.menu,
          {boundaries.fileCommands, errorSinks.command, resourceNames,
              boundaries.soundGroups, boundaries.soundLeaves, boundaries.sound, boundaries.menuSource}),
      sound_(resourceNames, boundaries.sound, boundaries.fileCommands,
          errorSinks.command, boundaries.soundGroups, boundaries.soundLeaves),
      jump_(&sources.control),
      fileJump_(boundaries.fileCommands, boundaries.scriptSwitch, errorSinks.command),
      compare_(state.values, errorSinks.compare, &sources.arithmetic),
      ifTrue_(state.values, &sources.control),
      ifFalse_(state.values, &sources.control),
      fileCall_(boundaries.fileCommands, boundaries.scriptSwitch, errorSinks.command),
      fileReturn_(boundaries.fileCommands, boundaries.scriptSwitch, errorSinks.command),
      call_(state.localCallStack, &sources.control),
      return_(state.localCallStack, &sources.control),
      program_(state.scriptCommand, boundaries.program, randSource,
          {boundaries.fileCommands, errorSinks.command, boundaries.programMessages, resourceNames,
              boundaries.soundGroups, boundaries.soundLeaves, boundaries.sound, boundaries.update, music_}),
      title_(&sources.title),
      scene_(sceneReplay, sceneSource, errorSinks.scene, &sources.control),
      dateWindow_(boundaries.date, errorSinks.command),
      datePlaceReset_(boundaries.date, errorSinks.command),
      save_(sources.save),
      follow_(boundaries.fileCommands, boundaries.scriptSwitch, errorSinks.command),
      voice_(voiceNames, boundaries.fileCommands, errorSinks.command, boundaries.voiceSource),
      core_({
          &invalid_, &assign_, &setFlag_, &clearFlag_, &end_, &wait_, &update_,
          &text_, &menu_, &sound_, &music_, &jump_, &fileJump_, &compare_,
          &ifTrue_, &ifFalse_, &fileCall_, &fileReturn_, &call_, &return_,
          &program_, &title_, &scene_, &dateWindow_, &datePlaceReset_, &save_,
          &follow_, &voice_, &nop_}, state.NativeBody(), state.Publication(), &sources.execution, memory)
{
}

ScrVmRuntimeComposition::ScrVmRuntimeComposition(
    ScrVmRuntimeStateBundle& state,
    const ScrVmRuntimeSources sources,
    const IScrResourceNameSource& resourceNames,
    const IScrVmVoiceNameSource& voiceNames,
    const ScrVmExecutableProfile&,
    IScrVmSceneSource& sceneSource,
    IScrVmSceneReplaySource& sceneReplay,
    const ScrVmRuntimeBoundaries boundaries,
    const ScrVmRuntimeErrorSinks errorSinks,
    IScrVmProgramRandSource& randSource,
    IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
    : ScrVmRuntimeComposition(state, sources, resourceNames, voiceNames, sceneSource, sceneReplay,
          boundaries, errorSinks, randSource, memory, strings)
{
}

const ScrVmCore& ScrVmRuntimeComposition::Core() const noexcept
{
    return core_;
}
}
