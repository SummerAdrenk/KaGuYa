#pragma once

#include <Platform/FramePump.h>

namespace KaGuYa::Start::Platform
{
class IFrameRuntimeServices
{
public:
    virtual ~IFrameRuntimeServices() = default;

    virtual void BeginFrameState() = 0;
    virtual void UpdateInput() = 0;
    // Start.exe:46D420，仅执行输入触发后的场景前置状态更新，不包含 SceneController/VM。
    virtual void UpdatePreSceneEngine() = 0;

    [[nodiscard]] virtual bool ModalProcessingEnabled() const = 0;
    [[nodiscard]] virtual bool AdvanceModal() = 0;
    virtual void DrawModal() = 0;

    [[nodiscard]] virtual bool HasPendingSceneWork() const = 0;
    virtual void RunSceneTasks() = 0;
    [[nodiscard]] virtual bool DirectMoviePresent() const = 0;
    virtual void RenderScreen() = 0;
    [[nodiscard]] virtual bool FrameWaitEnabled() const = 0;
    virtual void WaitForFrame() = 0;

    [[nodiscard]] virtual bool ExitRequested() const = 0;
};

class FrameRuntimeCoordinator final : public IFrameRuntime
{
public:
    explicit FrameRuntimeCoordinator(IFrameRuntimeServices& services) noexcept;

    [[nodiscard]] bool TickFrame() override;
    [[nodiscard]] bool ExitRequested() const override;

private:
    IFrameRuntimeServices& services_;
};
}
