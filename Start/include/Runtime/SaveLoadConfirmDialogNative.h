#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/RootTransitionFrameRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 4EACA0/4F8EB0：各union只建立原存储，不提前写未初始化的标量。
struct SaveLoadConfirmDialogNativeBodyE4h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime capturedAt5C; };
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t actionFiveAt70;
    union { GuiClickedEventReferences eventsAt74; };
    std::int32_t modeAt80;
    union { YtlReferenceSlotRuntime maskAt84; };
    union { YtlReferenceSlotRuntime labelsAt90; };
    union { YtlReferenceSlotRuntime previewAt9C; };
    union { GuiControlSurfaceReferences digitsAtA8[4]; };
    union { GuiControlSurfaceReferences specialPagesAtD8; };
    SaveLoadConfirmDialogNativeBodyE4h() noexcept {}
};
static_assert(sizeof(SaveLoadConfirmDialogNativeBodyE4h) == 0xE4);
static_assert(alignof(SaveLoadConfirmDialogNativeBodyE4h) == 4);
static_assert(std::is_standard_layout_v<SaveLoadConfirmDialogNativeBodyE4h>);
static_assert(std::is_trivially_destructible_v<SaveLoadConfirmDialogNativeBodyE4h>);
#define SAVE_CONFIRM_OFFSET(field, offset) static_assert(offsetof(SaveLoadConfirmDialogNativeBodyE4h, field) == offset)
SAVE_CONFIRM_OFFSET(common, 0); SAVE_CONFIRM_OFFSET(capturedAt5C, 0x5C);
SAVE_CONFIRM_OFFSET(resultAt68, 0x68); SAVE_CONFIRM_OFFSET(phaseAt6C, 0x6C);
SAVE_CONFIRM_OFFSET(actionFiveAt70, 0x70); SAVE_CONFIRM_OFFSET(eventsAt74, 0x74);
SAVE_CONFIRM_OFFSET(modeAt80, 0x80); SAVE_CONFIRM_OFFSET(maskAt84, 0x84);
SAVE_CONFIRM_OFFSET(labelsAt90, 0x90); SAVE_CONFIRM_OFFSET(previewAt9C, 0x9C);
SAVE_CONFIRM_OFFSET(digitsAtA8, 0xA8); SAVE_CONFIRM_OFFSET(specialPagesAtD8, 0xD8);
#undef SAVE_CONFIRM_OFFSET

// 5BE328..5BE3C0共39槽，5BE3C4是下一类RTTI；不是现代90h确认框。
struct SaveLoadConfirmDialogNativeVtable39 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* beginHookAt8C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* readEventsAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* endHookAt94)(GuiDialogNativeBody5Ch*);
    void (__thiscall* writeModeAt98)(GuiDialogNativeBody5Ch*, std::int32_t);
};
static_assert(sizeof(SaveLoadConfirmDialogNativeVtable39) == 0x9C);
static_assert(alignof(SaveLoadConfirmDialogNativeVtable39) == 4);
static_assert(std::is_standard_layout_v<SaveLoadConfirmDialogNativeVtable39>);
static_assert(offsetof(SaveLoadConfirmDialogNativeVtable39, common) == 0);
static_assert(offsetof(SaveLoadConfirmDialogNativeVtable39, beginHookAt8C) == 0x8C);
static_assert(offsetof(SaveLoadConfirmDialogNativeVtable39, readEventsAt90) == 0x90);
static_assert(offsetof(SaveLoadConfirmDialogNativeVtable39, endHookAt94) == 0x94);
static_assert(offsetof(SaveLoadConfirmDialogNativeVtable39, writeModeAt98) == 0x98);

