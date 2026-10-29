#include <Logging/Logger.h>

#include "Surface.h"
#include "GraphicsState.h"
#include "SurfaceManager.h"
#include "DibBitmap.h"

#include <Windows.h>

#include <algorithm>
#include <bit>
#include <cstdint>
#include <new>

using KaGuYa::Graphics::DibBitmap;

namespace
{

DibBitmap* Resolve(const int handle)
{
    auto* bitmap = reinterpret_cast<DibBitmap*>(static_cast<std::intptr_t>(handle));
    auto* manager = KaGuYa::Graphics::GetSurfaceManager();
    return manager && manager->bitmaps.Contains(bitmap) ? bitmap : nullptr;
}

int Handle(DibBitmap* bitmap)
{
    static_assert(sizeof(void*) == sizeof(int));
    return static_cast<int>(reinterpret_cast<std::intptr_t>(bitmap));
}

struct Region
{
    int left;
    int top;
    int right;
    int bottom;
};

struct TransferLayout
{
    Region source;
    Region destination;
    Region clippedDestination;
};

enum class TransferLayoutResult
{
    Invalid,
    Empty,
    Ready
};

bool IsEmpty(const Region& region) noexcept
{
    return region.left >= region.right || region.top >= region.bottom;
}

Region ClampRect(const int width, const int height, const int rectAddress) noexcept
{
    if (rectAddress == 0)
    {
        return {0, 0, width, height};
    }

    const auto* rect = reinterpret_cast<const RECT*>(static_cast<std::intptr_t>(rectAddress));
    Region result{rect->left, rect->top, rect->right, rect->bottom};
    if (width < result.left || result.right < 0)
    {
        result.left = 0;
        result.right = 0;
    }
    else
    {
        result.left = (std::max)(0, result.left);
        result.right = (std::min)(width, result.right);
    }
    if (height < result.top || result.bottom < 0)
    {
        result.top = 0;
        result.bottom = 0;
    }
    else
    {
        result.top = (std::max)(0, result.top);
        result.bottom = (std::min)(height, result.bottom);
    }
    return result;
}

TransferLayoutResult ResolveTransferLayout(const int destinationWidth, const int destinationHeight,
    const int sourceWidth, const int sourceHeight, int destinationX, int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor, TransferLayout& result) noexcept
{
    // Graphics.dll:1000E940/1000E9F0 将尾部四项交给 1000F840：
    // SIZE*、源 RECT*、目标裁剪 RECT*、九宫格锚点。DIB 与 Surface 共用此布局协议。
    const Region source = ClampRect(sourceWidth, sourceHeight, sourceRectAddress);
    if (IsEmpty(source))
    {
        return TransferLayoutResult::Invalid;
    }

    int width = source.right - source.left;
    int height = source.bottom - source.top;
    if (destinationSizeAddress != 0)
    {
        const auto* size = reinterpret_cast<const SIZE*>(
            static_cast<std::intptr_t>(destinationSizeAddress));
        width = size->cx;
        height = size->cy;
    }
    if (width <= 0 || height <= 0)
    {
        return TransferLayoutResult::Invalid;
    }

    switch (anchor)
    {
    case 1: destinationX -= width / 2; break;
    case 2: destinationX -= width; break;
    case 3: destinationY -= height / 2; break;
    case 4: destinationX -= width / 2; destinationY -= height / 2; break;
    case 5: destinationX -= width; destinationY -= height / 2; break;
    case 6: destinationY -= height; break;
    case 7: destinationX -= width / 2; destinationY -= height; break;
    case 8: destinationX -= width; destinationY -= height; break;
    default: break;
    }

    const Region destination{destinationX, destinationY, destinationX + width, destinationY + height};
    const Region clip = ClampRect(destinationWidth, destinationHeight, destinationClipAddress);
    const Region clipped{
        (std::max)(destination.left, clip.left),
        (std::max)(destination.top, clip.top),
        (std::min)(destination.right, clip.right),
        (std::min)(destination.bottom, clip.bottom)};
    if (IsEmpty(clipped))
    {
        return TransferLayoutResult::Empty;
    }

    result = {source, destination, clipped};
    return TransferLayoutResult::Ready;
}

void DrawSurfacePixelToDib(std::uint8_t* const destination, const std::uint8_t* const source,
    const bool destinationHasAlpha) noexcept
{
    const unsigned int sourceAlpha = source[3];
    if (!destinationHasAlpha)
    {
        if (destination[0] != source[0] || destination[1] != source[1] ||
            destination[2] != source[2])
        {
            for (int channel = 0; channel < 3; ++channel)
            {
                destination[channel] = static_cast<std::uint8_t>(
                    destination[channel] * (255u - sourceAlpha) / 255u +
                    source[channel] * sourceAlpha / 255u);
            }
            destination[3] = 0;
        }
        return;
    }

    const unsigned int destinationContribution =
        static_cast<unsigned int>(destination[3]) * (255u - sourceAlpha) / 255u;
    const unsigned int outputAlpha = sourceAlpha + destinationContribution;
    if (outputAlpha == 0)
    {
        return;
    }
    const unsigned int sourceWeight = 255u * sourceAlpha / outputAlpha;
    const unsigned int destinationWeight = 255u * destinationContribution / outputAlpha;
    for (int channel = 0; channel < 3; ++channel)
    {
        destination[channel] = static_cast<std::uint8_t>(
            source[channel] * sourceWeight / 255u +
            destination[channel] * destinationWeight / 255u);
    }
    destination[3] = static_cast<std::uint8_t>(outputAlpha);
}

void CopySurfacePixelToDib(std::uint8_t* const destination, const std::uint8_t* const source,
    const bool sourceHasAlpha, const bool destinationHasAlpha) noexcept
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = destinationHasAlpha ? (sourceHasAlpha ? source[3] : 0xFF) : 0;
}
}

