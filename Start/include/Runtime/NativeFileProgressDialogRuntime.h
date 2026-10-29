#pragma once

#include <Windows.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 样本18 CProgressDlg；创建线程同时负责消息泵与析构，HWND不拥有本对象。
class NativeFileProgressDialogRuntime final
{
public:
    explicit NativeFileProgressDialogRuntime(HINSTANCE resourceInstance);
    ~NativeFileProgressDialogRuntime() noexcept;
    NativeFileProgressDialogRuntime(const NativeFileProgressDialogRuntime&) = delete;
    NativeFileProgressDialogRuntime& operator=(const NativeFileProgressDialogRuntime&) = delete;

    [[nodiscard]] HWND Window() const noexcept;
    void PumpAt485C70();
    [[nodiscard]] bool ConsumeCancelRequest() noexcept;

private:
    struct State;
    std::unique_ptr<State> state_;
};
}
