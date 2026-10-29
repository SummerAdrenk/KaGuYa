#pragma once

#include <Runtime/SystemDialogNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 原4EC8A0：80h Modal前缀之后是mode、两个Surface Ref和数字Surface数组Ref。
// union禁止Ref默认初始化提前改写原未写区；正文没有服务、modern owner或旁路状态。
struct SaveLoadMessageBoxNativeBodyA8h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime backgroundAt5C; };
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t keyboardLatchAt70;
    union { GuiClickedEventReferences eventsAt74; };
    std::int32_t modeAt80;
    union { YtlReferenceSlotRuntime pageAndSlotBaseAt84; };
    union { YtlReferenceSlotRuntime pageOnlyBaseAt90; };
    union { YtlReferenceSlotRuntime digitsAt9C; };
    SaveLoadMessageBoxNativeBodyA8h() noexcept {}
};
static_assert(sizeof(SaveLoadMessageBoxNativeBodyA8h) == 0xA8);
static_assert(alignof(SaveLoadMessageBoxNativeBodyA8h) == 4);
static_assert(std::is_standard_layout_v<SaveLoadMessageBoxNativeBodyA8h>);
static_assert(std::is_trivially_destructible_v<SaveLoadMessageBoxNativeBodyA8h>);
#define SAVELOAD_MESSAGEBOX_OFFSET(f, o) static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, f) == o)
SAVELOAD_MESSAGEBOX_OFFSET(common, 0x00);
SAVELOAD_MESSAGEBOX_OFFSET(backgroundAt5C, 0x5C);
SAVELOAD_MESSAGEBOX_OFFSET(resultAt68, 0x68);
SAVELOAD_MESSAGEBOX_OFFSET(phaseAt6C, 0x6C);
SAVELOAD_MESSAGEBOX_OFFSET(keyboardLatchAt70, 0x70);
SAVELOAD_MESSAGEBOX_OFFSET(eventsAt74, 0x74);
SAVELOAD_MESSAGEBOX_OFFSET(modeAt80, 0x80);
SAVELOAD_MESSAGEBOX_OFFSET(pageAndSlotBaseAt84, 0x84);
SAVELOAD_MESSAGEBOX_OFFSET(pageOnlyBaseAt90, 0x90);
SAVELOAD_MESSAGEBOX_OFFSET(digitsAt9C, 0x9C);
#undef SAVELOAD_MESSAGEBOX_OFFSET
static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, backgroundAt5C) == offsetof(SystemDialogNativeBody88h, backgroundAt5C));
static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, resultAt68) == offsetof(SystemDialogNativeBody88h, resultAt68));
static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, phaseAt6C) == offsetof(SystemDialogNativeBody88h, phaseAt6C));
static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, keyboardLatchAt70) == offsetof(SystemDialogNativeBody88h, keyboardLatchAt70));
static_assert(offsetof(SaveLoadMessageBoxNativeBodyA8h, eventsAt74) == offsetof(SystemDialogNativeBody88h, eventsAt74));

// 实读PE 5BE614..5BE6A8，共38槽；5BE6AC为下一类RTTI。
using SaveLoadMessageBoxNativeVtable38 = SystemDialogNativeVtable38;
static_assert(sizeof(SaveLoadMessageBoxNativeVtable38) == 0x98);

// 原606280为两个12-byte记录；描述符由type确定原步长，不能缩成现代七控件列表。
struct SaveLoadMessageBoxNativeControlGroup final
{
    std::int32_t typeAt00;
    std::int32_t countAt04;
    const void* descriptorsAt08;
};
struct SaveLoadMessageBoxNativeNavigationGroup final
{
    std::int32_t inputAt00;
    const GuiDialogNavigationMapping* pairsAt04;
};
static_assert(sizeof(SaveLoadMessageBoxNativeControlGroup) == 0x0C);
static_assert(sizeof(SaveLoadMessageBoxNativeNavigationGroup) == 8);

// repo尚无这四个完整子工厂。依赖必填，输出必须是真实native Ref12/Control正文。
// 只委托一个子工厂，不允许把父Populate、Prepare或Poll塞进Source。
class ISaveLoadMessageBoxNativeChildFactories
{
public:
    virtual ~ISaveLoadMessageBoxNativeChildFactories() = default;
    virtual YtlReferenceSlotRuntime* CreateAt5043B0(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor48h) = 0;
    virtual YtlReferenceSlotRuntime* CreateAt503BB0(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor44h) = 0;
    virtual YtlReferenceSlotRuntime* CreateAt503E60(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor2Ch) = 0;
    virtual YtlReferenceSlotRuntime* CreateAt505090(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor2Ch) = 0;
};
struct SaveLoadMessageBoxNativeServices final
{
    // 共用Modal表只用此既有Source的Root/Draw/ModalFlag/Capture/DrawBackground叶。
    ISystemDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGraphicsSurfaceRawCopyApi& copy;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    ISaveLoadMessageBoxNativeChildFactories& childFactories;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};

