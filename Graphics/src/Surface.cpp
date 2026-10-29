#include "Surface.h"
#include "SurfaceManager.h"

#include <Logging/Logger.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{
void StoreBgra(std::uint8_t* destination, const std::uint8_t blue, const std::uint8_t green,
    const std::uint8_t red, const std::uint8_t alpha) noexcept
{
    destination[0] = blue;
    destination[1] = green;
    destination[2] = red;
    destination[3] = alpha;
}
}

bool InitializeSurfacePixels(SurfaceInfo& surface, const int width, const int height, const int pixelFormat,
    const int clearOnCreate)
{
    if (width < 0 || height < 0)
    {
        // CMemSurface::1000FE40 没有负尺寸分支，负值会进入无效的 32-bit 分配乘法；明确拒绝而不构造损坏对象。
        Logging::WriteFormat(Logging::Level::Error, "SurfaceCreate: 拒绝负尺寸 %dx%d。", width, height);
        return false;
    }
    if (width > (std::numeric_limits<int>::max)() / 4)
    {
        // 即使 height 为 0，原实现的 width*4 仍会包装成负 pitch；重写版明确拒绝该损坏状态。
        Logging::WriteFormat(Logging::Level::Error, "SurfaceCreate: 行距溢出，width=%d。", width);
        return false;
    }

    const std::uint64_t byteCount = static_cast<std::uint64_t>(width) * height * 4u;
    if (byteCount > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max()))
    {
        Logging::WriteFormat(Logging::Level::Error, "SurfaceCreate: 像素缓冲尺寸溢出 %dx%d。", width, height);
        return false;
    }

    // 原1000FE86成功后才写Info的布局；pixels失败不能提前提交描述字段。
    surface.pixels.Allocate(static_cast<std::size_t>(byteCount), clearOnCreate != 0);
    surface.width = width;
    surface.height = height;
    surface.pitch = width * 4;
    // 第三参数只取低BYTE；原内部格式分别为7/12。
    surface.sourcePixelFormat = static_cast<unsigned char>(pixelFormat) != 0 ? 12 : 7;
    surface.clearOnCreate = clearOnCreate != 0;
    surface.valid = true;
    return true;
}

Surface* RegisterSurface(SurfaceManager& manager, const SurfaceReference& surface)
{
    if (!surface) return nullptr;
    Surface* result = surface.get();
    manager.surfaces.Append(surface);
    // 原1000E651：登记表增持同一个body Ref；失败仍由调用者owner释放并向外传播。
    return result;
}

Surface* CreateSurface(const int width, const int height, const int pixelFormat, const int clearOnCreate)
{
    // 原DB60的arg0在进入分配前已捕获；回调后不能向新根的登记表提交旧调用的资源。
    auto& manager = RequireSurfaceManager();
    try
    {
        alignas(SurfaceReference) std::byte storage[sizeof(SurfaceReference)];
        auto* owner = ::new (storage) SurfaceReference;
        // 原DBAE撤body保护，DBC3建control，DBD1之后才激活局部Ref展开。
        owner->Publish(new Surface);
        ReferenceRetirement retirement(*owner);
        if (!InitializeSurfacePixels(*owner->get()->info, width, height, pixelFormat, clearOnCreate))
        {
            retirement.Dismiss();
            owner->RetireCachedControl();
            return nullptr;
        }
        Surface* result = RegisterSurface(manager, *owner);
        retirement.Dismiss();
        owner->RetireCachedControl(); // 原DC75先撤EH，正常尾不重读/清局部Ref。
        Logging::WriteFormat(Logging::Level::Trace,
            "SurfaceCreate: handle=%p size=%dx%d alphaRaw=%08X format=%d clear=%d。",
            static_cast<void*>(result), width, height, static_cast<unsigned int>(pixelFormat),
            result->info->sourcePixelFormat, result->info->clearOnCreate ? 1 : 0);
        return result;
    }
    catch (...)
    {
        // 原1000DB60只有Ref展开，没有catch；Surface导出的分配异常必须向外传播。
        Logging::Write(Logging::Level::Error, "SurfaceCreate: 对象、像素或登记失败，清理后传播异常。");
        throw;
    }
}

