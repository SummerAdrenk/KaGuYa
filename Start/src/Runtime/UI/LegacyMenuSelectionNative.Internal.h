#pragma once
#include <Runtime/LegacyMenuSelectionNative.h>
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <memory>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct LegacyMenuSelectionPanel final
{
    GuiDialogNativeBody5Ch common;
    GuiClickedEventReferences events{};
    YtlReferenceSlotRuntime canvas{};
    RECT bounds{};
    std::array<POINT, 3> labelPoints{};
    std::array<COLORREF, 3> colors{};
    LegacyMenuSelectionDialog* owner;
    int result = -1, confirmLatch = 0;
    bool system;
};
struct LegacyMenuSelectionDialog final
{
    GuiDialogIntoNativeBody68h into;
    LegacyMenuSelectionServices* services;
    LegacyMenuSelectionPanel* main = nullptr;
    LegacyMenuSelectionPanel* system = nullptr;
    RECT bounds{};
    int phase = 0, result = -1;
};
namespace LegacyMenuSelectionDetail
{
using Base = GuiDialogNativeBody5Ch;
using Panel = LegacyMenuSelectionPanel;
using Top = LegacyMenuSelectionDialog;
using Slot = YtlReferenceSlotRuntime;
struct Ref final
{
    Slot slot{};
    Ref() = default;
    explicit Ref(Slot value) : slot(value) {}
    ~Ref() noexcept { ReleaseYtlReference(slot); }
    Ref(const Ref&) = delete;
};
inline Slot Copy(const Slot& source) noexcept
{ auto value = source; if (value.controlAt08) ++value.controlAt08->referenceCountAt04; return value; }
struct Leaves final
{
    GraphicsSurfaceNativeScope surfaces;
    GuiControlGraphicsScope graphics;
    GuiInputNativeScope input;
    GuiDialogNativeScope screen;
    explicit Leaves(LegacyMenuSelectionServices& s) : surfaces(s.surfaces), graphics(s.graphics), input(s.input), screen(s.screen) {}
};
inline auto& Services(Panel& p) { return *p.owner->services; }
inline auto& Profile(Panel& p) { return RequireLegacyMenuSelectionProfile(Services(p).profile); }
const GuiDialogNativeVtable35& PanelMethods() noexcept;
const GuiDialogIntoNativeVtable37& TopMethods() noexcept;
void NewPanel(Slot&, Top&, bool system);
void RenderLabels(Panel&);
void SystemBegin(Panel&);
void SystemEvent(Panel&, int);
void DestroyPanel(Panel&) noexcept;
}
}
