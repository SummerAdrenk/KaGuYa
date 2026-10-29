#pragma once

#include <Windows.h>
#include "BackendLibrary.h"
#include "RendererState.h"

#include <cstdint>

namespace KaGuYa::Graphics
{
// Graphics.dll:10002B10 通过 RenderGL.dll 的 C 导出驱动 OpenGL 后端。
// 本桥接层只复刻该模块边界，不在 Graphics 内替代或猜测 RenderGL 的渲染行为。
class RenderGlBridge final : public RendererState
{
public:
    RenderGlBridge();
    RenderGlBridge(const RenderGlBridge&) = delete;
    RenderGlBridge& operator=(const RenderGlBridge&) = delete;
    ~RenderGlBridge() noexcept(false) override;

    BorrowedPrimaryPixels Initialize(const char* libraryDirectory, HWND window, int width, int height);
    void Shutdown();

    int Kind() const noexcept override { return 2; }
    SurfaceInfo* Screen() const noexcept override { return primary_.get(); }
    RendererReference<DirtyRectState>& Dirty() noexcept override { return dirty_; }
    RendererReference<SurfaceManager>& Manager() noexcept override { return manager_; }
    RendererReference<SurfaceInfo>& Primary() noexcept { return primary_; }
    bool IsActive() const noexcept { return Module().HasObject(); }
    void Upload(const RECT* dirtyRects, int dirtyRectCount);
    bool Present();
    bool PresentPaint(PAINTSTRUCT paint);
    const int* Resize(int width, int height, int mode);

private:
    using InitializeProc = unsigned char (__cdecl*)(HWND, int, int);
    using NoArgumentProc = int (__cdecl*)();
    using VoidProc = void (__cdecl*)();
    using PaintProc = void (__cdecl*)(PAINTSTRUCT);
    using BitsProc = std::uint8_t* (__cdecl*)();
    using ResizeProc = const int* (__cdecl*)(int, int, int);
    using UpdatePrimaryProc = unsigned char (__cdecl*)(const RECT*);
    using GetErrorMessageProc = const char* (__cdecl*)();

    BackendLibrary& Module() noexcept
    { return *std::launder(reinterpret_cast<BackendLibrary*>(module_)); }
    const BackendLibrary& Module() const noexcept
    { return *std::launder(reinterpret_cast<const BackendLibrary*>(module_)); }
    RendererReference<SurfaceInfo> primary_;
    int width_, height_;
    RendererReference<DirtyRectState> dirty_;
    RendererReference<SurfaceManager> manager_;
    alignas(BackendLibrary) std::byte module_[sizeof(BackendLibrary)];
};
static_assert(sizeof(RenderGlBridge) == 0x3C);
}
