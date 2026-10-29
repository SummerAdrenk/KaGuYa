#include "GraffitiCountDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace GraffitiCountDialogNativeDetail;
#define SLOT(name, function) reinterpret_cast<decltype(GuiDialogNativeVtable35::name)>(&Entries::function)
GuiDialogNativeVtable35 Base() noexcept
{
    auto result = GuiDialogBaseVtable();
    // 5C08E8的00槽与509ED0逐指令同义，但保留派生509EA0入口身份。
    result.dirtyAt00 = SLOT(dirtyAt00, Dirty);
    result.deletingDestructorAt04 = SLOT(deletingDestructorAt04, DeleteBase);
    return result;
}
GuiDialogNativeVtable35 LikableTable() noexcept
{
    auto result = Base();
    result.deletingDestructorAt04 = SLOT(deletingDestructorAt04, DeleteLikable);
    result.beginAt44 = SLOT(beginAt44, BeginLikable);
    result.pollAt48 = SLOT(pollAt48, PollLikable);
    result.endAt4C = SLOT(endAt4C, EndLikable);
    result.processInputAt54 = SLOT(processInputAt54, InputLikable);
    return result;
}
GuiDialogNativeVtable35 WeightTable() noexcept
{
    auto result = Base();
    result.deletingDestructorAt04 = SLOT(deletingDestructorAt04, DeleteWeight);
    result.beginAt44 = SLOT(beginAt44, BeginWeight);
    result.pollAt48 = SLOT(pollAt48, PollWeight);
    result.endAt4C = SLOT(endAt4C, EndWeight);
    result.processInputAt54 = SLOT(processInputAt54, InputWeight);
    return result;
}
#undef SLOT
constexpr std::array<std::uint32_t, 35> likable{
    0x509EA0,0x4FC870,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50CAC0,0x52D610,0x508D50,0x50AB50,
    0x519210,0x525FB0,0x5231C0,0x5230B0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80};
constexpr std::array<std::uint32_t, 35> weight{
    0x509EA0,0x4FE7C0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50FBD0,0x530AE0,0x509620,0x50AB50,
    0x51B1C0,0x525FB0,0x5231C0,0x5230B0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80};
}
const GuiDialogNativeVtable35& GraffitiCountDialogBaseVtable() noexcept
{ static const auto table = Base(); return table; }
const GuiDialogNativeVtable35& GraffitiLikableCountDialogVtable() noexcept
{ static const auto table = LikableTable(); return table; }
const GuiDialogNativeVtable35& GraffitiWeightCountDialogVtable() noexcept
{ static const auto table = WeightTable(); return table; }
const std::array<std::uint32_t, 35>& GraffitiLikableCountDialogOriginalSlots() noexcept { return likable; }
const std::array<std::uint32_t, 35>& GraffitiWeightCountDialogOriginalSlots() noexcept { return weight; }
}