extern "C" int __cdecl DIBitmapCreate(const int width, const int height, int bitsPerPixel, const int alpha)
{
    // 原1000E6C0先throwing new；FuncInfo10054B2C无catch，拒绝参数也不能跳过此分配。
    // 原1000E764：body及Info构造后独立建立body Ref，然后才调用GDI Create。
    using namespace KaGuYa::Graphics;
    auto& manager = RequireSurfaceManager(); // 原E6C0 arg0跨越构造/GDI调用保存，不晚取全局根。
    alignas(DibBitmapReference) std::byte storage[sizeof(DibBitmapReference)];
    auto* owner = ::new (storage) DibBitmapReference;
    owner->Publish(new DibBitmap);
    ReferenceRetirement retirement(*owner); // 原E76F在body control成功后才建立本层EH。
    auto* result = owner->get();
    // 用户入口保留局部body Ref；正常Create失败只在这个调用者退局部引用。
    if (!result->Create(width, height, bitsPerPixel, static_cast<unsigned char>(alpha)))
    {
        retirement.Dismiss();
        owner->RetireCachedControl();
        return 0;
    }
    manager.bitmaps.Append(*owner);
    retirement.Dismiss();
    owner->RetireCachedControl();
    return Handle(result);
}

extern "C" int __cdecl DIBitmapRelease(const int handle)
{
    auto* candidate = reinterpret_cast<DibBitmap*>(static_cast<std::intptr_t>(handle));
    auto* manager = KaGuYa::Graphics::GetSurfaceManager();
    return manager && manager->bitmaps.Erase(candidate) ? 1 : 0;
}

extern "C" int __cdecl GetDIBitmapDC(const int handle)
{
    const auto* bitmap = Resolve(handle);
    return bitmap == nullptr ? 0 : static_cast<int>(reinterpret_cast<std::intptr_t>(bitmap->dc));
}

extern "C" int __cdecl GetDIBitmapHBITMAP(const int handle)
{
    const auto* bitmap = Resolve(handle);
    return bitmap == nullptr ? 0 : static_cast<int>(reinterpret_cast<std::intptr_t>(bitmap->bitmap));
}

extern "C" std::uint64_t __cdecl GetDIBitmapSize(const int handle)
{
    const auto* bitmap = Resolve(handle);
    return bitmap == nullptr ? 0 : static_cast<std::uint32_t>(bitmap->info->width) |
        (static_cast<std::uint64_t>(static_cast<std::uint32_t>(bitmap->info->height)) << 32);
}

extern "C" int __cdecl DIBitmapClear(const int handle, const int color, const int rectAddress)
{
    auto* bitmap = Resolve(handle);
    if (bitmap == nullptr || bitmap->info->pixels.data() == nullptr) return 0;
    bitmap->info->clearColor = static_cast<std::uint32_t>(color);
    if (bitmap->info->Lock() != 0) return 0;

    // 100083C0 的操作 0 只处理内部格式 7/12；低位 DIB 保持原样但仍返回成功。
    if (bitmap->bitsPerPixel != 32)
    {
        bitmap->info->Unlock();
        return 1;
    }

    const Region region = ClampRect(bitmap->info->width, bitmap->info->height, rectAddress);
    const auto packedColor = static_cast<std::uint32_t>(color);
    for (int y = region.top; y < region.bottom; ++y)
    {
        auto* pixel = static_cast<std::uint8_t*>(bitmap->info->pixels.data()) +
            static_cast<std::size_t>(y) * bitmap->info->pitch + region.left * 4;
        for (int x = region.left; x < region.right; ++x, pixel += 4)
        {
            pixel[0] = static_cast<std::uint8_t>(packedColor);
            pixel[1] = static_cast<std::uint8_t>(packedColor >> 8);
            pixel[2] = static_cast<std::uint8_t>(packedColor >> 16);
            pixel[3] = (bitmap->info->sourcePixelFormat == 12) ? static_cast<std::uint8_t>(packedColor >> 24) : 0;
        }
    }
    bitmap->info->Unlock();
    return 1;
}

