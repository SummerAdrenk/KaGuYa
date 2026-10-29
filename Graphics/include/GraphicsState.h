#pragma once

#include <Windows.h>
#include <atlstr.h>
#include "TextRenderer.h"
#include "RendererReference.h"
#include "SurfaceManager.h"
#include "RendererState.h"
#include "NativeAnsiString.h"

#include <array>
#include <memory>
#include <mutex>
#include <string>

namespace KaGuYa::Graphics
{
class SurfaceInfo;
struct Surface;
class RenderDxBridge;
class RenderGlBridge;
class GdiBackendState;

struct Runtime
{
    ~Runtime() noexcept(false);
    std::recursive_mutex mutex;
    bool initialized = false;
    int backendMode = 0;
    HWND window = nullptr;
    int width = 0;
    int height = 0;
    int option1 = 0;
    int option2 = 0;
    int magFiltering = 0;
    int minFiltering = 0;
    RendererReference<RendererState> renderer;
    RenderDxBridge* Dx() const noexcept;
    RenderGlBridge* Gl() const noexcept;
    GdiBackendState* Gdi() const noexcept;
    SurfaceInfo* Screen() const noexcept;
    RendererReference<DirtyRectState>& Dirty() const;
    NativeAnsiString errorMessage;
};

Runtime& GetRuntime() noexcept;
bool InitializeRuntime(int backendMode, HWND window, int width, int height, int option1, int option2);
void ShutdownRuntime();
void CloseRuntime();
const int* ResizeRuntime(int width, int height, int option);
void AddDirtyRect(const RECT* rect);
void ClearDirtyRects();
bool PresentToWindow();
bool PresentToPaint(const PAINTSTRUCT& paint);
int GetFilteringCapabilities();
int GetMagFiltering();
int GetMinFiltering();
void SetMagFiltering(int value);
void SetMinFiltering(int value);
int CheckRendererDevice();
void ResetRenderer();
void SetError(std::string message);
void AppendNativeError(const char* message);
}
