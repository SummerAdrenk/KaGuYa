#include "GdiDraw.h"
#include "Surface.h"
#include "SurfaceTransfer.h"

#include <atlstr.h>

namespace KaGuYa::Graphics
{
static_assert(sizeof(GdiModuleReference) == 12);
static_assert(sizeof(ATL::CStringData) == 16);
static_assert(sizeof(RECT) == 16);

GdiDrawModule::~GdiDrawModule() noexcept(false)
{
    // 原10008120/10008134：正文只卸载模块，业务Release属于Draw.Close。
    if (handle)
    {
        FreeLibrary(handle);
        handle = nullptr;
    }
}

GdiDrawModule* GdiDrawModule::Destroy(const std::uint32_t flags)
{
    // 原10008141：析构正常完成后才按flags释放，不用delete表达式补偿异常。
    this->~GdiDrawModule();
    if (flags & 1u) ::operator delete(this);
    return this;
}

void GdiOpaqueReference::RetireOwned()
{
    // 原10005C3A..10005C64：只走已知Ref协议；回调后重读control，保留index。
    if (!control) return;
    if (--control->references == 0)
    {
        if (control->owning) control->DestroyPayload();
        if (control) control->DeleteControl(1);
    }
    control = nullptr;
    base = nullptr;
}

GdiPlusDraw::GdiPlusDraw()
{
    // 原10002123..10002138仅构造两组Ref；HWND/宽/高留到Initialize成功尾。
    ::new (library_) GdiModuleReference;
}

GdiPlusDraw::~GdiPlusDraw() noexcept(false)
{
    // 原10005C32..10005C67/state0：Close及unused退休期间均保护library。
    // 展开中再次抛出沿用栈析构语义，不能改成catch后手工清理。
    struct LibraryUnwind
    {
        GdiModuleReference* value;
        ~LibraryUnwind() noexcept(false) { if (value) value->~GdiModuleReference(); }
    } unwind{&Library()};
    Close();
    unused_.RetireOwned();
    unwind.value = nullptr;
    Library().~GdiModuleReference();
}

GdiDraw* GdiPlusDraw::Destroy(const std::uint32_t flags)
{
    // 原10005CB0：析构抛出时不执行后面的operator delete。
    this->~GdiPlusDraw();
    if (flags & 1u) ::operator delete(this);
    return this;
}

FARPROC GdiPlusDraw::Resolve(const char* name, const bool protectConstruction) const
{
    // 原10007B70及各内联查符号点：先取Ref地址，再构造真实CStringA；
    // 返回前销毁名称串，调用者随后才调用导出。不缓存、不补空对象/句柄门控。
    // 原10006309/10006536在GetProcAddress前已撤串保护，API异常不补析构。
    GdiDrawModule* const module = Library().get();
    alignas(ATL::CStringA) std::byte storage[sizeof(ATL::CStringA)];
    ATL::CStringA* symbol;
    if (protectConstruction) symbol = ::new (storage) ATL::CStringA(name);
    else
    {
        // 原05F5C/0614D取nil后赋正文，没有构造展开；placement new带正文仍会
        // 在OOM时自动析构CString基类，故这里分开默认构造与赋值。
        symbol = ::new (storage) ATL::CStringA;
        *symbol = name;
    }
    const FARPROC procedure = GetProcAddress(module->handle, symbol->GetString());
    symbol->~CStringT();
    return procedure;
}

void GdiPlusDraw::Close()
{
    // 原10005F20按base而非HMODULE门控；Release缺失仍退库，抛出则跳过退库。
    if (Library())
    {
        const auto release = reinterpret_cast<void (__cdecl*)()>(Resolve("Release", false));
        if (release) release();
    }
    Library().RetireOwned();
}

int GdiPlusDraw::Initialize(HWND window, int width, int height)
{
    // 原1000601B/10008160：先分配8字节正文，退旧后发布base，再分配24字节control。
    // Publish抛出不补删已发布正文；路径串必须晚于这两次真实分配。
    Library().Publish(new GdiDrawModule);
    bool loaded;
    {
        GdiDrawModule* const module = Library().get();
        // 原10006010无路径串的局部展开动作，正常10006111才释放字符串。
        alignas(ATL::CStringA) std::byte storage[sizeof(ATL::CStringA)];
        auto* path = ::new (storage) ATL::CStringA;
        *path = "Lib\\RenderGDIp.dll";
        const HMODULE handle = LoadLibraryA(path->GetString());
        module->handle = handle;
        loaded = handle != nullptr;
        path->~CStringT();
    }
    // 原100060F5..10006113先发布HMODULE并销毁路径串，随后才查Initialize。
    if (loaded)
    {
        const auto initialize = reinterpret_cast<bool (__cdecl*)(int, int)>(Resolve("Initialize", false));
        if (initialize && initialize(width, height))
        {
            // 原100061EF..100061F6成功尾：高、宽、窗口；外部布尔仅消费AL。
            height_ = height;
            width_ = width;
            window_ = window;
            return 0;
        }
    }
    // 原100061D0..100061DC：仅正常失败Close后返回7，异常不转返回值或DIB。
    Close();
    return 7;
}

bool GdiPlusDraw::Copy(RECT clip, SurfaceInfo& source)
{
    if (!Library()) return false;
    const auto lock = reinterpret_cast<bool (__cdecl*)(RECT, DWORD)>(Resolve("LockBits"));
    if (!lock) return false;
    // 原10006342..10006365：按值传完整逻辑画布的四DWORD，另传flags=3。
    const RECT entire{0, 0, width_, height_};
    if (!lock(entire, 3)) return false;

    // 原10006394/100063A1之后才到100063B2/100063BF：查Pitch、调Pitch、查Bits、调Bits。
    // 缺导出、导出抛出或后续Info/copy抛出，都不额外补库UnlockBits。
    const auto getPitch = reinterpret_cast<int (__cdecl*)()>(Resolve("GetPitch"));
    if (!getPitch) return false;
    const int pitch = getPitch();
    const auto getBits = reinterpret_cast<std::uint8_t* (__cdecl*)()>(Resolve("GetBits"));
    if (!getBits) return false;
    std::uint8_t* const bits = getBits();

    bool copied;
    {
        // 原100063C8..10006426：真实栈Info及locker构造后借用像素，不验证pitch/bits。
        SurfaceInfo destination;
        destination.width = width_;
        destination.height = height_;
        destination.valid = true;
        destination.pitch = pitch;
        destination.pixels.Borrow(bits,
            static_cast<std::size_t>(pitch) * static_cast<std::size_t>(destination.height));
        destination.sourcePixelFormat = 7;
        copied = CopyGdiSurface(destination, source, &clip) == 0;
    }
    // 原10006440先析构Info，10006452才迟查UnlockBits；正常copy非0也走此尾。
    const auto unlock = reinterpret_cast<void (__cdecl*)()>(Resolve("UnlockBits"));
    if (unlock) unlock();
    return copied;
}

bool GdiPlusDraw::Blt(HDC target, int x, int y, int width, int height, int sx, int sy, DWORD rop)
{
    // 原10006490没有base空值早退；10006593传八DWORD并原样返回AL。
    const auto blt = reinterpret_cast<bool (__cdecl*)(HDC, int, int, int, int, int, int, DWORD)>(
        Resolve("BltImage"));
    if (!blt) return false;
    return blt(target, x, y, width, height, sx, sy, rop);
}

bool GdiPlusDraw::Stretch(HDC target, int x, int y, int width, int height,
    int sx, int sy, int sw, int sh, DWORD /*rop*/)
{
    // 原10006631有base空值早退；10006707仅转发九DWORD，不传虚槽最后的rop。
    if (!Library()) return false;
    const auto draw = reinterpret_cast<bool (__cdecl*)(HDC, int, int, int, int, int, int, int, int)>(
        Resolve("DrawImage"));
    if (!draw) return false;
    return draw(target, x, y, width, height, sx, sy, sw, sh);
}
}
