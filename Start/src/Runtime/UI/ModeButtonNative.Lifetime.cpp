#include "ModeButtonNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace ModeButtonNativeDetail;
void ReleaseModeButtonReferenceRangeNativeAt498C10(Slot* first, Slot* last)
{
    // 原498C10没有terminate保护；重用可抛原语，回调后重读control，不清bias。
    for (; first != last; first = Advance(first, 1)) ReleaseYtlReference(*first);
}
void ReleaseModeButtonNavigationRangeNativeAt498CA0(Slot* first, Slot* last)
{ ReleaseModeButtonReferenceRangeNativeAt498C10(first, last); }
void DestroyModeButtonChildrenNativeAt4A3A00(ModeButtonReferenceVector& value) noexcept
{
    // 5EE5C4 EHFlags=5。与43EAA0同布局/释放顺序/大块回溯校验，复用引用vector原语。
    DestroyGuiControlSurfaceReferencesAt43EAA0(value);
}
void DestroyModeButtonNavigationNativeAt4A3AB0(ModeButtonReferenceVector& value) noexcept
{
    // 5EE640 EHFlags=5；498CA0与498C10逐槽同序，不借用GUI导航对象的业务结构。
    DestroyGuiControlSurfaceReferencesAt43EAA0(value);
}
void ClearModeButtonChildrenNativeAt4A8490(Body& body)
{
    auto* last = Read(body.childrenAt14.end);
    auto* first = Read(body.childrenAt14.begin);
    ReleaseModeButtonReferenceRangeNativeAt498C10(first, last);
    body.childrenAt14.end = Read(body.childrenAt14.begin);
}
void DestroyButtonBaseNativeAt4A4BB0(Body& body) noexcept
{
    // 5EE674：两个state均terminate；首次范围清理成功后才写end，再清navigation/dirty。
    body.vptrAt00 = &IBtnProcNativeVtable();
    ClearModeButtonChildrenNativeAt4A8490(body);
    DestroyModeButtonNavigationNativeAt4A3AB0(body.navigationAt30);
    ReleaseYtlReference(body.dirtyAt24);
    DestroyModeButtonChildrenNativeAt4A3A00(body.childrenAt14);
}
void DestroyModeButtonNativeAt4A4D90(Body& body) noexcept
{
    // 5EE6EC：不先重写IMode表；publicGui释放完成后才进入IBtnProc析构。
    ReleaseYtlReference(body.publicGuiAt44);
    DestroyButtonBaseNativeAt4A4BB0(body);
}
Body* DeleteButtonBaseNativeAt4A7270(Body* body, std::uint32_t flags) noexcept
{
    DestroyButtonBaseNativeAt4A4BB0(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x3C});
    return body;
}
Body* DeleteModeButtonNativeAt4A7330(Body* body, std::uint32_t flags) noexcept
{
    // 原scalar分支没有flags&2数组处理；5EE718清理异常直接terminate。
    DestroyModeButtonNativeAt4A4D90(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x50});
    return body;
}
}
