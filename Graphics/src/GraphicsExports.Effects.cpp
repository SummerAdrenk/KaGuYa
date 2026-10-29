#include "Surface.h"
#include "SurfaceTransfer.h"
#include "SurfaceBlur.h"
#include "GraphicsState.h"

#include <Logging/Logger.h>

#include <Windows.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace
{
struct Region
{
    int left;
    int top;
    int right;
    int bottom;
};

struct BlitLayout
{
    Region source;
    Region destination;
    Region clippedDestination;
};

// 1000FAED..1000FBD2的裁剪起点、商/余数及error-1；像素核先累加余数再取样。
// 不能换成floor(position*source/destination)：294->236的缩略图会错选大量像素。
class BlitAxis final
{
public:
    BlitAxis(int origin, int sourceSpan, int destinationSpan, int clippedOffset) noexcept
        : coordinate_(origin), quotient_(sourceSpan / destinationSpan),
          remainder_(sourceSpan % destinationSpan), denominator_(destinationSpan)
    {
        std::int64_t error = std::int64_t(sourceSpan) * clippedOffset - destinationSpan;
        if (error > 0)
        {
            const auto advance = error / destinationSpan + 1;
            coordinate_ += advance;
            error -= advance * destinationSpan;
        }
        error_ = error - 1;
    }
    int Next() noexcept
    {
        error_ += remainder_;
        if (error_ >= 0) { ++coordinate_; error_ -= denominator_; }
        const auto sampled = coordinate_;
        coordinate_ += quotient_;
        return static_cast<int>(sampled);
    }
private:
    std::int64_t coordinate_, quotient_, remainder_, denominator_, error_;
};

Region ClampRegion(const KaGuYa::Graphics::SurfacePixels& surface, const int rectAddress) noexcept
{
    if (rectAddress == 0)
    {
        return {0, 0, surface.width, surface.height};
    }

    const auto* rect = reinterpret_cast<const RECT*>(static_cast<std::intptr_t>(rectAddress));
    return {
        (std::clamp)(static_cast<int>(rect->left), 0, surface.width),
        (std::clamp)(static_cast<int>(rect->top), 0, surface.height),
        (std::clamp)(static_cast<int>(rect->right), 0, surface.width),
        (std::clamp)(static_cast<int>(rect->bottom), 0, surface.height)};
}

bool IsEmpty(const Region& region) noexcept
{
    return region.left >= region.right || region.top >= region.bottom;
}

bool ResolveBlitLayout(const KaGuYa::Graphics::SurfacePixels& destination,
    const KaGuYa::Graphics::SurfacePixels& source,
    int destinationX, int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor,
    BlitLayout& result) noexcept
{
    // Graphics.dll:1000F840：Draw/Blt 的后四项依次为 SIZE*、源 RECT*、
    // 目标裁剪 RECT* 与九宫格锚点，不是四个源坐标。
    Region sourceRegion{0, 0, source.width, source.height};
    if (sourceRectAddress != 0)
    {
        const auto* sourceRect = reinterpret_cast<const RECT*>(
            static_cast<std::intptr_t>(sourceRectAddress));
        sourceRegion.left = (std::max)(0, static_cast<int>(sourceRect->left));
        sourceRegion.top = (std::max)(0, static_cast<int>(sourceRect->top));
        sourceRegion.right = (std::min)(source.width, static_cast<int>(sourceRect->right));
        sourceRegion.bottom = (std::min)(source.height, static_cast<int>(sourceRect->bottom));
    }
    if (IsEmpty(sourceRegion))
    {
        return false;
    }

    int destinationWidth = sourceRegion.right - sourceRegion.left;
    int destinationHeight = sourceRegion.bottom - sourceRegion.top;
    if (destinationSizeAddress != 0)
    {
        const auto* destinationSize = reinterpret_cast<const SIZE*>(
            static_cast<std::intptr_t>(destinationSizeAddress));
        destinationWidth = static_cast<int>(destinationSize->cx);
        destinationHeight = static_cast<int>(destinationSize->cy);
    }
    if (destinationWidth <= 0 || destinationHeight <= 0)
    {
        return false;
    }

    switch (anchor)
    {
    case 1:
        destinationX -= destinationWidth / 2;
        break;
    case 2:
        destinationX -= destinationWidth;
        break;
    case 3:
        destinationY -= destinationHeight / 2;
        break;
    case 4:
        destinationX -= destinationWidth / 2;
        destinationY -= destinationHeight / 2;
        break;
    case 5:
        destinationX -= destinationWidth;
        destinationY -= destinationHeight / 2;
        break;
    case 6:
        destinationY -= destinationHeight;
        break;
    case 7:
        destinationX -= destinationWidth / 2;
        destinationY -= destinationHeight;
        break;
    case 8:
        destinationX -= destinationWidth;
        destinationY -= destinationHeight;
        break;
    default:
        break;
    }

    const Region destinationRegion{
        destinationX,
        destinationY,
        destinationX + destinationWidth,
        destinationY + destinationHeight};
    Region destinationClip{0, 0, destination.width, destination.height};
    if (destinationClipAddress != 0)
    {
        const auto* clip = reinterpret_cast<const RECT*>(
            static_cast<std::intptr_t>(destinationClipAddress));
        destinationClip.left = (std::max)(0, static_cast<int>(clip->left));
        destinationClip.top = (std::max)(0, static_cast<int>(clip->top));
        destinationClip.right = (std::min)(destination.width, static_cast<int>(clip->right));
        destinationClip.bottom = (std::min)(destination.height, static_cast<int>(clip->bottom));
    }

    const Region clippedDestination{
        (std::max)(destinationRegion.left, destinationClip.left),
        (std::max)(destinationRegion.top, destinationClip.top),
        (std::min)(destinationRegion.right, destinationClip.right),
        (std::min)(destinationRegion.bottom, destinationClip.bottom)};
    if (IsEmpty(clippedDestination))
    {
        return false;
    }

    result = {sourceRegion, destinationRegion, clippedDestination};
    return true;
}

template <typename Operation>
bool TransformPixels(KaGuYa::Graphics::SurfacePixels& surface, const int rectAddress, Operation operation) noexcept
{
    const Region region = ClampRegion(surface, rectAddress);
    if (IsEmpty(region))
    {
        return true;
    }
    for (int y = region.top; y < region.bottom; ++y)
    {
        auto* pixel = surface.pixels.data() + y * surface.pitch + region.left * 4;
        for (int x = region.left; x < region.right; ++x, pixel += 4)
        {
            operation(pixel);
        }
    }
    return true;
}

template<typename Operation>
char TransformLockedPixels(KaGuYa::Graphics::SurfaceInfo& surface, const int rectAddress, Operation operation)
{
    // 原083C0/08B70：只接受Lock的0结果；正常尾Unlock，异常不增加补偿guard。
    if (surface.Lock() != 0) return 0;
    if (surface.sourcePixelFormat == 7 || surface.sourcePixelFormat == 12)
        TransformPixels(surface, rectAddress, operation);
    surface.Unlock();
    return 1;
}

std::uint8_t ColorByte(const std::uint32_t color, const int index) noexcept
{
    return static_cast<std::uint8_t>(color >> (index * 8));
}

std::uint8_t MultiplyByte(const std::uint8_t value, const std::uint8_t factor) noexcept
{
    // byte_1006A2B8 由 sub_1000F770 初始化，等价于 floor(value * factor / 255)。
    return static_cast<std::uint8_t>(static_cast<unsigned int>(value) * factor / 255u);
}

void BlendPixel(std::uint8_t* const destination, const std::uint8_t* const source,
    const std::uint8_t globalAlpha, const bool useGlobalAlpha,
    const bool sourceHasAlpha, const bool destinationHasAlpha) noexcept
{
    // 10027770 / 100280E4..1002814B：7 -> 12不按目标alpha归一化；
    // 10028180的7 -> 7也先比较RGB。相同时完全不写，否则固定全局权重混RGB。
    if (useGlobalAlpha && !sourceHasAlpha)
    {
        if (source[0] == destination[0] && source[1] == destination[1] && source[2] == destination[2])
            return;
        for (int channel = 0; channel < 3; ++channel)
            destination[channel] = static_cast<std::uint8_t>(MultiplyByte(source[channel], globalAlpha) +
                MultiplyByte(destination[channel], static_cast<std::uint8_t>(255u - globalAlpha)));
        destination[3] = destinationHasAlpha ? 255 : 0;
        return;
    }
    // 10026090与10026DAC：12 -> 7普通/全局路径均先比较RGB；相同连X也不写。
    // 不能推广到格式12目标的独立alpha合成路径。
    if (sourceHasAlpha && !destinationHasAlpha &&
        source[0] == destination[0] && source[1] == destination[1] && source[2] == destination[2])
        return;
    // 10010140.c：普通 Blt 使用 source alpha；10010090.c：BlendBlt 先乘全局 alpha。
    // format 7 的第 4 byte 固定为 0，但业务含义是“没有 alpha”，合成时必须按不透明处理。
    const unsigned int sourceAlpha = sourceHasAlpha ? source[3] : 255u;
    const unsigned int destinationAlpha = destinationHasAlpha ? destination[3] : 255u;
    const unsigned int sourceContribution = useGlobalAlpha
        ? sourceAlpha * globalAlpha / 255u
        : sourceAlpha;
    // 10026DB6..10026E18：格式 12 源绘制到格式 7 主屏幕时，背景权重取
    // 255 - (sourceAlpha * globalAlpha / 255)，不能只取 255 - globalAlpha。
    const unsigned int destinationFactor = useGlobalAlpha && !destinationHasAlpha
        ? 255u - sourceContribution
        : (useGlobalAlpha ? 255u - globalAlpha : 255u - sourceAlpha);
    const unsigned int destinationContribution =
        destinationAlpha * destinationFactor / 255u;
    const unsigned int outputAlpha = sourceContribution + destinationContribution;
    if (outputAlpha == 0)
    {
        return;
    }

    const auto sourceWeight = static_cast<std::uint8_t>(255u * sourceContribution / outputAlpha);
    const auto destinationWeight = static_cast<std::uint8_t>(255u * destinationContribution / outputAlpha);
    for (int channel = 0; channel < 3; ++channel)
    {
        destination[channel] = static_cast<std::uint8_t>(
            MultiplyByte(source[channel], sourceWeight) + MultiplyByte(destination[channel], destinationWeight));
    }
    if (destinationHasAlpha)
    {
        destination[3] = static_cast<std::uint8_t>(outputAlpha);
    }
    else if (sourceHasAlpha)
    {
        // 原1002617E与10026E1D..10026E1F：RGB不同时两条12 -> 7路径都写X=0。
        destination[3] = 0;
    }
}

enum class BlitMode
{
    Copy,
    SourceOver,
    GlobalAlpha
};

bool BlitSurfacePixels(KaGuYa::Graphics::SurfacePixels& destination, const KaGuYa::Graphics::SurfacePixels& source,
    const int destinationX, const int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor,
    const BlitMode mode, const std::uint8_t globalAlpha = 255) noexcept
{
    BlitLayout layout{};
    if (!ResolveBlitLayout(destination, source, destinationX, destinationY,
            destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, layout))
    {
        // 1000F840 对完全落在裁剪区外的合法绘制返回 1；这是成功空操作，
        // 上层会继续提交后续图层，不能把它误报为绘制失败。
        return true;
    }

    {
        const bool sourceHasAlpha = source.sourcePixelFormat >= 10;
        const bool destinationHasAlpha = destination.sourcePixelFormat >= 10;
        const int sourceWidth = layout.source.right - layout.source.left;
        const int sourceHeight = layout.source.bottom - layout.source.top;
        const int destinationWidth = layout.destination.right - layout.destination.left;
        const int destinationHeight = layout.destination.bottom - layout.destination.top;
        const bool unscaled = sourceWidth == destinationWidth && sourceHeight == destinationHeight;
        const auto* sourcePixels = source.pixels.data();
        BlitAxis vertical(layout.source.top, sourceHeight, destinationHeight,
            layout.clippedDestination.top - layout.destination.top);
        for (int destinationRow = layout.clippedDestination.top;
            destinationRow < layout.clippedDestination.bottom; ++destinationRow)
        {
            const int sourceY = unscaled ? layout.source.top + destinationRow - layout.destination.top
                : vertical.Next();
            auto* destinationPixel = destination.pixels.data() +
                destinationRow * destination.pitch + layout.clippedDestination.left * 4;
            if (unscaled)
            {
                const int sourceX = layout.source.left +
                    layout.clippedDestination.left - layout.destination.left;
                const auto* sourcePixel = sourcePixels + sourceY * source.pitch + sourceX * 4;
                const int width = layout.clippedDestination.right - layout.clippedDestination.left;
                if (mode == BlitMode::Copy && source.sourcePixelFormat == destination.sourcePixelFormat)
                {
                    std::memmove(destinationPixel, sourcePixel, static_cast<std::size_t>(width) * 4u);
                    continue;
                }
                for (int column = 0; column < width; ++column,
                    destinationPixel += 4, sourcePixel += 4)
                {
                    if (mode == BlitMode::Copy)
                    {
                        destinationPixel[0] = sourcePixel[0];
                        destinationPixel[1] = sourcePixel[1];
                        destinationPixel[2] = sourcePixel[2];
                        destinationPixel[3] = destinationHasAlpha
                            ? (sourceHasAlpha ? sourcePixel[3] : 0xFF)
                            : 0;
                    }
                    else
                    {
                        BlendPixel(destinationPixel, sourcePixel, globalAlpha,
                            mode == BlitMode::GlobalAlpha, sourceHasAlpha, destinationHasAlpha);
                    }
                }
                continue;
            }

            BlitAxis horizontal(layout.source.left, sourceWidth, destinationWidth,
                layout.clippedDestination.left - layout.destination.left);
            for (int destinationColumn = layout.clippedDestination.left;
                destinationColumn < layout.clippedDestination.right;
                ++destinationColumn, destinationPixel += 4)
            {
                const int sourceX = horizontal.Next();
                const auto* sourcePixel = sourcePixels + sourceY * source.pitch + sourceX * 4;
                if (mode == BlitMode::Copy)
                {
                    destinationPixel[0] = sourcePixel[0];
                    destinationPixel[1] = sourcePixel[1];
                    destinationPixel[2] = sourcePixel[2];
                    // 1001DB40同格式缩放也复制完整DWORD；跨格式才转换第4字节。
                    destinationPixel[3] = source.sourcePixelFormat == destination.sourcePixelFormat
                        ? sourcePixel[3] : destinationHasAlpha
                            ? (sourceHasAlpha ? sourcePixel[3] : 0xFF) : 0;
                }
                else
                {
                    BlendPixel(destinationPixel, sourcePixel, globalAlpha,
                        mode == BlitMode::GlobalAlpha, sourceHasAlpha, destinationHasAlpha);
                }
            }
        }
        return true;
    }
}

bool BlitSurface(KaGuYa::Graphics::SurfaceInfo& destination, KaGuYa::Graphics::SurfaceInfo& source,
    const int destinationX, const int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor,
    const BlitMode mode, const std::uint8_t globalAlpha = 255)
{
    return KaGuYa::Graphics::WithSurfaceTransferLocks(destination, source, [&]
    {
        return BlitSurfacePixels(destination, source, destinationX, destinationY, destinationSizeAddress,
            sourceRectAddress, destinationClipAddress, anchor, mode, globalAlpha);
    });
}

bool RemapAlphaSurface(KaGuYa::Graphics::SurfaceInfo& destination, KaGuYa::Graphics::SurfaceInfo& source,
    const int* const mappingParameters, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    if (mappingParameters == nullptr)
    {
        return false;
    }

    std::array<std::uint8_t, 256> alphaMap{};
    const int lowerIndex = mappingParameters[0];
    const int upperIndex = mappingParameters[1];
    const int lowerAlpha = mappingParameters[2];
    const int upperAlpha = mappingParameters[3];
    // 原100101F0：两端直接截低BYTE，仅中间段clamp；upper段可覆盖lower段。
    for (int value = 0; value < 256 && value <= lowerIndex; ++value)
        alphaMap[value] = static_cast<std::uint8_t>(lowerAlpha);
    for (std::int64_t value = (std::max)(0ll, std::int64_t(lowerIndex) + 1);
        value < 256 && value < upperIndex; ++value)
    {
        const auto numerator = std::bit_cast<std::int32_t>((std::uint32_t(value) - std::uint32_t(lowerIndex)) *
            (std::uint32_t(upperAlpha) - std::uint32_t(lowerAlpha)));
        const auto denominator = std::bit_cast<std::int32_t>(std::uint32_t(upperIndex) - std::uint32_t(lowerIndex));
        if (!denominator || (numerator == INT32_MIN && denominator == -1))
            throw std::out_of_range("Universal mapping IDIV undefined");
        const auto mapped = std::bit_cast<std::int32_t>(std::uint32_t(numerator / denominator) + std::uint32_t(lowerAlpha));
        alphaMap[std::size_t(value)] = static_cast<std::uint8_t>((std::clamp)(mapped, 0, 255));
    }
    for (int value = (std::clamp)(upperIndex, 0, 256); value < 256; ++value)
        alphaMap[value] = static_cast<std::uint8_t>(upperAlpha);

    // 原11130先完成栈表；120DD/120F2再锁DI/SI，仅DI有展开保护，不按地址去重。
    return KaGuYa::Graphics::WithSurfaceTransferLocks(destination, source, [&]
    {
        if (destination.sourcePixelFormat != 7 && destination.sourcePixelFormat != 12)
            return destination.sourcePixelFormat == source.sourcePixelFormat;
        if (source.sourcePixelFormat != 7 && source.sourcePixelFormat != 12) return false;
        BlitLayout layout{};
        if (!ResolveBlitLayout(destination, source, destinationX, destinationY,
                destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, layout))
        {
            // 原 DLL 定向差分确认：空源区、空目标区或完全裁出均作为成功空操作返回。
            return true;
        }
        if (destination.sourcePixelFormat == 7)
        {
            return true;
        }

        const bool sourceHasAlpha = source.sourcePixelFormat == 12;
        const std::uint8_t opaqueMappedAlpha = alphaMap[255];
        const int sourceWidth = layout.source.right - layout.source.left;
        const int sourceHeight = layout.source.bottom - layout.source.top;
        const int destinationWidth = layout.destination.right - layout.destination.left;
        const int destinationHeight = layout.destination.bottom - layout.destination.top;
        const bool unscaled = sourceWidth == destinationWidth && sourceHeight == destinationHeight;
        BlitAxis vertical(layout.source.top, sourceHeight, destinationHeight,
            layout.clippedDestination.top - layout.destination.top);
        for (int destinationRow = layout.clippedDestination.top;
            destinationRow < layout.clippedDestination.bottom; ++destinationRow)
        {
            auto* destinationPixel = destination.pixels.data() +
                destinationRow * destination.pitch + layout.clippedDestination.left * 4;
            const int sourceY = unscaled ? layout.source.top + destinationRow - layout.destination.top
                : vertical.Next();
            BlitAxis horizontal(layout.source.left, sourceWidth, destinationWidth,
                layout.clippedDestination.left - layout.destination.left);
            for (int destinationColumn = layout.clippedDestination.left;
                destinationColumn < layout.clippedDestination.right;
                ++destinationColumn, destinationPixel += 4)
            {
                const int sourceX = unscaled ? layout.source.left + destinationColumn - layout.destination.left
                    : horizontal.Next();
                const auto* sourcePixel = source.pixels.data() + sourceY * source.pitch + sourceX * 4;
                // 操作码 20 的 1002E990 只写目标 alpha；RGB 来自目标 Surface，保持不变。
                destinationPixel[3] = sourceHasAlpha ? alphaMap[sourcePixel[3]] : opaqueMappedAlpha;
            }
        }
        return true;
    });
}

std::uint32_t SampleCenterPixel(const KaGuYa::Graphics::SurfacePixels& surface) noexcept
{
    if (surface.width <= 0 || surface.height <= 0 || surface.pixels.empty())
    {
        return 0;
    }
    const auto* pixel = surface.pixels.data() +
        (surface.height / 2) * surface.pitch + (surface.width / 2) * 4;
    return *reinterpret_cast<const std::uint32_t*>(pixel);
}

void TraceFrameOperation(const char* const operation,
    const KaGuYa::Graphics::SurfacePixels& source, const KaGuYa::Graphics::SurfacePixels& destination,
    const int sourceHandle, const int destinationHandle, const int result,
    const int destinationX, const int destinationY, const int destinationSizeAddress,
    const int sourceRectAddress, const int destinationClipAddress, const int anchor,
    const int globalAlpha = -1) noexcept
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if ((source.width != runtime.width || source.height != runtime.height) &&
        (destination.width != runtime.width || destination.height != runtime.height))
    {
        return;
    }
    BlitLayout layout{};
    const bool hasPixels = ResolveBlitLayout(destination, source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, layout);
    // 高频帧提交只写 Trace；默认 info 日志不得影响游戏时序。
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Trace,
        "%s: src=%08X(%dx%d,f%d,center=%08X) dst=%08X(%dx%d,f%d,center=%08X) "
        "dstPoint=(%d,%d) source={%d,%d,%d,%d} destination={%d,%d,%d,%d} "
        "clipped={%d,%d,%d,%d} hasPixels=%d anchor=%d alpha=%d result=%d.",
        operation, sourceHandle, source.width, source.height, source.sourcePixelFormat, SampleCenterPixel(source),
        destinationHandle, destination.width, destination.height, destination.sourcePixelFormat,
        SampleCenterPixel(destination), destinationX, destinationY,
        layout.source.left, layout.source.top, layout.source.right, layout.source.bottom,
        layout.destination.left, layout.destination.top, layout.destination.right, layout.destination.bottom,
        layout.clippedDestination.left, layout.clippedDestination.top,
        layout.clippedDestination.right, layout.clippedDestination.bottom,
        hasPixels ? 1 : 0, anchor, globalAlpha, result);
}

