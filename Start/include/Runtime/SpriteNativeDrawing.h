#pragma once
#include <Runtime/SpriteNative.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsWindowSurfaceApi;
std::int32_t DrawSpriteNativeAt4768D0(SpriteNativeBodyA0h&, const RECT*, IGraphicsWindowSurfaceApi&);
std::int32_t CompositeSpriteNativeAt46B390(SpriteNativeBodyA0h&, GraphicsSurfaceNativeBody*,
    const RECT*, IGraphicsWindowSurfaceApi&);
}
