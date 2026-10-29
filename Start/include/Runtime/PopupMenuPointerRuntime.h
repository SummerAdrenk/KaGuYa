#pragma once

#include <Windows.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class WindowModeManagerRuntime;
class WindowModeView;
// 4513DE/4513EA系统叶，BOOL由原调用忽略，普通异常不在此边界截断。
class IPopupMenuPointerPlatformApi
{
public:
    virtual ~IPopupMenuPointerPlatformApi() = default;
    virtual BOOL ClientToScreen(HWND window, POINT& point) = 0;
    virtual BOOL SetCursorPos(int x, int y) = 0;
};
class Win32PopupMenuPointerPlatformApi final : public IPopupMenuPointerPlatformApi
{
public:
    BOOL ClientToScreen(HWND window, POINT& point) override;
    BOOL SetCursorPos(int x, int y) override;
};

// 4512E0的外部取源边界，不是原游戏对象的虚表；每个方法只读取当次来源。
class IPopupMenuPointerContext
{
public:
    virtual ~IPopupMenuPointerContext() = default;
    virtual void SynchronizePointerRootAt413BE0() = 0;
    virtual WindowModeView PointerModeAt0C() const = 0;
    virtual std::int32_t ReadGuiWidthAt18() = 0;
    virtual std::int32_t ReadGuiHeightAt1C() = 0;
    virtual HWND PointerWindowAt2C() = 0;
};

void MovePopupPointerAt4512E0(IPopupMenuPointerContext& source,
    IPopupMenuPointerPlatformApi& platform, POINT logical);
}
