#include "WindowThunkNative.Internal.h"
#include <intrin.h>

#if !defined(_M_IX86)
#error 原窗口thunk只支持MSVC的x86调用约定及PEB布局
#endif

namespace KaGuYa::Start::Runtime
{
namespace
{
constinit WindowThunkNativeCache processCache{};
thread_local WindowThunkNativeServices* activeServices = nullptr;

class Win32Api final : public IWindowThunkNativeApi
{
public:
    HANDLE GetProcessHeap() override { return ::GetProcessHeap(); }
    void* HeapAlloc(HANDLE heap, DWORD flags, SIZE_T size) override { return ::HeapAlloc(heap, flags, size); }
    BOOL HeapFree(HANDLE heap, DWORD flags, void* data) override { return ::HeapFree(heap, flags, data); }
    HMODULE LoadLibraryExA(LPCSTR name, HANDLE file, DWORD flags) override { return ::LoadLibraryExA(name, file, flags); }
    FARPROC GetProcAddress(HMODULE module, LPCSTR name) override { return ::GetProcAddress(module, name); }
    void* EncodePointer(void* pointer) override { return ::EncodePointer(pointer); }
    void* DecodePointer(void* pointer) override { return ::DecodePointer(pointer); }
    void PublicationBarrier() override
    {
        volatile LONG local = 0;
        static_cast<void>(::InterlockedOr(&local, 0));
    }
    void CheckIndirectCall(FARPROC) override {} // 权威PE的540A32为retn；不伪造CFG校验。
    BOOL IsProcessorFeaturePresent(DWORD feature) override { return ::IsProcessorFeaturePresent(feature); }
    PSLIST_HEADER volatile* PebThunkSListSlot() override
    {
        return reinterpret_cast<PSLIST_HEADER volatile*>(static_cast<std::uintptr_t>(__readfsdword(0x30)) + 0x34);
    }
    PSLIST_HEADER CompareExchangeSListSlot(PSLIST_HEADER volatile* slot, PSLIST_HEADER value, PSLIST_HEADER expected) override
    {
        return static_cast<PSLIST_HEADER>(::InterlockedCompareExchangePointer(
            reinterpret_cast<PVOID volatile*>(slot), value, expected));
    }
    void InitializeSListHead(PSLIST_HEADER head) override { ::InitializeSListHead(head); }
    PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER head) override { return ::InterlockedPopEntrySList(head); }
    PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER head, PSLIST_ENTRY entry) override
    { return ::InterlockedPushEntrySList(head, entry); }
    void* VirtualAlloc(void* address, SIZE_T size, DWORD type, DWORD protect) override
    { return ::VirtualAlloc(address, size, type, protect); }
    BOOL VirtualFree(void* address, SIZE_T size, DWORD type) override { return ::VirtualFree(address, size, type); }
    void RaiseException(DWORD code, DWORD flags, DWORD count, const ULONG_PTR* args) override
    { ::RaiseException(code, flags, count, args); }
    HANDLE GetCurrentProcess() override { return ::GetCurrentProcess(); }
    BOOL FlushInstructionCache(HANDLE process, const void* address, SIZE_T size) override
    { return ::FlushInstructionCache(process, address, size); }
};
static_assert(std::is_trivially_destructible_v<Win32Api>);
constinit Win32Api processApi{};
}
IWindowThunkNativeApi& DefaultWindowThunkNativeApi() noexcept
{
    return processApi;
}
WindowThunkNativeServices ProcessWindowThunkNativeServices() noexcept
{
    return {processCache,processApi};
}
WindowThunkNativeScope::WindowThunkNativeScope(WindowThunkNativeServices& services) noexcept
    :WindowThunkNativeScope(&services){}
WindowThunkNativeScope::WindowThunkNativeScope(WindowThunkNativeServices* services) noexcept
    : previous_(activeServices)
{
    activeServices = services;
}
WindowThunkNativeScope::~WindowThunkNativeScope() noexcept
{
    activeServices = previous_;
}
namespace WindowThunkNativeDetail
{
WindowThunkNativeServices CurrentServices() noexcept
{
    return activeServices ? *activeServices : ProcessWindowThunkNativeServices();
}
}
}
