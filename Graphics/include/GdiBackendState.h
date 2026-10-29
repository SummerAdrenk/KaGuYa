#pragma once

#include <Windows.h>
#include "GdiDraw.h"
#include "RendererState.h"

namespace KaGuYa::Graphics
{
// 原50h renderer实际持有四组Ref和视口缓存，不把资源生命周期拆到全局Runtime。
class GdiBackendState final : public RendererState
{
public:
    GdiBackendState() = default;
    GdiBackendState(const GdiBackendState&) = delete;
    GdiBackendState& operator=(const GdiBackendState&) = delete;
    ~GdiBackendState() noexcept(false);

    int Kind() const noexcept override { return 0; }
    SurfaceInfo* Screen() const noexcept override;
    RendererReference<DirtyRectState>& Dirty() noexcept override { return dirty_; }
    RendererReference<SurfaceManager>& Manager() noexcept override { return manager_; }
    RendererReference<Surface>& Primary() noexcept { return primary_; }

    bool InitializeDraw(HWND window, int width, int height);
    void Close();
    bool HasDraw() const noexcept { return static_cast<bool>(draw_); }
    GdiDraw& Draw() const;

private:
    RendererReference<GdiDraw, GdiDrawDelete> draw_;
    RendererReference<Surface> primary_;
    RendererReference<DirtyRectState> dirty_;
    RendererReference<SurfaceManager> manager_;
public:
    std::array<int, 4> viewport;
    int resizeMode;
    int targetWidth, targetHeight;
};
static_assert(sizeof(GdiBackendState) == 0x50);
}
