#include "RenderGlBridge.h"
#include "GraphicsState.h"
#include "Surface.h"

namespace KaGuYa::Graphics
{
namespace
{
// 原10050410..100504E0的CP932正文。
constexpr char LoadError[] = "RenderGL.dll\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char InitializeError[] = "RenderGL.dll - Initialize\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char PitchError[] = "RenderGL.dll - GetPitch\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char BitsError[] = "RenderGL.dll - GetBits\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char ErrorQueryError[] = "RenderGL.dll - GetErrorMessage\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char InitializeFailed[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73\x81\x49";
}

RenderGlBridge::RenderGlBridge() { ::new (module_) BackendLibrary; }

RenderGlBridge::~RenderGlBridge() noexcept(false)
{
    ReferenceRetirement primary(primary_);
    ReferenceRetirement dirty(dirty_);
    ReferenceRetirement manager(manager_);
    Module().~BackendLibrary();
    manager.Retire();
    dirty.Retire();
    primary.Retire();
}

BorrowedPrimaryPixels RenderGlBridge::Initialize(const char* directory, HWND window, int width, int height)
{
    if (!Module().Load(directory, "RenderGL.dll")) throw const_cast<char*>(LoadError);
    const auto initialize = Module().Require<InitializeProc>("Initialize", InitializeError);
    if (!initialize(window, width, height))
    {
        const auto error = Module().ResolveAs<GetErrorMessageProc>("GetErrorMessage");
        AppendNativeError(error ? error() : ErrorQueryError);
        throw const_cast<char*>(InitializeFailed);
    }
    width_ = width;
    height_ = height;
    const int pitch = Module().Require<NoArgumentProc>("GetPitch", PitchError)();
    auto* bits = Module().Require<BitsProc>("GetBits", BitsError)();
    return {pitch, bits};
}

void RenderGlBridge::Shutdown()
{
    primary_.RetireOwned();
    Module().Close();
}

void RenderGlBridge::Upload(const RECT* dirtyRects, int dirtyRectCount)
{
    if (!IsActive() || dirtyRectCount == 0) return;
    const auto update = Module().ResolveAs<UpdatePrimaryProc>("UpdatePrimary");
    if (!update) return;
    for (int i = 0; i < dirtyRectCount; ++i)
        if (!update(dirtyRects + i)) break; // 原1000307B只检查AL，与DX不同。
}

bool RenderGlBridge::Present()
{
    if (!IsActive()) return false;
    const auto present = Module().ResolveAs<VoidProc>("RenderSceen");
    if (!present) return false;
    present();
    return true;
}

bool RenderGlBridge::PresentPaint(PAINTSTRUCT paint)
{
    if (!IsActive()) return false;
    const auto present = Module().ResolveAs<PaintProc>("RenderSceenHDC");
    if (!present) return false;
    present(paint);
    return true;
}

const int* RenderGlBridge::Resize(int width, int height, int mode)
{
    if (!IsActive()) return nullptr;
    const auto resize = Module().ResolveAs<ResizeProc>("Resize");
    return resize ? resize(width, height, mode) : nullptr;
}
}
