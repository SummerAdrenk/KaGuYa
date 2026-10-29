#pragma once
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
// 只借用Win32叶，不携带ATL模块或额外CRT初始化/退出资格。
class IMainFrameApplicationNativeApi
{
protected:
    ~IMainFrameApplicationNativeApi() = default;
public:
    virtual int GetSystemMetrics(int) = 0;
    virtual BOOL AdjustWindowRectEx(RECT*, DWORD, BOOL, DWORD) = 0;
    virtual BOOL OffsetRect(RECT*, int, int) = 0;
    virtual HWND CreateWindowExA(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int,
        HWND, HMENU, HINSTANCE, void*) = 0;
    virtual BOOL ShowWindow(HWND, int) = 0;
    virtual void SetLastError(DWORD) = 0;
    virtual int MessageBoxA(HWND, LPCSTR, LPCSTR, UINT) = 0;
};
IMainFrameApplicationNativeApi& DefaultMainFrameApplicationNativeApi() noexcept;
}
