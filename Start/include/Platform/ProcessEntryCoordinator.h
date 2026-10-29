#pragma once

#include <Windows.h>

#include <string>

namespace KaGuYa::Start::Platform
{
class IProcessEntryApi
{
public:
    virtual ~IProcessEntryApi() = default;

    virtual void InitializeComApartment() = 0;
    virtual void PrimeDefaultWindowProcedure() = 0;
    virtual void InitializeCommonControls() = 0;
    [[nodiscard]] virtual HANDLE CreateOwnedMutex(const char* name) = 0;
    [[nodiscard]] virtual DWORD ReadLastError() const = 0;
    virtual void ReleaseOwnedMutex(HANDLE mutex) noexcept = 0;
    virtual void CloseKernelHandle(HANDLE handle) noexcept = 0;
    virtual void UninitializeComApartment() noexcept = 0;
};

class Win32ProcessEntryApi final : public IProcessEntryApi
{
public:
    void InitializeComApartment() override;
    void PrimeDefaultWindowProcedure() override;
    void InitializeCommonControls() override;
    [[nodiscard]] HANDLE CreateOwnedMutex(const char* name) override;
    [[nodiscard]] DWORD ReadLastError() const override;
    void ReleaseOwnedMutex(HANDLE mutex) noexcept override;
    void CloseKernelHandle(HANDLE handle) noexcept override;
    void UninitializeComApartment() noexcept override;
};

class IProcessEntryServices
{
public:
    virtual ~IProcessEntryServices() = default;

    virtual void InitializeFramework(HINSTANCE moduleInstance) = 0;
    virtual void LoadRuntimeConfiguration(
        HINSTANCE moduleInstance,
        HINSTANCE previousInstance,
        LPSTR commandLine,
        int showCommand) = 0;
    [[nodiscard]] virtual bool HasRequiredRuntimeProfile() const = 0;
    [[nodiscard]] virtual bool HasRequiredEnvironmentSlotZero() const = 0;
    virtual void ShowMissingRuntimeProfileError() = 0;
    virtual void ShowMissingEnvironmentError() = 0;
    [[nodiscard]] virtual std::string GameIdentifier() const = 0;
    [[nodiscard]] virtual int RunApplication(LPSTR commandLine, int showCommand) = 0;

    virtual void ReleaseWindowRegistry() noexcept = 0;
    virtual void ReleaseAuxiliaryRegistry() noexcept = 0;
    virtual void ReleaseModuleFactories() noexcept = 0;
    virtual void RunTerminationCallbacks() noexcept = 0;
    virtual void ShutdownFramework() noexcept = 0;
};

class ProcessEntryCoordinator final
{
public:
    explicit ProcessEntryCoordinator(IProcessEntryApi& api) noexcept;

    [[nodiscard]] int Run(
        IProcessEntryServices& services,
        HINSTANCE moduleInstance,
        HINSTANCE previousInstance,
        LPSTR commandLine,
        int showCommand);

private:
    IProcessEntryApi& api_;
};
}
