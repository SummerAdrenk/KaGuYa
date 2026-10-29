#include <Runtime/RootPopupMenuServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootToolWindowServicesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScriptVoiceSource.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T& Require(T* object, const char* description)
{
    if (!object) throw std::logic_error(description);
    return *object;
}
DrawLayersView CurrentLayers(RootRuntimeOwner& root)
{
    const auto layers=root.RequirePublished().PostWindow().DrawLayersAt18();
    if(!layers)throw std::logic_error("Popup requires the published DrawManager (modern safety boundary)");
    return layers;
}
}
RootPopupMenuServicesRuntime::RootPopupMenuServicesRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, RootToolWindowServicesRuntime& tools,
    IPopupMenuPointerPlatformApi& pointer) noexcept
    : environment_(environment), root_(root), tools_(tools), pointer_(pointer) {}
UnSaveDataRuntime& RootPopupMenuServicesRuntime::UnSaveAt4180C0()
{ return environment_.Archives().UnSaveData(); }
std::int32_t RootPopupMenuServicesRuntime::TextGlyphCursorAt50()
{
    return CurrentLayers(root_).TextAtSlotThree().GlyphCursorAt50();
}
DrawToolView RootPopupMenuServicesRuntime::CurrentTool() const
{
    return tools_.BorrowToolAtSlotTwo(CurrentLayers(root_));
}
bool RootPopupMenuServicesRuntime::ToolWindowEnabledAtSlot2()
{ return CurrentTool().EnabledByteAt0C() != 0; }
void RootPopupMenuServicesRuntime::RefreshToolWindowAt434FB0(std::int32_t value)
{
    // 509245先固定本次工具receiver；434FB0各内部查询仍由tools逐次取源。
    CurrentTool().SetModeAt434FB0(value);
}
void RootPopupMenuServicesRuntime::WriteScenePendingCommandAt1C(std::string_view cp932)
{
    const auto scene=root_.RequirePublished().PostWindow().BorrowSceneManagerAt30();
    if(!scene.IsNative()&&!scene)throw std::logic_error("Popup requires the published controlled SceneManager");
    scene.WritePendingCommandAt1C(cp932);
}
void RootPopupMenuServicesRuntime::ReplayEnvironmentVoiceBatchAt42C4F0()
{
    // 509285先保存root，50928D读取Environment，50929F才从先前root取声音槽。
    root_.EnsureCreatedAt413BE0();
    auto& capturedRoot = root_.RequirePublished();
    const auto* const names = &UnSaveAt4180C0().ArrayReferenceAt3C().RawSlot();
    const auto sound = capturedRoot.PostWindow().VoiceReceiverAt24();
    if (!sound) throw std::logic_error("Popup requires the captured root SoundManager (modern safety boundary)");
    static_cast<void>(sound.PlayBatchRawAt42C4F0(names));
}
void RootPopupMenuServicesRuntime::PopSceneAt5328E0()
{
    const auto scene=root_.RequirePublished().PostWindow().BorrowSceneManagerAt30();
    if(!scene.IsNative()&&!scene)throw std::logic_error("Popup requires the published controlled SceneManager");
    scene.PopThenActivatePendingAt4C1760();
}
std::int32_t RootPopupMenuServicesRuntime::ReadGuiWidthAt18()
{ return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().widthAt18); }
std::int32_t RootPopupMenuServicesRuntime::ReadGuiHeightAt1C()
{ return std::bit_cast<std::int32_t>(environment_.CaptureAt4180C0().heightAt1C); }
void RootPopupMenuServicesRuntime::SynchronizePointerRootAt413BE0() { root_.EnsureCreatedAt413BE0(); }
WindowModeView RootPopupMenuServicesRuntime::PointerModeAt0C() const
{
    return root_.RequirePublished().WindowModeAt0C();
}
HWND RootPopupMenuServicesRuntime::PointerWindowAt2C()
{ return reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C); }
void RootPopupMenuServicesRuntime::MovePointerAt4512E0(POINT logical)
{ MovePopupPointerAt4512E0(*this, pointer_, logical); }
}
