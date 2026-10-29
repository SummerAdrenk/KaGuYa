#include <Runtime/NativeArchiveRuntime.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
// x86 fastcall 的占位 EDX 保持 thiscall 的 ECX/栈参数与 ret 4。
void __fastcall PureSerialize(void*, void*, void*)
{
    _purecall();
    std::terminate();
}

void* __fastcall DeleteBase(void* body, void*, std::uint32_t flags) noexcept
{
    // 412230：只退首 DWORD；bit0 决定 sized-delete 4，返回原 this。
    RetireNativeArchive(body);
    if (flags & 1u) ::operator delete(body, std::size_t{4});
    return body;
}

// 与既有 raw native 表相同的静态函数地址初始化；无懒建、owner 或析构。
const NativeArchiveVtable2 baseMethods{
    reinterpret_cast<decltype(NativeArchiveVtable2::serializeAt00)>(&PureSerialize),
    reinterpret_cast<decltype(NativeArchiveVtable2::deleteAt04)>(&DeleteBase)};
}

const NativeArchiveVtable2& NativeArchiveBaseMethods() noexcept
{
    return baseMethods;
}

void RetireNativeArchive(void* body) noexcept
{
    // 40F860：按对象表示只写首槽，兼容旧 C++ IArchive 的退役入口。
    const auto* table = &baseMethods;
    std::memcpy(body, &table, sizeof(table));
}

void* DeleteNativeArchiveAtVslot04(void* body, std::uint32_t flags) noexcept
{
    // 只按当次表分派，不猜正文 kind，也不借 C++ 基类指针。
    const void* table;
    decltype(NativeArchiveVtable2::deleteAt04) entry;
    std::memcpy(&table, body, sizeof(table));
    std::memcpy(&entry, static_cast<const unsigned char*>(table) + 4, sizeof(entry));
    return entry(body, flags);
}
}
