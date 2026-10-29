#include "GraphicsApi.h"

#include "NativeObjectReference.h"
#include "Surface.h"
#include "SurfaceManager.h"
#include <Logging/Logger.h>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <new>

namespace
{
std::uint64_t PackSize(const int width, const int height) noexcept
{
    return static_cast<std::uint32_t>(width) | (static_cast<std::uint64_t>(static_cast<std::uint32_t>(height)) << 32);
}

std::uint32_t ReadCenterPixel(const KaGuYa::Graphics::SurfacePixels& surface) noexcept
{
    if (surface.width <= 0 || surface.height <= 0 || surface.pixels.empty())
    {
        return 0;
    }

    const auto* pixel = surface.pixels.data() +
        (surface.height / 2) * surface.pitch + (surface.width / 2) * 4;
    return *reinterpret_cast<const std::uint32_t*>(pixel);
}

// 原1000EC70/1000EE90/1000EF80：仅平凡byte数组，复用共享Ref的两次分配及delete[]协议。
using SurfaceSaveBuffer = KaGuYa::Graphics::NativeObjectReference<std::uint8_t, 1,
    KaGuYa::Graphics::NativeArrayStorageDelete<std::uint8_t>>;
}

extern "C" int __cdecl SurfaceCreate(const int width, const int height, const int pixelFormat,
    const int clearOnCreate)
{
    return KaGuYa::Graphics::ToHandle(
        KaGuYa::Graphics::CreateSurface(width, height, pixelFormat, clearOnCreate));
}

extern "C" int __cdecl SurfaceRelease(const GraphicsHandle handle)
{
    return KaGuYa::Graphics::ReleaseSurface(handle) ? 1 : 0;
}

extern "C" int __cdecl GetSurfacePtr(const GraphicsHandle handle)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    return surface == nullptr ? 0 : static_cast<int>(reinterpret_cast<std::intptr_t>(surface->pixels.data()));
}

extern "C" int __cdecl GetSurfacePitch(const GraphicsHandle handle)
{
    const auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    return surface == nullptr ? 0 : surface->pitch;
}

extern "C" int __cdecl GetSurfacePixelSize(const GraphicsHandle handle)
{
    const auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    return surface == nullptr ? 0 : surface->pixelSize;
}

extern "C" int __cdecl GetSurfacePixel(const GraphicsHandle handle, const int x, const int y)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr || surface->Lock() != 0) return 0;
    if (x < 0 || y < 0 || x >= surface->width || y >= surface->height)
    {
        surface->Unlock();
        return 0;
    }
    if (surface->sourcePixelFormat != 7 && surface->sourcePixelFormat != 12)
    {
        // 原1002F603先写输出0，不读取未知格式的pixels。
        surface->Unlock();
        return 0;
    }
    const auto* pixel = surface->pixels.data() + y * surface->pitch + x * 4;
    const std::uint32_t value = *reinterpret_cast<const std::uint32_t*>(pixel);
    // 1002F590 对格式 7 只组装 BGR 三个通道；即使内存高 byte 非零，导出值的 alpha 仍为 0。
    const auto result = surface->sourcePixelFormat == 7 ? value & 0x00FFFFFFu : value;
    surface->Unlock();
    return static_cast<int>(result);
}

extern "C" std::uint64_t __cdecl GetSurfaceSize(const GraphicsHandle handle)
{
    const auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    return surface == nullptr || !surface->valid ? 0 : PackSize(surface->width, surface->height);
}

extern "C" int __cdecl SurfaceLoadBMP(const GraphicsHandle handle, const int, const int bitmapInfo,
    const int pixels, const int flags)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    const bool loaded = surface != nullptr && KaGuYa::Graphics::LoadBitmapPixels(*surface,
        reinterpret_cast<const void*>(static_cast<std::intptr_t>(bitmapInfo)),
        reinterpret_cast<const void*>(static_cast<std::intptr_t>(pixels)), flags);

    int sourceWidth = 0;
    int sourceHeight = 0;
    unsigned int sourceBits = 0;
    unsigned int sourceCompression = 0;
    if (bitmapInfo != 0)
    {
        const auto* info = reinterpret_cast<const std::uint8_t*>(static_cast<std::intptr_t>(bitmapInfo));
        sourceWidth = *reinterpret_cast<const std::int32_t*>(info + 4);
        sourceHeight = *reinterpret_cast<const std::int32_t*>(info + 8);
        sourceBits = *reinterpret_cast<const std::uint16_t*>(info + 14);
        sourceCompression = *reinterpret_cast<const std::uint32_t*>(info + 16);
    }
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
        "SurfaceLoadBMP: handle=%08X src=%dx%d/%ubit compression=%u flags=%08X pixels=%08X "
        "dst=%dx%d/f%d center=%08X result=%d.",
        static_cast<unsigned int>(handle), sourceWidth, sourceHeight, sourceBits, sourceCompression,
        static_cast<unsigned int>(flags), static_cast<unsigned int>(pixels),
        surface == nullptr ? 0 : surface->width, surface == nullptr ? 0 : surface->height,
        surface == nullptr ? 0 : surface->sourcePixelFormat,
        surface == nullptr ? 0u : ReadCenterPixel(*surface), loaded ? 1 : 0);
    return loaded ? 1 : 0;
}

