#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct ProgramSelectDialogNativeServices;
class IProgramSelectDialogNativeSource;
// 4DF420/4DFCA0：直接GuiDialog派生；5C是manager Ref，绝不是Into的vector。
struct ProgramSystemDialogNativeBody68h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime clickedManagerAt5C; };
    ProgramSystemDialogNativeBody68h() noexcept {}
};
static_assert(sizeof(ProgramSystemDialogNativeBody68h) == 0x68);
static_assert(alignof(ProgramSystemDialogNativeBody68h) == 4);
static_assert(offsetof(ProgramSystemDialogNativeBody68h, common) == 0);
static_assert(offsetof(ProgramSystemDialogNativeBody68h, clickedManagerAt5C) == 0x5C);
static_assert(std::is_standard_layout_v<ProgramSystemDialogNativeBody68h>);
static_assert(std::is_trivially_destructible_v<ProgramSystemDialogNativeBody68h>);

class IProgramSystemDialogNativeSource
{
public:
    virtual ~IProgramSystemDialogNativeSource() = default;
    // 只借两个真实叶；禁止将Begin/Poll、菜单构造或字符串业务整体转交Source。
    virtual std::int32_t PollClickedEventManagerAt50FDB0(GuiClickedEventReferences*,
        std::int32_t clear, std::uint32_t site) = 0;
    virtual YtlReferenceSlotRuntime& RootProgramSlotAt30(void* capturedRoot) = 0;
};
struct ProgramSystemDialogNativeServices final
{
    IProgramSystemDialogNativeSource& source;
    // 显式借用父服务的graphics/input/screen/surfaces及Env/Root叶，不修改父头。
    ProgramSelectDialogNativeServices& parent;
    IGuiControlResourceLoadServices& loader;
};
ProgramSystemDialogNativeServices* SelectedProgramSystemDialogNativeServices() noexcept;
ProgramSystemDialogNativeServices& RequireProgramSystemDialogNativeServices();

struct ProgramSystemDialogNativeControlGroup final
{
    std::int32_t kindAt00, countAt04;
    const void* descriptorsAt08;
};
static_assert(sizeof(ProgramSystemDialogNativeControlGroup) == 0x0C);
struct ProgramSystemDialogNativeRoute final
{
    std::int32_t eventAt00;
    const char* nameAt04;
};
static_assert(sizeof(ProgramSystemDialogNativeRoute) == 8);
std::span<const ProgramSystemDialogNativeControlGroup, 4> ProgramSystem00DialogNativeGroupsSample18() noexcept;
std::span<const ProgramSystemDialogNativeControlGroup, 4> ProgramSystem01DialogNativeGroupsSample18() noexcept;
std::span<const ProgramSystemDialogNativeRoute, 6> ProgramSystem00DialogNativeRoutesSample18() noexcept;
std::span<const ProgramSystemDialogNativeRoute, 6> ProgramSystem01DialogNativeRoutesSample18() noexcept;

const GuiDialogNativeVtable35& ProgramSystem00DialogNativeVtable() noexcept;
const GuiDialogNativeVtable35& ProgramSystem01DialogNativeVtable() noexcept;
ProgramSystemDialogNativeBody68h* ConstructProgramSystem00DialogNativeAt4DF420(
    void* raw68, GuiDialogNativeBody5Ch* parent, std::int32_t id);
ProgramSystemDialogNativeBody68h* ConstructProgramSystem01DialogNativeAt4DFCA0(
    void* raw68, GuiDialogNativeBody5Ch* parent, std::int32_t id);
ProgramSystemDialogNativeBody68h* ConstructProgramSystem00DialogNativeAt4DF420(
    void* raw68, GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramSystemDialogNativeServices&);
ProgramSystemDialogNativeBody68h* ConstructProgramSystem01DialogNativeAt4DFCA0(
    void* raw68, GuiDialogNativeBody5Ch* parent, std::int32_t id, ProgramSystemDialogNativeServices&);
// 显式描述符入口供FI/可变原表核对；仍完整执行原八种工厂switch，不补导航/Input。
ProgramSystemDialogNativeBody68h* ConstructProgramSystemDialogNativeWithGroups(
    void* raw68, GuiDialogNativeBody5Ch* parent, std::int32_t id, bool mode01,
    std::span<const ProgramSystemDialogNativeControlGroup, 4>, ProgramSystemDialogNativeServices&);
void DestroyProgramSystem00DialogNative(ProgramSystemDialogNativeBody68h&) noexcept;
void DestroyProgramSystem01DialogNative(ProgramSystemDialogNativeBody68h&) noexcept;
ProgramSystemDialogNativeBody68h* DeleteProgramSystem00DialogNativeAt4FC2B0(
    ProgramSystemDialogNativeBody68h*, std::uint32_t flags) noexcept;
ProgramSystemDialogNativeBody68h* DeleteProgramSystem01DialogNativeAt4FC350(
    ProgramSystemDialogNativeBody68h*, std::uint32_t flags) noexcept;
void BeginProgramSystem00DialogNativeAt50C300(ProgramSystemDialogNativeBody68h&);
void BeginProgramSystem01DialogNativeAt50C5F0(ProgramSystemDialogNativeBody68h&);
std::int32_t PollProgramSystem00DialogNativeAt52D090(ProgramSystemDialogNativeBody68h&);
std::int32_t PollProgramSystem01DialogNativeAt52D270(ProgramSystemDialogNativeBody68h&);
// 5C10AC/5C10F4真实typed18控制块；此分配叶只写vptr/payload，计数/owning/stride/count
// 由caller按原发布顺序初始化。父ctor仍自行创建它的child Ref，不要再增添一层owner。
EnvironmentObjectReferenceControl* NewProgramSystem00DialogNativeControl(
    ProgramSystemDialogNativeBody68h*);
EnvironmentObjectReferenceControl* NewProgramSystem01DialogNativeControl(
    ProgramSystemDialogNativeBody68h*);

struct ProgramSystemDialogNativeEhEvidence final
{
    std::uint32_t functionVA, funcInfoVA, unwindMapVA, flags;
    std::span<const std::int32_t> previousStates;
    std::span<const std::uint32_t> actions;
};
struct ProgramSystemDialogNativeTypeEvidence final
{
    std::uint32_t constructorVA, vtableVA, referenceVtableVA, referenceLocatorVA;
    std::uint32_t referenceDestroyVA, referenceDeleteVA;
    std::span<const std::uint32_t, 35> slots;
};
std::span<const ProgramSystemDialogNativeTypeEvidence, 2> ProgramSystemDialogNativeTypesSample18() noexcept;
std::span<const ProgramSystemDialogNativeEhEvidence, 8> ProgramSystemDialogNativeEhSample18() noexcept;
struct ProgramSystemDialogNativeStringEvidence final { std::uint32_t va; const char* bytes; };
std::span<const ProgramSystemDialogNativeStringEvidence> ProgramSystemDialogNativeStringsSample18() noexcept;
}
