#pragma once

#include <Platform/Input.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
struct WindowEnvironmentRuntime;
class WindowModeManagerRuntime;
class WindowModeView;
class SaveDataEnvironmentArchive;
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

class IWindowModeCommandPlatformApi
{
public:
    virtual ~IWindowModeCommandPlatformApi() = default;
    virtual BOOL PostWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    virtual int ShowMessage(HWND window, const char* text, const char* caption, UINT flags) = 0;
};

class Win32WindowModeCommandPlatformApi final : public IWindowModeCommandPlatformApi
{
public:
    BOOL PostWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;
    int ShowMessage(HWND window, const char* text, const char* caption, UINT flags) override;
};

// 42F040 的两个模式命令；由 MainFrame 成员映射直接调用，不注册到 CWinHook。
// 借用实际 owning 槽，在 SetMode 返回后重新解析；不缓存整个消息期间的旧对象。
class WindowModeCommandsRuntime final : public Platform::IWindowMessageHook
{
public:
    // 生产入口只借用source；每个原413BE0/4180C0取值点重新解析，不持有Ref。
    WindowModeCommandsRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        IWindowModeCommandPlatformApi& platform) noexcept;
    // 保留显式现代fixture入口，不作为生产原Ref槽的替代。
    WindowModeCommandsRuntime(WindowEnvironmentRuntime& environment,
        const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
        const std::shared_ptr<SaveDataEnvironmentArchive>& saveSlot,
        IWindowModeCommandPlatformApi& platform) noexcept;

    [[nodiscard]] bool Dispatch(unsigned int messageMapId, UINT message,
        WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    void PostInitialWindowModeCommandAt48AE19();

private:
    [[nodiscard]] WindowModeView CurrentMode() const;
    [[nodiscard]] SaveDataEnvironmentArchive& CurrentSave() const;
    [[nodiscard]] HWND CurrentWindow() const;
    EnvironmentStartupRuntime* environment_ = nullptr;
    RootRuntimeOwner* root_ = nullptr;
    WindowEnvironmentRuntime* modernEnvironment_ = nullptr;
    const std::shared_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    const std::shared_ptr<SaveDataEnvironmentArchive>* saveSlot_ = nullptr;
    IWindowModeCommandPlatformApi& platform_;
};
}