char BlitToScreen(const int sourceHandle, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor,
    const BlitMode mode, const char* const operation,
    const std::uint8_t globalAlpha = 255)
{
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceHandle);
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (source == nullptr || runtime.Screen() == nullptr)
    {
        return 0;
    }
    const char result = BlitSurface(*runtime.Screen(), *source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor,
        mode, globalAlpha) ? 1 : 0;
    TraceFrameOperation(operation, *source, *runtime.Screen(),
        sourceHandle, KaGuYa::Graphics::ToHandle(runtime.Screen()), result,
        destinationX, destinationY, destinationSizeAddress, sourceRectAddress,
        destinationClipAddress, anchor,
        mode == BlitMode::GlobalAlpha ? static_cast<int>(globalAlpha) : -1);
    return result;
}

bool AddSubSurfacePixels(KaGuYa::Graphics::SurfacePixels& destination,
    const KaGuYa::Graphics::SurfacePixels& control, const int rectAddress) noexcept
{
    Region region = ClampRegion(destination, rectAddress);
    region.right = (std::min)(region.right, control.width);
    region.bottom = (std::min)(region.bottom, control.height);
    if (IsEmpty(region))
    {
        return false;
    }

    for (int y = region.top; y < region.bottom; ++y)
    {
        auto* destinationPixel = destination.pixels.data() + y * destination.pitch + region.left * 4;
        const auto* controlPixel = control.pixels.data() + y * control.pitch + region.left * 4;
        for (int x = region.left; x < region.right; ++x, destinationPixel += 4, controlPixel += 4)
        {
            // 1000C850.c：control alpha 的 bit0/1/2 分别控制 R/G/B 减法；未置位则做饱和加法。
            for (int channel = 0; channel < 3; ++channel)
            {
                const int subtractBit = 1 << (2 - channel);
                const int value = (controlPixel[3] & subtractBit) != 0
                    ? static_cast<int>(destinationPixel[channel]) - controlPixel[channel]
                    : static_cast<int>(destinationPixel[channel]) + controlPixel[channel];
                destinationPixel[channel] = static_cast<std::uint8_t>((std::clamp)(value, 0, 255));
            }
        }
    }
    return true;
}