void BindBorrowedPrimarySurface(SurfaceInfo& surface, const int width, const int height, const int pitch,
    std::uint8_t* const pixels)
{
    // 原100012E5..10001335/10002E17直接绑定后端GetBits/GetPitch，不分配或登记第二张像素图。
    // 拒绝不能安全寻址的后端布局；不裁图、不伪造像素或静默替换后端。
    const auto rowBytes = static_cast<std::int64_t>(width) * 4;
    const auto size = static_cast<std::int64_t>(pitch) * height;
    if (width < 0 || height < 0 || pitch < 0 || pitch < rowBytes ||
        size > static_cast<std::int64_t>((std::numeric_limits<std::size_t>::max)()) ||
        (size != 0 && pixels == nullptr))
        throw std::invalid_argument("invalid external primary Surface layout");
    // 原10001312先完成Ref发布，再写布局；控制块失败时不能提前留下完整布局。
    surface.width = width;
    surface.height = height;
    surface.pitch = pitch;
    surface.sourcePixelFormat = 7;
    surface.pixels.Borrow(pixels, static_cast<std::size_t>(size));
    surface.valid = true;
}

bool ReleaseSurface(const int handle)
{
    auto* const surface = reinterpret_cast<Surface*>(static_cast<std::intptr_t>(handle));
    if (surface == nullptr)
    {
        return false;
    }

    // 原DCB0没有跨payload析构持有独立锁；回调查询登记表不应被额外互斥锁阻塞。
    auto* manager = GetSurfaceManager();
    return manager && manager->surfaces.Erase(surface);
}

Surface* ResolveSurfaceBody(const int handle) noexcept
{
    // 原10004576..4592只检查body登记；Blur在分配/取样回调之后才按原时点查询Info。
    auto* const candidate = reinterpret_cast<Surface*>(static_cast<std::intptr_t>(handle));
    auto* manager = GetSurfaceManager();
    return candidate && manager && manager->surfaces.Contains(candidate) && candidate->magic == SurfaceMagic
        ? candidate : nullptr;
}

SurfaceInfo* ResolveSurface(const int handle) noexcept
{
    auto* const candidate = reinterpret_cast<Surface*>(static_cast<std::intptr_t>(handle));
    // 原1000D970只查manager登记；GetScreen给出的是独立描述对象，不是用户Surface句柄。
    auto* manager = GetSurfaceManager();
    const bool owned = candidate && manager && manager->surfaces.Contains(candidate);
    return owned && candidate->magic == SurfaceMagic ? candidate->info.get() : nullptr;
}

int ToHandle(Surface* const surface) noexcept
{
    static_assert(sizeof(void*) == sizeof(int), "Graphics.dll 必须以 Win32/x86 构建。");
    return static_cast<int>(reinterpret_cast<std::intptr_t>(surface));
}

int ToHandle(SurfaceInfo* const surface) noexcept
{
    return static_cast<int>(reinterpret_cast<std::intptr_t>(surface));
}

bool ClearSurface(SurfacePixels& surface, const std::uint32_t color, const bool preserveAlpha) noexcept
{
    if (surface.width == 0 || surface.height == 0) return true;
    for (int y = 0; y < surface.height; ++y)
    {
        // 后端pitch可大于width*4；只操作有效像素，不能把行末padding当成画面。
        auto* row = surface.pixels.data() + static_cast<std::size_t>(y) * surface.pitch;
        for (int x = 0; x < surface.width; ++x)
        {
            auto* pixel = row + static_cast<std::size_t>(x) * 4;
            const std::uint8_t alpha = surface.sourcePixelFormat >= 10
                ? (preserveAlpha ? pixel[3] : static_cast<std::uint8_t>(color >> 24)) : 0;
            pixel[0] = static_cast<std::uint8_t>(color);
            pixel[1] = static_cast<std::uint8_t>(color >> 8);
            pixel[2] = static_cast<std::uint8_t>(color >> 16);
            pixel[3] = alpha;
        }
    }
    return true;
}

