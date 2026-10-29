#include <Runtime/ScrSaveParams2Native.h>
#include "../Persistence/NativeSerialize18.Internal.h"

#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
void __fastcall SerializeSaveParams2(void* body, void*, void* native18)
{
    SerializeSaveParams2Native18At48B6B0(*static_cast<ScrSaveParams2Native*>(body), native18);
}

void* __fastcall DeleteSaveParams2(void* body, void*, std::uint32_t flags) noexcept
{
    return static_cast<ScrSaveParams2Native*>(body)->DeleteAtVslot04(flags);
}

const NativeArchiveVtable2 methods{
    reinterpret_cast<decltype(NativeArchiveVtable2::serializeAt00)>(&SerializeSaveParams2),
    reinterpret_cast<decltype(NativeArchiveVtable2::deleteAt04)>(&DeleteSaveParams2)};
}

const NativeArchiveVtable2& SaveParams2NativeMethods() noexcept
{
    return methods;
}

ScrSaveParams2Native::ScrSaveParams2Native() noexcept
    : vptrAt00(&methods)
{
    // 46CF87..46CF98 只对 +0C 起的 800h 字节执行 memset，不读取两个未写字段。
    std::memset(values.data(), 0, sizeof(values));
}

ScrSaveParams2Native::~ScrSaveParams2Native() noexcept
{
    // 46924A：只退化 IArchive 首槽，所有正文 DWORD 保持原值。
    RetireNativeArchive(this);
}

void* ScrSaveParams2Native::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    // 469243..469264：仅 bit0 决定释放；bit1 不是数组标志，其余位全部忽略。
    // 先退化为 IArchive，再以 80Ch 调用 53FBFB；不论 flags 都返回原 this。
    this->~ScrSaveParams2Native();
    if (flags & 1u) ::operator delete(this, sizeof(ScrSaveParams2Native));
    return this;
}

void ScrSaveParams2Native::Serialize(SerializeBuffer& archive)
{
    // 48B6BF..48B6CE 传原字段地址；对应无符号类型可别名访问，不能用局部值回写。
    archive.SerializeSlot3CDword(reinterpret_cast<std::uint32_t&>(scriptFileIdAtSaveParams04));
    archive.SerializeSlot3CDword(reinterpret_cast<std::uint32_t&>(instructionIndexAtSaveParams08));

    // 48B6D8..48B70E：两次 +3C 成功后仅查一次 +04，两分支各逐项调用 +44。
    // 不预读、不批量复制、不补默认值；异常保留已完成的字段及底层游标。
    if (archive.IsWriting())
    {
        for (auto& value : values)
            archive.SerializeSlot44Dword(reinterpret_cast<std::uint32_t&>(value));
        return;
    }
    for (auto& value : values)
        archive.SerializeSlot44Dword(reinterpret_cast<std::uint32_t&>(value));
}

void SerializeSaveParams2Native18At48B6B0(ScrSaveParams2Native& body, void* native18)
{
    using namespace NativeSerialize18Detail;
    // 48B6BD/48B6C6：两次 +3C 独立重读表，直接传原字段地址。
    Argument(native18, 0x3C, &body.scriptFileIdAtSaveParams04);
    Argument(native18, 0x3C, &body.instructionIndexAtSaveParams08);
    // 48B6D1..48B70E：仅查一次 mode；两分支各逐项重读 +44，不快照/批量回写。
    if (Mode(native18))
    {
        for (auto& value : body.values) Argument(native18, 0x44, &value);
        return;
    }
    for (auto& value : body.values) Argument(native18, 0x44, &value);
}
}