// 只接系统叶/单点getter。构造、准备、输入、Poll、绘制均由本原生实现承担。
class ISaveLoadConfirmDialogNativeSource : public IGuiControlResourceLoadServices
{
public:
    virtual std::uint8_t LoadSurfaceAt480000(void* receiver,
        YtlReferenceSlotRuntime& output, const char* name) = 0;
    virtual const RuntimeCString& MaskNameAt61A608() = 0;
    virtual const char* FontFaceAt61A60C() = 0;
    virtual bool CreateTextFontAt4388B0(const GraphicsTextNativeFontParameters*) = 0;
    virtual IGraphicsTextApi& TextAt4182F0() = 0;
    // 保留18号历史数据查询接口；跨游戏显示只读profile，不以邻接18号数据兜底。
    // 非法mode/超长数字串在消费者明确拒绝；正式Host不得打开/装载原EXE文件。
    virtual std::int32_t ReadImmutableDwordAtOriginalVa(std::uint32_t address) = 0;
};
struct SaveLoadConfirmDialogNativeServices final
{
    ISaveLoadConfirmDialogNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsSurfaceRawCopyApi& rawCopy;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGuiModalRootSource& root;
    IModalDialogRunnerServices& modal;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 必须从Save构造前覆盖所有调用，直到最后一个E4h Ref退役；不拥有正文或元数据。
class SaveLoadConfirmDialogNativeScope final
{
public:
    explicit SaveLoadConfirmDialogNativeScope(SaveLoadConfirmDialogNativeServices&) noexcept;
    explicit SaveLoadConfirmDialogNativeScope(SaveLoadConfirmDialogNativeServices*) noexcept;
    ~SaveLoadConfirmDialogNativeScope() noexcept;
    SaveLoadConfirmDialogNativeScope(const SaveLoadConfirmDialogNativeScope&) = delete;
    SaveLoadConfirmDialogNativeScope& operator=(const SaveLoadConfirmDialogNativeScope&) = delete;
private:
    SaveLoadConfirmDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SaveLoadConfirmDialogNativeServices& RequireSaveLoadConfirmDialogNativeServices();
const SaveLoadConfirmDialogNativeVtable39& SaveLoadConfirmDialogNativeVtable() noexcept;
std::span<const std::uint32_t, 39> SaveLoadConfirmDialogNativeOriginalSlots() noexcept;
std::span<const GuiStaticDescriptorNative24h, 3> SaveLoadConfirmDialogNativeStaticDescriptors() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 4> SaveLoadConfirmDialogNativeButtonDescriptors() noexcept;
std::span<const char* const> SaveLoadConfirmDialogNativeDigitNames(std::uint32_t group) noexcept;
const char* SaveLoadConfirmDialogNativeMaskNameSample18() noexcept;
const char* SaveLoadConfirmDialogNativeFontFaceSample18() noexcept;
// 编译内置5B6AC4..5B6D27的153个原DWORD；无Root/PE文件依赖。
// 只接4字节对齐VA，映射外抛CAtlException(E_INVALIDARG)，不造默认值。
std::int32_t ReadSaveLoadConfirmDialogNativeImmutableDword(std::uint32_t originalVa);

GuiDialogNativeBody5Ch* ConstructSaveLoadConfirmDialogNativeAt4EACA0(void* storage);
void DestroySaveLoadConfirmDialogNativeAt4F8EB0(GuiDialogNativeBody5Ch*) noexcept;
GuiDialogNativeBody5Ch* DeleteSaveLoadConfirmDialogNativeAt4FDD40(
    GuiDialogNativeBody5Ch*, std::uint32_t flags) noexcept;
void WriteSaveLoadConfirmDialogNativeModeAt536140(GuiDialogNativeBody5Ch*, std::int32_t) noexcept;
// 三业务入口：+98写mode；535FF0消费四个已拥有CString；538430消费Ref12。
// owned参数应置于无自动ATL析构的存储中，成功或异常后均不得再次析构。
void ConfigureSaveLoadConfirmDialogNativeAt535FF0(GuiDialogNativeBody5Ch*,
    RuntimeCString& ownedPage, RuntimeCString& ownedSlot, RuntimeCString& ownedTimestamp,
    RuntimeCString& ownedScene, std::uint8_t saveMode);
void AssignSaveLoadConfirmDialogNativePreviewAt538430(GuiDialogNativeBody5Ch*,
    YtlReferenceSlotRuntime& ownedArgument);
// 借用便捷入口先按原CString复制规则建立四个参数，然后调用消费入口。
void ConfigureSaveLoadConfirmDialogNativeBorrowed(GuiDialogNativeBody5Ch*,
    const RuntimeCString& page, const RuntimeCString& slot, const RuntimeCString& timestamp,
    const RuntimeCString& scene, std::uint8_t saveMode);
// 每次模态虚调用重新解析同一原Ref；metadata借用且长寿，正文不保存target。
class SaveLoadConfirmDialogNativeModalTarget final : public IModalDialogRuntimeTargetReference,
    private IModalDialogRuntimeTarget
{
public:
    explicit SaveLoadConfirmDialogNativeModalTarget(YtlReferenceSlotRuntime& reference) noexcept
        : reference_(reference) {}
    IModalDialogRuntimeTarget* ResolveAt4A5330() override;
private:
    GuiDialogNativeBody5Ch& Resolve() const;
    void BeginAtVslot44() override;
    bool PollAtVslot48() override;
    bool EndAtVslot4C() override;
    std::int32_t ResultAtVslot50() const override;
    YtlReferenceSlotRuntime& reference_;
};
std::int32_t RunSaveLoadConfirmDialogNativeAt5071D0(YtlReferenceSlotRuntime&);
}
