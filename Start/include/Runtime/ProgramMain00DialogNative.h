#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/ProgramTrainingPartsNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/InputRootNative.h>
#include <optional>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ProgramSelectDialogNativeServices;
class IProgramSelectDialogNativeSource;

struct ProgramMain00DialogNativeBodyC0h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime eventsAt5C; };
    union { YtlReferenceSlotRuntime unusedReferenceAt68; };
    union { YtlReferenceSlotRuntime partsAt74; };
    union { YtlReferenceSlotRuntime calendarAt80; };
    std::int32_t characterAt8C;
    std::int32_t resultAt90;
    std::int32_t keyboardLatchAt94; // 510970/510670；ctor不写，ResetInput负责初始化。
    std::int32_t phaseAt98;
    std::int32_t stageAt9C;
    std::int32_t weightBeforeAtA0;
    std::int32_t weightAfterAtA4;
    std::int32_t stressBeforeAtA8;
    std::int32_t stressAfterAtAC;
    std::int32_t trainingLevelAtB0;
    std::int32_t delayAtB4;
    std::int32_t amountAtB8;
    std::int32_t operationAtBC;
    ProgramMain00DialogNativeBodyC0h() noexcept {}
};
static_assert(sizeof(ProgramMain00DialogNativeBodyC0h) == 0xC0);
static_assert(alignof(ProgramMain00DialogNativeBodyC0h) == 4);
static_assert(std::is_standard_layout_v<ProgramMain00DialogNativeBodyC0h>);
static_assert(std::is_trivially_destructible_v<ProgramMain00DialogNativeBodyC0h>);
#define PROGRAM_MAIN00_OFFSET(member, offset) \
    static_assert(offsetof(ProgramMain00DialogNativeBodyC0h, member) == offset)
PROGRAM_MAIN00_OFFSET(common, 0x00);
PROGRAM_MAIN00_OFFSET(eventsAt5C, 0x5C);
PROGRAM_MAIN00_OFFSET(unusedReferenceAt68, 0x68);
PROGRAM_MAIN00_OFFSET(partsAt74, 0x74);
PROGRAM_MAIN00_OFFSET(calendarAt80, 0x80);
PROGRAM_MAIN00_OFFSET(characterAt8C, 0x8C);
PROGRAM_MAIN00_OFFSET(resultAt90, 0x90);
PROGRAM_MAIN00_OFFSET(keyboardLatchAt94, 0x94);
PROGRAM_MAIN00_OFFSET(phaseAt98, 0x98);
PROGRAM_MAIN00_OFFSET(stageAt9C, 0x9C);
PROGRAM_MAIN00_OFFSET(weightBeforeAtA0, 0xA0);
PROGRAM_MAIN00_OFFSET(weightAfterAtA4, 0xA4);
PROGRAM_MAIN00_OFFSET(stressBeforeAtA8, 0xA8);
PROGRAM_MAIN00_OFFSET(stressAfterAtAC, 0xAC);
PROGRAM_MAIN00_OFFSET(trainingLevelAtB0, 0xB0);
PROGRAM_MAIN00_OFFSET(delayAtB4, 0xB4);
PROGRAM_MAIN00_OFFSET(amountAtB8, 0xB8);
PROGRAM_MAIN00_OFFSET(operationAtBC, 0xBC);
#undef PROGRAM_MAIN00_OFFSET

