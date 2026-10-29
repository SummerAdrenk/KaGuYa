#include <Runtime/RootSettingSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSettingSceneNativeSource::CaptureEnvironmentAt4180C0()
{ return dependencies_.environment.CaptureAt4180C0(); }
void RootSettingSceneNativeSource::EnsureRootAt413BE0()
{ dependencies_.root.EnsureCreatedAt413BE0(); }
void* RootSettingSceneNativeSource::RootAt6198C4()
{ return dependencies_.root.ReadPublishedAt6198C4(); }
YtlReferenceSlotRuntime& RootSettingSceneNativeSource::RootSceneManagerSlotAt30(void* fixedRoot)
{
    // Payload 不是原 21Ch Root；只借其唯一原生 30h Ref，不做 raw+30h 强转。
    return static_cast<RootRuntimePayload*>(fixedRoot)->SceneManagerSlotAt30();
}
void RootSettingSceneNativeSource::PopThenActivatePendingAt4C1760(void* nativeManager)
{
    // 与SaveLoad一致借调用期ManagerScope；固定receiver不重选根，也不缓存旧代服务。
    SceneManagerView(static_cast<SceneManagerNativeBody48h*>(nativeManager), RequireSceneManagerNativeServices())
        .PopThenActivatePendingAt4C1760();
}
GuiDialogNativeBody5Ch* RootSettingSceneNativeSource::ConstructSettingDialogAt4F1BF0(void* storage)
{ return dependencies_.dialog.ConstructSettingDialogAt4F1BF0(storage); }
}
