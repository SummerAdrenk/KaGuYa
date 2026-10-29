#include "ModeButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ModeButtonNativeBody50h;
using Table = ModeButtonNativeVtable13;
Body* __fastcall DeleteBase(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteButtonBaseNativeAt4A7270(body, flags); }
Body* __fastcall DeleteMode(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteModeButtonNativeAt4A7330(body, flags); }
std::int32_t __fastcall Process(Body* body, void*)
{ return ProcessModeButtonNativeAt4BE930(*body); }
std::int32_t __fastcall Invalidate(Body* body, void*, RECT rect)
{ return InvalidateModeButtonNativeAt4B4570(*body, rect); }
std::uint8_t __fastcall Release(Body* body, void*)
{ return ReadModeButtonReleaseNativeAt4A9C60(*body); }
std::int32_t __fastcall InputBase(Body* body, void*)
{ return ReadButtonBaseInputNativeAt4A9D90(*body); }
std::int32_t __fastcall InputMode(Body* body, void*)
{ return ReadModeButtonInputNativeAt4AA200(*body); }
void __fastcall ResetAll(Body* body, void*)
{ ResetModeButtonChildrenNativeAt4A73F0(*body); }
std::int32_t __fastcall Reset(Body* body, void*, std::int32_t index)
{ return ResetModeButtonChildNativeAt4AACD0(*body, index); }
std::int32_t __fastcall Focus(Body* body, void*, std::int32_t index)
{ return FocusModeButtonChildNativeAt4ACD30(*body, index); }
std::int32_t __fastcall Press(Body* body, void*, std::int32_t index)
{ return PressModeButtonChildNativeAt4C1980(*body, index); }
std::int32_t __fastcall Commit(Body* body, void*, std::int32_t index)
{ return CommitModeButtonChildNativeAt4B10F0(*body, index); }
std::int32_t __fastcall Select(Body* body, void*, std::int32_t index) noexcept
{ return SelectModeButtonNativeAt4C2200(*body, index); }
std::int32_t __fastcall Current(Body* body, void*) noexcept
{ return CurrentModeButtonNativeAt4ACFA0(*body); }
std::int32_t __fastcall Navigate(Body* body, void*)
{ return NavigateModeButtonNativeAt4B4920(*body); }

// 原PE两表逐槽核对；fastcall桥仅占位EDX，栈参数和thiscall清栈一致。
// 直接静态初始化函数地址，不增加运行期构表或全局初始化次序依赖。
const Table base{
    reinterpret_cast<decltype(Table::deleteAt00)>(&DeleteBase),
    reinterpret_cast<decltype(Table::processAt04)>(&Process),
    reinterpret_cast<decltype(Table::invalidateAt08)>(&Invalidate),
    reinterpret_cast<decltype(Table::releaseAt0C)>(&Release),
    reinterpret_cast<decltype(Table::inputAt10)>(&InputBase),
    reinterpret_cast<decltype(Table::resetAllAt14)>(&ResetAll),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset),
    reinterpret_cast<decltype(Table::focusAt1C)>(&Focus),
    reinterpret_cast<decltype(Table::pressAt20)>(&Press),
    reinterpret_cast<decltype(Table::commitAt24)>(&Commit),
    reinterpret_cast<decltype(Table::selectAt28)>(&Select),
    reinterpret_cast<decltype(Table::currentAt2C)>(&Current),
    reinterpret_cast<decltype(Table::navigateAt30)>(&Navigate)};
const Table mode{
    reinterpret_cast<decltype(Table::deleteAt00)>(&DeleteMode),
    reinterpret_cast<decltype(Table::processAt04)>(&Process),
    reinterpret_cast<decltype(Table::invalidateAt08)>(&Invalidate),
    reinterpret_cast<decltype(Table::releaseAt0C)>(&Release),
    reinterpret_cast<decltype(Table::inputAt10)>(&InputMode),
    reinterpret_cast<decltype(Table::resetAllAt14)>(&ResetAll),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset),
    reinterpret_cast<decltype(Table::focusAt1C)>(&Focus),
    reinterpret_cast<decltype(Table::pressAt20)>(&Press),
    reinterpret_cast<decltype(Table::commitAt24)>(&Commit),
    reinterpret_cast<decltype(Table::selectAt28)>(&Select),
    reinterpret_cast<decltype(Table::currentAt2C)>(&Current),
    reinterpret_cast<decltype(Table::navigateAt30)>(&Navigate)};
}
const ModeButtonNativeVtable13& IBtnProcNativeVtable() noexcept { return base; }
const ModeButtonNativeVtable13& IModeBtnProcNativeVtable() noexcept { return mode; }
}
