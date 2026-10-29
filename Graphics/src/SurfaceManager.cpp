#include "SurfaceManager.h"
#include "GraphicsState.h"
#include "Surface.h"
#include "DibBitmap.h"

#include <stdexcept>

namespace KaGuYa::Graphics
{
SurfaceManager::~SurfaceManager() noexcept(false)
{
    // 原05910的state1只保护text/surfaces；当前DIB表抛出时不能再次退休该表。
    ReferenceRetirement surfacesUnwind(surfaces);
    ReferenceRetirement textUnwind(text);
    bitmaps.RetireOwned();
    textUnwind.Retire();
    surfacesUnwind.Retire();
}

SurfaceManager* GetSurfaceManager() noexcept
{
    auto* renderer = GetRuntime().renderer.get();
    return renderer ? renderer->Manager().get() : nullptr;
}

SurfaceManager& RequireSurfaceManager()
{
    auto* manager = GetSurfaceManager();
    if (!manager) throw std::logic_error("Graphics resource manager is not initialized");
    return *manager;
}
}
