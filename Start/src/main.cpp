#include <Logging/Logger.h>
#include <Runtime/ProcessRootApplicationNativeRuntime.h>
#include <Runtime/ProcessWinMainNativeRuntime.h>
#include <Windows.h>

// 原43D6E0接收ANSI命令行；不经UTF-8/wide转换改变原MBCS参数域。
int WINAPI WinMain(HINSTANCE image, HINSTANCE previous, LPSTR line, int show)
{
    static_assert(sizeof(void*) == 4, "Start requires the x86 ABI.");
    KaGuYa::Logging::Initialize(L"Start_RS");
    KaGuYa::Start::Runtime::ConstructProcessRootApplicationNativeRuntimeAtEntry();
    KaGuYa::Logging::Write(KaGuYa::Logging::Level::Info, "Start_RS: 进入原生WinMain运行链。");
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
        "Start_RS runtime locale: ACP=%u OEM=%u threadLCID=%08lX", GetACP(), GetOEMCP(), GetThreadLocale());
    const int result = KaGuYa::Start::Runtime::RunProcessWinMainNativeAt43D6E0(image, previous, line, show);
    KaGuYa::Logging::Shutdown();
    return result;
}
