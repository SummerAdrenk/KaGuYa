#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ProgramSelectDialogNativeServices;
class IProgramSelectDialogNativeSource;
struct ScriptParamsNativeBody;

// 4DE520/5C04DC：5C是事件manager，不是Into的嵌套dialog vector。
struct ProgramMain01DialogNativeBody88h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime managerAt5C; };
    union { YtlReferenceSlotRuntime reservedReferenceAt68; };
    union { YtlReferenceSlotRuntime backgroundAt74; };
    std::int32_t selectionAt80;
    std::int32_t confirmLatchAt84; // ctor不写；510690清零，510A80按下/释放时使用。
    ProgramMain01DialogNativeBody88h() noexcept {}
};
static_assert(sizeof(ProgramMain01DialogNativeBody88h) == 0x88);
static_assert(alignof(ProgramMain01DialogNativeBody88h) == 4);
static_assert(std::is_standard_layout_v<ProgramMain01DialogNativeBody88h>);
static_assert(std::is_trivially_destructible_v<ProgramMain01DialogNativeBody88h>);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, common) == 0);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, managerAt5C) == 0x5C);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, reservedReferenceAt68) == 0x68);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, backgroundAt74) == 0x74);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, selectionAt80) == 0x80);
static_assert(offsetof(ProgramMain01DialogNativeBody88h, confirmLatchAt84) == 0x84);

// 只借真实singleton/参数叶；不把5281F0、501880或整段选择业务交给Source。
class IProgramMain01DialogNativeSource
{
public:
    virtual ~IProgramMain01DialogNativeSource() = default;
    // site为原调用点；5018D0/501B80为与4180C0同体的内联锁路径。
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site) = 0;
    virtual void* RegistryAt417990(EnvironmentRootStorage*, std::uint32_t index,
        std::uint32_t site) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t site) = 0;
    virtual void* RootAt6198C4(std::uint32_t site) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* root) = 0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* root, std::uint32_t site) = 0;
    virtual std::int32_t ResolveNamedAt498020(void* archive, const char* name,
        std::uint32_t site) = 0;
    virtual std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*, std::int32_t index,
        std::uint32_t site) = 0;
    virtual std::uint8_t LoadSurfaceAt480000(void* draw, YtlReferenceSlotRuntime* output,
        const char* name, std::uint32_t site) = 0;
};

// Main可直接借ProgramSelect已接通的同名叶；无默认成功实现。
class ProgramMain01DialogNativeSharedSource final : public IProgramMain01DialogNativeSource
{
public:
    explicit ProgramMain01DialogNativeSharedSource(IProgramSelectDialogNativeSource& source) noexcept
        : source_(source) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    void* RegistryAt417990(EnvironmentRootStorage*, std::uint32_t, std::uint32_t) override;
    void EnsureRootAt413BE0(std::uint32_t) override;
    void* RootAt6198C4(std::uint32_t) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*, std::uint32_t) override;
    std::int32_t ResolveNamedAt498020(void*, const char*, std::uint32_t) override;
    std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*, std::int32_t, std::uint32_t) override;
    std::uint8_t LoadSurfaceAt480000(void*, YtlReferenceSlotRuntime*, const char*, std::uint32_t) override;
private:
    IProgramSelectDialogNativeSource& source_;
};
struct ProgramMain01DialogNativeServices final
{
    IProgramMain01DialogNativeSource& source;
    ProgramSelectDialogNativeServices& shared;
    IGuiControlResourceLoadServices& loader;
};
class ProgramMain01DialogNativeScope final
{
public:
    explicit ProgramMain01DialogNativeScope(ProgramMain01DialogNativeServices&) noexcept;
    explicit ProgramMain01DialogNativeScope(ProgramMain01DialogNativeServices*) noexcept;
    ~ProgramMain01DialogNativeScope() noexcept;
    ProgramMain01DialogNativeScope(const ProgramMain01DialogNativeScope&) = delete;
    ProgramMain01DialogNativeScope& operator=(const ProgramMain01DialogNativeScope&) = delete;
private:
    ProgramMain01DialogNativeServices* previous_;
};
ProgramMain01DialogNativeServices* SelectedProgramMain01DialogNativeServices() noexcept;
ProgramMain01DialogNativeServices& RequireProgramMain01DialogNativeServices();
const GuiDialogNativeVtable35& ProgramMain01DialogNativeVtable() noexcept;
ProgramMain01DialogNativeBody88h* ConstructProgramMain01DialogNativeAt4DE520(void* raw88,
    GuiDialogNativeBody5Ch* parent, std::int32_t id);
