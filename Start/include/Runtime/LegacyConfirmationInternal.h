#pragma once
#include <Runtime/LegacyConfirmation.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Logging/Logger.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::LegacyConfirmationDetail
{
using Slot = YtlReferenceSlotRuntime;
using Common = GuiDialogNativeBody5Ch;
struct Scope final
{
    GraphicsSurfaceNativeScope surfaces;
    GuiControlGraphicsScope graphics;
    GuiInputNativeScope input;
    GuiDialogNativeScope screen;
    explicit Scope(LegacyConfirmationServices& s) noexcept
        : surfaces(s.surfaces), graphics(s.graphics), input(s.input), screen(s.screen) {}
};
struct RefLife final
{
    Slot& slot;
    bool armed = true;
    ~RefLife() noexcept { if (armed) ReleaseYtlReference(slot); }
    void Finish() { armed = false; ReleaseYtlReference(slot); }
};
[[noreturn]] inline void Reject(const char* text)
{ Logging::Write(Logging::Level::Error, text); throw std::logic_error(text); }
inline GraphicsSurfaceNativeBody& Surface(const Slot& slot)
{
    auto* p = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot);
    if (!p) Reject("Baku confirmation: null Surface in original dereference domain");
    return *p;
}
inline void Copy(Slot& target, const Slot& source) noexcept
{ CopyGuiClickedEventReference(target, source); }
struct TextLife final
{
    RuntimeCString& value;
    bool armed = true;
    ~TextLife() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    void Finish() { armed = false; DestroyRuntimeCStringAt40CD40(&value); }
};
struct TextSlot final { union { RuntimeCString value; }; TextSlot() noexcept {} };

void ReleaseReferences(GuiControlChildrenStorage&);
void DestroyDialog(Common&);
void NewSurface(Slot& fresh, IGraphicsSurfaceApi&, IRuntimeStringManagerSource&);
void CreateStatic(Slot& fresh, Common*, const GuiStaticDescriptorNative24h&, LegacyConfirmationServices&);
void CreateButton(Slot& fresh, Common*, const GuiButtonDescriptorNative2Ch&, LegacyConfirmationServices&);
const GuiButtonNativeVtable29& ButtonTable();
void DestroyControl(GuiControlNativeBody50h&, std::size_t bytes, std::uint32_t flags);
void CreateFactoryToken(Slot& fresh);
void NewEventManager(Slot& fresh);
}
