#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/SaveLoadDialogNativeScope.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace SaveLoadDialogNativeDetail
{
void ReleaseWithoutClearing(Slot& slot)
{
    auto* control=slot.controlAt08;
    if(control&&--control->referenceCountAt04==0)
    {
        if(control->owningAt08)control->DestroyPayloadAtVslot00();
        if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
void DestroyCompletedMembers(Body& body) noexcept
{
    // 5F5368 state0D..0及4F9030：成员完成后的展开与正常删除共享原逆序，基类最后退化。
    ReleaseYtlReference(body.slotConfirmationAt174);
    ReleaseYtlReference(body.deleteConfirmationAt168);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.newMarkerAt150);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.noPreviewAt144);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.noDataAt138);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.dateDigitsAt12C);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.slotDigitsAt120);
    DestroySaveLoadPagePositionsNativeAt4F7130(body.pagePositionsAt114);
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.pageDigitsAtF8);
    ReleaseYtlReference(body.eventsAtEC);
    ReleaseYtlReference(body.blackAtA0);
    ReleaseYtlReference(body.canvasAt94);
    ReleaseYtlReference(body.capturedAt88);
    DestroyGuiDialogIntoNativeAt4F7B80(&body.common);
}
}
void DestroySaveLoadDialogNativeAt4F9030(SaveLoadDialogNativeBody184h* body,
    SaveLoadDialogNativeConstructionServices& services) noexcept
{
    SaveLoadDialogNativeConstructionScope scope(services);
    body->common.vptrAt00=&services.dispatch.common;
    SaveLoadDialogNativeDetail::DestroyCompletedMembers(*body);
}
SaveLoadDialogNativeBody184h* DeleteSaveLoadDialogNativeAt4FDD70(SaveLoadDialogNativeBody184h* body,
    std::uint32_t flags,SaveLoadDialogNativeConstructionServices& services) noexcept
{
    DestroySaveLoadDialogNativeAt4F9030(body,services);
    if(flags&1u)::operator delete(body,std::size_t{0x184});
    return body;
}
}
