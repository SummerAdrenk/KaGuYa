#include "ProgramSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
void DestroyProgramSelectDialogNativeAt4F8D50(ProgramSelectDialogNativeBodyECh& body) noexcept
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    Base(body).vptrAt00 = &ProgramSelectDialogNativeBaseVtable().inherited.common;
    ReleaseYtlReference(body.canvasAtA4);
    ReleaseYtlReference(body.blackAt98);
    Base(body).vptrAt00 = &GuiDialogIntoBaseVtable().common;
    DestroyNestedGuiDialogReferencesAt4F6DC0(body.into.dialogsAt5C);
    DestroyGuiDialogAt4F7AC0(&Base(body));
}
namespace
{
ProgramSelectDialogNativeBodyECh* DeleteTop(ProgramSelectDialogNativeBodyECh* body,
    std::uint32_t flags, bool mode01) noexcept
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    Base(*body).vptrAt00 = &(mode01 ? ProgramSelectDialogNativeTop01Vtable() :
        ProgramSelectDialogNativeTop00Vtable()).inherited.common;
    DestroyProgramSelectDialogNativeAt4F8D50(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0xEC});
    return body;
}
}
ProgramSelectDialogNativeBodyECh* DeleteProgramSelectDialogNativeAt4FC140(
    ProgramSelectDialogNativeBodyECh* body, std::uint32_t flags) noexcept
{ return DeleteTop(body, flags, false); }
ProgramSelectDialogNativeBodyECh* DeleteProgramSelectDialogNativeAt4FC280(
    ProgramSelectDialogNativeBodyECh* body, std::uint32_t flags) noexcept
{ return DeleteTop(body, flags, true); }
}