extern "C" int __cdecl SurfaceLoadALP(const GraphicsHandle handle, const int mode, const int width,
    const int height, const int pixels)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    // Start.exe 只初始化该参数栈槽的低 byte；原 DLL 也在 1000E151 仅用 movzx 读取低 byte。
    // 因此导出边界必须丢弃未初始化的高 24 位，不能把完整 int 当作模式编号校验。
    const int normalizedMode = static_cast<unsigned char>(mode);
    const bool loaded = surface != nullptr && KaGuYa::Graphics::LoadAlpPixels(*surface, normalizedMode,
        width, height, reinterpret_cast<const void*>(static_cast<std::intptr_t>(pixels)));
    KaGuYa::Logging::WriteFormat(loaded ? KaGuYa::Logging::Level::Trace : KaGuYa::Logging::Level::Error,
        "SurfaceLoadALP: handle=%08X modeRaw=%08X mode=%d src=%dx%d pixels=%08X "
        "dst=%dx%d/f%d center=%08X result=%d.",
        static_cast<unsigned int>(handle), static_cast<unsigned int>(mode), normalizedMode,
        width, height, static_cast<unsigned int>(pixels),
        surface == nullptr ? 0 : surface->width, surface == nullptr ? 0 : surface->height,
        surface == nullptr ? 0 : surface->sourcePixelFormat,
        surface == nullptr ? 0u : ReadCenterPixel(*surface), loaded ? 1 : 0);
    return loaded ? 1 : 0;
}

extern "C" int __cdecl SurfaceLoadANM(const GraphicsHandle handle, const int width, const int height,
    const int channels, const int pixels)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    const bool loaded = surface != nullptr && KaGuYa::Graphics::LoadAnmPixels(*surface, width, height, channels,
        reinterpret_cast<const void*>(static_cast<std::intptr_t>(pixels)));
    KaGuYa::Logging::WriteFormat(loaded ? KaGuYa::Logging::Level::Trace : KaGuYa::Logging::Level::Error,
        "SurfaceLoadANM: handle=%08X src=%dx%d/%dch pixels=%08X dst=%dx%d/f%d center=%08X result=%d.",
        static_cast<unsigned int>(handle), width, height, channels, static_cast<unsigned int>(pixels),
        surface == nullptr ? 0 : surface->width, surface == nullptr ? 0 : surface->height,
        surface == nullptr ? 0 : surface->sourcePixelFormat,
        surface == nullptr ? 0u : ReadCenterPixel(*surface), loaded ? 1 : 0);
    return loaded ? 1 : 0;
}