bool CopySurfaceRect(SurfacePixels& destination, const SurfacePixels& source, int dx, int dy, int sx, int sy, int width, int height) noexcept
{
    if (width <= 0 || height <= 0)
    {
        return true;
    }

    if (sx < 0) { dx -= sx; width += sx; sx = 0; }
    if (sy < 0) { dy -= sy; height += sy; sy = 0; }
    if (dx < 0) { sx -= dx; width += dx; dx = 0; }
    if (dy < 0) { sy -= dy; height += dy; dy = 0; }
    width = std::min({width, source.width - sx, destination.width - dx});
    height = std::min({height, source.height - sy, destination.height - dy});
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    // 同一 Surface 且区域重叠时，按方向选择行序，保持 memmove 语义。
    const bool reverse = &destination == &source && dy > sy;
    for (int row = 0; row < height; ++row)
    {
        const int index = reverse ? height - 1 - row : row;
        auto* destinationRow = destination.pixels.data() + (dy + index) * destination.pitch + dx * 4;
        const auto* sourceRow = source.pixels.data() + (sy + index) * source.pitch + sx * 4;
        std::memmove(destinationRow, sourceRow, static_cast<std::size_t>(width) * 4);
    }
    return true;
}

bool LoadBitmapPixels(SurfacePixels& destination, const void* const bitmapInfo, const void* const pixelData, const int flags) noexcept
{
    if (bitmapInfo == nullptr)
    {
        return false;
    }

    const auto* info = static_cast<const std::uint8_t*>(bitmapInfo);
    const int width = *reinterpret_cast<const std::int32_t*>(info + 4);
    const int signedHeight = *reinterpret_cast<const std::int32_t*>(info + 8);
    const auto bitCount = *reinterpret_cast<const std::uint16_t*>(info + 14);
    const auto compression = *reinterpret_cast<const std::uint32_t*>(info + 16);
    if (compression != 0 || (bitCount != 16 && bitCount != 24))
    {
        return false;
    }

    // 1000DFB0 对 height <= 0 不进入逐行循环，仍返回成功；它不支持 top-down BMP。
    if (signedHeight <= 0 || width == 0)
    {
        return true;
    }
    if (width < 0 || pixelData == nullptr)
    {
        return false;
    }
    if (width > destination.width || signedHeight > destination.height)
    {
        Logging::WriteFormat(Logging::Level::Error,
            "SurfaceLoadBMP: 源尺寸 %dx%d 超出目标 %dx%d，拒绝原实现会发生的越界写入。",
            width, signedHeight, destination.width, destination.height);
        return false;
    }

    const int sourcePitch = ((width * bitCount / 8) + 3) & ~3;
    const auto* source = static_cast<const std::uint8_t*>(pixelData);
    const bool destinationHasAlpha = destination.sourcePixelFormat >= 10;
    const bool flipHorizontal = (flags & 1) != 0;
    for (int y = 0; y < signedHeight; ++y)
    {
        const int sourceY = signedHeight - 1 - y;
        const int destinationY = (flags & 2) != 0 ? signedHeight - 1 - y : y;
        auto* output = destination.pixels.data() + destinationY * destination.pitch;
        const auto* input = source + sourceY * sourcePitch;
        for (int x = 0; x < width; ++x)
        {
            const int destinationX = flipHorizontal ? width - 1 - x : x;
            if (bitCount == 24)
            {
                StoreBgra(output + destinationX * 4, input[x * 3], input[x * 3 + 1], input[x * 3 + 2],
                    destinationHasAlpha ? 0xFF : 0x00);
            }
            else
            {
                // 1000DFB0 对 16-bit 分支仍固定向 1002EED0 传源格式 5；该转换器按
                // 三字节 BGR 前进，而行首步长仍由 16-bit pitch 计算。这里保留原 DLL 行为。
                StoreBgra(output + destinationX * 4, input[x * 3], input[x * 3 + 1], input[x * 3 + 2],
                    destinationHasAlpha ? 0xFF : 0x00);
            }
        }
    }
    return true;
}

