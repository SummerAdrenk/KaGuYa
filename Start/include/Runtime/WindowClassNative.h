#pragma once
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 原61A698的38h ATL模块正文；451DA0只读+4/+8，不借现代profile代存。
struct WindowClassModuleNativeBody38h final
{
    DWORD sizeAt00;
    HINSTANCE instanceAt04;
    HINSTANCE resourceInstanceAt08;
    DWORD buildVersionAt0C;
    const GUID* versionGuidAt10;
    CRITICAL_SECTION resourceLockAt14;
    HINSTANCE* resourceInstancesAt2C;
    std::uint32_t resourceCountAt30;
    std::uint32_t resourceArrayWordAt34; // 53E998/53E9DA已确认容量；保留既有字段名，不改变布局。
};
// 原601028；+1C创建链与+20原子数组共用+4的锁，不能拆成独立registrar锁。
struct WindowClassWinModuleNativeBody2Ch final
{
    DWORD sizeAt00;
    CRITICAL_SECTION lockAt04;
    void* creationHeadAt1C;
    ATOM* atomsAt20;
    std::uint32_t atomCountAt24;
    std::uint32_t atomCapacityAt28;
};
// 原CLASSINFO：30h WNDCLASSEXA后接原字段，37字节名字紧跟16位atom。
struct WindowClassNativeInfo68h final
{
    WNDCLASSEXA descriptorAt00;
    LPCSTR superclassAt30;
    WNDPROC previousProcedureAt34;
    LPCSTR cursorNameAt38;
    BOOL systemCursorAt3C;
    ATOM atomAt40;
    char generatedNameAt42[37];
    BYTE paddingAt67;
};
static_assert(sizeof(void*) == 4);
static_assert(sizeof(CRITICAL_SECTION) == 0x18);
static_assert(sizeof(WNDCLASSEXA) == 0x30);
static_assert(sizeof(WindowClassModuleNativeBody38h) == 0x38);
static_assert(sizeof(WindowClassWinModuleNativeBody2Ch) == 0x2C);
static_assert(sizeof(WindowClassNativeInfo68h) == 0x68);
static_assert(alignof(WindowClassNativeInfo68h) == 4);
static_assert(alignof(WindowClassWinModuleNativeBody2Ch) == 4);
#define WINDOW_CLASS_NATIVE_OFFSET(type, field, offset) static_assert(offsetof(type, field) == offset)
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, instanceAt04, 4);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, resourceInstanceAt08, 8);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, buildVersionAt0C, 0x0C);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, versionGuidAt10, 0x10);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, resourceLockAt14, 0x14);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, resourceInstancesAt2C, 0x2C);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, resourceCountAt30, 0x30);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassModuleNativeBody38h, resourceArrayWordAt34, 0x34);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassWinModuleNativeBody2Ch, lockAt04, 4);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassWinModuleNativeBody2Ch, creationHeadAt1C, 0x1C);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassWinModuleNativeBody2Ch, atomsAt20, 0x20);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassWinModuleNativeBody2Ch, atomCountAt24, 0x24);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassWinModuleNativeBody2Ch, atomCapacityAt28, 0x28);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, descriptorAt00, 0);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, superclassAt30, 0x30);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, previousProcedureAt34, 0x34);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, cursorNameAt38, 0x38);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, systemCursorAt3C, 0x3C);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, atomAt40, 0x40);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, generatedNameAt42, 0x42);
WINDOW_CLASS_NATIVE_OFFSET(WindowClassNativeInfo68h, paddingAt67, 0x67);
WINDOW_CLASS_NATIVE_OFFSET(WNDCLASSEXA, lpfnWndProc, 8);
WINDOW_CLASS_NATIVE_OFFSET(WNDCLASSEXA, hInstance, 0x14);
WINDOW_CLASS_NATIVE_OFFSET(WNDCLASSEXA, hCursor, 0x1C);
WINDOW_CLASS_NATIVE_OFFSET(WNDCLASSEXA, lpszClassName, 0x28);
#undef WINDOW_CLASS_NATIVE_OFFSET
static_assert(std::is_standard_layout_v<WindowClassModuleNativeBody38h>);
static_assert(std::is_standard_layout_v<WindowClassWinModuleNativeBody2Ch>);
static_assert(std::is_standard_layout_v<WindowClassNativeInfo68h>);
static_assert(std::is_trivially_copyable_v<WindowClassNativeInfo68h>);
static_assert(std::is_trivially_destructible_v<WindowClassWinModuleNativeBody2Ch>);

