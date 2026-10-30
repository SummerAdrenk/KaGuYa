#include <Runtime/ReadScriptEffectNative.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Function>
Function Method(void* body, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, body, sizeof(table));
    Function function;
    std::memcpy(&function, table + offset, sizeof(function));
    return function;
}

std::uint8_t __fastcall Ready(void* body, void*) noexcept
{
    std::uint8_t ready;
    std::memcpy(&ready, static_cast<const std::byte*>(body) + 4, sizeof(ready));
    return ready;
}
void* __fastcall DeleteBase(void* body, void*, std::uint32_t flags) noexcept
{
    // 4A730A..4A7315：恢复5B4D00，保留ready/padding，仅bit0控制标量释放。
    const auto* table = &ReadScriptEffectBaseNativeMethods();
    std::memcpy(body, &table, sizeof(table));
    if (flags & 1u) ::operator delete(body, std::size_t{8});
    return body;
}
// 原5B4D00的纯调用叶是cdecl；经具名桥接正常调用CRT，不能直接跨调用约定调用它。
[[noreturn]] std::uint8_t __fastcall PureTick(void*, void*) { _purecall(); std::terminate(); }
[[noreturn]] void __fastcall PureDraw(void*, void*) { _purecall(); std::terminate(); }
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const ReadScriptEffectNativeVtable baseMethods{
    reinterpret_cast<decltype(ReadScriptEffectNativeVtable::tickAt00)>(&PureTick),
    reinterpret_cast<decltype(ReadScriptEffectNativeVtable::drawAt04)>(&PureDraw),
    reinterpret_cast<decltype(ReadScriptEffectNativeVtable::readyAt08)>(&Ready),
    reinterpret_cast<decltype(ReadScriptEffectNativeVtable::deleteAt0C)>(&DeleteBase)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

const ReadScriptEffectNativeVtable& ReadScriptEffectBaseNativeMethods() noexcept { return baseMethods; }

std::uint8_t TickReadScriptEffectNative(void* body)
{ return Method<std::uint8_t(__thiscall*)(void*)>(body, 0)(body); }
void DrawReadScriptEffectNative(void* body)
{ Method<void(__thiscall*)(void*)>(body, 4)(body); }
std::uint8_t ReadyReadScriptEffectNative(void* body)
{ return Method<std::uint8_t(__thiscall*)(void*)>(body, 8)(body); }
void DeleteReadScriptEffectNative(void* body, std::uint32_t flags)
{ (void)Method<void*(__thiscall*)(void*, std::uint32_t)>(body, 0x0C)(body, flags); }
}
