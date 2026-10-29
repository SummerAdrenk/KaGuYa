#include "WindowThunkNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace WindowThunkNativeDetail;

WindowThunkNativeBody08h* __cdecl AllocateWindowThunkNativeAt53F0E4()
{
    auto services=CurrentServices();return AllocateWindowThunkNativeAt53F0E4(services);
}
void __stdcall InitializeWindowThunkNativeAt53F1E6(void* body, std::uintptr_t procedure, void* firstParameter)
{
    auto services=CurrentServices();InitializeWindowThunkNativeAt53F1E6(body, procedure, firstParameter, services);
}
WNDPROC __stdcall WindowThunkNativeCodeAt53F149(void* body)
{
    auto services=CurrentServices();return WindowThunkNativeCodeAt53F149(body, services);
}
void __stdcall FreeWindowThunkNativeAt53F190(void* body)
{
    auto services=CurrentServices();FreeWindowThunkNativeAt53F190(body, services);
}

WindowThunkNativeBody08h* AllocateWindowThunkNativeAt53F0E4(WindowThunkNativeServices& services)
{
    auto& api = services.api;
    auto* body = static_cast<WindowThunkNativeBody08h*>(api.HeapAlloc(api.GetProcessHeap(), 8, 8));
    if (!body) return nullptr;
    const auto function = ResolveAt53ECEA(services);
    body->flagAt00 = function ? 0u : 1u;
    void* data;
    if (function)
    {
        api.CheckIndirectCall(function);
        data = reinterpret_cast<AllocateData>(function)();
    }
    else
    {
        data = AllocateLegacyAt53EF63(services);
    }
    body->dataAt04 = data;
    if (!data)
    {
        static_cast<void>(api.HeapFree(api.GetProcessHeap(), 0, body));
        return nullptr;
    }
    // 53F0E4没有EH清理；上方解析、分配或RaiseException展开时不补回收body。
    return body;
}

void InitializeWindowThunkNativeAt53F1E6(void* raw, std::uintptr_t procedure, void* firstParameter,
    WindowThunkNativeServices& services)
{
    auto* body = static_cast<WindowThunkNativeBody08h*>(raw);
    if (!body) return;
    auto* data = Read(body->dataAt04);
    if (!data) return;
    if (Read(body->flagAt00))
    {
        WindowThunkNativeScope scope(services);
        static_cast<void>(static_cast<LegacyCode0Dh*>(data)->InitializeAt53EF24(procedure, firstParameter));
        return;
    }
    const auto function = ResolveAt53EE4A(services);
    if (function)
    {
        data = Read(body->dataAt04);
        services.api.CheckIndirectCall(function);
        reinterpret_cast<InitializeData>(function)(data, reinterpret_cast<void*>(procedure),
            reinterpret_cast<std::size_t>(firstParameter));
    }
}

WNDPROC WindowThunkNativeCodeAt53F149(void* raw, WindowThunkNativeServices& services)
{
    auto* body = static_cast<WindowThunkNativeBody08h*>(raw);
    if (!body) return nullptr;
    auto* data = Read(body->dataAt04);
    if (!data) return nullptr;
    if (Read(body->flagAt00)) return reinterpret_cast<WNDPROC>(data);
    const auto function = ResolveAt53EC3A(services);
    if (!function) return nullptr;
    data = Read(body->dataAt04);
    services.api.CheckIndirectCall(function);
    return reinterpret_cast<DataToCode>(function)(data);
}

void FreeWindowThunkNativeAt53F190(void* raw, WindowThunkNativeServices& services)
{
    auto* body = static_cast<WindowThunkNativeBody08h*>(raw);
    if (!body) return;
    auto& api = services.api;
    if (Read(body->dataAt04))
    {
        if (Read(body->flagAt00))
        {
            FreeLegacyAt53F049(Read(body->dataAt04), services);
        }
        else
        {
            const auto function = ResolveAt53ED9A(services);
            if (function)
            {
                auto* data = Read(body->dataAt04);
                api.CheckIndirectCall(function);
                reinterpret_cast<FreeData>(function)(data);
            }
        }
    }
    static_cast<void>(api.HeapFree(api.GetProcessHeap(), 0, body));
}
}
