#pragma once

#include <Windows.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
struct WindowEnvironmentRuntime;
class EnvironmentStartupRuntime;
class WindowModeManagerRuntime;
class WindowModeView;
class RootRuntimeOwner;

class IRootLogicalRectangleSynchronization
{
public:
    virtual ~IRootLogicalRectangleSynchronization() = default;
    virtual void SynchronizeRuntimeAt413BE0() = 0;
};

// 仅隔离原系统叶；实际环境与模式状态仍由生产对象提供。
class IRootLogicalRectanglePlatformApi
{
public:
    virtual ~IRootLogicalRectanglePlatformApi() = default;
    [[nodiscard]] virtual BOOL IsIconic(HWND window) = 0;
    [[nodiscard]] virtual BOOL GetClientRect(HWND window, RECT& output) = 0;
};

class Win32RootLogicalRectanglePlatformApi final
    : public IRootLogicalRectanglePlatformApi
{
public:
    [[nodiscard]] BOOL IsIconic(HWND window) override;
    [[nodiscard]] BOOL GetClientRect(HWND window, RECT& output) override;
};

class RootLogicalRectangleRuntime final
{
public:
    // 所有借用必须长于本对象；同步必需提供，不复制环境、模式槽或 viewport。
    RootLogicalRectangleRuntime(const WindowEnvironmentRuntime& environment,
        const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
        IRootLogicalRectangleSynchronization& synchronization,
        IRootLogicalRectanglePlatformApi& platform) noexcept;

    // 478E50：借用实际Environment及前置unique owning槽，构造不读HWND/尺寸/mode。
    RootLogicalRectangleRuntime(const EnvironmentStartupRuntime& environment,
        const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
        IRootLogicalRectangleSynchronization& synchronization,
        IRootLogicalRectanglePlatformApi& platform) noexcept;

    // 原 478E50 的输出参数和返回指针指向同一 RECT；失败保留已写前缀。
    RootLogicalRectangleRuntime(const EnvironmentStartupRuntime& environment,
        RootRuntimeOwner& root, IRootLogicalRectangleSynchronization& synchronization,
        IRootLogicalRectanglePlatformApi& platform) noexcept;

    // 原 478E50 的输出参数和返回指针指向同一 RECT；失败保留已写前缀。
    // 保留 x86 无符号 DIV；零分母不降级，也不改写为 C++ 业务异常。
    [[nodiscard]] RECT& QueryAt478E50(RECT& output) const;

private:
    [[nodiscard]] WindowModeView CurrentMode() const;
    [[nodiscard]] HWND CurrentWindow() const;
    [[nodiscard]] LONG CurrentWidth() const;
    [[nodiscard]] LONG CurrentHeight() const;

    const WindowEnvironmentRuntime* environment_ = nullptr;
    const std::shared_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    const EnvironmentStartupRuntime* startupEnvironment_ = nullptr;
    const std::unique_ptr<WindowModeManagerRuntime>* uniqueModeSlot_ = nullptr;
    RootRuntimeOwner* root_ = nullptr;
    IRootLogicalRectangleSynchronization& synchronization_;
    IRootLogicalRectanglePlatformApi& platform_;
};
}