std::uint8_t Luminance(const std::uint8_t* const pixel) noexcept
{
    // sub_1000F770 建立三张 float 表，10009DE0/100082E0 按 B、G、R 相加后向零取整。
    const float blue = static_cast<float>(pixel[0]) * 0.1144800037145615f;
    const float green = static_cast<float>(pixel[1]) * 0.5866100192070007f;
    const float red = static_cast<float>(pixel[2]) * 0.2989099919795990f;
    return static_cast<std::uint8_t>(static_cast<int>(green + red + blue));
}

char ApplyClearSurface(KaGuYa::Graphics::SurfaceInfo& surface, const int rectAddress,
    const std::uint32_t color, const bool preserveAlpha)
{
    surface.clearColor = color;
    // 原1000FFB0先写颜色，100083C0再Lock；失败与异常不擅自补Unlock。
    if (surface.Lock() != 0) return 0;
    const bool destinationHasAlpha = surface.sourcePixelFormat == 12;
    TransformPixels(surface, rectAddress, [color, preserveAlpha, destinationHasAlpha](std::uint8_t* pixel)
    {
        const std::uint8_t alpha = pixel[3];
        pixel[0] = ColorByte(color, 0);
        pixel[1] = ColorByte(color, 1);
        pixel[2] = ColorByte(color, 2);
        pixel[3] = destinationHasAlpha
            ? (preserveAlpha ? alpha : ColorByte(color, 3))
            : 0;
    });
    surface.Unlock();
    return 1;
}

