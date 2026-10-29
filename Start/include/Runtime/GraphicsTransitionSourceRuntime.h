#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/LoadAfterSelectionScene.h>
#include <utility>

namespace KaGuYa::Start::Runtime
{
// CSurface 转场来源的强引用适配；实际像素与句柄仍由 RuntimeGraphicsSurface 持有。
class GraphicsTransitionSourceRuntime final : public ILoadAfterTransitionSource
{
public:
    explicit GraphicsTransitionSourceRuntime(std::shared_ptr<RuntimeGraphicsSurface> surface)
        : surface_(std::move(surface))
    {
        if (!surface_) throw std::invalid_argument("Transition surface is null");
    }
    RuntimeGraphicsSurface& Surface() const noexcept { return *surface_; }
private:
    std::shared_ptr<RuntimeGraphicsSurface> surface_;
};
}
