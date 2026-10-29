#pragma once

#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/NativeArchiveRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// sample18：5B2688 两槽为 48B6B0 / 469240；正文不保存 owner 或业务镜像。
class ScrSaveParams2Native final
{
public:
    static constexpr std::size_t ValueCount = 512u;
    static constexpr std::uint32_t OriginalObjectSize = 0x80C;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B2688;
    static constexpr std::uint32_t OriginalSerializeAddress = 0x48B6B0;
    static constexpr std::uint32_t OriginalDeletingDestructorAddress = 0x469240;

    // 46CF71..46CF9D：先 raw 分配再 placement 构造；+04/+08 保留原存储字节。
    ScrSaveParams2Native() noexcept;
    ~ScrSaveParams2Native() noexcept;
    ScrSaveParams2Native(const ScrSaveParams2Native&) = delete;
    ScrSaveParams2Native& operator=(const ScrSaveParams2Native&) = delete;

    // 明确旧宿主入口，不进入 native vptr。
    void Serialize(SerializeBuffer& archive);
    void* DeleteAtVslot04(std::uint32_t flags) noexcept;

    const NativeArchiveVtable2* vptrAt00;
    std::int32_t scriptFileIdAtSaveParams04;
    std::int32_t instructionIndexAtSaveParams08;
    std::array<std::int32_t, ValueCount> values;
};
static_assert(sizeof(ScrSaveParams2Native) == 0x80C);
static_assert(alignof(ScrSaveParams2Native) == 4);
static_assert(!std::is_polymorphic_v<ScrSaveParams2Native>);
static_assert(std::is_standard_layout_v<ScrSaveParams2Native>);
static_assert(offsetof(ScrSaveParams2Native, vptrAt00) == 0);
static_assert(offsetof(ScrSaveParams2Native, scriptFileIdAtSaveParams04) == 0x04);
static_assert(offsetof(ScrSaveParams2Native, instructionIndexAtSaveParams08) == 0x08);
static_assert(offsetof(ScrSaveParams2Native, values) == 0x0C);
}
