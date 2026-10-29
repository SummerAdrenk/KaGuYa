#pragma once
#include <Runtime/WindowThunkNative.h>

namespace KaGuYa::Start::Runtime::WindowThunkNativeDetail
{
template<class T> T Read(const T& value) noexcept
{
    return *static_cast<const volatile T*>(&value);
}
WindowThunkNativeServices CurrentServices() noexcept;

using AllocateData = void* (__stdcall*)();
using InitializeData = void (__stdcall*)(void*, void*, std::size_t);
using DataToCode = WNDPROC (__stdcall*)(void*);
using FreeData = void (__stdcall*)(void*);

FARPROC __cdecl ResolveAt53ECEA(void* volatile*);
FARPROC __cdecl ResolveAt53EE4A(void* volatile*);
FARPROC __cdecl ResolveAt53EC3A(void* volatile*);
FARPROC __cdecl ResolveAt53ED9A(void* volatile*);
bool __cdecl ResolveExportAt53EEFD(HMODULE, LPCSTR, void* volatile*);
void* __cdecl AllocateLegacyAt53EF63();
void __cdecl FreeLegacyAt53F049(void*);
BOOL __cdecl InitializePoolAt53F079();

FARPROC __cdecl ResolveAt53ECEA(WindowThunkNativeServices&);
FARPROC __cdecl ResolveAt53EE4A(WindowThunkNativeServices&);
FARPROC __cdecl ResolveAt53EC3A(WindowThunkNativeServices&);
FARPROC __cdecl ResolveAt53ED9A(WindowThunkNativeServices&);
bool __cdecl ResolveExportAt53EEFD(HMODULE, LPCSTR, void* volatile*, IWindowThunkNativeApi&);
void* __cdecl AllocateLegacyAt53EF63(WindowThunkNativeServices&);
void __cdecl FreeLegacyAt53F049(void*, WindowThunkNativeServices&);
BOOL __cdecl InitializePoolAt53F079(WindowThunkNativeServices&);

#pragma pack(push, 1)
struct LegacyCode0Dh final
{
    DWORD volatile moveAt00;
    DWORD volatile firstParameterAt04;
    BYTE volatile jumpAt08;
    DWORD volatile relativeProcedureAt09;
    BOOL __thiscall InitializeAt53EF24(std::uintptr_t, void*);
};
#pragma pack(pop)
static_assert(sizeof(LegacyCode0Dh) == 0x0D);
static_assert(offsetof(LegacyCode0Dh, relativeProcedureAt09) == 9);
}
