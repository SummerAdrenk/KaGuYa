#include <Runtime/LayerWindowNativeBase.h>

#include <cstdlib>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Base = LayerItemNativeBody14h;
using Table = LayerItemNativeFrameVtable;
void __fastcall Draw(Base*, void*, const RECT*) {}
void __fastcall Composite(Base*, void*, GraphicsSurfaceNativeBody*, const RECT*) {}
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
    { return DeleteLayerWindowNativeAt469870(*reinterpret_cast<LayerWindowNativeBody24h*>(self), flags); }
RECT* __fastcall Bounds(Base* self, void*, RECT* output)
{
    *output = reinterpret_cast<LayerWindowNativeBody24h*>(self)->boundsAt14;
    return output;
}
std::uint8_t __fastcall Request(Base*, void*, std::uint8_t, std::int32_t) { return 0; }
void __fastcall Advance(Base*, void*) {}
[[noreturn]] void __fastcall Reset(Base*, void*) { _purecall(); std::terminate(); }
const Table methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(Table::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(Table::requestAt10)>(&Request),
    reinterpret_cast<decltype(Table::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset)};
}

const LayerItemNativeFrameVtable& LayerWindowNativeBaseMethods() noexcept { return methods; }
void DestroyLayerWindowNativeAt464910(LayerWindowNativeBody24h& body) noexcept
{
    body.layer.vptrAt00 = methods.AsBase();
    DestroyLayerItemNativeAt441310(body.layer);
}
LayerItemNativeBody14h* DeleteLayerWindowNativeAt469870(LayerWindowNativeBody24h& body,
    std::uint32_t flags) noexcept
{
    DestroyLayerWindowNativeAt464910(body);
    auto* const result = &body.layer;
    if (flags & 1u) ::operator delete(&body, std::size_t{0x24});
    return result;
}
}
