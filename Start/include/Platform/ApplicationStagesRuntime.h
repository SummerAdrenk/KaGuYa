#pragma once

#include <Platform/ApplicationCoordinator.h>
#include <Platform/WindowHost.h>
#include <Runtime/PostWindowLifecycle.h>
#include <Runtime/PreWindowInitialization.h>

namespace KaGuYa::Start::Runtime { class EnvironmentStartupRuntime; }

namespace KaGuYa::Start::Platform
{
class IMainWindowCreationRuntime
{
public:
    virtual ~IMainWindowCreationRuntime() = default;
    [[nodiscard]] virtual HWND CreateMainWindow() = 0;
};

class MainWindowCreationRuntime final : public IMainWindowCreationRuntime
{
public:
    MainWindowCreationRuntime(
        WindowHost& host,
        WindowClassRegistrar& registrar,
        HINSTANCE moduleInstance,
        HINSTANCE resourceInstance,
        const WindowCreationPlan& plan,
        HMENU childWindowIdentifier,
        HWND& publishedWindow) noexcept;

    [[nodiscard]] HWND CreateMainWindow() override;

private:
    WindowHost& host_;
    WindowClassRegistrar& registrar_;
    HINSTANCE moduleInstance_ = nullptr;
    HINSTANCE resourceInstance_ = nullptr;
    const WindowCreationPlan& plan_;
    HMENU childWindowIdentifier_ = nullptr;
    HWND& publishedWindow_;
};

// 生产入口按调用时的 Environment 构造 plan，并直接发布根 +2C，不持有旧配置快照。
class EnvironmentMainWindowCreationRuntime final : public IMainWindowCreationRuntime
{
public:
    EnvironmentMainWindowCreationRuntime(Runtime::EnvironmentStartupRuntime& environment,
        WindowHost& host, WindowClassRegistrar& registrar, HINSTANCE moduleInstance,
        HINSTANCE resourceInstance, const IWindowGeometryApi& geometry,
        HMENU childWindowIdentifier) noexcept;
    [[nodiscard]] HWND CreateMainWindow() override;

private:
    Runtime::EnvironmentStartupRuntime& environment_;
    WindowHost& host_;
    WindowClassRegistrar& registrar_;
    HINSTANCE moduleInstance_;
    HINSTANCE resourceInstance_;
    const IWindowGeometryApi& geometry_;
    HMENU childWindowIdentifier_;
};

class ApplicationStagesRuntime final : public IApplicationStages
{
public:
    ApplicationStagesRuntime(
        Runtime::PreWindowLifecycleState& preWindowState,
        Runtime::IPreWindowServices& preWindowServices,
        IMainWindowCreationRuntime& windowCreation,
        Runtime::IPostWindowRuntimeServices& postWindowServices) noexcept;

    [[nodiscard]] bool InitializeBeforeWindow() override;
    [[nodiscard]] HWND CreateMainWindow() override;
    [[nodiscard]] bool InitializeAfterWindow() override;
    void ReleaseSubsystems() override;

    [[nodiscard]] const Runtime::PostWindowLifecycleState& PostWindowState() const noexcept
    {
        return postWindowLifecycle_.State();
    }

private:
    Runtime::PreWindowLifecycleState& preWindowState_;
    Runtime::IPreWindowServices& preWindowServices_;
    IMainWindowCreationRuntime& windowCreation_;
    Runtime::IPostWindowRuntimeServices& postWindowServices_;
    Runtime::PreWindowInitializer preWindowInitializer_;
    Runtime::PostWindowLifecycle postWindowLifecycle_;
};
}
