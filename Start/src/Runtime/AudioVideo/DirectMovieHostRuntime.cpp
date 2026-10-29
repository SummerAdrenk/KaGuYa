#include <Runtime/DirectMovieHostRuntime.h>
#include <Runtime/WindowEnvironmentRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
DirectMovieHostRuntime::DirectMovieHostRuntime(const WindowEnvironmentRuntime& environment,
    const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
    decltype(&::MessageBoxA) messageBox) noexcept
    : environment_(&environment), modeSlot_(&modeSlot), messageBox_(messageBox)
{
}

DirectMovieHostRuntime::DirectMovieHostRuntime(const EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, decltype(&::MessageBoxA) messageBox) noexcept
    : startupEnvironment_(&environment), root_(&root), messageBox_(messageBox)
{
}

HWND DirectMovieHostRuntime::MainWindow() const
{
    // 4BC778只取本次HWND；此入口不引入413BE0同步。
    return startupEnvironment_ ? reinterpret_cast<HWND>(startupEnvironment_->CaptureAt4180C0().windowAt2C) : environment_->window;
}

WindowModeView DirectMovieHostRuntime::CurrentMode() const
{
    if (root_) root_->EnsureCreatedAt413BE0();
    if (root_) return root_->RequirePublished().WindowModeAt0C();
    const auto* mode = modeSlot_->get();
    // 原空槽随后非法访问；明确暴露缺失依赖，不退回窗口模式或默认矩形。
    if (!mode) throw std::logic_error("DirectMovie window mode owning slot is empty.");
    return mode->Borrow();
}

DirectMoviePosition DirectMovieHostRuntime::CurrentVideoPosition() const
{
    DirectMoviePosition result;
    // 4B0025同步后读mode byte；固定旧构造仍是显式输入，不隐式建立root。
    if (CurrentMode().ModeAt04() != 0)
    {
        // 4B0060同步后4B008C/94读x/y；先保存标量，不跨下次同步使用旧viewport。
        const auto& origin = CurrentMode().RequireViewportAt0C();
        const auto x = origin[0];
        const auto y = origin[1];
        // 4B009A再次同步，4B00C6/D1读取当前mode的width/height，不是RECT右下坐标。
        const auto& size = CurrentMode().RequireViewportAt0C();
        result.x = x;
        result.width = size[2];
        result.y = y;
        result.height = size[3];
    }
    else
    {
        // 4B00EF..4B0115：窗口模式取环境逻辑尺寸，原点为零。
        result.width = startupEnvironment_ ? std::bit_cast<long>(startupEnvironment_->WindowFields().widthAt18)
            : environment_->gameProfile.logicalWidth;
        result.height = startupEnvironment_ ? std::bit_cast<long>(startupEnvironment_->WindowFields().heightAt1C)
            : environment_->gameProfile.logicalHeight;
    }
    return result;
}

void DirectMovieHostRuntime::ShowDirectShowError(const char* messageCp932)
{
    // 420C5A/60先借用Environment+10字符指针，420C64再取+2C；没有局部CString保活。
    // 系统叶重入后不再访问旧caption，返回值不改变随后电影函数抛出的错误码。
    const auto* captured = startupEnvironment_ ? &startupEnvironment_->CaptureAt4180C0() : nullptr;
    const char* caption = captured ? captured->titleAt10.GetString()
        : environment_->gameProfile.gameTitle.c_str();
    const HWND window = captured ? reinterpret_cast<HWND>(captured->windowAt2C) : environment_->window;
    static_cast<void>(messageBox_(window, messageCp932, caption, MB_ICONERROR));
}
}
