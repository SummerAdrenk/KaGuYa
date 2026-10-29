#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GraphicsTextRuntime.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/DrawTransitionView.h>
#include <Runtime/DrawManagerRuntime.h>
#include <Runtime/TextDisplaySceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
class TextDisplayGraphicsSurface final : public ITextDisplaySurfaceResource
{
public:
    explicit TextDisplayGraphicsSurface(IGraphicsSurfaceApi& api) noexcept;
    [[nodiscard]] RuntimeGraphicsSurface& Surface() noexcept;
private:
    RuntimeGraphicsSurface surface_;
};

// TextDisplay 的真实 Surface 服务。图层内容仍由 CDrawManager 的注册列表提供，
// 这里只借用注册层与转场manager；Surface/资源仍是既有现代拥有，不代表整个原生化。
// 不伪造 Pattern/Window 绘制，也不负责场景的脚本状态和拥有数组帧调度。
class TextDisplayGraphicsRuntime final
{
public:
    TextDisplayGraphicsRuntime(IGraphicsSurfaceApi& surfaces,
        IGraphicsSurfaceCopyApi& copy, IGraphicsSurfaceCompositionApi& composition,
        IGraphicsSurfaceTransitionApi& transition, IGraphicsScreenSizeApi& sizes,
        IGraphicsScreenUpdateApi& screen, IGraphicsTextApi& text, DrawTransitionView manager) noexcept;
    // 保留显式现代构造入口，适配为同一manager的借用view，不新建转场owner。
    TextDisplayGraphicsRuntime(IGraphicsSurfaceApi& surfaces,
        IGraphicsSurfaceCopyApi& copy, IGraphicsSurfaceCompositionApi& composition,
        IGraphicsSurfaceTransitionApi& transition, IGraphicsScreenSizeApi& sizes,
        IGraphicsScreenUpdateApi& screen, IGraphicsTextApi& text, const DrawManagerRuntime& manager) noexcept;

    [[nodiscard]] SIZE ReadGraphicsTargetSize() const;
    [[nodiscard]] TextDisplaySurfaceRef CreateTextSurface(
        std::int32_t width, std::int32_t height, bool alpha, bool clear);
    [[nodiscard]] bool BlitAllText(const TextDisplaySurfaceRef& target);
    [[nodiscard]] TextDisplaySurfaceRef CaptureCurrentFrame();
    [[nodiscard]] TextDisplaySurfaceRef CaptureDisplayLayersWithAlpha(RegisteredLayersView layers);
    [[nodiscard]] TextDisplaySurfaceRef CreateCompositeFromBase(const TextDisplaySurfaceRef& base);
    void BlitTextSurfaceIntoComposite(const TextDisplaySurfaceRef& composite,
        const TextDisplaySurfaceRef& text);
    void DrawTextSurface(const TextDisplaySurfaceRef& text, const RECT& dirtyRect);
    void RenderTransition(const TextDisplayTransitionOperation& operation);
    [[nodiscard]] static RuntimeGraphicsSurface& RequireSurface(const TextDisplaySurfaceRef& resource);

private:
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsSurfaceCopyApi& copy_;
    IGraphicsSurfaceCompositionApi& composition_;
    IGraphicsSurfaceTransitionApi& transition_;
    IGraphicsScreenSizeApi& sizes_;
    IGraphicsScreenUpdateApi& screen_;
    IGraphicsTextApi& text_;
    DrawTransitionView manager_;
};
}
