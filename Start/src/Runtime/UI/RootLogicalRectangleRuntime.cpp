#include <Runtime/RootLogicalRectangleRuntime.h>

#include <Runtime/WindowEnvironmentRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

#include <bit>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(LONG) == 4 && sizeof(int) == 4);

LONG Add(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(
        static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
}

LONG Subtract(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(
        static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));
}

// 4793B7/4794C0/479617/47970E：乘积截断为 DWORD，EDX=0 后无符号除。
LONG Scale(LONG factor, LONG value, LONG denominator)
{
    LONG result;
    __asm
    {
        mov eax, factor
        imul eax, value
        xor edx, edx
        div denominator
        mov result, eax
    }
    return result;
}
}

BOOL Win32RootLogicalRectanglePlatformApi::IsIconic(HWND window)
{
    return ::IsIconic(window);
}

BOOL Win32RootLogicalRectanglePlatformApi::GetClientRect(HWND window, RECT& output)
{
    return ::GetClientRect(window, &output);
}

RootLogicalRectangleRuntime::RootLogicalRectangleRuntime(
    const WindowEnvironmentRuntime& environment,
    const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
    IRootLogicalRectangleSynchronization& synchronization,
    IRootLogicalRectanglePlatformApi& platform) noexcept
    : environment_(&environment), modeSlot_(&modeSlot),
      synchronization_(synchronization), platform_(platform)
{
}

RootLogicalRectangleRuntime::RootLogicalRectangleRuntime(
    const EnvironmentStartupRuntime& environment,
    const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
    IRootLogicalRectangleSynchronization& synchronization,
    IRootLogicalRectanglePlatformApi& platform) noexcept
    : startupEnvironment_(&environment), uniqueModeSlot_(&modeSlot),
      synchronization_(synchronization), platform_(platform)
{
}

RootLogicalRectangleRuntime::RootLogicalRectangleRuntime(
    const EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
    IRootLogicalRectangleSynchronization& synchronization,
    IRootLogicalRectanglePlatformApi& platform) noexcept
    : startupEnvironment_(&environment), root_(&root), synchronization_(synchronization), platform_(platform) {}

HWND RootLogicalRectangleRuntime::CurrentWindow() const
{
    return startupEnvironment_ ? reinterpret_cast<HWND>(startupEnvironment_->CaptureAt4180C0().windowAt2C) : environment_->window;
}

LONG RootLogicalRectangleRuntime::CurrentWidth() const
{
    // 4791D3/4793A5/479610读取Environment+18，不重查Params，也不缓存WindowFields快照。
    return startupEnvironment_ ? std::bit_cast<LONG>(startupEnvironment_->WindowFields().widthAt18)
        : environment_->gameProfile.logicalWidth;
}

LONG RootLogicalRectangleRuntime::CurrentHeight() const
{
    // 4790C8/4794B5/479707读取Environment+1C，保留高bit而不是做数值裁剪。
    return startupEnvironment_ ? std::bit_cast<LONG>(startupEnvironment_->WindowFields().heightAt1C)
        : environment_->gameProfile.logicalHeight;
}

WindowModeView RootLogicalRectangleRuntime::CurrentMode() const
{
    if (root_) return root_->RequirePublished().WindowModeAt0C();
    // 原空 Ref 随后访问空正文；同现有窗口宿主，在该边界明确失败而非补 mode 0。
    const auto* mode = uniqueModeSlot_ ? uniqueModeSlot_->get() : modeSlot_->get();
    if (!mode)
        throw std::logic_error("478E50 requires the current window mode owner");
    return mode->Borrow();
}

RECT& RootLogicalRectangleRuntime::QueryAt478E50(RECT& output) const
{
    output = {};
    const HWND window = CurrentWindow();
    if (platform_.IsIconic(window) != FALSE)
        return output;

    // 4790C8 先读高度；4791D3 再读宽度，发布后才进入第一次根同步。
    const LONG initialHeight = CurrentHeight();
    output = RECT{0, 0, CurrentWidth(), initialHeight};
    synchronization_.SynchronizeRuntimeAt413BE0();
    if (CurrentMode().ModeAt04() == 0)
        return output;

    synchronization_.SynchronizeRuntimeAt413BE0();
    const auto& offsets = CurrentMode().RequireViewportAt0C();
    const LONG offsetY = offsets[1];
    const LONG offsetX = offsets[0];

    synchronization_.SynchronizeRuntimeAt413BE0();
    const auto& dimensions = CurrentMode().RequireViewportAt0C();
    const LONG modeHeight = dimensions[3];
    const LONG modeWidth = dimensions[2];

    // 原横向左边界也除以 modeHeight；不能按常规缩放公式纠正成 modeWidth。
    output.left = Subtract(0,
        Scale(CurrentWidth(), offsetX, modeHeight));
    RECT client{};
    output.top = Subtract(0,
        Scale(CurrentHeight(), offsetY, modeHeight));
    if (platform_.GetClientRect(window, client) == FALSE)
        return output;

    const LONG remainingX = Subtract(Subtract(Subtract(
        client.right, offsetX), modeWidth), client.left);
    const LONG remainingY = Subtract(Subtract(Subtract(
        client.bottom, modeHeight), offsetY), client.top);
    output.right = Add(output.right,
        Scale(CurrentWidth(), remainingX, modeWidth));
    output.bottom = Add(output.bottom,
        Scale(CurrentHeight(), remainingY, modeHeight));
    return output;
}
}
