#pragma once

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ScrSaveParamsRuntimeState;
class ScrSaveParams2Native;

// 原 IArchive 两槽 ABI；slot00 的参数是真实 native18，不是 SerializeBuffer。
struct NativeArchiveVtable2
{
    void (__thiscall* serializeAt00)(void*, void*);
    void* (__thiscall* deleteAt04)(void*, std::uint32_t) noexcept;
};
static_assert(sizeof(void*) == 4 && sizeof(NativeArchiveVtable2) == 8);

const NativeArchiveVtable2& NativeArchiveBaseMethods() noexcept;
void RetireNativeArchive(void* body) noexcept;
void* DeleteNativeArchiveAtVslot04(void* body, std::uint32_t flags) noexcept;
const NativeArchiveVtable2& SaveParamsNativeMethods() noexcept;
const NativeArchiveVtable2& SaveParams2NativeMethods() noexcept;
void SerializeSaveParamsNative18At48B720(ScrSaveParamsRuntimeState& body, void* native18);
void SerializeSaveParams2Native18At48B6B0(ScrSaveParams2Native& body, void* native18);
}
