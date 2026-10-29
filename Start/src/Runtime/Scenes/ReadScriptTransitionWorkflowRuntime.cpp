#include <Runtime/ReadScriptTransitionWorkflowRuntime.h>

#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/ReadScriptEffectOwnerNative.h>
#include <Runtime/ReadScriptEffectFactoryNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptTransitionWorkflowRuntime::ReadScriptTransitionWorkflowRuntime(
    IReadScriptEffectFactory& factory, ScrVmRuntimeStateBundle& bundle,
    const ScrVmExecutableProfile& profile, const SaveDataEnvironmentArchive& save,
    UnSaveDataRuntime& unsave, Platform::MouseView mouse,
    Platform::KeyboardView keyboard, DrawManagerLayerRuntime& manager,
    IReadScriptSoundWaitSource& sound, IGraphicsScreenUpdateApi& screen,
    std::uint8_t& rootByteAt1B0) noexcept
    : resources_(std::in_place), resourcesView_(&*resources_),
      updateEffect_(*resources_, bundle.scriptStorage, factory),
      suspendWait_(*resources_, bundle, profile, save, unsave, mouse, keyboard,
          manager, sound, screen, rootByteAt1B0)
{
}

ReadScriptTransitionWorkflowRuntime::ReadScriptTransitionWorkflowRuntime(ReadScriptEffectOwnerNative& owner,
    IReadScriptUpdateEffectActions& actions, IReadScriptWaitStateSource& state,
    IDrawLayersViewSource& draw, IReadScriptSoundWaitSource& sound,
    IGraphicsScreenUpdateApi& screen, PatternLayerNativeRenderServices& patterns,
    ReadScriptEffectFactoryNativeServices& nativeServices) noexcept
    : resourcesView_(&owner), nativeServices_(&nativeServices), updateEffect_(actions),
      suspendWait_(owner, state, draw, sound, screen, patterns) {}

bool ReadScriptTransitionWorkflowRuntime::RunUpdateEffect(ReadScriptSceneState& state)
{
    std::optional<ReadScriptEffectNativeCallScope> scope;
    if (nativeServices_) scope.emplace(*nativeServices_);
    return updateEffect_.RunAt4C4760(state);
}

bool ReadScriptTransitionWorkflowRuntime::RunSuspend(ReadScriptSceneState& state)
{
    std::optional<ReadScriptEffectNativeCallScope> scope;
    if (nativeServices_) scope.emplace(*nativeServices_);
    return suspendWait_.RunSuspendAt4AC4C0(state);
}

bool ReadScriptTransitionWorkflowRuntime::RunWait(ReadScriptSceneState& state)
{
    std::optional<ReadScriptEffectNativeCallScope> scope;
    if (nativeServices_) scope.emplace(*nativeServices_);
    return suspendWait_.RunWaitAt4C4FC0(state);
}

void ReadScriptTransitionWorkflowRuntime::ReleaseResourcesAt4C0990()
{
    std::optional<ReadScriptEffectNativeCallScope> scope;
    if (nativeServices_) scope.emplace(*nativeServices_);
    resourcesView_->ReleaseResourcesAt4C0990();
}

ReadScriptEffectOwnerRuntime& ReadScriptTransitionWorkflowRuntime::Resources()
{
    if (!resources_) throw std::logic_error("Native workflow has no modern effect owner");
    return *resources_;
}
}
