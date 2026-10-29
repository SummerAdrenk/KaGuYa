#pragma once
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
// 原601208：不同于Edit的68h CLASSINFO；13字节名字后是DWORD图标资源号。
struct MainFrameWindowClassNativeInfo54h final
{
    WNDCLASSEXA descriptorAt00;
    LPCSTR superclassAt30;
    WNDPROC previousProcedureAt34;
    LPCSTR cursorNameAt38;
    BOOL systemCursorAt3C;
    ATOM atomAt40;
    char generatedNameAt42[13];
    BYTE unwrittenAt4F;
    DWORD iconResourceAt50;
};
static_assert(sizeof(MainFrameWindowClassNativeInfo54h) == 0x54);
static_assert(offsetof(MainFrameWindowClassNativeInfo54h, atomAt40) == 0x40);
static_assert(offsetof(MainFrameWindowClassNativeInfo54h, generatedNameAt42) == 0x42);
static_assert(offsetof(MainFrameWindowClassNativeInfo54h, iconResourceAt50) == 0x50);
class IMainFrameWindowClassNativeApi
{
protected:
    ~IMainFrameWindowClassNativeApi() = default;
public:
    virtual void EnterCriticalSection(LPCRITICAL_SECTION) = 0;
    virtual void LeaveCriticalSection(LPCRITICAL_SECTION) = 0;
    virtual BOOL GetClassInfoExA(HINSTANCE, LPCSTR, WNDCLASSEXA*) = 0;
    virtual HCURSOR LoadCursorA(HINSTANCE, LPCSTR) = 0;
    virtual int FormatAtlName(char*, std::size_t, const void*) = 0;
    virtual int GetSystemMetrics(int) = 0;
    virtual HANDLE LoadImageA(HINSTANCE, LPCSTR, UINT, int, int, UINT) = 0;
    virtual ATOM RegisterClassExA(const WNDCLASSEXA*) = 0;
};
IMainFrameWindowClassNativeApi& DefaultMainFrameWindowClassNativeApi() noexcept;
struct MainFrameWindowClassNativeServices final
{
    WindowClassWinModuleNativeBody2Ch& winModule;
    // 原hModule与hInstance独立且有晚读；只借地址，不缓存为构造时的两值。
    HINSTANCE& imageInstance;
    HINSTANCE& resourceInstance;
    IMainFrameWindowClassNativeApi& api;
};
// 仅用于全新长寿存储；类注册后绝不再次重置。
void InitializeMainFrameWindowClassNativeAt601208(MainFrameWindowClassNativeInfo54h&, WNDPROC initialAt438340) noexcept;
ATOM RegisterMainFrameWindowClassNativeAt4316F0(MainFrameWindowClassNativeInfo54h&, WNDPROC* previous,
    MainFrameWindowClassNativeServices&);
}
