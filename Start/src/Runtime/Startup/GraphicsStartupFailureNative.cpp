#include <Runtime/GraphicsStartupFailureNative.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/FileInfoProgressRuntime.h>
#include <Runtime/IFileInfoProgress.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
[[noreturn]] void ReportGraphicsInitializationFailureNativeAt48AECE(
    NativeGraphicsRuntimeLibrary& graphics, FileManagerRootRuntime& fileManager,
    EnvironmentStartupRuntime& environment, IFileInfoProgressConfirmation& messages,
    IRuntimeStringManagerSource& strings)
{
    union Text final
    {
        RuntimeCString value;
        Text() noexcept {}
        ~Text() noexcept {}
    } text;
    InitializeEmptyRuntimeCString(text.value, strings); // 48AED1 -> 40B5D0；失败时无state7资格。
    struct Unwind final
    {
        RuntimeCString& value;
        ~Unwind() noexcept { DestroyRuntimeCStringAt40CD40(&value); }
    } unwind{text.value}; // 48AED6；59AC7D在展开时释放当前槽，不缓存header。
    FormatRuntimeCStringAt417130(text.value, "%s\n", graphics.ReadErrorAt479FE0());
    const auto* progress = fileManager.InitializationProgress();
    // 保留既有受限入口的null拒绝边界，不把它签为原版null正文的硬故障语义。
    if (!progress) throw std::logic_error("Graphics initialization report requires the root progress owner");
    const HWND window = progress->WindowAt47ADF0();
    auto& selected = environment.CaptureAt4180C0(); // 48AF09；4816F0的null caption选择该Env的+10。
    static_cast<void>(messages.Confirm(window, text.value.GetString(), selected.titleAt10.GetString(), 0x10u));
    auto* saved = static_cast<SaveDataEnvironmentArchive*>(environment.Archives().CaptureArchiveAt4180C0(1));
    saved->bytesAt04[0] = 0; // 48AF25..48AF3F：仅报告正常返回后重新捕获slot1并写byte。
    throw 1;
}
}