char ApplyClear(const int handle, const int rectAddress, const std::uint32_t color, const bool preserveAlpha)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    return surface ? ApplyClearSurface(*surface, rectAddress, color, preserveAlpha) : 0;
}

char ApplyColor(const int handle, const std::uint32_t color, const int rectAddress, const int operation)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    return TransformLockedPixels(*surface, rectAddress, [color, operation](std::uint8_t* pixel)
    {
        for (int channel = 0; channel < 3; ++channel)
        {
            const std::uint8_t operand = ColorByte(color, channel);
            if (operation == 2)
            {
                pixel[channel] = static_cast<std::uint8_t>((std::min)(255, static_cast<int>(pixel[channel]) + operand));
            }
            else if (operation == 3)
            {
                pixel[channel] = static_cast<std::uint8_t>((std::max)(0, static_cast<int>(pixel[channel]) - operand));
            }
            else
            {
                pixel[channel] = MultiplyByte(pixel[channel], operand);
            }
        }
    });
}

char ApplyMosaic(const int handle, const int blockSize, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr || blockSize <= 0)
    {
        return 0;
    }
    if (surface->Lock() != 0) return 0;
    const Region region = ClampRegion(*surface, rectAddress);
    for (int y = region.top; y < region.bottom; y += blockSize)
    {
        for (int x = region.left; x < region.right; x += blockSize)
        {
            const auto* source = surface->pixels.data() + y * surface->pitch + x * 4;
            const std::uint32_t color = *reinterpret_cast<const std::uint32_t*>(source);
            const int blockBottom = (std::min)(region.bottom, y + blockSize);
            const int blockRight = (std::min)(region.right, x + blockSize);
            for (int blockY = y; blockY < blockBottom; ++blockY)
            {
                auto* destination = reinterpret_cast<std::uint32_t*>(
                    surface->pixels.data() + blockY * surface->pitch + x * 4);
                std::fill(destination, destination + (blockRight - x), color);
            }
        }
    }
    surface->Unlock();
    return 1;
}

