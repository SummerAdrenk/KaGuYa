#pragma once
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 原53F0E4的唯一heap 8h正文；不得放入owner、服务或另一份code。
struct WindowThunkNativeBody08h final
{
    std::uint32_t flagAt00;
    void* dataAt04;
};
static_assert(sizeof(void*) == 4);
static_assert(sizeof(WindowThunkNativeBody08h) == 8);
static_assert(alignof(WindowThunkNativeBody08h) == 4);
static_assert(offsetof(WindowThunkNativeBody08h, flagAt00) == 0);
static_assert(offsetof(WindowThunkNativeBody08h, dataAt04) == 4);
static_assert(std::is_standard_layout_v<WindowThunkNativeBody08h>);
static_assert(std::is_trivially_copyable_v<WindowThunkNativeBody08h>);
static_assert(std::is_trivially_destructible_v<WindowThunkNativeBody08h>);

// 原61A6D0..61A6E4的进程缓存，在8h正文外；零初始化，无退出清理。
struct WindowThunkNativeCache final
{
    PSLIST_HEADER volatile listHeadAt61A6D0 = nullptr;
    void* volatile allocateAt61A6D4 = nullptr;
    void* volatile initializeAt61A6D8 = nullptr;
    void* volatile codeAt61A6DC = nullptr;
    void* volatile freeAt61A6E0 = nullptr;
    BYTE volatile readyAt61A6E4 = 0;
};
static_assert(offsetof(WindowThunkNativeCache, allocateAt61A6D4) == 4);
static_assert(offsetof(WindowThunkNativeCache, initializeAt61A6D8) == 8);
static_assert(offsetof(WindowThunkNativeCache, codeAt61A6DC) == 0x0C);
static_assert(offsetof(WindowThunkNativeCache, freeAt61A6E0) == 0x10);
static_assert(offsetof(WindowThunkNativeCache, readyAt61A6E4) == 0x14);
static_assert(sizeof(WindowThunkNativeCache) == 0x18);
static_assert(std::is_standard_layout_v<WindowThunkNativeCache>);
static_assert(std::is_trivially_destructible_v<WindowThunkNativeCache>);

class IWindowThunkNativeApi
{
protected:
    // 服务始终借用，禁止经基类删除；默认静态API不登记额外CRT析构。
    ~IWindowThunkNativeApi() = default;
public:
    virtual HANDLE GetProcessHeap() = 0;
    virtual void* HeapAlloc(HANDLE, DWORD, SIZE_T) = 0;
    virtual BOOL HeapFree(HANDLE, DWORD, void*) = 0;
    virtual HMODULE LoadLibraryExA(LPCSTR, HANDLE, DWORD) = 0;
    virtual FARPROC GetProcAddress(HMODULE, LPCSTR) = 0;
    virtual void* EncodePointer(void*) = 0;
    virtual void* DecodePointer(void*) = 0;
    virtual void PublicationBarrier() = 0;
    virtual void CheckIndirectCall(FARPROC) = 0;
    virtual BOOL IsProcessorFeaturePresent(DWORD) = 0;
    virtual PSLIST_HEADER volatile* PebThunkSListSlot() = 0;
    virtual PSLIST_HEADER CompareExchangeSListSlot(PSLIST_HEADER volatile*, PSLIST_HEADER, PSLIST_HEADER) = 0;
    virtual void InitializeSListHead(PSLIST_HEADER) = 0;
    virtual PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER) = 0;
    virtual PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER, PSLIST_ENTRY) = 0;
    virtual void* VirtualAlloc(void*, SIZE_T, DWORD, DWORD) = 0;
    virtual BOOL VirtualFree(void*, SIZE_T, DWORD) = 0;
    virtual void RaiseException(DWORD, DWORD, DWORD, const ULONG_PTR*) = 0;
    virtual HANDLE GetCurrentProcess() = 0;
    virtual BOOL FlushInstructionCache(HANDLE, const void*, SIZE_T) = 0;
};
struct WindowThunkNativeServices final
{
    WindowThunkNativeCache& cache;
    IWindowThunkNativeApi& api;
};
IWindowThunkNativeApi& DefaultWindowThunkNativeApi() noexcept;
// 返回两份借用引用的元数据，不复制cache，不引入service的CRT初始化/退出。
WindowThunkNativeServices ProcessWindowThunkNativeServices() noexcept;

// 仅借用外部服务；不随正文保存，也不赋予原路径额外资源回收资格。
class WindowThunkNativeScope final
{
public:
    explicit WindowThunkNativeScope(WindowThunkNativeServices&) noexcept;
    explicit WindowThunkNativeScope(WindowThunkNativeServices*) noexcept;
    ~WindowThunkNativeScope() noexcept;
    WindowThunkNativeScope(const WindowThunkNativeScope&) = delete;
    WindowThunkNativeScope& operator=(const WindowThunkNativeScope&) = delete;
private:
    WindowThunkNativeServices* previous_;
};

// 可直接接原端点raw+14的void*槽；初始化无成功返回值，取代码另走53F149。
WindowThunkNativeBody08h* __cdecl AllocateWindowThunkNativeAt53F0E4();
void __stdcall InitializeWindowThunkNativeAt53F1E6(void*, std::uintptr_t procedure, void* firstParameter);
WNDPROC __stdcall WindowThunkNativeCodeAt53F149(void*);
void __stdcall FreeWindowThunkNativeAt53F190(void*);

WindowThunkNativeBody08h* AllocateWindowThunkNativeAt53F0E4(WindowThunkNativeServices&);
void InitializeWindowThunkNativeAt53F1E6(void*, std::uintptr_t, void*, WindowThunkNativeServices&);
WNDPROC WindowThunkNativeCodeAt53F149(void*, WindowThunkNativeServices&);
void FreeWindowThunkNativeAt53F190(void*, WindowThunkNativeServices&);
}
