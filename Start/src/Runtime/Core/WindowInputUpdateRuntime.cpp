#include <Runtime/WindowInputUpdateRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootLogicalRectangleRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <stdexcept>
#include <bit>

namespace KaGuYa::Start::Runtime
{
WindowInputUpdateRuntime::WindowInputUpdateRuntime(Platform::IInputRuntime& input,
    const HWND& window, const Platform::ParamsGameProfile& profile,
    const WindowModeManagerRuntime& mode, const Platform::IMousePositionApi& mousePosition) noexcept
    : input_(input), window_(&window), profile_(&profile), mode_(&mode), mousePosition_(mousePosition)
{
}

WindowInputUpdateRuntime::WindowInputUpdateRuntime(Platform::IInputRuntime& input,
    const EnvironmentStartupRuntime& environment, const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
    IRootLogicalRectangleSynchronization& root, const Platform::IMousePositionApi& mousePosition) noexcept
    : input_(input), environment_(&environment), modeSlot_(&modeSlot), root_(&root), mousePosition_(mousePosition)
{
}

HWND WindowInputUpdateRuntime::Window() const
{
    return environment_ ? reinterpret_cast<HWND>(environment_->CaptureAt4180C0().windowAt2C) : *window_;
}

WindowInputUpdateRuntime::WindowInputUpdateRuntime(Platform::IInputRuntime& input,
    const EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
    const Platform::IMousePositionApi& mousePosition) noexcept
    : input_(input), environment_(&environment), root_(&root), owner_(&root), mousePosition_(mousePosition)
{
}

int WindowInputUpdateRuntime::Width() const
{
    return environment_ ? std::bit_cast<int>(environment_->WindowFields().widthAt18) : profile_->logicalWidth;
}

int WindowInputUpdateRuntime::Height() const
{
    return environment_ ? std::bit_cast<int>(environment_->WindowFields().heightAt1C) : profile_->logicalHeight;
}

WindowModeView WindowInputUpdateRuntime::CurrentMode() const
{
    if (!root_) return mode_->Borrow();
    root_->SynchronizeRuntimeAt413BE0();
    // 450B09/450B3F/450B7B各自读当前根，不能把首代根的mode槽借用跨换根缓存。
    if (owner_) return owner_->RequirePublished().WindowModeAt0C();
    const auto& slot = *modeSlot_;
    if (!slot) throw std::logic_error("450AA0 requires the current window mode owner");
    return slot->Borrow();
}

void WindowInputUpdateRuntime::UpdateAt450AA0()
{
    // producer固定本次input receiver，再按450AA0实际分支调用Source；不先预取Mouse。
    input_.UpdateWithSource(*this,mousePosition_);
}
std::uint8_t WindowInputUpdateRuntime::ReadModeAt450B01(){return CurrentMode().ModeAt04();}
const std::int32_t* WindowInputUpdateRuntime::ReadViewportAt450B3F(){return CurrentMode().RequireViewportAt0C().data();}
const std::int32_t* WindowInputUpdateRuntime::ReadViewportAt450B7B(){return CurrentMode().RequireViewportAt0C().data();}
}
