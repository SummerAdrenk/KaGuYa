#pragma once

#include <Runtime/GuiControlResourceNative.h>

namespace KaGuYa::Start::Runtime
{
// 4F55A0：三个原生Surface向量；5C10C4仅属于外部typed18控制块。
struct ProgramTrainingPartsNativeBody74h final
{
    union { GuiControlSurfaceReferences canvasesAt00; };
    union { GuiControlSurfaceReferences stressAt0C; };
    RECT stressBoundsAt18;
    union { GuiControlSurfaceReferences symbolsAt28; };
    RECT numberBoundsAt34;
    union { YtlReferenceSlotRuntime effectAt44; };
    union { YtlReferenceSlotRuntime increaseAt50; };
    union { YtlReferenceSlotRuntime decreaseAt5C; };
    union { YtlReferenceSlotRuntime levelAt68; };
    ProgramTrainingPartsNativeBody74h() noexcept {}
};
static_assert(sizeof(ProgramTrainingPartsNativeBody74h) == 0x74);
static_assert(alignof(ProgramTrainingPartsNativeBody74h) == 4);
static_assert(!std::is_polymorphic_v<ProgramTrainingPartsNativeBody74h>);
static_assert(std::is_standard_layout_v<ProgramTrainingPartsNativeBody74h>);
static_assert(std::is_trivially_destructible_v<ProgramTrainingPartsNativeBody74h>);
#define PROGRAM_PARTS_OFFSET(member, offset) \
    static_assert(offsetof(ProgramTrainingPartsNativeBody74h, member) == offset)
PROGRAM_PARTS_OFFSET(canvasesAt00, 0x00);
PROGRAM_PARTS_OFFSET(stressAt0C, 0x0C);
PROGRAM_PARTS_OFFSET(stressBoundsAt18, 0x18);
PROGRAM_PARTS_OFFSET(symbolsAt28, 0x28);
PROGRAM_PARTS_OFFSET(numberBoundsAt34, 0x34);
PROGRAM_PARTS_OFFSET(effectAt44, 0x44);
PROGRAM_PARTS_OFFSET(increaseAt50, 0x50);
PROGRAM_PARTS_OFFSET(decreaseAt5C, 0x5C);
PROGRAM_PARTS_OFFSET(levelAt68, 0x68);
#undef PROGRAM_PARTS_OFFSET

ProgramTrainingPartsNativeBody74h* ConstructProgramTrainingPartsNativeAt4F55A0(void* raw74);
void DestroyProgramTrainingPartsNativeAt4F9D40(ProgramTrainingPartsNativeBody74h&) noexcept;
// Main00内联adopt：先发布成员control，再逐字段初始化；失败不补删完成正文。
void AdoptProgramTrainingPartsNativeAt4DD0E0(YtlReferenceSlotRuntime&,
    ProgramTrainingPartsNativeBody74h*);
}
