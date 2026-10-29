#pragma once
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GraphicsSurfaceSaveApi.h>
#include <Runtime/GraphicsDirtyRectApi.h>
#include <Runtime/GraphicsWindowModeApi.h>
#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/PatternPixelFilterRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 既有场景装配的借用接口集合；无模块owner、初始化快照或错误串副本。
// 原生细分消费者继续直接借各NativeApi，不额外经过模块预查询。
class IGraphicsRuntimeCalls : public IGraphicsSurfaceApi, public IGraphicsSurfaceCompositionApi,
    public IGraphicsSurfaceCopyApi, public IGraphicsScreenUpdateApi,
    public IGraphicsSurfaceTransitionApi, public IGraphicsSurfacePixelApi,
    public IGraphicsWindowSurfaceApi, public IGraphicsTextApi, public IGraphicsTextGlyphApi,
    public IGraphicsScreenSizeApi, public IGraphicsTransitApi, public IGraphicsDirtyRectApi,
    public IGraphicsWindowModeApi, public IGraphicsSurfaceSaveApi, public IGraphicsPatternFilterApi
{
public:
    bool ModuleLoaded() const override = 0;
    virtual void SetTextFirstName(const char*) = 0;
    virtual void SetTextSecondName(const char*) = 0;
    virtual void RenderScreenIfLoaded() = 0;
};
}
