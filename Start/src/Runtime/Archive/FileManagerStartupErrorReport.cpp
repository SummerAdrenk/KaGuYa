#include <Runtime/FileManagerStartupErrorReport.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NativeMessageBox final : public IFileInfoProgressConfirmation
{
public:
    int Confirm(HWND owner, const char* text, const char* caption, UINT flags) override
    {
        return ::MessageBoxA(owner, text, caption, flags);
    }
};
IFileInfoProgressConfirmation& NativeMessages()
{
    static NativeMessageBox messages;
    return messages;
}

// 原18号PE 5B3374：30个CP932字节；没有换行、文件名或额外后缀。
constexpr char FailureText[] =
    "\x83\x74\x83\x40\x83\x43\x83\x8B\x8F\xEE\x95\xF1\x82\xCC"
    "\x8F\x89\x8A\xFA\x89\xBB\x82\xC9\x8E\xB8\x94\x73\x81\x49\x81\x49";
static_assert(sizeof(FailureText) == 31);
}

FileManagerStartupErrorReport::FileManagerStartupErrorReport(RuntimeStringBorrow title)
    : FileManagerStartupErrorReport(title, NativeMessages()) {}

FileManagerStartupErrorReport::FileManagerStartupErrorReport(RuntimeStringBorrow title,
    IFileInfoProgressConfirmation& messages) noexcept : title_(title), messages_(messages) {}

void FileManagerStartupErrorReport::ReportScanFailure(HWND progressWindow)
{
    // 4816F0仅对null caption采用环境+10；root恰好传null，所以此处取当前默认标题。
    // API返回值不决定失败码：48AEC9的int1由root负责，本包装不捕获报告自身异常。
    static_cast<void>(messages_.Confirm(progressWindow, FailureText, title_.View().data(), 0x10u));
}
}
