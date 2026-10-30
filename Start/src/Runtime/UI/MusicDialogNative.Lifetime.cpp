#include "MusicDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
MusicDialogNativeBody84h* DeleteMusicDialogNativeAt4FCC00(MusicDialogNativeBody84h* body,
    std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &MusicDialogNativeVtable();
    MusicDialogNativeDetail::RefLife events{body->eventsAt74};
    events.Reload();
    DestroyGuiDialogAt4F7AC0(&body->common);
    if (flags & 1u) ::operator delete(body, std::size_t{0x84});
    return body;
}
namespace
{
using Common = GuiDialogNativeBody5Ch;
using Body = MusicDialogNativeBody84h;
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common* body, void*)
{ return MusicDialogNativeDetail::Resolve<GuiDirtyRectNativeBody80h>(body->dirtyAt50); }
Common* __fastcall Delete(Common* body, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteMusicDialogNativeAt4FCC00(reinterpret_cast<Body*>(body), flags)); }
void __fastcall Begin(Common* body, void*) { BeginMusicDialogNativeAt50D090(*reinterpret_cast<Body*>(body)); }
std::int32_t __fastcall Poll(Common* body, void*) { return PollMusicDialogNativeAt52DD40(*reinterpret_cast<Body*>(body)); }
std::int32_t __fastcall End(Common*, void*) { return 0; }
std::int32_t __fastcall Result(Common* body, void*)
{ return MusicDialogNativeResultAt50AB80(*reinterpret_cast<Body*>(body)); }
}
const GuiDialogNativeVtable35& MusicDialogNativeVtable() noexcept
{
    static const GuiDialogNativeVtable35 table = []
    {
        auto t = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.dirtyAt00 = reinterpret_cast<decltype(t.dirtyAt00)>(&Dirty);
        t.deletingDestructorAt04 = reinterpret_cast<decltype(t.deletingDestructorAt04)>(&Delete);
        t.upperInputAt34 = reinterpret_cast<decltype(t.upperInputAt34)>(&MusicDialogNativeDetail::UpperInputAt51EB70);
        t.activeInputAt38 = reinterpret_cast<decltype(t.activeInputAt38)>(&MusicDialogNativeDetail::ActiveInputAt510C80);
        t.resetInputAt3C = reinterpret_cast<decltype(t.resetInputAt3C)>(&MusicDialogNativeDetail::ResetInputAt5106F0);
        t.beginAt44 = reinterpret_cast<decltype(t.beginAt44)>(&Begin);
        t.pollAt48 = reinterpret_cast<decltype(t.pollAt48)>(&Poll);
        t.endAt4C = reinterpret_cast<decltype(t.endAt4C)>(&End);
        t.resultAt50 = reinterpret_cast<decltype(t.resultAt50)>(&Result);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
}