extern "C" unsigned char __cdecl SurfaceSave(const GraphicsHandle handle, const HANDLE outputFile,
    const RECT* const sourceRect)
{
    const auto* candidate = reinterpret_cast<const KaGuYa::Graphics::Surface*>(static_cast<std::intptr_t>(handle));
    auto* const manager = KaGuYa::Graphics::GetSurfaceManager();
    // 原10003972会解引用坏root；保留1934安全拒绝，不把原AV当成等价合同。
    if (manager == nullptr)
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "SurfaceSave: missing renderer/manager root rejected (original 10003972).");
        return 1;
    }
    if (candidate == nullptr || !manager->surfaces.Contains(candidate))
    {
        // 原10003977..3987：未登记且HANDLE为0仍返回0，不解引用surface/RECT。
        if (outputFile == nullptr) return 0;
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "SurfaceSave: unregistered handle=%08X with non-null file rejected (original 1000398F).",
            static_cast<unsigned int>(handle));
        return 1;
    }
    const auto* surface = candidate->magic == KaGuYa::Graphics::SurfaceMagic ? candidate->info.get() : nullptr;
    if (surface == nullptr)
    {
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "SurfaceSave: invalid registered surface/Info handle=%08X rejected (original 10003998/10006A80).",
            static_cast<unsigned int>(handle));
        return 1;
    }
    // 原10006A80只借用Info；1000DD20不检查valid/locked，也没有Lock/Unlock。

    RECT rect;
    // 原1002F356仅在空RECT参数时调用SetRect，不检查其BOOL，也不捕获异常。
    if (sourceRect == nullptr) SetRect(&rect, 0, 0, surface->width, surface->height);
    else rect = *sourceRect;
    if (surface->width < rect.left || rect.right < 0)
    {
        rect.left = rect.right = 0;
    }
    else
    {
        rect.left = (std::max)(rect.left, 0L);
        rect.right = (std::min)(rect.right, static_cast<LONG>(surface->width));
    }
    if (surface->height < rect.top || rect.bottom < 0)
    {
        rect.top = rect.bottom = 0;
    }
    else
    {
        rect.top = (std::max)(rect.top, 0L);
        rect.bottom = (std::min)(rect.bottom, static_cast<LONG>(surface->height));
    }

    const int width = std::bit_cast<std::int32_t>(std::uint32_t(rect.right) - std::uint32_t(rect.left));
    const int height = std::bit_cast<std::int32_t>(std::uint32_t(rect.bottom) - std::uint32_t(rect.top));
    if (width <= 0 || height <= 0)
    {
        // 与原 DLL 一致：空矩形不写文件，但返回“无错误”。
        return 0;
    }

    // 原1000DD7F..DD93会32位包装；保留1934的64位安全检查，并拒绝原ECAA的signed bytes<=0域。
    // 空矩形已返回；合法NULL/INVALID文件仍先分配编码，最后才交给WriteFile。
    const std::uint64_t rowBytes64 = (std::uint64_t(width) * 3u + 3u) & ~std::uint64_t(3);
    const std::uint64_t fileBytes64 = rowBytes64 * std::uint64_t(height) + 54u;
    if (fileBytes64 > std::uint64_t((std::numeric_limits<std::int32_t>::max)()) ||
        fileBytes64 > std::uint64_t((std::numeric_limits<std::size_t>::max)()))
    {
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "SurfaceSave: BMP allocation size overflow rejected for %dx%d (original 1000DD7F/1000ECAA).",
            width, height);
        return 1;
    }
    const auto rowBytes = static_cast<std::uint32_t>(rowBytes64);
    const auto fileBytes = static_cast<DWORD>(fileBytes64);
    alignas(SurfaceSaveBuffer) std::byte storage[sizeof(SurfaceSaveBuffer)];
    auto& output = *::new (storage) SurfaceSaveBuffer;
    KaGuYa::Graphics::ReferenceRetirement unwind(output); // 原1000DDA2，分配前即激活Ref展开。
    // 原1000EEAA先发布raw正文，EEB2分配control失败时仍遗留正文，不加额外owner。
    output.Publish(static_cast<std::uint8_t*>(::operator new[](fileBytes)), 1, fileBytes);
    std::memset(output.get(), 0, output.Count()); // 原1000DDD8，包括行尾padding。

    BITMAPFILEHEADER fileHeader{};
    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = fileBytes;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    BITMAPINFOHEADER infoHeader{};
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biXPelsPerMeter = 3780;
    infoHeader.biYPelsPerMeter = 3780;
    std::memcpy(output.get(), &fileHeader, sizeof(fileHeader));
    std::memcpy(output.get() + sizeof(fileHeader), &infoHeader, sizeof(infoHeader));

    // 原编码器只对内部格式 7 写 BGR；其他格式仍会写出头部和全零像素区。
    if (surface->sourcePixelFormat == 7)
    {
        auto* destinationRow = output.get() + fileHeader.bfOffBits;
        for (int y = rect.bottom - 1; y >= rect.top; --y, destinationRow += rowBytes)
        {
            // 1000DD20 使用裁剪后的 right-left 作为输出宽度，但源行地址没有叠加 left*4。
            // 这是原 DLL 的既有行为：left 只缩短保存宽度，实际仍从每行 x=0 开始取样。
            const auto offset = std::bit_cast<std::int32_t>(std::uint32_t(y) * std::uint32_t(surface->pitch));
            const auto* source = surface->pixels.data() + offset;
            for (int x = 0; x < width; ++x)
            {
                destinationRow[x * 3 + 0] = source[x * 4 + 0];
                destinationRow[x * 3 + 1] = source[x * 4 + 1];
                destinationRow[x * 3 + 2] = source[x * 4 + 2];
            }
        }
    }

    DWORD written = 0;
    const BOOL succeeded = WriteFile(outputFile, output.get(), fileBytes, &written, nullptr);
    // 原1000DF51/DF59只检查完整BOOL，不校验written；WriteFile抛出则由05230展开。
    unwind.Dismiss();
    // 原1000DF60..DF83正常尾先撤EH再退缓存control；placement Ref不能再自动析构一次。
    output.RetireCachedControl();
    return succeeded ? 0 : 1;
}
