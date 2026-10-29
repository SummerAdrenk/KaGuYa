#include <Runtime/RootReadScriptYieldNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/ProgramFixedActionRuntime.h>
#include <Runtime/ScrParamsResetRuntime.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/GameSceneRoutes.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
RootRuntimePayload& Capture(RootRuntimeOwner& owner)
{owner.EnsureCreatedAt413BE0();return *owner.ReadPublishedAt6198C4();}
SceneManagerView Scenes(RootRuntimePayload& root)
{return SceneManagerView(ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(root.SceneManagerSlotAt30()),RequireSceneManagerNativeServices());}
}
RootReadScriptYieldNative::RootReadScriptYieldNative(RootRuntimeOwner& root,RootScriptRecoveryRuntime& recovery,
    IEnvironmentRootMemory& memory,IRuntimeStringManagerSource& strings)noexcept
    :root_(root),recovery_(recovery),memory_(memory),strings_(strings){}
void RootReadScriptYieldNative::PrepareCurrentScript()
{
    // 该成员仅属于旧受控DispatchOneStep；原4BFFD0没有preparer，误接必须暴露。
    throw std::logic_error("Native 4BFFD0 must not call the controlled script preparer");
}
void RootReadScriptYieldNative::CleanupCurrentEnvironment(){recovery_.CleanupCurrentEnvironment();}
void RootReadScriptYieldNative::ResetAt46CE30()
{auto& params=Capture(root_).ParamsAt47BDB0();ScrParamsResetRuntime::ResetCapturedAt46CE30(params,memory_,strings_);}
void RootReadScriptYieldNative::ResetAt4C1760(){Scenes(Capture(root_)).PopThenActivatePendingAt4C1760();}
void RootReadScriptYieldNative::ExecuteFixedAction(ScrVmFixedAction action)
{
    const auto name=GameFixedActionSceneName(action);
    Scenes(Capture(root_)).PushActiveScene(name.data());
}
}
