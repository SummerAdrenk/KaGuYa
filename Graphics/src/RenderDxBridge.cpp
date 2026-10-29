#include "RenderDxBridge.h"
#include "GraphicsState.h"
#include "Surface.h"

#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{
// 原10050150..10050248的CP932正文；缺必需导出在Initialize内部按char*捕获。
constexpr char LoadError[] = "RenderDX.dll\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char InitializeError[] = "RenderDX.dll - Initialize\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char PitchError[] = "RenderDX.dll - GetPitch\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char BitsError[] = "RenderDX.dll - GetBits\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char ErrorQueryError[] = "RenderDX.dll - GetErrorMessage\x8A\xD6\x90\x94\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x81\x49";
constexpr char InitializeFailed[] = "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73\x81\x49";
}

RenderDxBridge::RenderDxBridge() { ::new (module_) BackendLibrary; }

RenderDxBridge::~RenderDxBridge() noexcept(false)
{
    // 原059E0先library；其后manager/dirty/primary有分层展开保护。
    ReferenceRetirement primary(primary_);
    ReferenceRetirement dirty(dirty_);
    ReferenceRetirement manager(manager_);
    Module().~BackendLibrary();
    manager.Retire();
    dirty.Retire();
    primary.Retire();
}

BorrowedPrimaryPixels RenderDxBridge::Initialize(const char* directory, HWND window, int width, int height, bool padding)
{
    if (!Module().Load(directory, "RenderDX.dll")) throw const_cast<char*>(LoadError);
    const auto initialize = Module().Require<InitializeProc>("Initialize", InitializeError);
    if (!initialize(window, width, height, 1, padding ? 1 : 0))
    {
        const auto error = Module().ResolveAs<GetErrorMessageProc>("GetErrorMessage");
        AppendNativeError(error ? error() : ErrorQueryError);
        throw const_cast<char*>(InitializeFailed);
    }
    // 原10001292/100012C2：Initialize成功后才依次查询并调用Pitch、Bits。
    width_ = width;
    height_ = height;
    const int pitch = Module().Require<NoArgumentProc>("GetPitch", PitchError)();
    auto* bits = Module().Require<BitsProc>("GetBits", BitsError)();
    return {pitch, bits};
}

void RenderDxBridge::Shutdown()
{
    primary_.RetireOwned();
    Module().Close();
}

void RenderDxBridge::Upload(const RECT* dirtyRects, int dirtyRectCount)
{
    if (!IsActive() || dirtyRectCount == 0) return;
    // 原10001DE0每批只查一次；主画布已直接引用backend像素，无第二次CPU复制。
    const auto update = Module().ResolveAs<UpdatePrimaryProc>("UpdatePrimary");
    if (!update) return;
    for (int i = 0; i < dirtyRectCount; ++i)
        if (!update(dirtyRects + i)) break; // DX检查完整EAX，不能只检查AL。
}

bool RenderDxBridge::Present()
{
    if (!IsActive()) return false;
    const auto present = Module().ResolveAs<VoidProc>("RenderSceen");
    if (!present) return false;
    present(); // 原10001684不检查返回值。
    return true;
}

bool RenderDxBridge::PresentPaint(PAINTSTRUCT paint)
{
    if (!IsActive()) return false;
    const auto present = Module().ResolveAs<PaintProc>("RenderSceenHDC");
    if (!present) return false;
    present(paint); // 原10001776..10001787按值传完整40h字节，即使当前DX叶不读取。
    return true;
}

const int* RenderDxBridge::Resize(int width, int height, int mode)
{
    if (!IsActive()) return nullptr;
    const auto resize = Module().ResolveAs<ResizeProc>("Resize");
    return resize ? resize(width, height, mode) : nullptr;
}

int RenderDxBridge::FilteringCapabilities() const
{
    if (!IsActive()) return 0;
    const auto get = Module().ResolveAs<NoArgumentProc>("IsFiltering");
    return get ? get() : 0;
}

int RenderDxBridge::GetMagFiltering() const
{
    if (!IsActive()) return 0;
    const auto get = Module().ResolveAs<NoArgumentProc>("GetMagFiltering");
    return get ? get() : 0;
}

int RenderDxBridge::GetMinFiltering() const
{
    if (!IsActive()) return 0;
    const auto get = Module().ResolveAs<NoArgumentProc>("GetMinFiltering");
    return get ? get() : 0;
}

void RenderDxBridge::SetMagFiltering(int value)
{
    if (!IsActive()) return;
    const auto set = Module().ResolveAs<SetFilteringProc>("SetMagFiltering");
    if (set) set(value);
}

void RenderDxBridge::SetMinFiltering(int value)
{
    if (!IsActive()) return;
    const auto set = Module().ResolveAs<SetFilteringProc>("SetMinFiltering");
    if (set) set(value);
}

int RenderDxBridge::TestCooperativeLevel() const
{
    // 原10001FA0在库对象为空时解引用零地址；安全边界明确拒绝，不伪装设备正常。
    if (!IsActive()) throw std::logic_error("TestCooperativeLevel requires a library object");
    const auto test = Module().ResolveAs<NoArgumentProc>("TestCooperativeLevel");
    return test ? test() : 0;
}

void RenderDxBridge::Reset()
{
    if (!IsActive()) return;
    const auto reset = Module().ResolveAs<VoidProc>("RenderReset");
    if (reset) reset();
}
}
