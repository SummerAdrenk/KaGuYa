#include "CgDisplayWindowNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Base = LayerItemNativeBody14h;
using Table = LayerItemNativeFrameVtable;
using namespace CgDisplayWindowNativeDetail;
thread_local CgDisplayWindowNativeServices* activeServices = nullptr;
Body& Derived(Base* base) noexcept { return *reinterpret_cast<Body*>(base); }
std::uint32_t __fastcall Draw(Base* base, void*, const RECT* clip)
{ return Render(Derived(base), nullptr, clip, nullptr, true); }
std::uint32_t __fastcall Composite(Base* base, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ return Render(Derived(base), target, clip, nullptr, false); }
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags)
{ DeleteCgDisplayWindowNativeAt4A6030(Derived(base), flags); return base; }
RECT* __fastcall Bounds(Base* base, void*, RECT* out)
{ return ReadCgDisplayWindowBoundsNativeAt479DB0(Derived(base), out); }
std::uint8_t __fastcall Query(Base*, void*, std::uint8_t first, std::int32_t second)
{ return QueryCgDisplayWindowNativeAt4C1240(first, std::uint32_t(second)); }
void __fastcall Advance(Base* base, void*)
{ AdvanceCgDisplayWindowNativeAt4A7470(Derived(base)); }
void __fastcall Reset(Base* base, void*)
{ ResetCgDisplayWindowNativeAt4AF1E0(Derived(base)); }
const Table methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(Table::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(Table::requestAt10)>(&Query),
    reinterpret_cast<decltype(Table::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset)};
}
const LayerItemNativeFrameVtable& CgDisplayWindowNativeMethods() noexcept { return methods; }
CgDisplayWindowNativeScope::CgDisplayWindowNativeScope(CgDisplayWindowNativeServices& services) noexcept
    : previous_(activeServices) { activeServices = &services; }
CgDisplayWindowNativeScope::~CgDisplayWindowNativeScope() noexcept { activeServices = previous_; }
namespace CgDisplayWindowNativeDetail
{
CgDisplayWindowNativeServices& Services()
{
    if (!activeServices) throw std::logic_error("CG display native drawing requires an external scope");
    return *activeServices;
}
}
}
