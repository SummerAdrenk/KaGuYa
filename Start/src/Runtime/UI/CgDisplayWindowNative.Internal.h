#pragma once
#include <Runtime/CgDisplayWindowNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::CgDisplayWindowNativeDetail
{
using Body = CgDisplayWindowNativeBody60h;
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
inline std::int32_t X(const Body& body) noexcept
{ return std::bit_cast<std::int32_t>(body.window.layer.valueAt04); }
inline std::int32_t Y(const Body& body) noexcept
{ return std::bit_cast<std::int32_t>(body.window.layer.valueAt08); }
RECT SpriteBounds(const SpriteNativeBodyA0h&);
void AddParentDirty(Body&, const RECT*);
CgDisplayWindowNativeServices& Services();
std::uint32_t Render(Body&, GraphicsSurfaceNativeBody*, const RECT*,
    IGraphicsWindowSurfaceApi*, bool primary);
}
