#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <utility>
#include "RendererReference.h"
#include "SurfaceReference.h"

namespace KaGuYa::Graphics
{
inline constexpr std::uint32_t SurfaceMagic = 0x5347594B; // "KYGS"

class PixelBuffer final
{
public:
    void Allocate(const std::size_t size, const bool clear)
    {
        // 原1000FE86调用throwing new[]；不能把合法尺寸的OOM伪装成创建返回false。
        std::unique_ptr<std::uint8_t[]> replacement(new std::uint8_t[size]);
        if (clear && size != 0)
        {
            std::memset(replacement.get(), 0, size);
        }
        storage_ = std::move(replacement);
        borrowed_ = nullptr;
        size_ = size;
    }

    void assign(const std::size_t size, const std::uint8_t value)
    {
        std::unique_ptr<std::uint8_t[]> replacement(new std::uint8_t[size]);
        if (size != 0)
        {
            std::memset(replacement.get(), value, size);
        }
        storage_ = std::move(replacement);
        borrowed_ = nullptr;
        size_ = size;
    }

    void Borrow(std::uint8_t* bytes, const std::size_t size) noexcept
    {
        storage_.reset();
        borrowed_ = bytes;
        size_ = size;
    }
    void Release() noexcept { storage_.reset(); borrowed_ = nullptr; size_ = 0; }
    std::uint8_t* data() noexcept { return storage_ ? storage_.get() : borrowed_; }
    const std::uint8_t* data() const noexcept { return storage_ ? storage_.get() : borrowed_; }
    std::uint8_t& operator[](const std::size_t index) noexcept { return data()[index]; }
    const std::uint8_t& operator[](const std::size_t index) const noexcept { return data()[index]; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

private:
    std::unique_ptr<std::uint8_t[]> storage_;
    std::uint8_t* borrowed_ = nullptr;
    std::size_t size_ = 0;
};

// CPU临时图仅持有像素布局，不应隐式创建原生Surface/Info/locker链。
struct SurfacePixels
{
    int width = 0;
    int height = 0;
    int pitch = 0;
    int pixelSize = 4;
    int sourcePixelFormat = 0;
    bool clearOnCreate = false;
    std::uint32_t clearColor = 0; // 原像素描述对象 +0x24，由 SurfaceClear 两个导出写入。
    PixelBuffer pixels;
};

class SurfaceLocker
{
public:
    virtual int Lock() = 0;
    virtual int Unlock() = 0;
    virtual ~SurfaceLocker() noexcept(false) = default;
};

class NullSurfaceLocker final : public SurfaceLocker
{
public:
    int Lock() override { return 0; }
    int Unlock() override { return 0; }
};

// 原CSurfaceInfo既是像素描述对象，也实现ISurfaceLocker并转发给所持locker。
// 原Info调用者经vslot0/4派发；不能用final把外部可观察的虚调用去虚化。
class SurfaceInfo : public SurfaceLocker, public SurfacePixels
{
public:
    SurfaceInfo();
    ~SurfaceInfo() noexcept(false) override;
    int Lock() override;
    int Unlock() override;
    bool valid = false;
    bool locked = false;
    RendererReference<SurfaceLocker> locker;
};

struct Surface final
{
    Surface();
    ~Surface() noexcept(false);
    std::uint32_t magic = SurfaceMagic;
    RendererReference<SurfaceInfo> info;
};
struct SurfaceManager;

// 原100111C0/10011A30/10011BA0仅为目标建立异常Unlock保护；源不能统一套第二个guard。
template<class Operation>
bool WithSurfaceTransferLocks(SurfaceInfo& destination, SurfaceInfo& source, Operation&& operation)
{
    if (destination.Lock() != 0) return false;
    struct DestinationUnwind
    {
        SurfaceInfo* value;
        ~DestinationUnwind() noexcept(false) { if (value) value->Unlock(); }
    } unwind{&destination};
    if (source.Lock() != 0)
    {
        unwind.value = nullptr;
        destination.Unlock();
        return false;
    }
    const bool result = operation();
    source.Unlock();
    unwind.value = nullptr;
    destination.Unlock();
    return result;
}

Surface* CreateSurface(int width, int height, int pixelFormat, int clearOnCreate);
bool InitializeSurfacePixels(SurfaceInfo& surface, int width, int height, int pixelFormat, int clearOnCreate);
Surface* RegisterSurface(SurfaceManager& manager, const SurfaceReference& surface);
void BindBorrowedPrimarySurface(SurfaceInfo& surface, int width, int height, int pitch, std::uint8_t* pixels);
bool ReleaseSurface(int handle);
Surface* ResolveSurfaceBody(int handle) noexcept;
SurfaceInfo* ResolveSurface(int handle) noexcept;
int ToHandle(Surface* surface) noexcept;
int ToHandle(SurfaceInfo* surface) noexcept;
bool ClearSurface(SurfacePixels& surface, std::uint32_t color, bool preserveAlpha) noexcept;
bool CopySurfaceRect(SurfacePixels& destination, const SurfacePixels& source, int dx, int dy, int sx, int sy, int width, int height) noexcept;
bool LoadBitmapPixels(SurfacePixels& destination, const void* bitmapInfo, const void* pixels, int flags) noexcept;
bool LoadAlpPixels(SurfacePixels& destination, int mode, int width, int height, const void* pixels) noexcept;
bool LoadAnmPixels(SurfacePixels& destination, int width, int height, int channels, const void* pixels) noexcept;
}
