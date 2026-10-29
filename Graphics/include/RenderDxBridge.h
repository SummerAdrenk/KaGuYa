#pragma once

#include <Windows.h>
#include "BackendLibrary.h"
#include "RendererState.h"

#include <cstdint>

namespace KaGuYa::Graphics
{
// Graphics.dll 原版通过这组导出驱动 RenderDX.dll。桥接层只负责 ABI 与主画布绑定，
// 不在 Graphics 内另写一套近似的 D3D 提交器。
class RenderDxBridge final : public RendererState
{
public:
    RenderDxBridge();
    RenderDxBridge(const RenderDxBridge&) = delete;
    RenderDxBridge& operator=(const RenderDxBridge&) = delete;
    ~RenderDxBridge() noexcept(false) override;

    BorrowedPrimaryPixels Initialize(const char* libraryDirectory, HWND window, int width, int height,
        bool padTextureToPowerOfTwo);
    void Shutdown();

    int Kind() const noexcept override { return 1; }
    SurfaceInfo* Screen() const noexcept override { return primary_.get(); }
    RendererReference<DirtyRectState>& Dirty() noexcept override { return dirty_; }
    RendererReference<SurfaceManager>& Manager() noexcept override { return manager_; }
    RendererReference<SurfaceInfo>& Primary() noexcept { return primary_; }
    bool IsActive() const noexcept { return Module().HasObject(); }
    void Upload(const RECT* dirtyRects, int dirtyRectCount);
    bool Present();
    bool PresentPaint(PAINTSTRUCT paint);
    const int* Resize(int width, int height, int mode);
    int FilteringCapabilities() const;
    int GetMagFiltering() const;
    int GetMinFiltering() const;
    void SetMagFiltering(int value);
    void SetMinFiltering(int value);
    int TestCooperativeLevel() const;
    void Reset();

private:
    using InitializeProc = int (__cdecl*)(HWND, int, int, int, int);
    using NoArgumentProc = int (__cdecl*)();
    using VoidProc = void (__cdecl*)();
    using PaintProc = void (__cdecl*)(PAINTSTRUCT);
    using BitsProc = std::uint8_t* (__cdecl*)();
    using ResizeProc = const int* (__cdecl*)(int, int, int);
    using SetFilteringProc = void (__cdecl*)(int);
    using UpdatePrimaryProc = int (__cdecl*)(const RECT*);
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
static_assert(sizeof(RenderDxBridge) == 0x3C);
}
