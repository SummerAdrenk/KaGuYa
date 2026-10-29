#include <Platform/ProcessEntryCoordinator.h>

#include <CommCtrl.h>
#include <Objbase.h>

namespace KaGuYa::Start::Platform
{
namespace
{
class OwnedMutex final
{
public:
    OwnedMutex(IProcessEntryApi& api, const HANDLE handle) noexcept
        : api_(api), handle_(handle)
    {
    }

    ~OwnedMutex()
    {
        if (handle_ != nullptr)
        {
            // 43D848..43D860 即使 CreateMutexA 报已存在，也照样尝试 Release 后 Close。
            api_.ReleaseOwnedMutex(handle_);
            api_.CloseKernelHandle(handle_);
        }
    }

    OwnedMutex(const OwnedMutex&) = delete;
    OwnedMutex& operator=(const OwnedMutex&) = delete;

private:
    IProcessEntryApi& api_;
    HANDLE handle_ = nullptr;
};
}

void Win32ProcessEntryApi::InitializeComApartment()
{
    static_cast<void>(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
}

void Win32ProcessEntryApi::PrimeDefaultWindowProcedure()
{
    static_cast<void>(DefWindowProcA(nullptr, 0, 0, 0));
}

void Win32ProcessEntryApi::InitializeCommonControls()
{
    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_BAR_CLASSES;
    static_cast<void>(InitCommonControlsEx(&controls));
}

HANDLE Win32ProcessEntryApi::CreateOwnedMutex(const char* const name)
{
    return CreateMutexA(nullptr, TRUE, name);
}

DWORD Win32ProcessEntryApi::ReadLastError() const
{
    return GetLastError();
}

void Win32ProcessEntryApi::ReleaseOwnedMutex(const HANDLE mutex) noexcept
{
    static_cast<void>(ReleaseMutex(mutex));
}

void Win32ProcessEntryApi::CloseKernelHandle(const HANDLE handle) noexcept
{
    static_cast<void>(CloseHandle(handle));
}

void Win32ProcessEntryApi::UninitializeComApartment() noexcept
{
    CoUninitialize();
}

ProcessEntryCoordinator::ProcessEntryCoordinator(IProcessEntryApi& api) noexcept
    : api_(api)
{
}

int ProcessEntryCoordinator::Run(
    IProcessEntryServices& services,
    const HINSTANCE moduleInstance,
    const HINSTANCE previousInstance,
    LPSTR const commandLine,
    const int showCommand)
{
    // 43D6E6..43D711：原入口不检查这三个 Win32 调用的返回值。
    api_.InitializeComApartment();
    api_.PrimeDefaultWindowProcedure();
    api_.InitializeCommonControls();
    services.InitializeFramework(moduleInstance);
    services.LoadRuntimeConfiguration(
        moduleInstance, previousInstance, commandLine, showCommand);

    int result = 0;
    if (!services.HasRequiredRuntimeProfile())
    {
        services.ShowMissingRuntimeProfileError();
    }
    else if (!services.HasRequiredEnvironmentSlotZero())
    {
        services.ShowMissingEnvironmentError();
    }
    else
    {
        std::string mutexName = services.GameIdentifier();
        mutexName += "_Running";
        OwnedMutex mutex(api_, api_.CreateOwnedMutex(mutexName.c_str()));

        // 43D80F..43D81D 实际读取两次 LastError：第二次只在首值不是 183 时发生。
        const DWORD firstError = api_.ReadLastError();
        if (firstError != ERROR_ALREADY_EXISTS)
        {
            const DWORD secondError = api_.ReadLastError();
            if (secondError != ERROR_INVALID_HANDLE)
            {
                result = services.RunApplication(commandLine, showCommand);
            }
        }
    }

    // 43D866..43D983：这是显式固定顺序，不用成员析构顺序代替。
    services.ReleaseWindowRegistry();
    services.ReleaseAuxiliaryRegistry();
    services.ReleaseModuleFactories();
    services.RunTerminationCallbacks();
    services.ShutdownFramework();
    api_.UninitializeComApartment();
    return result;
}
}