bool LoadAlpPixels(SurfacePixels& destination, const int mode, const int width, const int height, const void* const pixelData) noexcept
{
    if (mode < 0 || mode > 3 || width < 0 || height < 0)
    {
        return false;
    }
    // 1000E110 对零高度直接走成功出口，不访问 payload。
    if (height == 0 || width == 0)
    {
        return true;
    }
    if (pixelData == nullptr)
    {
        return false;
    }
    if (width > destination.width || height > destination.height)
    {
        Logging::WriteFormat(Logging::Level::Error,
            "SurfaceLoadALP: 源尺寸 %dx%d 超出目标 %dx%d，拒绝原实现会发生的越界写入。",
            width, height, destination.width, destination.height);
        return false;
    }
    const auto* source = static_cast<const std::uint8_t*>(pixelData);
    const int sourcePitch = mode == 0 ? width : (mode == 3 ? ((width * 3 + 3) & ~3) : width * 4);
    const bool destinationHasAlpha = destination.sourcePixelFormat == 12;
    for (int y = 0; y < height; ++y)
    {
        const auto* input = source + (height - 1 - y) * sourcePitch;
        auto* output = destination.pixels.data() + y * destination.pitch;
        for (int x = 0; x < width; ++x)
        {
            if (mode == 0)
            {
                output[x * 4 + 3] = input[x];
            }
            else if (mode == 3)
            {
                StoreBgra(output + x * 4, input[x * 3], input[x * 3 + 1], input[x * 3 + 2],
                    destinationHasAlpha ? 0xFF : 0x00);
            }
            else
            {
                if (destinationHasAlpha)
                {
                    std::memcpy(output + x * 4, input + x * 4, 4);
                }
                else
                {
                    StoreBgra(output + x * 4, input[x * 4], input[x * 4 + 1], input[x * 4 + 2], 0x00);
                }
            }
        }
    }
    return true;
}

bool LoadAnmPixels(SurfacePixels& destination, const int width, const int height, const int channels, const void* const pixelData) noexcept
{
    // 1000E2B0 在零宽或零高时不读取 payload，仍返回成功。
    if (width == 0 || height == 0)
    {
        return true;
    }
    if (width < 0 || height < 0)
    {
        return false;
    }
    if (pixelData == nullptr)
    {
        return false;
    }
    if (width > destination.width || height > destination.height)
    {
        Logging::WriteFormat(Logging::Level::Error,
            "SurfaceLoadANM: 源尺寸 %dx%d 超出目标 %dx%d，拒绝原实现会发生的越界写入。",
            width, height, destination.width, destination.height);
        return false;
    }
    const auto* source = static_cast<const std::uint8_t*>(pixelData);
    // 1000E2D6的行距仍乘原channels；1000E2D9..E3仅4选格式12，其余选BGR格式5。
    const auto sourcePitch = std::bit_cast<std::int32_t>(std::uint32_t(width) * std::uint32_t(channels));
    const int sourceStep = channels == 4 ? 4 : 3;
    const bool destinationHasAlpha = destination.sourcePixelFormat >= 10;
    for (int y = 0; y < height; ++y)
    {
        const auto rowOffset = std::bit_cast<std::int32_t>(std::uint32_t(height - 1 - y) * std::uint32_t(sourcePitch));
        const auto* input = source + rowOffset;
        auto* output = destination.pixels.data() + y * destination.pitch;
        for (int x = 0; x < width; ++x)
        {
            StoreBgra(output + x * 4, input[x * sourceStep], input[x * sourceStep + 1], input[x * sourceStep + 2],
                destinationHasAlpha ? (channels == 4 ? input[x * sourceStep + 3] : 0xFF) : 0x00);
        }
    }
    return true;
}
}