// scope必须覆盖Save构造/EH、Prepare、全部虚调用、父/child最后一份Ref释放。
// nullptr明确遮蔽外层服务；TLS只借当前scope，不保存业务数据或拥有引用。
class SaveLoadMessageBoxNativeScope final
{
public:
    explicit SaveLoadMessageBoxNativeScope(SaveLoadMessageBoxNativeServices&) noexcept;
    explicit SaveLoadMessageBoxNativeScope(SaveLoadMessageBoxNativeServices*) noexcept;
    ~SaveLoadMessageBoxNativeScope() noexcept;
    SaveLoadMessageBoxNativeScope(const SaveLoadMessageBoxNativeScope&) = delete;
    SaveLoadMessageBoxNativeScope& operator=(const SaveLoadMessageBoxNativeScope&) = delete;
private:
    SaveLoadMessageBoxNativeServices* previous_;
    std::optional<SystemDialogNativeServices> modalServices_;
    SystemDialogNativeScope modalScope_;
};
SaveLoadMessageBoxNativeServices& RequireSaveLoadMessageBoxNativeServices();
const SaveLoadMessageBoxNativeVtable38& SaveLoadMessageBoxNativeVtable() noexcept;
// publicNativeVtable供主线直接选表；不是构造器注入的假dispatch。
inline const SaveLoadMessageBoxNativeVtable38& SaveLoadMessageBoxPublicNativeVtable() noexcept
{ return SaveLoadMessageBoxNativeVtable(); }
SaveLoadMessageBoxNativeBodyA8h* ConstructSaveLoadMessageBoxNativeAt4EC8A0(void* storageA8h);
GuiDialogNativeBody5Ch* ConstructSaveLoadMessageBoxAt4EC8A0(void* storageA8h);
// 显式数据重定位入口，使原switch全部八type可达；无持久数据副本，默认ctor仍固定原两组。
SaveLoadMessageBoxNativeBodyA8h* ConstructSaveLoadMessageBoxNativeAt4EC8A0(void* storageA8h,
    std::span<const SaveLoadMessageBoxNativeControlGroup> groups,
    std::span<const SaveLoadMessageBoxNativeNavigationGroup> navigation);
void DestroySaveLoadMessageBoxNativeAt4F92E0(SaveLoadMessageBoxNativeBodyA8h*) noexcept;
SaveLoadMessageBoxNativeBodyA8h* DeleteSaveLoadMessageBoxNativeAt4FDDA0(
    SaveLoadMessageBoxNativeBodyA8h*, std::uint32_t flags) noexcept;
void BeginSaveLoadMessageBoxNativeAt50EAA0(GuiDialogNativeBody5Ch*);
std::int32_t PollSaveLoadMessageBoxModalNativeAt52DBC0(GuiDialogNativeBody5Ch*);
std::int32_t EndSaveLoadMessageBoxNativeAt508E80(GuiDialogNativeBody5Ch*);
std::int32_t PollSaveLoadMessageBoxNativeAt5253D0(GuiDialogNativeBody5Ch*);
void PrepareSaveLoadMessageBoxPageNativeAt502950(GuiDialogNativeBody5Ch*, std::int32_t pageNumber);
void PrepareSaveLoadMessageBoxPageAndSlotNativeAt502B10(GuiDialogNativeBody5Ch*,
    std::int32_t pageNumber, std::int32_t slotNumber);
void DrawSaveLoadMessageBoxNumberNativeAt529FD0(GuiDialogNativeBody5Ch*,
    YtlReferenceSlotRuntime* borrowedSurface, std::int32_t number, std::uint32_t pageFlag);

std::span<const SaveLoadMessageBoxNativeControlGroup, 2> SaveLoadMessageBoxNativeGroupsSample18() noexcept;
std::span<const SaveLoadMessageBoxNativeNavigationGroup, 2> SaveLoadMessageBoxNativeNavigationSample18() noexcept;
std::span<const GuiStaticDescriptorNative24h, 5> SaveLoadMessageBoxNativeStaticsSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 2> SaveLoadMessageBoxNativeButtonsSample18() noexcept;
std::span<const char* const, 10> SaveLoadMessageBoxNativeDigitsSample18() noexcept;
std::span<const POINT, 5> SaveLoadMessageBoxNativeDigitPositionsSample18() noexcept;
std::span<const std::uint32_t, 38> SaveLoadMessageBoxNativeOriginalVtableSample18() noexcept;
std::span<const std::array<std::uint32_t, 9>, 5> SaveLoadMessageBoxNativeRawStaticsSample18() noexcept;
std::span<const std::array<std::uint32_t, 11>, 2> SaveLoadMessageBoxNativeRawButtonsSample18() noexcept;
}
