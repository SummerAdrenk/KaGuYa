#include <Runtime/SaveLoadSceneNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/InformationDisplayView.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
RootRuntimePayload& CaptureRoot(RootRuntimeOwner& root)
{ root.EnsureCreatedAt413BE0(); return *root.Published(); }
SceneManagerNativeBody48h* Scenes(RootRuntimePayload& root)
{ return ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(root.SceneManagerSlotAt30()); }
}
RootSaveLoadSceneNativeSource::RootSaveLoadSceneNativeSource(RootSaveLoadSceneNativeDependencies dependencies) noexcept
    : dependencies_(dependencies) {}
std::uint8_t RootSaveLoadSceneNativeSource::ReadRootByteAt1AF()
{ return CaptureRoot(dependencies_.root).ReadByteAt1AF(); }
EnvironmentRootStorage* RootSaveLoadSceneNativeSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{ return dependencies_.environment.CaptureAt4180C0(); }
std::uint8_t RootSaveLoadSceneNativeSource::ReadRootByteAt219()
{ return CaptureRoot(dependencies_.root).ReadByteAt219(); }
void RootSaveLoadSceneNativeSource::WriteRootByteAt219(const std::uint8_t& value, std::uint32_t)
{
    auto& receiver = CaptureRoot(dependencies_.root);
    receiver.WriteByteAt219(*static_cast<const volatile std::uint8_t*>(&value));
}
void RootSaveLoadSceneNativeSource::WriteRootSubobjectAt4AD230DwordAt10(std::uint32_t value)
{
    // 固定本次根，只借所选父的+10；不重选根、不复制Suppressed字段。
    CaptureRoot(dependencies_.root).PostWindow().BorrowInformationDisplayAt3C().SuppressedAt10() = value;
}
void RootSaveLoadSceneNativeSource::PopThenActivatePendingAt4C1760()
{
    auto* receiver = Scenes(CaptureRoot(dependencies_.root));
    SceneManagerView(receiver, RequireSceneManagerNativeServices()).PopThenActivatePendingAt4C1760();
}
void RootSaveLoadSceneNativeSource::ClearScenesAt4C1460()
{
    auto* receiver = Scenes(CaptureRoot(dependencies_.root));
    SceneManagerView(receiver, RequireSceneManagerNativeServices()).ClearScenes();
}
void RootSaveLoadSceneNativeSource::QueueAt47F920(const char* name)
{
    auto* receiver = Scenes(CaptureRoot(dependencies_.root));
    SceneManagerView(receiver, RequireSceneManagerNativeServices()).QueueScene(name);
}
void RootSaveLoadSceneNativeSource::ResetAndQueueTitleAt47E180()
{
    auto* receiver = Scenes(CaptureRoot(dependencies_.root));
    dependencies_.recovery.ResetAndQueueTitleAt47E180(SceneManagerView(receiver, RequireSceneManagerNativeServices()));
}
}
