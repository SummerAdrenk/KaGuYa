#include "WindowThunkNative.Internal.h"

namespace KaGuYa::Start::Runtime::WindowThunkNativeDetail
{
bool __cdecl ResolveExportAt53EEFD(HMODULE module, LPCSTR name, void* volatile* slot,
    IWindowThunkNativeApi& api)
{
    const auto function = api.GetProcAddress(module, name);
    if (!function) return false;
    *slot = api.EncodePointer(reinterpret_cast<void*>(function));
    return true;
}
bool __cdecl ResolveExportAt53EEFD(HMODULE module, LPCSTR name, void* volatile* slot)
{
    return ResolveExportAt53EEFD(module, name, slot, CurrentServices().api);
}

namespace
{
FARPROC Resolve(WindowThunkNativeServices& services, void* volatile* selected)
{
    auto& api = services.api;
    auto& cache = services.cache;
    if (cache.readyAt61A6E4)
        return reinterpret_cast<FARPROC>(api.DecodePointer(*selected));
    const auto module = api.LoadLibraryExA("atlthunk.dll", nullptr, 0x800);
    if (!module) return nullptr;
    // 四个原resolver逐指令同形；失败留部分槽、留模块引用，下一次从头重试。
    if (!ResolveExportAt53EEFD(module, "AtlThunk_AllocateData", &cache.allocateAt61A6D4, api) ||
        !ResolveExportAt53EEFD(module, "AtlThunk_InitData", &cache.initializeAt61A6D8, api) ||
        !ResolveExportAt53EEFD(module, "AtlThunk_DataToCode", &cache.codeAt61A6DC, api) ||
        !ResolveExportAt53EEFD(module, "AtlThunk_FreeData", &cache.freeAt61A6E0, api))
        return nullptr;
    api.PublicationBarrier();
    cache.readyAt61A6E4 = 1;
    return reinterpret_cast<FARPROC>(api.DecodePointer(*selected));
}
}
FARPROC __cdecl ResolveAt53ECEA(void* volatile* slot) { auto services=CurrentServices();return Resolve(services, slot); }
FARPROC __cdecl ResolveAt53EE4A(void* volatile* slot) { auto services=CurrentServices();return Resolve(services, slot); }
FARPROC __cdecl ResolveAt53EC3A(void* volatile* slot) { auto services=CurrentServices();return Resolve(services, slot); }
FARPROC __cdecl ResolveAt53ED9A(void* volatile* slot) { auto services=CurrentServices();return Resolve(services, slot); }
FARPROC __cdecl ResolveAt53ECEA(WindowThunkNativeServices& services)
{
    return Resolve(services, &services.cache.allocateAt61A6D4);
}
FARPROC __cdecl ResolveAt53EE4A(WindowThunkNativeServices& services)
{
    return Resolve(services, &services.cache.initializeAt61A6D8);
}
FARPROC __cdecl ResolveAt53EC3A(WindowThunkNativeServices& services)
{
    return Resolve(services, &services.cache.codeAt61A6DC);
}
FARPROC __cdecl ResolveAt53ED9A(WindowThunkNativeServices& services)
{
    return Resolve(services, &services.cache.freeAt61A6E0);
}
}
