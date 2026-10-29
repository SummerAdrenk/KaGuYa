#pragma once

#include <Platform/MainWindow.h>

namespace KaGuYa::Start::Runtime
{
class ISoundRuntimeLibrary;
class IGraphicsRuntimeCalls;

// 原420F90跨Sound/Graphics两个模块，不能按旧接口名称只绑定Graphics。
class MainWindowSubsystemBridge final : public Platform::IMainWindowSubsystemBridge
{
public:
    MainWindowSubsystemBridge(ISoundRuntimeLibrary& sound, IGraphicsRuntimeCalls& graphics) noexcept;
    void ActivateAppRestore(WPARAM active) override;
    void RenderReset() override;

private:
    ISoundRuntimeLibrary& sound_;
    IGraphicsRuntimeCalls& graphics_;
};
}