char ApplyFlip(const int handle, const int flags, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    if (surface->Lock() != 0) return 0;
    const Region region = ClampRegion(*surface, rectAddress);
    if ((flags & 1) != 0)
    {
        for (int y = region.top; y < region.bottom; ++y)
        {
            auto* row = reinterpret_cast<std::uint32_t*>(surface->pixels.data() + y * surface->pitch);
            std::reverse(row + region.left, row + region.right);
        }
    }
    if ((flags & 2) != 0)
    {
        const int width = region.right - region.left;
        for (int y = 0; y < (region.bottom - region.top) / 2; ++y)
        {
            auto* top = reinterpret_cast<std::uint32_t*>(
                surface->pixels.data() + (region.top + y) * surface->pitch) + region.left;
            auto* bottom = reinterpret_cast<std::uint32_t*>(
                surface->pixels.data() + (region.bottom - y - 1) * surface->pitch) + region.left;
            std::swap_ranges(top, top + width, bottom);
        }
    }
    surface->Unlock();
    return 1;
}

}

namespace KaGuYa::Graphics
{
int ClearTextWorkSurface(SurfaceInfo& surface)
{
    // 原100083F7..10008404：完整 EAX==0 才成功，颜色在 Lock 后读取，不能先捕获或写回。
    if (surface.Lock() != 0) return 1;
    const std::uint32_t color = surface.clearColor;
    if (surface.sourcePixelFormat == 7 || surface.sourcePixelFormat == 12)
    {
        if (surface.width > 0 && surface.height > 0)
        {
            const auto row = static_cast<std::int64_t>(surface.height - 1) * surface.pitch;
            const auto end = row + static_cast<std::int64_t>(surface.width) * 4;
            if (row < 0 || end < 0 || !surface.pixels.data() ||
                static_cast<std::uint64_t>(end) > surface.pixels.size())
            {
                Logging::Write(Logging::Level::Error, "文字工作区清区超出像素分配，明确拒绝原失效bits域；不补Unlock。");
                throw std::out_of_range("Text work clear outside allocation");
            }
        }
        ClearSurface(surface, color, false);
    }
    // 原1000844A/1000848F：正常尾忽略 Unlock 返回值；不支持格式也返回0，无异常补偿。
    surface.Unlock();
    return 0;
}

int SubmitTextSurface(SurfaceInfo& dst, SurfaceInfo& src, const int x, const int y, const bool blend)
{
    int result = 0;
    // 原100111EB/100111FD、10011A5C/10011A70：先锁目标再锁源，再检查格式；不按地址去重。
    const bool locked = WithSurfaceTransferLocks(dst, src, [&]
    {
        if (src.sourcePixelFormat != 12 ||
            (dst.sourcePixelFormat != 7 && dst.sourcePixelFormat != 12))
        {
            result = 2;
            return true;
        }
        // 原10010290参数为{&point,0,0,0,0}；mode0复制，mode10复用10026090/10010140合成。
        // 原10011356/10011B81正常尾返回0，不传播像素内核的空裁剪返回值。
        (void)BlitSurfacePixels(dst, src, x, y, 0, 0, 0, 0,
            blend ? BlitMode::SourceOver : BlitMode::Copy);
        return true;
    });
    return locked ? result : 1;
}

int CopyGdiSurface(SurfaceInfo& dst, SurfaceInfo& src, const RECT* clip)
{
    int result = 0;
    const bool locked = WithSurfaceTransferLocks(dst, src, [&]
    {
        if ((src.sourcePixelFormat != 7 && src.sourcePixelFormat != 12) ||
            (dst.sourcePixelFormat != 7 && dst.sourcePixelFormat != 12))
        {
            result = 2;
            return true;
        }
        // 原0739F/06426：dirty是目的clip，不是sourceRect，也不直接memcpy拒越界。
        (void)BlitSurfacePixels(dst, src, 0, 0, 0, 0,
            static_cast<int>(reinterpret_cast<std::intptr_t>(clip)), 0, BlitMode::Copy);
        return true;
    });
    return locked ? result : 1;
}
}

