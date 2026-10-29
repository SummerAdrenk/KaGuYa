#include "GraphicsApi.h"

#include "GraphicsState.h"
#include "Surface.h"
#include <Logging/Logger.h>

#include <cstdint>
#include <stdexcept>

namespace
{
std::uint64_t PackSize(const int width, const int height) noexcept
{
    return static_cast<std::uint32_t>(width) | (static_cast<std::uint64_t>(static_cast<std::uint32_t>(height)) << 32);
}
}

extern "C" int __cdecl InitSystem(const int backendMode, const int windowHandle, const int width, const int height,
    const int option1, const int option2)
{
    KaGuYa::Logging::Initialize(L"Graphics");
    return KaGuYa::Graphics::InitializeRuntime(backendMode,
        reinterpret_cast<HWND>(static_cast<std::intptr_t>(windowHandle)), width, height, option1, option2) ? 1 : 0;
}

extern "C" void __cdecl ExitSystem()
{
    KaGuYa::Graphics::CloseRuntime();
}

extern "C" int __cdecl GetErrorMessage()
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return static_cast<int>(reinterpret_cast<std::intptr_t>(runtime.errorMessage.GetString()));
}

extern "C" int __cdecl GetScreen()
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return KaGuYa::Graphics::ToHandle(runtime.Screen());
}

extern "C" std::uint64_t __cdecl GetScreenSize()
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    // 原10001C80从primary取尺寸，不从已经关闭的renderer缓存伪造有效值。
    if (!runtime.Screen()) throw std::logic_error("GetScreenSize requires a primary Surface");
    return PackSize(runtime.Screen()->width, runtime.Screen()->height);
}

extern "C" void __cdecl ClearDirtyRect()
{
    KaGuYa::Graphics::ClearDirtyRects();
    KaGuYa::Logging::Write(KaGuYa::Logging::Level::Trace, "ClearDirtyRect: dirty=0.");
}

extern "C" void __cdecl RenderReset()
{
    KaGuYa::Graphics::ResetRenderer();
}

extern "C" void __cdecl RenderSceen()
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    int dirtyCount = 0;
    {
        std::scoped_lock lock(runtime.mutex);
        dirtyCount = runtime.renderer && runtime.renderer->Dirty() ? runtime.renderer->Dirty()->count : 0;
    }

    // 外部后端primary直接绑定后端缓冲；RenderSceen按原dirty时序查询并提交。
    const bool presented = KaGuYa::Graphics::PresentToWindow();
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
        "RenderSceen: dirty=%d present=%d.", dirtyCount, presented ? 1 : 0);
}

extern "C" void __cdecl RenderSceenHDC(const PAINTSTRUCT paint)
{
    // Start.exe 在 WM_PAINT 中把 BeginPaint 得到的 64-byte PAINTSTRUCT 按值传入。
    // 原 DX 后端只做 Present；软件后端对应原 GDI 路径，把内部 screen 提交到该 HDC。
    const bool presented = KaGuYa::Graphics::PresentToPaint(paint);
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
        "RenderSceenHDC: paint={%ld,%ld,%ld,%ld} present=%d.",
        paint.rcPaint.left, paint.rcPaint.top, paint.rcPaint.right, paint.rcPaint.bottom,
        presented ? 1 : 0);
}

extern "C" const int* __cdecl ScreenResize(const int width, const int height, const int option)
{
    return KaGuYa::Graphics::ResizeRuntime(width, height, option);
}

extern "C" void __cdecl InvalidatedRect(const RECT* rect)
{
    KaGuYa::Graphics::AddDirtyRect(rect);
    if (rect == nullptr)
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Trace, "InvalidatedRect: full screen.");
        return;
    }
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
        "InvalidatedRect: rect={%ld,%ld,%ld,%ld}.",
        rect->left, rect->top, rect->right, rect->bottom);
}

extern "C" int __cdecl CheckDeviceLost()
{
    return KaGuYa::Graphics::CheckRendererDevice();
}

extern "C" int __cdecl IsScreenMagMinFiltering()
{
    return KaGuYa::Graphics::GetFilteringCapabilities();
}

extern "C" int __cdecl GetScreenMagFiltering()
{
    return KaGuYa::Graphics::GetMagFiltering();
}

extern "C" void __cdecl SetScreenMagFiltering(const int value)
{
    KaGuYa::Graphics::SetMagFiltering(value);
}

extern "C" int __cdecl GetScreenMinFiltering()
{
    return KaGuYa::Graphics::GetMinFiltering();
}

extern "C" void __cdecl SetScreenMinFiltering(const int value)
{
    KaGuYa::Graphics::SetMinFiltering(value);
}
