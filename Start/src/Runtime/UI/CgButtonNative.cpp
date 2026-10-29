#include "CgButtonNative.Internal.h"
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using B = CgButtonNativeBody84h;
B* __fastcall Delete(B* b, void*, unsigned f) { return DeleteCgButtonNativeAt4A6170(b, f); }
void* __fastcall DeleteShared(void* b,void*,unsigned f)
{DestroyCgScButtonNativeAt4A4C60(b);if(f&1u)::operator delete(b,std::size_t{0x84});return b;}
ModeButtonNativeBody50h& Common(void* b)noexcept{return CgButtonNativeDetail::Fields84::At<ModeButtonNativeBody50h>(b,0);}
int __fastcall Process(void* b, void*) { return ProcessCgScButtonNativeAt4BEAF0(b); }
int __fastcall Invalidate(void* b, void*, RECT r) { return InvalidateModeButtonNativeAt4B4570(Common(b), r); }
std::uint8_t __fastcall Release(void* b, void*) { return ReadModeButtonReleaseNativeAt4A9C60(Common(b)); }
int __fastcall Input(void* b, void*) { return ReadCgScButtonInputNativeAt4A9FC0(b); }
void __fastcall ResetAll(void* b, void*) { ResetModeButtonChildrenNativeAt4A73F0(Common(b)); }
int __fastcall Reset(void* b, void*, int i) { return ResetModeButtonChildNativeAt4AACD0(Common(b), i); }
int __fastcall Focus(void* b, void*, int i) { return FocusModeButtonChildNativeAt4ACD30(Common(b), i); }
int __fastcall Press(void* b, void*, int i) { return PressModeButtonChildNativeAt4C1980(Common(b), i); }
int __fastcall Commit(void* b, void*, int i) { return CommitModeButtonChildNativeAt4B10F0(Common(b), i); }
int __fastcall Select(void* b, void*, int i) { return SelectModeButtonNativeAt4C2200(Common(b), i); }
int __fastcall Current(void* b, void*) { return CurrentModeButtonNativeAt4ACFA0(Common(b)); }
int __fastcall Navigate(void* b, void*) { return NavigateCgScButtonNativeAt4B4BC0(b); }
void __fastcall Draw(void* b, void*) { DrawCgScButtonNativeAt4ABE80(b); }
void __fastcall Composite(void* b, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ CompositeCgScButtonNativeAt4A80B0(b, target, clip); }
#define SHARED_SLOT(member,function) reinterpret_cast<decltype(ICgScButtonNativeVtable15::member)>(&function)
const ICgScButtonNativeVtable15 shared{
    SHARED_SLOT(deleteAt00,DeleteShared),SHARED_SLOT(processAt04,Process),SHARED_SLOT(invalidateAt08,Invalidate),
    SHARED_SLOT(releaseAt0C,Release),SHARED_SLOT(inputAt10,Input),SHARED_SLOT(resetAllAt14,ResetAll),
    SHARED_SLOT(resetAt18,Reset),SHARED_SLOT(focusAt1C,Focus),SHARED_SLOT(pressAt20,Press),
    SHARED_SLOT(commitAt24,Commit),SHARED_SLOT(selectAt28,Select),SHARED_SLOT(currentAt2C,Current),
    SHARED_SLOT(navigateAt30,Navigate),SHARED_SLOT(drawAt34,Draw),SHARED_SLOT(compositeAt38,Composite)};
#undef SHARED_SLOT
#define CG_SLOT(member, function) reinterpret_cast<decltype(CgButtonNativeVtable15::member)>(&function)
const CgButtonNativeVtable15 methods{
    CG_SLOT(deleteAt00, Delete), CG_SLOT(processAt04, Process), CG_SLOT(invalidateAt08, Invalidate),
    CG_SLOT(releaseAt0C, Release), CG_SLOT(inputAt10, Input), CG_SLOT(resetAllAt14, ResetAll),
    CG_SLOT(resetAt18, Reset), CG_SLOT(focusAt1C, Focus), CG_SLOT(pressAt20, Press),
    CG_SLOT(commitAt24, Commit), CG_SLOT(selectAt28, Select), CG_SLOT(currentAt2C, Current),
    CG_SLOT(navigateAt30, Navigate), CG_SLOT(drawAt34, Draw), CG_SLOT(compositeAt38, Composite)};
#undef CG_SLOT
}
const CgButtonNativeVtable15& CgButtonNativeVtable() noexcept { return methods; }
const ICgScButtonNativeVtable15& ICgScButtonNativeVtable()noexcept{return shared;}
const CgButtonNativeVtable15& CgButtonNativeDetail::IntermediateMethods() noexcept
{
    // 原5B49D0只有一张表；保持CG/Replay退役表身份相同，仅解释x86函数指针ABI。
    return reinterpret_cast<const CgButtonNativeVtable15&>(shared);
}
}
