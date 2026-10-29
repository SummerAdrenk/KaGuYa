#pragma once

#include <Platform/FramePump.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class IGraphicsPresentNativeApi;
class RootPreSceneEngineUpdateRuntime;
class RootRuntimeOwner;
class RootRuntimePayload;
class WindowInputUpdateRuntime;

// 424E10/43E980 的生产入口：每次帧调用建立独立的4834F0借用上下文。
// 输入、Environment及DLL为进程共享服务，不复制到runtime root正文。
class RootFrameRuntime final : public Platform::IFrameRuntime, public Platform::IExitState
{
public:
    RootFrameRuntime(RootRuntimeOwner& root, EnvironmentStartupRuntime& environment,
        WindowInputUpdateRuntime& input, RootPreSceneEngineUpdateRuntime& preScene,
        IGraphicsPresentNativeApi& graphics) noexcept;

    [[nodiscard]] bool TickFrame() override;
    // 原this入口；root必须活过本次调用，不能在帧内销毁该receiver。
    [[nodiscard]] bool TickFrameAt4834F0(RootRuntimePayload& entry);
    [[nodiscard]] bool ExitRequested() const override;

private:
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    WindowInputUpdateRuntime& input_;
    RootPreSceneEngineUpdateRuntime& preScene_;
    IGraphicsPresentNativeApi& graphics_;
};
}
