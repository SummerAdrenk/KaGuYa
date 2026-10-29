#pragma once

#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ReadScriptEffectNativeBase8h;
struct ReadScriptEffectNativeVtable final
{
    std::uint8_t(__thiscall* tickAt00)(ReadScriptEffectNativeBase8h*);
    void(__thiscall* drawAt04)(ReadScriptEffectNativeBase8h*);
    std::uint8_t(__thiscall* readyAt08)(ReadScriptEffectNativeBase8h*);
    ReadScriptEffectNativeBase8h*(__thiscall* deleteAt0C)(ReadScriptEffectNativeBase8h*, std::uint32_t);
};
// 5B4D00/4B0730/4A7300：原Effect共同前缀，不包含现代虚析构或Finish槽。
struct ReadScriptEffectNativeBase8h final
{
    const ReadScriptEffectNativeVtable* vptrAt00;
    std::uint8_t readyAt04;
    std::uint8_t unwrittenAt05[3];
};
static_assert(sizeof(ReadScriptEffectNativeVtable) == 0x10);
static_assert(sizeof(ReadScriptEffectNativeBase8h) == 8);
static_assert(offsetof(ReadScriptEffectNativeBase8h, readyAt04) == 4);

const ReadScriptEffectNativeVtable& ReadScriptEffectBaseNativeMethods() noexcept;
// 不要求派生正文的C++类型相同；经当前原四槽分派，不能显式额外Finish一次。
std::uint8_t TickReadScriptEffectNative(void* body);
void DrawReadScriptEffectNative(void* body);
std::uint8_t ReadyReadScriptEffectNative(void* body);
void DeleteReadScriptEffectNative(void* body, std::uint32_t flags);
}