class IProgramMain00DialogNativeSource
{
public:
    virtual ~IProgramMain00DialogNativeSource() = default;
    // 原内联6198C8锁/ensure与4180C0函数捕获是两个不同调用点。
    virtual void EnsureEnvironmentInline(std::uint32_t site) = 0;
    virtual EnvironmentRootStorage* EnvironmentAt6198C8(std::uint32_t site) = 0;
    virtual InputRootNativeBody24h* InputAt418210(std::uint32_t site) = 0;
    virtual const char* SoundNameAt61A6xx(std::uint32_t address) = 0;
    virtual const std::int32_t* SeVolumeAtVslot24(void* soundParams) = 0;
    virtual std::uint8_t PlaySeAt4BCD70(void* sound, const char* name,
        std::int32_t channel, std::int32_t volume, std::uint8_t restart,
        std::uint32_t site) = 0;
};
struct ProgramMain00DialogNativeServices final
{
    ProgramSelectDialogNativeServices& shared;
    IProgramMain00DialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
};
class ProgramMain00DialogNativeScope final
{
public:
    explicit ProgramMain00DialogNativeScope(ProgramMain00DialogNativeServices&) noexcept;
    explicit ProgramMain00DialogNativeScope(ProgramMain00DialogNativeServices*) noexcept;
    ~ProgramMain00DialogNativeScope() noexcept;
    ProgramMain00DialogNativeScope(const ProgramMain00DialogNativeScope&) = delete;
    ProgramMain00DialogNativeScope& operator=(const ProgramMain00DialogNativeScope&) = delete;
private:
    ProgramMain00DialogNativeServices* previous_;
    // null只清本类selector；独立GeneralConfirm等共享域保持原绑定。
    std::optional<GraphicsSurfaceNativeScope> surfaces_;
    std::optional<GuiControlGraphicsScope> graphics_;
    std::optional<GuiInputNativeScope> input_;
    std::optional<GuiDialogNativeScope> screen_;
};
ProgramMain00DialogNativeServices* SelectedProgramMain00DialogNativeServices() noexcept;
ProgramMain00DialogNativeServices& RequireProgramMain00DialogNativeServices();
const GuiDialogNativeVtable35& ProgramMain00DialogNativeVtable() noexcept;
ProgramMain00DialogNativeBodyC0h* ConstructProgramMain00DialogNativeAt4DCEB0(void* rawC0,
    GuiDialogNativeBody5Ch* parent, std::int32_t id);
ProgramMain00DialogNativeBodyC0h* ConstructProgramMain00DialogNativeAt4DCEB0(void* rawC0,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramMain00DialogNativeServices&);
void DestroyProgramMain00DialogNativeAt4F75B0(ProgramMain00DialogNativeBodyC0h&) noexcept;
ProgramMain00DialogNativeBodyC0h* DeleteProgramMain00DialogNativeAt4FC110(
    ProgramMain00DialogNativeBodyC0h*, std::uint32_t flags) noexcept;
void BeginProgramMain00DialogNativeAt50C230(ProgramMain00DialogNativeBodyC0h&);
std::int32_t PollProgramMain00DialogNativeAt52C990(ProgramMain00DialogNativeBodyC0h&);
std::int32_t EndProgramMain00DialogNativeAt508C10(ProgramMain00DialogNativeBodyC0h&);
void ChangeProgramMain00DialogNativeAt518820(ProgramMain00DialogNativeBodyC0h&,
    YtlReferenceSlotRuntime ownedChild);
std::int32_t TrainingRecordProgramMain00DialogNativeAt50B470(
    ProgramMain00DialogNativeBodyC0h&, std::int32_t operation);
std::int32_t PrepareProgramMain00DialogNativeAt50BCC0(ProgramMain00DialogNativeBodyC0h&);
void AnimateProgramMain00DialogNativeAt51BB50(ProgramMain00DialogNativeBodyC0h&);
void RefreshProgramMain00DialogNativeAt5009C0(ProgramMain00DialogNativeBodyC0h&, std::int32_t enabled);
void RenderProgramMain00DialogNativeAt5279C0(ProgramMain00DialogNativeBodyC0h&,
    std::int32_t character, std::uint32_t stress, std::int32_t trainingLevel,
    std::int32_t weight, std::int32_t levelUp, std::int32_t effect,
    std::int32_t increase, std::int32_t decrease, std::int32_t invalidate);
void CalendarProgramMain00DialogNativeAt505920(ProgramMain00DialogNativeBodyC0h&);
void CommitProgramMain00DialogNativeAt508860(ProgramMain00DialogNativeBodyC0h&);

struct ProgramTrainingRecordNative14h final
{ std::int32_t event, weightCost, stressGain, affectionGain, dependenceGain; };
static_assert(sizeof(ProgramTrainingRecordNative14h) == 0x14);
struct ProgramMain00DialogNativeEhEvidence final
{
    std::uint32_t functionVA, funcInfoVA, flags;
    std::span<const std::int32_t> previousStates;
    std::span<const std::uint32_t> actions;
};
std::span<const std::uint32_t, 35> ProgramMain00DialogNativeOriginalSlotsSample18() noexcept;
std::span<const ProgramTrainingRecordNative14h, 27> ProgramMain00DialogNativeRecordsSample18() noexcept;
std::span<const ProgramMain00DialogNativeEhEvidence> ProgramMain00DialogNativeEhSample18() noexcept;
}
