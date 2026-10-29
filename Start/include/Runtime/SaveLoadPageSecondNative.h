#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 5BE4F0..5BE57C：末槽是三参数purecall，不是可返回成功的默认翻页器。
struct SaveLoadPageSecondNativeVtable36 final
{
    GuiDialogNativeVtable35 inheritedGui35;
    std::int32_t (__thiscall* changePageAt8C)(GuiDialogNativeBody5Ch*,
        std::int32_t page, std::int32_t notify, std::int32_t force);
};
static_assert(sizeof(SaveLoadPageSecondNativeVtable36) == 0x90);
static_assert(offsetof(SaveLoadPageSecondNativeVtable36, inheritedGui35) == 0);
static_assert(offsetof(SaveLoadPageSecondNativeVtable36, changePageAt8C) == 0x8C);
static_assert(std::is_standard_layout_v<SaveLoadPageSecondNativeVtable36>);

// 4EF5E0只在GuiDialog构造成功后写+70/+80的Ref与+8C三指针。
// +5C/+60/+7C/+98..A8必须保留调用方字节；服务和派生状态不进入正文。
struct SaveLoadPageSecondNativeBodyACh final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t resultAt5C;
    RECT boundsAt60;
    union { YtlReferenceSlotRuntime managerAt70; };
    std::int32_t pageAt7C;
    union { YtlReferenceSlotRuntime canvasAt80; };
    union { GuiControlSurfaceReferences surfacesAt8C; };
    std::int32_t keyboardEventAt98;
    std::int32_t pressPageAt9C;
    std::int32_t repeatCountAtA0;
    std::int32_t repeatEventAtA4;
    std::int32_t activeModeAtA8;
    // 现代共享UI状态，不进入存档wire；原字段偏移保持，所有分配统一使用sizeof。
    std::int32_t firstTabIndex = 0;
    std::int32_t hoveredSlotIndex = -1;
    union { YtlReferenceSlotRuntime hoverSurface; };
    SaveLoadPageSecondNativeBodyACh() noexcept {}
};
static_assert(sizeof(SaveLoadPageSecondNativeBodyACh) == 0xC0);
static_assert(alignof(SaveLoadPageSecondNativeBodyACh) == 4);
static_assert(std::is_standard_layout_v<SaveLoadPageSecondNativeBodyACh>);
static_assert(std::is_trivially_destructible_v<SaveLoadPageSecondNativeBodyACh>);
#define SAVELOAD_SECOND_OFFSET(field, offset) \
    static_assert(offsetof(SaveLoadPageSecondNativeBodyACh, field) == offset)
SAVELOAD_SECOND_OFFSET(common, 0);
SAVELOAD_SECOND_OFFSET(resultAt5C, 0x5C);
SAVELOAD_SECOND_OFFSET(boundsAt60, 0x60);
SAVELOAD_SECOND_OFFSET(managerAt70, 0x70);
SAVELOAD_SECOND_OFFSET(pageAt7C, 0x7C);
SAVELOAD_SECOND_OFFSET(canvasAt80, 0x80);
SAVELOAD_SECOND_OFFSET(surfacesAt8C, 0x8C);
SAVELOAD_SECOND_OFFSET(keyboardEventAt98, 0x98);
SAVELOAD_SECOND_OFFSET(pressPageAt9C, 0x9C);
SAVELOAD_SECOND_OFFSET(repeatCountAtA0, 0xA0);
SAVELOAD_SECOND_OFFSET(repeatEventAtA4, 0xA4);
SAVELOAD_SECOND_OFFSET(activeModeAtA8, 0xA8);
#undef SAVELOAD_SECOND_OFFSET

class ISaveLoadPageSecondNativeSource
{
public:
    virtual ~ISaveLoadPageSecondNativeSource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual SaveFileStateNativeArchive* SaveAt417990(EnvironmentRootStorage*, std::uint32_t slot) = 0;
};
// 实际Environment peer，借同一owner和slot6，绝不复制现代catalog。
class EnvironmentSaveLoadPageSecondNativeSource final : public ISaveLoadPageSecondNativeSource
{
public:
    explicit EnvironmentSaveLoadPageSecondNativeSource(EnvironmentRootOwner& owner) noexcept : owner_(owner) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    SaveFileStateNativeArchive* SaveAt417990(EnvironmentRootStorage*, std::uint32_t) override;
private:
    EnvironmentRootOwner& owner_;
};
struct SaveLoadPageSecondNativeServices final
{
    ISaveLoadPageSecondNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 必须覆盖业务调用及逃逸child/page/canvas的最后一次Ref释放。
class SaveLoadPageSecondNativeScope final
{
public:
    explicit SaveLoadPageSecondNativeScope(SaveLoadPageSecondNativeServices&) noexcept;
    explicit SaveLoadPageSecondNativeScope(SaveLoadPageSecondNativeServices*) noexcept;
    ~SaveLoadPageSecondNativeScope() noexcept;
    SaveLoadPageSecondNativeScope(const SaveLoadPageSecondNativeScope&) = delete;
    SaveLoadPageSecondNativeScope& operator=(const SaveLoadPageSecondNativeScope&) = delete;
private:
    SaveLoadPageSecondNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SaveLoadPageSecondNativeServices& RequireSaveLoadPageSecondNativeServices();
const SaveLoadPageSecondNativeVtable36& SaveLoadPageSecondNativeVtable() noexcept;
const GuiDialogNativeVtable35& SaveLoadPageBaseNativeVtable() noexcept;
std::span<const std::uint32_t, 36> SaveLoadPageSecondNativeOriginalSlots() noexcept;

GuiDialogNativeBody5Ch* ConstructSaveLoadPageSecondNativeAt4EF5E0(void* rawAC,
    GuiDialogNativeBody5Ch* parent, std::int32_t id);
void DestroySaveLoadPageSecondNativeAt4F9470(SaveLoadPageSecondNativeBodyACh*) noexcept;
SaveLoadPageSecondNativeBodyACh* DeleteSaveLoadPageSecondNativeAt4FDFB0(
    SaveLoadPageSecondNativeBodyACh*, std::uint32_t flags) noexcept;
void BeginSaveLoadPageSecondNativeAt50EB50(GuiDialogNativeBody5Ch*);
std::int32_t PollSaveLoadPageSecondNativeAt52F8B0(GuiDialogNativeBody5Ch*);
std::int32_t EndSaveLoadPageSecondNativeAt5094C0(GuiDialogNativeBody5Ch*) noexcept;
std::int32_t ResultSaveLoadPageSecondNativeAt50ABC0(GuiDialogNativeBody5Ch*) noexcept;
std::int32_t ActiveInputSaveLoadPageSecondNativeAt511080(GuiDialogNativeBody5Ch*);
void ResetInputSaveLoadPageSecondNativeAt510790(GuiDialogNativeBody5Ch*);
std::int32_t PriorityInputSaveLoadPageSecondNativeAt511620(GuiDialogNativeBody5Ch*);
void ProcessInputSaveLoadPageSecondNativeAt519CE0(GuiDialogNativeBody5Ch*);
void CompositeSaveLoadPageBaseNativeAt5249C0(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime, const RECT*);
void DrawSaveLoadPageBaseNativeAt524960(GuiDialogNativeBody5Ch*, const RECT*);
}
