#pragma once
#include <Runtime/RuntimeStringBorrow.h>

#include <Runtime/NativeFileProgressDialogRuntime.h>
#include <Runtime/IFileInfoProgress.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IFileInfoProgressConfirmation
{
public:
    virtual ~IFileInfoProgressConfirmation() = default;
    virtual int Confirm(HWND owner, const char* textCp932, const char* titleCp932,
        UINT flags) = 0;
};

// title借自原环境+10，须覆盖本对象寿命；取消确认时重新取得其c_str，不缓存旧地址。
class FileInfoProgressRuntime final : public IFileInfoProgress
{
public:
    FileInfoProgressRuntime(HINSTANCE resourceInstance, RuntimeStringBorrow titleCp932);
    FileInfoProgressRuntime(HINSTANCE resourceInstance, RuntimeStringBorrow titleCp932,
        IFileInfoProgressConfirmation& confirmation);
    ~FileInfoProgressRuntime() noexcept override = default;
    FileInfoProgressRuntime(const FileInfoProgressRuntime&) = delete;
    FileInfoProgressRuntime& operator=(const FileInfoProgressRuntime&) = delete;

    [[nodiscard]] bool PollCancelAt47F0B0() override;
    LRESULT StepAt46E820() override;
    LRESULT AddRangeAt46A5A0(std::uint32_t delta) override;
    [[nodiscard]] HWND WindowAt47ADF0() const noexcept override;

private:
    NativeFileProgressDialogRuntime dialog_;
    RuntimeStringBorrow title_;
    IFileInfoProgressConfirmation& confirmation_;
};
}