extern "C" char __cdecl SurfaceBlt(const int destinationHandle, const int sourceHandle,
    const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceHandle);
    if (source == nullptr || destination == nullptr || sourceHandle == destinationHandle)
    {
        return 0;
    }
    // 10003D70.c/1000E360.c：第一句柄提供提交目标，第二句柄构造源描述；
    // 普通 Blt 是否使用 alpha 由源 Surface 的格式决定。
    const BlitMode mode = source->sourcePixelFormat < 10 ? BlitMode::Copy : BlitMode::SourceOver;
    const char result = BlitSurface(*destination, *source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, mode) ? 1 : 0;
    TraceFrameOperation("SurfaceBlt", *source, *destination, sourceHandle, destinationHandle, result,
        destinationX, destinationY, destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor);
    return result;
}

extern "C" char __cdecl SurfaceCopyBlt(const int destinationHandle, const int sourceHandle,
    const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceHandle);
    if (source == nullptr || destination == nullptr || sourceHandle == destinationHandle)
    {
        return 0;
    }
    const char result = BlitSurface(*destination, *source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, BlitMode::Copy) ? 1 : 0;
    TraceFrameOperation("SurfaceCopyBlt", *source, *destination, sourceHandle, destinationHandle, result,
        destinationX, destinationY, destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor);
    return result;
}

