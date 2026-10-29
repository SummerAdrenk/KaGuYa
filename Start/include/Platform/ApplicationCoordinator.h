#pragma once

#include <Platform/MessageLoop.h>

#include <Windows.h>

namespace KaGuYa::Start::Platform
{
class IApplicationStages
{
public:
    virtual ~IApplicationStages() = default;

    [[nodiscard]] virtual bool InitializeBeforeWindow() = 0;
    [[nodiscard]] virtual HWND CreateMainWindow() = 0;
    [[nodiscard]] virtual bool InitializeAfterWindow() = 0;
    virtual void ReleaseSubsystems() = 0;
};

class IApplicationWindowApi
{
public:
    virtual ~IApplicationWindowApi() = default;
    virtual void PostClose(HWND window) = 0;
    virtual void Show(HWND window, int command) = 0;
};

class Win32ApplicationWindowApi final : public IApplicationWindowApi
{
public:
    void PostClose(HWND window) override;
    void Show(HWND window, int command) override;
};

class ApplicationCoordinator final
{
public:
    ApplicationCoordinator(
        MessageLoop& messageLoop,
        const IExitState& exitState,
        IApplicationWindowApi& windowApi) noexcept;

    [[nodiscard]] int Run(IApplicationStages& stages, int showCommand);

private:
    MessageLoop& messageLoop_;
    const IExitState& exitState_;
    IApplicationWindowApi& windowApi_;
};
}
