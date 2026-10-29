#pragma once

#include <Runtime/ProgramSelectDialogNativeBody.h>
#include <Runtime/ProgramSelectDialogNativeScope.h>
#include <array>
#include <span>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct ScriptParamsNativeBody;

// 只借原生叶receiver，不把完整Frame、业务或拥有权委托给Source。
class IProgramSelectDialogNativeSource
{
public:
    virtual ~IProgramSelectDialogNativeSource() = default;
    virtual void SetProgramModalFlag(bool)
    { throw std::logic_error("Program modal flag source is not bound"); }
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site) = 0;
    virtual void* RegistryAt417990(EnvironmentRootStorage*, std::uint32_t index,
        std::uint32_t site) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t site) = 0;
    virtual void* RootAt6198C4(std::uint32_t site) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* root) = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* root) = 0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* root, std::uint32_t site) = 0;
    virtual std::int32_t ResolveNamedAt498020(void* archive, const char* name,
        std::uint32_t site) = 0;
    virtual std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*, std::int32_t index,
        std::uint32_t site) = 0;
    virtual void WriteValueAt4C8220(ScriptParamsNativeBody*, std::int32_t index,
        std::int32_t value, std::uint32_t site) = 0;
    virtual std::uint8_t LoadSurfaceAt480000(void* draw, YtlReferenceSlotRuntime* output,
        const char* name, std::uint32_t site) = 0;
    virtual void* GraphicsAt4182F0(std::uint32_t site) = 0;
    virtual POINT* ScreenSizeAt47CC00(void* graphics, POINT* output, std::uint32_t site) = 0;
    virtual std::uint8_t InputAt4C3430(void* registry, std::uint32_t site) = 0;
    virtual std::uint8_t InputAt4AA970(void* registry, std::uint32_t site) = 0;
    virtual const char* TimeSoundNameAt61A664(std::uint32_t index) = 0;
    virtual const std::int32_t* BgmVolumeAtVslot14(void* params) = 0;
    virtual std::uint8_t PlayBgmAt42BFE0(void* sound, const char* name,
        std::int32_t volume, std::int32_t loop, std::uint32_t site) = 0;
    virtual std::uint8_t ClearSoundAt4384D0(void* sound, std::uint32_t site) = 0;
    virtual void StopSoundAt413270(void* sound,
        EnvironmentVectorStorage<RuntimeCString>* names, std::uint32_t site) = 0;
    virtual void CommitAt4B02E0(void* graphics, std::int32_t argument,
        std::uint32_t site) = 0;

    // 四种不同原生子类仍是必填依赖，不接受现代对象或占位正文。
    // 子ctor抛错时自行展开成员；caller仅释放对应raw正文。
    virtual GuiDialogNativeBody5Ch* ConstructMain00At4DCEB0(void* rawC0,
        GuiDialogNativeBody5Ch* parent, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructMain01At4DE520(void* raw88,
        GuiDialogNativeBody5Ch* parent, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructSystem00At4DF420(void* raw68,
        GuiDialogNativeBody5Ch* parent, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructSystem01At4DFCA0(void* raw68,
        GuiDialogNativeBody5Ch* parent, std::int32_t id) = 0;
};

struct ProgramSelectDialogNativeServices final
{
    IProgramSelectDialogNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGraphicsTransitApi& transit;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    ProgramMain00DialogNativeServices* main00=nullptr;
    ProgramMain01DialogNativeServices* main01=nullptr;
    ProgramSystemDialogNativeServices* system=nullptr;
};

ProgramSelectDialogNativeServices& RequireProgramSelectDialogNativeServices();
ProgramSelectDialogNativeServices* SelectedProgramSelectDialogNativeServices() noexcept;
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeBaseVtable() noexcept;
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeTop00Vtable() noexcept;
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeTop01Vtable() noexcept;
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4EAB70(void* rawEC);
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DE1E0(void* rawEC);
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DF0E0(void* rawEC);
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DE1E0(
    void* rawEC, ProgramSelectDialogNativeServices&);
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DF0E0(
    void* rawEC, ProgramSelectDialogNativeServices&);
void DestroyProgramSelectDialogNativeAt4F8D50(ProgramSelectDialogNativeBodyECh&) noexcept;
ProgramSelectDialogNativeBodyECh* DeleteProgramSelectDialogNativeAt4FC140(
    ProgramSelectDialogNativeBodyECh*, std::uint32_t flags) noexcept;
ProgramSelectDialogNativeBodyECh* DeleteProgramSelectDialogNativeAt4FC280(
    ProgramSelectDialogNativeBodyECh*, std::uint32_t flags) noexcept;
void BeginProgramSelectDialogNativeAt50E770(ProgramSelectDialogNativeBodyECh&);
std::int32_t PollProgramSelectDialogNativeAt52F6E0(ProgramSelectDialogNativeBodyECh&);
std::int32_t EndProgramSelectDialogNativeAt509340(ProgramSelectDialogNativeBodyECh&);
void AdvanceProgramSelectDialogNativeAt51C120(ProgramSelectDialogNativeBodyECh&);
void AdvanceProgramSelectDialogNativeAt51C4E0(ProgramSelectDialogNativeBodyECh&);
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt503AA0(
    ProgramSelectDialogNativeBodyECh&, YtlReferenceSlotRuntime* freshOutput,
    std::int32_t unusedArgument);
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt5038A0(
    ProgramSelectDialogNativeBodyECh&, YtlReferenceSlotRuntime* freshOutput,
    std::int32_t includeSystem);
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt5039A0(
    ProgramSelectDialogNativeBodyECh&, YtlReferenceSlotRuntime* freshOutput,
    std::int32_t includeSystem);
void UpdateProgramSelectDialogNativeAt521FD0(ProgramSelectDialogNativeBodyECh&);
void UpdateProgramSelectDialogNativeAt5221A0(ProgramSelectDialogNativeBodyECh&);
void RedrawProgramSelectDialogNativeAt530E40(ProgramSelectDialogNativeBodyECh&);
void ReopenProgramSelectDialogNativeAt530E80(ProgramSelectDialogNativeBodyECh&);
void ReopenProgramSelectDialogNativeAt531010(ProgramSelectDialogNativeBodyECh&);
std::span<const std::uint32_t, 42> ProgramSelectDialogNativeOriginalSlotsSample18(
    std::int32_t mode) noexcept; // -1为共同基表，0/1为实际Top表。
struct ProgramSelectDialogNativeEhEvidence final
{
    std::uint32_t functionVA, funcInfoVA, flags;
    std::span<const std::int32_t> previousStates;
    std::span<const std::uint32_t> actions;
};
std::span<const ProgramSelectDialogNativeEhEvidence> ProgramSelectDialogNativeEhSample18() noexcept;
}