extern "C" char __cdecl SurfaceClear(const int handle, const int color, const int rectAddress)
{
    return ApplyClear(handle, rectAddress, static_cast<std::uint32_t>(color), false);
}

extern "C" int __cdecl ClearScreen(const RECT* const rect)
{
    // 10003640 -> renderer +0x1C (10001CA0) -> operation 0：参数为 RECT*，
    // 颜色取屏幕描述对象 +0x24；成功返回 0，与 SurfaceClear 的 bool 包装不同。
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (runtime.Screen() == nullptr)
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "ClearScreen: screen is not initialized.");
        return 1;
    }
    return ApplyClearSurface(*runtime.Screen(),
        static_cast<int>(reinterpret_cast<std::intptr_t>(rect)),
        runtime.Screen()->clearColor, false) ? 0 : 1;
}

extern "C" char __cdecl SurfaceClearWithoutAlpha(const int handle, const int color, const int rectAddress)
{
    return ApplyClear(handle, rectAddress, static_cast<std::uint32_t>(color), true);
}

extern "C" char __cdecl SurfaceAddColor(const int handle, const int color, const int rectAddress)
{
    return ApplyColor(handle, static_cast<std::uint32_t>(color), rectAddress, 2);
}

extern "C" char __cdecl SurfaceSubColor(const int handle, const int color, const int rectAddress)
{
    return ApplyColor(handle, static_cast<std::uint32_t>(color), rectAddress, 3);
}

extern "C" char __cdecl SurfaceMulColor(const int handle, const int color, const int rectAddress)
{
    return ApplyColor(handle, static_cast<std::uint32_t>(color), rectAddress, 4);
}

extern "C" char __cdecl SurfaceFlush(const int handle, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    return TransformLockedPixels(*surface, rectAddress, [](std::uint8_t* pixel)
    {
        pixel[0] = static_cast<std::uint8_t>(~pixel[0]);
        pixel[1] = static_cast<std::uint8_t>(~pixel[1]);
        pixel[2] = static_cast<std::uint8_t>(~pixel[2]);
    });
}

extern "C" char __cdecl SurfaceMosaic(const int handle, const int blockSize, const int rectAddress)
{
    return ApplyMosaic(handle, blockSize, rectAddress);
}

extern "C" char __cdecl SurfaceMulAlpha(const int handle, const int color, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    const std::uint8_t factor = ColorByte(static_cast<std::uint32_t>(color), 3);
    return TransformLockedPixels(*surface, rectAddress, [factor](std::uint8_t* pixel)
    {
        pixel[3] = MultiplyByte(pixel[3], factor);
    });
}

extern "C" char __cdecl SurfaceFlip(const int handle, const int flags, const int rectAddress)
{
    return ApplyFlip(handle, flags, rectAddress);
}

extern "C" char __cdecl SurfaceFilter(const int handle, const int color, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    const std::uint32_t tint = static_cast<std::uint32_t>(color);
    return TransformLockedPixels(*surface, rectAddress, [tint](std::uint8_t* pixel)
    {
        const std::uint8_t luminance = Luminance(pixel);
        pixel[0] = MultiplyByte(luminance, ColorByte(tint, 0));
        pixel[1] = MultiplyByte(luminance, ColorByte(tint, 1));
        pixel[2] = MultiplyByte(luminance, ColorByte(tint, 2));
    });
}

extern "C" char __cdecl SurfaceFilter2(const int handle, const int color, const int rectAddress)
{
    auto* surface = KaGuYa::Graphics::ResolveSurface(handle);
    if (surface == nullptr)
    {
        return 0;
    }
    // 原1000884F先虚Lock、10008865再读format；不能让lambda捕获锁前的格式。
    if (surface->Lock() != 0) return 0;
    const std::uint32_t baseColor = static_cast<std::uint32_t>(color);
    const bool destinationHasAlpha = surface->sourcePixelFormat == 12;
    if (surface->sourcePixelFormat == 7 || surface->sourcePixelFormat == 12)
    {
        TransformPixels(*surface, rectAddress, [baseColor, destinationHasAlpha](std::uint8_t* pixel)
        {
            const std::uint8_t luminance = Luminance(pixel);
            for (int channel = 0; channel < 3; ++channel)
            {
                const std::uint8_t base = ColorByte(baseColor, channel);
                pixel[channel] = static_cast<std::uint8_t>(base + MultiplyByte(luminance, static_cast<std::uint8_t>(~base)));
            }
            pixel[3] = destinationHasAlpha ? 0xFF : 0;
        });
    }
    surface->Unlock();
    return 1;
}

