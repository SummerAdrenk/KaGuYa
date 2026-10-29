#pragma once

#include "DibBitmapReference.h"
#include "NativeObjectReference.h"
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <new>

namespace KaGuYa::Graphics
{
class SurfaceInfo;

// 原IGDIDraw十个虚槽；最后是带flags的删除入口，不让C++ delete异常自动补释放改变原前缀。
class GdiDraw
{
public:
    virtual HWND Window() const = 0;
    virtual int Width() const = 0;
    virtual int Height() const = 0;
    virtual void Close() = 0;
    virtual int Initialize(HWND window, int width, int height) = 0;
    virtual int Kind() const = 0;
    virtual bool Copy(RECT clip, SurfaceInfo& source) = 0;
    virtual bool Blt(HDC target, int x, int y, int width, int height, int sx, int sy, DWORD rop) = 0;
    virtual bool Stretch(HDC target, int x, int y, int width, int height, int sx, int sy, int sw, int sh, DWORD rop) = 0;
    virtual GdiDraw* Destroy(std::uint32_t flags) = 0;
protected:
    ~GdiDraw() = default;
};
struct GdiDrawDelete { void operator()(GdiDraw* draw) const { if (draw) draw->Destroy(1); } };
using GdiDrawReference = NativeObjectReference<GdiDraw, 4, GdiDrawDelete>;

// 原8字节CLoadLibrary，业务Release属于Draw.Close，资源析构只FreeLibrary。
struct GdiDrawModule
{
    virtual GdiDrawModule* Destroy(std::uint32_t flags);
    HMODULE handle = nullptr;
    ~GdiDrawModule() noexcept(false);
};
static_assert(sizeof(GdiDrawModule) == 8);
struct GdiDrawModuleDelete { void operator()(GdiDrawModule* module) const { if (module) module->Destroy(1); } };
using GdiModuleReference = NativeObjectReference<GdiDrawModule, 8, GdiDrawModuleDelete>;

// +1C的Ref在当前原链没有发布。只表达已证的引用协议，不猜其payload业务类型。
struct GdiOpaqueControl
{
    virtual void DestroyPayload() = 0;
    virtual void DeleteControl(std::uint32_t flags) = 0;
    std::uint32_t references;
    std::uint8_t owning;
};
struct GdiOpaqueReference
{
    void* base = nullptr;
    std::uint32_t index = 0;
    GdiOpaqueControl* control = nullptr;
    void RetireOwned();
};
static_assert(sizeof(GdiOpaqueReference) == 12);

class GdiPlusDraw final : public GdiDraw
{
public:
    GdiPlusDraw();
    ~GdiPlusDraw() noexcept(false);
    HWND Window() const override { return window_; }
    int Width() const override { return width_; }
    int Height() const override { return height_; }
    void Close() override;
    int Initialize(HWND window, int width, int height) override;
    int Kind() const override { return 1; }
    bool Copy(RECT clip, SurfaceInfo& source) override;
    bool Blt(HDC target, int x, int y, int width, int height, int sx, int sy, DWORD rop) override;
    bool Stretch(HDC target, int x, int y, int width, int height, int sx, int sy, int sw, int sh, DWORD rop) override;
    GdiDraw* Destroy(std::uint32_t flags) override;
private:
    GdiModuleReference& Library() noexcept
    { return *std::launder(reinterpret_cast<GdiModuleReference*>(library_)); }
    const GdiModuleReference& Library() const noexcept
    { return *std::launder(reinterpret_cast<const GdiModuleReference*>(library_)); }
    FARPROC Resolve(const char* name, bool protectConstruction = true) const;
    alignas(GdiModuleReference) std::byte library_[sizeof(GdiModuleReference)];
    HWND window_;
    int width_, height_;
    GdiOpaqueReference unused_;
};
static_assert(sizeof(GdiPlusDraw) == 0x28);

class DibDraw final : public GdiDraw
{
public:
    DibDraw();
    ~DibDraw() noexcept(false);
    HWND Window() const override { return window_; }
    int Width() const override { return width_; }
    int Height() const override { return height_; }
    void Close() override;
    int Initialize(HWND window, int width, int height) override;
    int Kind() const override { return 0; }
    bool Copy(RECT clip, SurfaceInfo& source) override;
    bool Blt(HDC target, int x, int y, int width, int height, int sx, int sy, DWORD rop) override;
    bool Stretch(HDC target, int x, int y, int width, int height, int sx, int sy, int sw, int sh, DWORD rop) override;
    GdiDraw* Destroy(std::uint32_t flags) override;
private:
    DibBitmapReference& Bitmap() noexcept
    { return *std::launder(reinterpret_cast<DibBitmapReference*>(bitmap_)); }
    alignas(DibBitmapReference) std::byte bitmap_[sizeof(DibBitmapReference)];
    HWND window_;
    int width_, height_;
};
static_assert(sizeof(DibDraw) == 0x1C);
}
