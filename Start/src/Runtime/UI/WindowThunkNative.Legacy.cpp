#include "WindowThunkNative.Internal.h"
#include <cstring>

namespace KaGuYa::Start::Runtime::WindowThunkNativeDetail
{
namespace
{
PSLIST_HEADER HeapSentinel() noexcept
{
    return reinterpret_cast<PSLIST_HEADER>(1);
}
void* AllocationFailure(IWindowThunkNativeApi& api)
{
    // 原53EFD1的异常可继续；处理器恢复执行后才走NULL及外层heap8回收。
    api.RaiseException(0xC0000017u, 0, 0, nullptr);
    return nullptr;
}
}

BOOL __cdecl InitializePoolAt53F079() { auto services=CurrentServices();return InitializePoolAt53F079(services); }
void* __cdecl AllocateLegacyAt53EF63() { auto services=CurrentServices();return AllocateLegacyAt53EF63(services); }
void __cdecl FreeLegacyAt53F049(void* data) { auto services=CurrentServices();FreeLegacyAt53F049(data, services); }

BOOL __cdecl InitializePoolAt53F079(WindowThunkNativeServices& services)
{
    auto& api = services.api;
    if (!api.IsProcessorFeaturePresent(0x0C))
    {
        services.cache.listHeadAt61A6D0 = HeapSentinel();
        return TRUE;
    }
    auto* slot = api.PebThunkSListSlot();
    auto* head = *slot;
    if (!head)
    {
        auto* candidate = static_cast<PSLIST_HEADER>(api.HeapAlloc(api.GetProcessHeap(), 8, 8));
        if (!candidate) return FALSE;
        api.InitializeSListHead(candidate);
        if (api.CompareExchangeSListSlot(slot, candidate, nullptr))
            static_cast<void>(api.HeapFree(api.GetProcessHeap(), 0, candidate));
        head = *slot;
    }
    services.cache.listHeadAt61A6D0 = head;
    return TRUE;
}

void* __cdecl AllocateLegacyAt53EF63(WindowThunkNativeServices& services)
{
    auto& api = services.api;
    auto* head = services.cache.listHeadAt61A6D0;
    if (!head)
    {
        if (!InitializePoolAt53F079(services)) return AllocationFailure(api);
        head = services.cache.listHeadAt61A6D0;
    }
    if (head == HeapSentinel())
    {
        auto* data = api.HeapAlloc(api.GetProcessHeap(), 8, 0x0D);
        return data ? data : AllocationFailure(api);
    }
    auto* data = api.InterlockedPopEntrySList(head);
    if (data)
    {
        // 仅首次pop清0Dh；沿用现有静态CRT memset，不创建CRT私有镜像缓存。
        std::memset(data, 0, 0x0D);
        return data;
    }
    auto* page = static_cast<std::byte*>(api.VirtualAlloc(nullptr, 0x1000, 0x1000, 0x40));
    if (!page) return AllocationFailure(api);
    static_cast<void>(*reinterpret_cast<const volatile DWORD*>(page)); // 原53EFE3仍有此读。
    data = api.InterlockedPopEntrySList(services.cache.listHeadAt61A6D0);
    if (data)
    {
        // 第二次pop成功直接返回原字节；释放本次竞争失败的页，不再清零。
        static_cast<void>(api.VirtualFree(page, 0, 0x8000));
        return data;
    }
    auto* const last = page + 0xFF0;
    do
    {
        static_cast<void>(api.InterlockedPushEntrySList(services.cache.listHeadAt61A6D0,
            reinterpret_cast<PSLIST_ENTRY>(page)));
        page += 0x10;
    } while (page < last);
    return page;
}

void __cdecl FreeLegacyAt53F049(void* data, WindowThunkNativeServices& services)
{
    if (!data) return;
    auto& api = services.api;
    auto* head = services.cache.listHeadAt61A6D0;
    if (head == HeapSentinel())
        static_cast<void>(api.HeapFree(api.GetProcessHeap(), 0, data));
    else
        static_cast<void>(api.InterlockedPushEntrySList(head, static_cast<PSLIST_ENTRY>(data)));
}

BOOL __thiscall LegacyCode0Dh::InitializeAt53EF24(std::uintptr_t procedure, void* firstParameter)
{
    firstParameterAt04 = static_cast<DWORD>(reinterpret_cast<std::uintptr_t>(firstParameter));
    const auto relative = static_cast<DWORD>(procedure) - static_cast<DWORD>(reinterpret_cast<std::uintptr_t>(this)) - 0x0Du;
    moveAt00 = 0x042444C7;
    jumpAt08 = 0xE9;
    relativeProcedureAt09 = relative;
    auto& api = CurrentServices().api;
    static_cast<void>(api.FlushInstructionCache(api.GetCurrentProcess(), this, 0x0D));
    return TRUE;
}
}