extern "C" bool __cdecl SurfaceAddSubSurface(const int destinationHandle, const int controlHandle,
    const int rectAddress)
{
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    const auto* control = KaGuYa::Graphics::ResolveSurface(controlHandle);
    if (destination == nullptr || control == nullptr || destination == control)
    {
        return false;
    }
    return AddSubSurfacePixels(*destination, *control, rectAddress);
}
extern "C" char __cdecl SurfaceBackup(const int destinationHandle, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    if (destination == nullptr || runtime.Screen() == nullptr)
    {
        return 0;
    }
    // 10003F70.c：SurfaceBackup 固定以当前 screen 为源，操作码 0 回读到目标 Surface。
    return BlitSurface(*destination, *runtime.Screen(), destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, BlitMode::Copy) ? 1 : 0;
}
extern "C" char __cdecl SurfaceBlendBlt(const int destinationHandle, const int sourceHandle,
    const int destinationX, const int destinationY, const char alpha,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceHandle);
    if (source == nullptr || destination == nullptr || sourceHandle == destinationHandle)
    {
        return 0;
    }
    const char result = BlitSurface(*destination, *source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor,
        BlitMode::GlobalAlpha, static_cast<std::uint8_t>(alpha)) ? 1 : 0;
    TraceFrameOperation("SurfaceBlendBlt", *source, *destination,
        sourceHandle, destinationHandle, result, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor,
        static_cast<std::uint8_t>(alpha));
    return result;
}

extern "C" char __cdecl SurfaceBlendDraw(const int sourceHandle, const int destinationX, const int destinationY,
    const int alpha, const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    return BlitToScreen(sourceHandle, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor,
        BlitMode::GlobalAlpha, "SurfaceBlendDraw", static_cast<std::uint8_t>(alpha));
}
extern "C" bool __cdecl SurfaceBlur(const int destinationHandle, const int sourceHandle,
    const int radius, const int rectAddress)
{
    auto* destination = KaGuYa::Graphics::ResolveSurfaceBody(destinationHandle);
    auto* source = KaGuYa::Graphics::ResolveSurfaceBody(sourceHandle);
    if (source == nullptr || destination == nullptr)
    {
        return false;
    }
    return KaGuYa::Graphics::BoxBlurSurface(*destination, *source, radius, rectAddress);
}
extern "C" char __cdecl SurfaceCopyDraw(const int sourceHandle, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    return BlitToScreen(sourceHandle, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor,
        BlitMode::Copy, "SurfaceCopyDraw");
}

extern "C" char __cdecl SurfaceDraw(const int sourceHandle, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    auto* source = KaGuYa::Graphics::ResolveSurface(sourceHandle);
    if (source == nullptr || runtime.Screen() == nullptr)
    {
        return 0;
    }
    const BlitMode mode = source->sourcePixelFormat < 10
        ? BlitMode::Copy
        : BlitMode::SourceOver;
    const char result = BlitSurface(*runtime.Screen(), *source, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor, mode) ? 1 : 0;
    TraceFrameOperation("SurfaceDraw", *source, *runtime.Screen(),
        sourceHandle, KaGuYa::Graphics::ToHandle(runtime.Screen()), result,
        destinationX, destinationY, destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor);
    return result;
}
extern "C" bool __cdecl SurfaceGaussBlur(const int destinationHandle, const int sourceHandle,
    const int radius, const int rectAddress)
{
    auto* destination = KaGuYa::Graphics::ResolveSurfaceBody(destinationHandle);
    auto* source = KaGuYa::Graphics::ResolveSurfaceBody(sourceHandle);
    if (source == nullptr || destination == nullptr)
    {
        return false;
    }
    return KaGuYa::Graphics::GaussianBlurSurface(*destination, *source, radius, rectAddress);
}
extern "C" int __cdecl SurfaceUniversalBlt(const int destinationHandle, const int sourceHandle,
    const int mappingParametersAddress, const int destinationX, const int destinationY,
    const int destinationSizeAddress, const int sourceRectAddress,
    const int destinationClipAddress, const int anchor)
{
    if (destinationHandle == sourceHandle) return 0; // 原1000E4F7拒绝的是handle。
    auto* destinationBody = KaGuYa::Graphics::ResolveSurfaceBody(destinationHandle);
    auto* sourceBody = KaGuYa::Graphics::ResolveSurfaceBody(sourceHandle);
    const auto* mappingParameters = reinterpret_cast<const int*>(
        static_cast<std::intptr_t>(mappingParametersAddress));
    if (!destinationBody || !sourceBody)
    {
        return 0;
    }
    // 原E522/E530两次source.+54，E572取SI后由D.+04/6830取DI。
    auto* formatInfo = sourceBody->info.get();
    if (!formatInfo || formatInfo->sourcePixelFormat < 10) return 0;
    formatInfo = sourceBody->info.get();
    if (!formatInfo || formatInfo->sourcePixelFormat < 10) return 0;
    auto* source = sourceBody->info.get();
    auto* destination = destinationBody->info.get();
    if (!source || !destination) return 0;
    const int result = RemapAlphaSurface(*destination, *source, mappingParameters, destinationX, destinationY,
        destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor) ? 1 : 0;
    TraceFrameOperation("SurfaceUniversalBlt", *source, *destination,
        sourceHandle, destinationHandle, result,
        destinationX, destinationY, destinationSizeAddress, sourceRectAddress, destinationClipAddress, anchor);
    return result;
}