ProgramMain01DialogNativeBody88h* ConstructProgramMain01DialogNativeAt4DE520(void* raw88,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramMain01DialogNativeServices&);
void DestroyProgramMain01DialogNative(ProgramMain01DialogNativeBody88h&) noexcept;
ProgramMain01DialogNativeBody88h* DeleteProgramMain01DialogNativeAt4FC170(
    ProgramMain01DialogNativeBody88h*, std::uint32_t flags) noexcept;
// 对应Top01的4DF170..4DF1C9；fresh Ref，new18失败不补删已完成88h正文。
YtlReferenceSlotRuntime* ConstructProgramMain01DialogNativeReferenceAt4DF170(
    YtlReferenceSlotRuntime& freshOutput, ProgramMain01DialogNativeBody88h* body);
void BeginProgramMain01DialogNativeAt50C2A0(ProgramMain01DialogNativeBody88h&);
std::int32_t PollProgramMain01DialogNativeAt52D040(ProgramMain01DialogNativeBody88h&);
void RefreshProgramMain01DialogNativeAt501880(ProgramMain01DialogNativeBody88h&);
void ComposeProgramMain01DialogNativeAt5281F0(ProgramMain01DialogNativeBody88h&);

struct ProgramMain01DialogNativeControlGroup final
{
    std::int32_t typeAt00, countAt04;
    const void* descriptorsAt08;
};
struct ProgramMain01DialogNativeNavigationGroup final
{
    std::int32_t inputAt00;
    const GuiDialogNavigationMapping* pairsAt04;
};
struct ProgramMain01DialogNativeAvailabilityRecord14h final
{
    const char* nameAt00;
    std::int32_t buttonAt04;
    std::int32_t symbolsAt08[3];
};
struct ProgramMain01DialogNativeAvailabilityRecord10h final
{
    const char* nameAt00;
    std::int32_t buttonAt04;
    std::int32_t symbolsAt08[2];
};
static_assert(sizeof(ProgramMain01DialogNativeControlGroup) == 12);
static_assert(sizeof(ProgramMain01DialogNativeNavigationGroup) == 8);
static_assert(sizeof(ProgramMain01DialogNativeAvailabilityRecord14h) == 0x14);
static_assert(sizeof(ProgramMain01DialogNativeAvailabilityRecord10h) == 0x10);
std::span<const ProgramMain01DialogNativeControlGroup, 2> ProgramMain01DialogNativeGroupsSample18() noexcept;
std::span<const ProgramMain01DialogNativeNavigationGroup, 2> ProgramMain01DialogNativeNavigationSample18() noexcept;
std::span<const ProgramMain01DialogNativeAvailabilityRecord14h, 4> ProgramMain01DialogNativeAvailabilitySample18() noexcept;
const ProgramMain01DialogNativeAvailabilityRecord10h& ProgramMain01DialogNativeLastAvailabilitySample18() noexcept;
const char* ProgramMain01DialogNativeBackgroundSample18() noexcept;
const char* const* ProgramMain01DialogNativeTimeNamesSample18() noexcept;
std::span<const char* const, 10> ProgramMain01DialogNativeDigitsSample18() noexcept;
std::span<const std::uint32_t, 35> ProgramMain01DialogNativeOriginalSlotsSample18() noexcept;
struct ProgramMain01DialogNativeEhEvidence final
{
    std::uint32_t functionVA, funcInfoVA, flags;
    std::span<const std::int32_t> previousStates;
    std::span<const std::uint32_t> actions;
};
std::span<const ProgramMain01DialogNativeEhEvidence, 5> ProgramMain01DialogNativeEhSample18() noexcept;
}