// 服务只借用，不进入任何原正文；替身可观察API返回后的重读及失败前缀。
class IWindowClassNativeApi
{
protected:
    ~IWindowClassNativeApi() = default;
public:
    virtual void EnterCriticalSection(LPCRITICAL_SECTION) = 0;
    virtual void LeaveCriticalSection(LPCRITICAL_SECTION) = 0;
    virtual BOOL GetClassInfoExA(HINSTANCE, LPCSTR, WNDCLASSEXA*) = 0;
    virtual HCURSOR LoadCursorA(HINSTANCE, LPCSTR) = 0;
    virtual ATOM RegisterClassExA(const WNDCLASSEXA*) = 0;
    virtual int FormatAtlName(char*, std::size_t, const void*) = 0;
    virtual void* Recalloc(void*, std::size_t, std::size_t) = 0;
    virtual void Free(void*) = 0;
    virtual BOOL UnregisterClassA(LPCSTR, HINSTANCE) = 0;
    virtual BOOL InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION, DWORD) = 0;
    virtual DWORD GetLastError() = 0;
    virtual void DeleteCriticalSection(LPCRITICAL_SECTION) = 0;
    virtual void RaiseException(DWORD, DWORD, DWORD, const ULONG_PTR*) = 0;
};
IWindowClassNativeApi& DefaultWindowClassNativeApi() noexcept;
class WindowClassNativeScope final
{
public:
    explicit WindowClassNativeScope(IWindowClassNativeApi&) noexcept;
    ~WindowClassNativeScope() noexcept;
    WindowClassNativeScope(const WindowClassNativeScope&) = delete;
    WindowClassNativeScope& operator=(const WindowClassNativeScope&) = delete;
private:
    IWindowClassNativeApi* previous_;
};

// 原四实参cdecl；46FC05/48565C多push的一项由caller清栈，callee不读它。
ATOM __cdecl RegisterWindowClassNativeAt451DA0(WindowClassModuleNativeBody38h*,
    WindowClassWinModuleNativeBody2Ch*, WindowClassNativeInfo68h*, WNDPROC*);
ATOM RegisterWindowClassNativeAt451DA0(WindowClassModuleNativeBody38h*,
    WindowClassWinModuleNativeBody2Ch*, WindowClassNativeInfo68h*, WNDPROC*, IWindowClassNativeApi&);

// 调用前正文采用PE初值{2Ch,0...}；只重建4012F0的锁初始化/失败写入，不重置链或数组。
void InitializeWindowClassWinModuleNativeAt4012F0(WindowClassWinModuleNativeBody2Ch&,
    BYTE& startupFailedAt6197E9, IWindowClassNativeApi& = DefaultWindowClassNativeApi());
// 原5AA340正常退出：忽略Unregister结果，晚读count/base，不清CLASSINFO缓存或创建链。
void ReleaseWindowClassWinModuleNativeAt5AA340(WindowClassWinModuleNativeBody2Ch&,
    const WindowClassModuleNativeBody38h&, IWindowClassNativeApi& = DefaultWindowClassNativeApi());

// 仅供全新长寿命存储，严禁已登记后重置；原过程VA由真实Initial回调重定位。
void InitializeEditWindowClassNativeAt601838(WindowClassNativeInfo68h&, WNDPROC initialAt48E090) noexcept;
void InitializeEditControlClassNativeAt6015F0(WindowClassNativeInfo68h&, WNDPROC initialAt48E020) noexcept;
// 485631..485656由caller执行：初值不是EDIT；已有superclass指针保持不变。
void PrepareEditControlClassNativeAt485631(WindowClassNativeInfo68h&) noexcept;
}
