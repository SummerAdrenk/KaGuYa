#include <Runtime/ProcessWinMainNativeRuntime.h>
#include <Runtime/ProcessRootApplicationNativeRuntime.h>
#include <Runtime/RootMainFrameNativeServicesRuntime.h>
#include <Runtime/ProcessEnvironmentRuntime.h>
#include <Runtime/ResourceVersionRuntime.h>
#include <Runtime/SystemDataEnvironmentValueRuntime.h>
#include <Runtime/WinMainNative.h>
#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct EntryRuntime final
{
    Win32EnvironmentPathApi paths;
    Win32ResourceVersionPlatformApi versions;
    Win32StartFileApi files;
    SystemDataEnvironmentLoggingDiagnostics diagnostics;
    Platform::Win32ProcessEntryApi platform;
    EnvironmentWinMainNativeSource source;
    EntryRuntime() noexcept
        : source(ProcessEnvironmentStartupRuntime(), paths, versions, files, diagnostics,
            ProcessRootMainFrameNativeServices()) {}
};
alignas(EntryRuntime) constinit std::byte entryRuntimeStorage[sizeof(EntryRuntime)]{};
EntryRuntime& Current() noexcept
{ return *std::launder(reinterpret_cast<EntryRuntime*>(entryRuntimeStorage)); }
}
IWinMainNativeSource& ConstructProcessWinMainNativeSourceAtEntry()
{ return (::new (static_cast<void*>(entryRuntimeStorage)) EntryRuntime)->source; }
int RunProcessWinMainNativeAt43D6E0(HINSTANCE image, HINSTANCE previous, LPSTR line, int show)
{
    auto& source = ConstructProcessWinMainNativeSourceAtEntry();
    // 不借旧DTO协调器，不给原43D6E0无caller-EH路径增加catch或自动Root释放。
    return RunWinMainNativeAt43D6E0(image, previous, line, show, source,
        Current().platform, ProcessWinMainNativeFramework());
}
}
