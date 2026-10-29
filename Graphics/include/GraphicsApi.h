#pragma once

#include <Windows.h>
#include <cstdint>

using GraphicsHandle = std::int32_t;

extern "C"
{
int __cdecl InitSystem(int backendMode, int windowHandle, int width, int height, int option1, int option2);
void __cdecl ExitSystem();
int __cdecl GetErrorMessage();
int __cdecl GetScreen();
std::uint64_t __cdecl GetScreenSize();
int __cdecl ClearScreen(const RECT* rect);
void __cdecl ClearDirtyRect();
void __cdecl RenderReset();
void __cdecl RenderSceen();
void __cdecl RenderSceenHDC(PAINTSTRUCT paint);
const int* __cdecl ScreenResize(int width, int height, int option);
void __cdecl InvalidatedRect(const RECT* rect);
int __cdecl CheckDeviceLost();
int __cdecl IsScreenMagMinFiltering();
int __cdecl GetScreenMagFiltering();
void __cdecl SetScreenMagFiltering(int value);
int __cdecl GetScreenMinFiltering();
void __cdecl SetScreenMinFiltering(int value);

int __cdecl SurfaceCreate(int width, int height, int pixelFormat, int flags);
int __cdecl SurfaceRelease(GraphicsHandle handle);
int __cdecl GetSurfacePtr(GraphicsHandle handle);
int __cdecl GetSurfacePitch(GraphicsHandle handle);
int __cdecl GetSurfacePixelSize(GraphicsHandle handle);
int __cdecl GetSurfacePixel(GraphicsHandle handle, int x, int y);
std::uint64_t __cdecl GetSurfaceSize(GraphicsHandle handle);
int __cdecl SurfaceLoadBMP(GraphicsHandle handle, int reserved, int bitmapInfo, int pixels, int flags);
int __cdecl SurfaceLoadALP(GraphicsHandle handle, int mode, int width, int height, int pixels);
int __cdecl SurfaceLoadANM(GraphicsHandle handle, int width, int height, int channels, int pixels);
unsigned char __cdecl SurfaceSave(GraphicsHandle handle, HANDLE outputFile, const RECT* sourceRect);

int __cdecl SetTextBasePos(int x, int y);
int __cdecl SetTextFirstName(const char* name);
int __cdecl SetTextSecondName(const char* name);
int __cdecl TextInitialize(const char* text, int x, int y);
int __cdecl TextCreateFont(const LOGFONTA* logFont, unsigned char characterSpacing,
    unsigned char lineSpacing, unsigned char renderMode, unsigned char vertical, COLORREF textColor,
    COLORREF shadowColor, int shadowOffsetX, int shadowOffsetY, COLORREF outlineColor,
    unsigned char outlineWidth);
int __cdecl GetTextLength();
RECT* __cdecl GetTextDrawRect(RECT* rect);
int __cdecl TextDraw(int index, RECT* drawnRect);
int __cdecl TextDrawAll(RECT* drawnRect);
int __cdecl TextBlt(GraphicsHandle destinationHandle, int index, RECT* drawnRect);
int __cdecl TextBltAll(GraphicsHandle destinationHandle, RECT* drawnRect);
int __cdecl CreateMojiSurface(unsigned int character);
}
