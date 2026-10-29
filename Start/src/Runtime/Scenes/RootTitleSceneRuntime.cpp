#include <Runtime/RootTitleSceneRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/InformationDisplayView.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootTitleSceneRuntime::RootTitleSceneRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root) noexcept
    : environment_(environment), root_(root) {}
RootRuntimePayload& RootTitleSceneRuntime::CaptureRoot() const
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished();
}
SceneManagerView RootTitleSceneRuntime::CaptureScenes() const
{
    const auto scenes = CaptureRoot().PostWindow().BorrowSceneManagerAt30();
    if (!scenes.IsNative()&&!scenes) throw std::logic_error("Title accesses absent controlled SceneManager");
    return scenes;
}
void RootTitleSceneRuntime::ClearDescriptorTargetDwordAt10()
{
    const auto information = CaptureRoot().PostWindow().BorrowInformationDisplayAt3C();
    // 保留本旧typed宿主既有的空父安全边界；原生RootTitleSceneNativeSource不使用此守卫。
    if (!information) throw std::logic_error("Title accesses absent root+3C InformationDisplay (modern safety boundary)");
    information.SuppressedAt10() = 0;
}
std::uint32_t RootTitleSceneRuntime::GameInputStateModeAt04() const
{
    return environment_.Archives().UnSaveData().ModeAt04();
}
RuntimeCStringResult RootTitleSceneRuntime::CloneCurrentSceneNameAt414540() const
{
    return CaptureScenes().ClonePendingCommandAt414540();
}
bool RootTitleSceneRuntime::EvaluateInputPredicate47F650()
{
    // 4BB005的root同步仍发生；47F650只有action19出现按下边沿才读取slot2+09。
    GameGuiInputStateRuntime input(CaptureRoot().PostWindow().Input());
    if (!input.ReadPressedAtVslot04(19)) return false;
    return environment_.Archives().UnSaveData().BytesAt08()[1] != 0;
}
void RootTitleSceneRuntime::ClearCurrentSceneName()
{
    CaptureScenes().WritePendingCommandAt1C({});
}
void RootTitleSceneRuntime::SetRootByteAt1AF(std::uint8_t value)
{
    CaptureRoot().WriteByteAt1AF(value);
}
void RootTitleSceneRuntime::PushActiveScene(const char* name)
{
    CaptureScenes().PushActiveScene(name);
}
}
