#pragma once
#include <Runtime/RuntimeStringBorrow.h>

#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/FileInfoProgressRuntime.h>

namespace KaGuYa::Start::Runtime
{
// root专用48AEB4报告；标题借自4816F0的环境+10，每次报告重取字符指针。
class FileManagerStartupErrorReport final : public IFileManagerStartupErrorReport
{
public:
    explicit FileManagerStartupErrorReport(RuntimeStringBorrow environmentTitle);
    FileManagerStartupErrorReport(RuntimeStringBorrow environmentTitle,
        IFileInfoProgressConfirmation& messages) noexcept;
    void ReportScanFailure(HWND progressWindow) override;

private:
    RuntimeStringBorrow title_;
    IFileInfoProgressConfirmation& messages_;
};
}
