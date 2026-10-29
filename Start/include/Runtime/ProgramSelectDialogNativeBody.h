#pragma once

#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4EAB70/4FC140/4FC280：只建立存储，即使使用{}也不清原未写字段。
struct ProgramSelectDialogNativeBodyECh final
{
    union { GuiDialogIntoNativeBody68h into; };
    std::int32_t phaseAt68;
    std::int32_t subphaseAt6C;
    std::int32_t pendingAt70;
    RECT boundsAt74;
    std::uint32_t initializedUnknownAt84[4];
    std::int32_t resultAt94;
    union { YtlReferenceSlotRuntime blackAt98; };
    union { YtlReferenceSlotRuntime canvasAtA4; };
    union { TransitOperation operationAtB0; };
    std::int32_t stepAtE0;
    std::byte unwrittenAtE4[8];
    ProgramSelectDialogNativeBodyECh() noexcept {}
};
static_assert(sizeof(ProgramSelectDialogNativeBodyECh) == 0xEC);
static_assert(alignof(ProgramSelectDialogNativeBodyECh) == 4);
static_assert(std::is_standard_layout_v<ProgramSelectDialogNativeBodyECh>);
static_assert(std::is_trivially_destructible_v<ProgramSelectDialogNativeBodyECh>);
static_assert(sizeof(TransitOperation) == 0x30);
#define PROGRAM_SELECT_OFFSET(field, offset) \
    static_assert(offsetof(ProgramSelectDialogNativeBodyECh, field) == offset)
PROGRAM_SELECT_OFFSET(into, 0x00);
PROGRAM_SELECT_OFFSET(phaseAt68, 0x68);
PROGRAM_SELECT_OFFSET(subphaseAt6C, 0x6C);
PROGRAM_SELECT_OFFSET(pendingAt70, 0x70);
PROGRAM_SELECT_OFFSET(boundsAt74, 0x74);
PROGRAM_SELECT_OFFSET(initializedUnknownAt84, 0x84);
PROGRAM_SELECT_OFFSET(resultAt94, 0x94);
PROGRAM_SELECT_OFFSET(blackAt98, 0x98);
PROGRAM_SELECT_OFFSET(canvasAtA4, 0xA4);
PROGRAM_SELECT_OFFSET(operationAtB0, 0xB0);
PROGRAM_SELECT_OFFSET(stepAtE0, 0xE0);
PROGRAM_SELECT_OFFSET(unwrittenAtE4, 0xE4);
#undef PROGRAM_SELECT_OFFSET

// 5C0128/5C01D4/5C03A0均为42槽；+A8是下一类RTTI，不是第43槽。
struct ProgramSelectDialogNativeVtable42 final
{
    GuiDialogIntoNativeVtable37 inherited;
    void (__thiscall* updateChildrenAt94)(GuiDialogNativeBody5Ch*);
    void (__thiscall* advanceTransitionAt98)(GuiDialogNativeBody5Ch*);
    YtlReferenceSlotRuntime* (__thiscall* captureAt9C)(GuiDialogNativeBody5Ch*,
        YtlReferenceSlotRuntime* freshOutput, std::int32_t includeSystem);
    void (__thiscall* redrawAtA0)(GuiDialogNativeBody5Ch*);
    void (__thiscall* reopenAtA4)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(ProgramSelectDialogNativeVtable42) == 0xA8);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, inherited) == 0);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, updateChildrenAt94) == 0x94);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, advanceTransitionAt98) == 0x98);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, captureAt9C) == 0x9C);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, redrawAtA0) == 0xA0);
static_assert(offsetof(ProgramSelectDialogNativeVtable42, reopenAtA4) == 0xA4);
}
