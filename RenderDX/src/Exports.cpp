#include "RenderDxState.h"

#include <Logging/Logger.h>

#include <cstdint>

extern "C" int __cdecl GetBits()
{
    return KaGuYa::RenderDX::Renderer::Instance().GetBits();
}

extern "C" int __cdecl GetErrorMessage()
{
    return KaGuYa::RenderDX::Renderer::Instance().GetErrorMessage();
}

extern "C" int __cdecl GetMagFiltering()
{
    return KaGuYa::RenderDX::Renderer::Instance().GetMagFiltering();
}

extern "C" int __cdecl GetMinFiltering()
{
    return KaGuYa::RenderDX::Renderer::Instance().GetMinFiltering();
}

extern "C" int __cdecl GetPitch()
{
    return KaGuYa::RenderDX::Renderer::Instance().GetPitch();
}

extern "C" int __cdecl Initialize(const int windowHandle, const int width, const int height,
    const int requirePostPixelBlend, const int padTextureToPowerOfTwo)
{
    return KaGuYa::RenderDX::Renderer::Instance().Initialize(
        reinterpret_cast<HWND>(static_cast<std::intptr_t>(windowHandle)), width, height,
        requirePostPixelBlend, padTextureToPowerOfTwo);
}

extern "C" int __cdecl IsFiltering()
{
    return KaGuYa::RenderDX::Renderer::Instance().IsFiltering();
}

extern "C" int __cdecl Release()
{
    const int result = KaGuYa::RenderDX::Renderer::Instance().Release();
    KaGuYa::Logging::Shutdown();
    return result;
}

extern "C" int __cdecl RenderReset()
{
    return KaGuYa::RenderDX::Renderer::Instance().RenderReset();
}

extern "C" int __cdecl RenderSceen()
{
    return KaGuYa::RenderDX::Renderer::Instance().RenderScreen();
}

extern "C" int __cdecl RenderSceenHDC()
{
    return KaGuYa::RenderDX::Renderer::Instance().RenderScreenHdc();
}

extern "C" const int* __cdecl Resize(const int width, const int height, const int mode)
{
    return KaGuYa::RenderDX::Renderer::Instance().Resize(width, height, mode);
}

extern "C" int __cdecl SetMagFiltering(const int mode)
{
    return KaGuYa::RenderDX::Renderer::Instance().SetMagFiltering(mode);
}

extern "C" int __cdecl SetMinFiltering(const int mode)
{
    return KaGuYa::RenderDX::Renderer::Instance().SetMinFiltering(mode);
}

extern "C" int __cdecl TestCooperativeLevel()
{
    return KaGuYa::RenderDX::Renderer::Instance().TestCooperativeLevel();
}

extern "C" int __cdecl UpdatePrimary(const RECT* const rect)
{
    return KaGuYa::RenderDX::Renderer::Instance().UpdatePrimary(rect);
}
