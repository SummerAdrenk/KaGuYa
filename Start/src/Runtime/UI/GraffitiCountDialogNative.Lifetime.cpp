#include "GraffitiCountDialogNative.Internal.h"
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiCountDialogNativeDetail;
namespace
{
thread_local GraffitiCountDialogNativeServices* services = nullptr;
}
__declspec(noinline) GraffitiCountDialogNativeServices& RequireGraffitiCountDialogNativeServices()
{
    if (!services) throw std::logic_error("Graffiti count dialog native Scope is not bound");
    return *services;
}
__declspec(noinline) GraffitiCountDialogNativeScope::GraffitiCountDialogNativeScope(
    GraffitiCountDialogNativeServices& bound) noexcept
    : previous_(services), surfaces_(bound.surfaces), graphics_(bound.graphics),
      input_(bound.input), screen_(bound.screen)
{ services = &bound; }
__declspec(noinline) GraffitiCountDialogNativeScope::~GraffitiCountDialogNativeScope() noexcept
{ services = previous_; }

void DestroyGraffitiCountDialogBaseAt4F7A40(Common& body) noexcept
{
    body.dialog.vptrAt00 = &GraffitiCountDialogBaseVtable();
    // 4F7A6B..A5：slot重读控制块，bias不清；无control时保留孤立base。
    ReleaseYtlReference(body.canvasAt6C);
    DestroyGuiDialogAt4F7AC0(&body.dialog);
}
Dialog* DeleteGraffitiLikableAt4FC870(Likable* body, std::uint32_t flags) noexcept
{
    auto* result = &body->common.dialog;
    body->common.dialog.vptrAt00 = &GraffitiLikableCountDialogVtable();
    DestroyGuiControlSurfaceReferencesAt43EAA0(body->common.digitsAt84);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body->common.backgroundsAt78);
    DestroyGraffitiCountDialogBaseAt4F7A40(body->common);
    if (flags & 1u) ::operator delete(body, std::size_t{0xB4});
    return result;
}
Dialog* DeleteGraffitiWeightAt4FE7C0(Weight* body, std::uint32_t flags) noexcept
{
    auto* result = &body->common.dialog;
    body->common.dialog.vptrAt00 = &GraffitiWeightCountDialogVtable();
    DestroyGuiControlSurfaceReferencesAt43EAA0(body->common.digitsAt84);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body->common.backgroundsAt78);
    DestroyGraffitiCountDialogBaseAt4F7A40(body->common);
    if (flags & 1u) ::operator delete(body, std::size_t{0xB8});
    return result;
}
Dialog* GraffitiCountDialogNativeConstructionSource::ConstructLikableAt4E13A0(void* raw)
{
    GraffitiCountDialogNativeScope scope(services_);
    return &ConstructGraffitiLikableAt4E13A0(::new(raw) Likable)->common.dialog;
}
Dialog* GraffitiCountDialogNativeConstructionSource::ConstructWeightAt4F4BC0(void* raw)
{
    GraffitiCountDialogNativeScope scope(services_);
    return &ConstructGraffitiWeightAt4F4BC0(::new(raw) Weight)->common.dialog;
}
namespace GraffitiCountDialogNativeDetail
{
void FinishCached(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
void FinishCurrentWithoutClearing(Slot& slot) noexcept
{
    auto* control = Read(slot.controlAt08);
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    }
}
namespace Entries
{
GuiDirtyRectNativeBody80h* __fastcall Dirty(Dialog* body, void*)
{ return Resolve<GuiDirtyRectNativeBody80h>(body->dirtyAt50); }
Dialog* __fastcall DeleteBase(Dialog* body, void*, std::uint32_t flags) noexcept
{
    DestroyGraffitiCountDialogBaseAt4F7A40(*reinterpret_cast<Common*>(body));
    if (flags & 1u) ::operator delete(body, std::size_t{0x78});
    return body;
}
Dialog* __fastcall DeleteLikable(Dialog* body, void*, std::uint32_t flags) noexcept
{ return DeleteGraffitiLikableAt4FC870(reinterpret_cast<Likable*>(body), flags); }
Dialog* __fastcall DeleteWeight(Dialog* body, void*, std::uint32_t flags) noexcept
{ return DeleteGraffitiWeightAt4FE7C0(reinterpret_cast<Weight*>(body), flags); }
}
}
}
