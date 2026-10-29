#pragma once

#include <Runtime/DirectMovieRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
struct WindowEnvironmentRuntime;
class WindowModeManagerRuntime;
class WindowModeView;
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

// 借用同一环境与root电影调用所用的模式槽，不缓存窗口/viewport快照。
class DirectMovieHostRuntime final : public IDirectMovieHost
{
public:
    DirectMovieHostRuntime(const WindowEnvironmentRuntime& environment,
        const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
        decltype(&::MessageBoxA) messageBox) noexcept;

    // 4AFFF0：借用稳定owner，每次413BE0后重取payload；所有依赖须长于本Host及电影。
    DirectMovieHostRuntime(const EnvironmentStartupRuntime& environment,
        RootRuntimeOwner& root, decltype(&::MessageBoxA) messageBox) noexcept;

    [[nodiscard]] HWND MainWindow() const override;
    [[nodiscard]] DirectMoviePosition CurrentVideoPosition() const override;
    void ShowDirectShowError(const char* messageCp932) override;

private:
    [[nodiscard]] WindowModeView CurrentMode() const;
    const WindowEnvironmentRuntime* environment_ = nullptr;
    const std::shared_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    const EnvironmentStartupRuntime* startupEnvironment_ = nullptr;
    RootRuntimeOwner* root_ = nullptr;
    decltype(&::MessageBoxA) messageBox_;
};
}
