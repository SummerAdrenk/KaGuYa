#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/PopupNativePopulation.h>
#include <Runtime/RootTransitionFrameRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IPopupMenuDialogServices;
class IModalDialogRunnerServices;
// 5199FF与519A5A各自取一次原输入singleton；取消键不能绕到dialog+70。
class IPopupMenuNativeCancelSource
{
public:
    virtual ~IPopupMenuNativeCancelSource() = default;
    virtual const IGuiInputNativeKeyboard& KeyboardAt5199FF() = 0;
    virtual std::uint8_t RightButtonAt519A5A() = 0;
};
struct PopupMenuNativeDependencies final
{
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsSurfaceCopyApi& copy;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGuiModalRootSource& root;
    IModalDialogRunnerServices& modal;
    IPopupMenuDialogServices& operations;
    IPopupMenuNativeCancelSource& cancel;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
struct PopupMenuNativeBodyA4h final
{
    GuiDialogNativeBody5Ch common;
    YtlReferenceSlotRuntime capturedAt5C;
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t pollStateAt70;
    std::int32_t constantAt74;
    std::int32_t confirmAt78;
    std::int32_t modeAt7C;
    GuiClickedEventReferences eventsAt80;
    GuiPopupOpaqueStorage recordsAt8C;
    GuiPopupOpaqueStorage recordsAt98;
};
static_assert(sizeof(PopupMenuNativeBodyA4h) == 0xA4);
static_assert(offsetof(PopupMenuNativeBodyA4h, common) == 0);
static_assert(offsetof(PopupMenuNativeBodyA4h, capturedAt5C) == 0x5C);
static_assert(offsetof(PopupMenuNativeBodyA4h, resultAt68) == 0x68);
static_assert(offsetof(PopupMenuNativeBodyA4h, constantAt74) == 0x74);
static_assert(offsetof(PopupMenuNativeBodyA4h, modeAt7C) == 0x7C);
static_assert(offsetof(PopupMenuNativeBodyA4h, eventsAt80) == 0x80);
static_assert(offsetof(PopupMenuNativeBodyA4h, recordsAt98) == 0x98);
static_assert(std::is_trivially_destructible_v<PopupMenuNativeBodyA4h>);

class PopupMenuNativeScope final
{
public:
    explicit PopupMenuNativeScope(PopupMenuNativeDependencies& dependencies) noexcept;
    ~PopupMenuNativeScope() noexcept;
    PopupMenuNativeScope(const PopupMenuNativeScope&) = delete;
    PopupMenuNativeScope& operator=(const PopupMenuNativeScope&) = delete;
private:
    PopupMenuNativeDependencies* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
PopupMenuNativeDependencies& RequirePopupMenuNativeDependencies();
const GuiDialogNativeVtable35& PopupMenuNativeVtable() noexcept;
PopupMenuNativeBodyA4h* ConstructPopupMenuAt4E9FF0(PopupMenuNativeBodyA4h* body);
void DestroyPopupMenuAt4F8CB0(PopupMenuNativeBodyA4h* body) noexcept;
// 原47C8A0借用getter；不Ensure、不Begin，control失败保留已经发布的base。
PopupMenuNativeBodyA4h* GetPopupMenuAt47C8A0(YtlReferenceSlotRuntime& rootSlot);
void RefreshPopupMenuAt4FFBE0(PopupMenuNativeBodyA4h& body);
void HidePopupMenuAt4FFAB0(PopupMenuNativeBodyA4h& body);

namespace PopupMenuNativeEntries
{
void __fastcall Begin(GuiDialogNativeBody5Ch*, void*);
std::int32_t __fastcall Poll(GuiDialogNativeBody5Ch*, void*);
std::int32_t __fastcall End(GuiDialogNativeBody5Ch*, void*);
void __fastcall Input(GuiDialogNativeBody5Ch*, void*);
void __fastcall Flush(GuiDialogNativeBody5Ch*, void*);
std::int32_t __fastcall ActiveInput(GuiDialogNativeBody5Ch*, void*);
void __fastcall Reset(GuiDialogNativeBody5Ch*, void*);
}
}