extern "C" int __cdecl DIBitmapCopy(const int destinationDib, const int sourceSurface,
    const int destinationX, const int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor)
{
    auto* destination = Resolve(destinationDib);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceSurface);
    if (destination == nullptr || source == nullptr) return 0;
    return KaGuYa::Graphics::WithSurfaceTransferLocks(*destination->info, *source, [&]() -> bool
    {
        if (destination->bitsPerPixel != 32) return false;

        TransferLayout layout{};
        const auto layoutResult = ResolveTransferLayout(destination->info->width, destination->info->height,
            source->width, source->height, destinationX, destinationY,
            destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, layout);
        if (layoutResult != TransferLayoutResult::Ready)
        {
            return layoutResult == TransferLayoutResult::Empty ? 1 : 0;
        }

        const int sourceWidth = layout.source.right - layout.source.left;
        const int sourceHeight = layout.source.bottom - layout.source.top;
        const int destinationWidth = layout.destination.right - layout.destination.left;
        const int destinationHeight = layout.destination.bottom - layout.destination.top;
        const bool sourceHasAlpha = source->sourcePixelFormat >= 10;
        for (int y = layout.clippedDestination.top; y < layout.clippedDestination.bottom; ++y)
        {
            const int sourceY = layout.source.top + static_cast<int>(
                static_cast<long long>(y - layout.destination.top) * sourceHeight / destinationHeight);
            auto* destinationPixel = static_cast<std::uint8_t*>(destination->info->pixels.data()) +
                static_cast<std::size_t>(y) * destination->info->pitch + layout.clippedDestination.left * 4;
            for (int x = layout.clippedDestination.left;
                x < layout.clippedDestination.right; ++x, destinationPixel += 4)
            {
                const int sourceX = layout.source.left + static_cast<int>(
                    static_cast<long long>(x - layout.destination.left) * sourceWidth / destinationWidth);
                const auto* sourcePixel = source->pixels.data() + sourceY * source->pitch + sourceX * 4;
                CopySurfacePixelToDib(destinationPixel, sourcePixel, sourceHasAlpha, (destination->info->sourcePixelFormat == 12));
            }
        }
        return 1;
    });
}

extern "C" int __cdecl DIBitmapDraw(const int destinationDib, const int sourceSurface,
    const int destinationX, const int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor)
{
    auto* destination = Resolve(destinationDib);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceSurface);
    if (destination == nullptr || source == nullptr) return 0;
    return KaGuYa::Graphics::WithSurfaceTransferLocks(*destination->info, *source, [&]() -> bool
    {

        const int expectedSourceFormat = destination->bitsPerPixel <= 16 ? 11 : 12;
        if (source->sourcePixelFormat != expectedSourceFormat)
        {
            return 0;
        }

        // 原 operation 10 对 15/16/24-bit DIB 只验证格式并返回成功，不写像素。
        if (destination->bitsPerPixel != 32)
        {
            return 1;
        }

        TransferLayout layout{};
        const auto layoutResult = ResolveTransferLayout(destination->info->width, destination->info->height,
            source->width, source->height, destinationX, destinationY,
            destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, layout);
        if (layoutResult != TransferLayoutResult::Ready)
        {
            return layoutResult == TransferLayoutResult::Empty ? 1 : 0;
        }

        const int sourceWidth = layout.source.right - layout.source.left;
        const int sourceHeight = layout.source.bottom - layout.source.top;
        const int destinationWidth = layout.destination.right - layout.destination.left;
        const int destinationHeight = layout.destination.bottom - layout.destination.top;
        const bool destinationHasAlpha = (destination->info->sourcePixelFormat == 12);
        for (int y = layout.clippedDestination.top; y < layout.clippedDestination.bottom; ++y)
        {
            const int sourceY = layout.source.top + static_cast<int>(
                static_cast<long long>(y - layout.destination.top) * sourceHeight / destinationHeight);
            auto* destinationPixel = static_cast<std::uint8_t*>(destination->info->pixels.data()) +
                static_cast<std::size_t>(y) * destination->info->pitch +
                layout.clippedDestination.left * 4;
            for (int x = layout.clippedDestination.left;
                x < layout.clippedDestination.right; ++x, destinationPixel += 4)
            {
                const int sourceX = layout.source.left + static_cast<int>(
                    static_cast<long long>(x - layout.destination.left) * sourceWidth / destinationWidth);
                const auto* sourcePixel = source->pixels.data() +
                    sourceY * source->pitch + sourceX * 4;
                DrawSurfacePixelToDib(destinationPixel, sourcePixel, destinationHasAlpha);
            }
        }
        return 1;
    });
}
