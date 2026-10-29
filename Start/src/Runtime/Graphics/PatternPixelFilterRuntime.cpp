#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/PatternFilterNative.h>

#include <bit>
#include <cstddef>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireArgumentCount(const std::span<const std::uint32_t> args,
    const std::size_t count, const std::int32_t mode)
{
    if (args.size() != count)
        throw mode;
}

[[nodiscard]] std::int32_t SignedWord(const std::uint32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(value);
}

template<class T>
[[nodiscard]] T ReadNative(const T& value) noexcept
{
    // 原逐DWORD读点也是非法内存的故障顺序，不能把后续读取提升到资格检查之前。
    return static_cast<const volatile T&>(value);
}

[[nodiscard]] std::uint32_t ReadArgument(const std::uint32_t* words,
    std::size_t index, bool native) noexcept
{
    return native ? ReadNative(words[index]) : words[index];
}

[[nodiscard]] std::uint32_t SignedColorPart(
    const std::uint32_t* words, const bool negative, const bool native) noexcept
{
    std::uint32_t color = 0;
    for (std::size_t index = 0; index != 3; ++index)
    {
        const auto word = ReadArgument(words, index, native);
        const bool isNegative = (word & 0x80000000u) != 0;
        // 442D4B..442DCF：按 DWORD 符号分组，再截低字节；负数用 neg al。
        // 无符号减法也精确覆盖 INT_MIN，避免 C++ 有符号取负溢出。
        const std::uint32_t channel = isNegative == negative
            ? (negative ? 0u - word : word) & 0xFFu : 0u;
        color = (color << 8u) | channel;
    }
    return color;
}

[[nodiscard]] std::uintptr_t TemporaryAddressAt444770(const YtlReferenceSlotRuntime& slot) noexcept
{
    if (!ReadNative(slot.baseAt00)) return 0;
    const auto* const control = ReadNative(slot.controlAt08);
    const auto stride = control ?
        ReadNative(static_cast<const EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    const auto bias = ReadNative(slot.biasAt04);
    return ReadNative(slot.baseAt00) + bias * stride;
}


}

namespace
{
bool ApplyPixelFilter(
    IGraphicsPatternFilterCalls& graphics,
    const std::int32_t explicitMode,
    const std::span<const std::uint32_t> args,
    const GraphicsSurfaceNativeBody* const destination,
    const GraphicsSurfaceNativeBody* const source,
    const GraphicsSurfaceNativeBody* const temporary,
    const RECT* const clip, const PatternFilterNativeBody20h* const receiver)
{
    try
    {
        const auto mode = receiver ? SignedWord(ReadNative(receiver->modeAt04)) : explicitMode;
        const bool native = receiver != nullptr;
        const auto arguments = [&](std::size_t count) -> const std::uint32_t*
        {
            if (!receiver)
            {
                RequireArgumentCount(args, count, mode);
                return args.data();
            }
            const auto* const end = ReadNative(receiver->argumentsAt08.end);
            const auto* const begin = ReadNative(receiver->argumentsAt08.begin);
            const auto bytes = reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin);
            // 原sub/sar按32位有符号差计算；不为native三指针构造带范围前提的span。
            if ((SignedWord(bytes) >> 2) != static_cast<std::int32_t>(count)) throw mode;
            return begin;
        };
        const auto destinationHandle = [&]
        {
            // 原在各分支直接读取对象+4；现代显式失败保留参数预检和模块查询前缀。
            if (!receiver && !destination) throw std::invalid_argument("Pattern filter destination is null");
            return receiver ? ReadNative(destination->handleAt04) : destination->Handle();
        };
        switch (mode)
        {
        case 1:
        case 8:
        case 9:
        {
            const auto* const words = arguments(3u);
            auto color = ReadArgument(words, 0, native) << 8u;
            color |= ReadArgument(words, 1, native);
            color <<= 8u;
            GraphicsSurfaceHandle target;
            if (native)
            {
                // 44296B/442A4A/4432A1：目标+4夹在参数1和参数2之间。
                target = destinationHandle();
                color |= ReadArgument(words, 2, true);
            }
            else
            {
                color |= ReadArgument(words, 2, false);
                target = destinationHandle();
            }
            if (mode == 1) graphics.SurfaceFilterIfLoaded(target, color, clip);
            else if (mode == 8) graphics.SurfaceMulColorIfLoaded(target, color, clip);
            else graphics.SurfaceFilter2IfLoaded(target, color, clip);
            break;
        }
        case 2:
        {
            // 442ABB：无参数数量检查，不借用任何额外 source。
            const auto target = destinationHandle();
            graphics.SurfaceFlushIfLoaded(target, clip);
            break;
        }
        case 3:
        {
            const auto* const words = arguments(2u);
            // 442B86..442B9F：检查 source 对象而非 handle，子模式只接受 0/1。
            if (source == nullptr) throw mode;
            const auto submode = ReadArgument(words, 0, native);
            if (submode != 0u && submode != 1u) throw mode;
            const auto sourceHandle = native ? ReadNative(source->handleAt04) : source->Handle();
            const auto target = destinationHandle();
            const auto radius = SignedWord(ReadArgument(words, 1, native));
            if (submode == 0u)
                graphics.SurfaceBlurIfLoaded(target, sourceHandle, radius, clip);
            else
                graphics.SurfaceGaussBlurIfLoaded(target, sourceHandle, radius, clip);
            break;
        }
        case 4:
        {
            const auto* const words = arguments(3u);
            const std::uint32_t add = SignedColorPart(words, false, native);
            if (add != 0u)
            {
                const auto target = destinationHandle();
                graphics.SurfaceAddColorIfLoaded(target, add, clip);
            }

            // 原函数在 AddColor 返回后才重读参数计算 SubColor；异常不回滚加色。
            // 442D96重取begin，三个通道固定读取；回调修改end不触发第二次长度检查。
            const auto* const current = receiver ? ReadNative(receiver->argumentsAt08.begin) : words;
            const std::uint32_t subtract = SignedColorPart(current, true, native);
            if (subtract != 0u)
            {
                const auto target = destinationHandle();
                graphics.SurfaceSubColorIfLoaded(target, subtract, clip);
            }
            break;
        }
        case 5:
        {
            const auto* const words = arguments(1u);
            const auto value = ReadArgument(words, 0, native);
            const auto target = destinationHandle();
            // 442F96先分支，再4182F0取根；>=255画到screen，不是CopyBlt。
            if (SignedWord(value) >= 255)
                graphics.SurfaceCopyDrawIfLoaded(target, 0, 0,
                    nullptr, nullptr, clip, 0);
            else if (value == 0u)
                graphics.SurfaceClearIfLoaded(target, 0u, clip);
            else
                graphics.SurfaceMulAlphaIfLoaded(target, value << 24u, clip);
            break;
        }
        case 6:
        {
            // 443135 只要求 Pattern+14 的 ref 非空，不检查 args 或临时图 handle。
            if (receiver ? ReadNative(receiver->temporaryAt14.baseAt00) == 0 : temporary == nullptr)
                throw mode;
            const auto target = destinationHandle();
            // 443147先读目标句柄，44314A再解Ref；base非零而地址回绕为0仍传给导出。
            const auto address = receiver ? TemporaryAddressAt444770(receiver->temporaryAt14) :
                reinterpret_cast<std::uintptr_t>(temporary);
            // 4431E9传对象地址，不取+4；原Graphics注册表未命中返回false，调用者忽略。
            graphics.SurfaceAddSubSurfaceIfLoaded(target, address, clip);
            break;
        }
        case 7:
        {
            const auto* const words = arguments(1u);
            const auto target = destinationHandle();
            const auto value = SignedWord(ReadArgument(words, 0, native));
            graphics.SurfaceMosaicIfLoaded(target, value, clip);
            break;
        }
        case 11:
        {
            const auto* const words = arguments(1u);
            const auto value = SignedWord(ReadArgument(words, 0, native));
            const auto target = destinationHandle();
            graphics.SurfaceFlipIfLoaded(target, value, clip);
            break;
        }
        default:
            // 442938..442942 的无符号范围判断及 mode10 均落到 442A14: AL=1。
            break;
        }
        return true;
    }
    catch (...)
    {
        // PE:5E40C4 -> try5E40B0(states0..12) -> handler5E40A0(flags40,type0)。
        // 44336D 跳 443373: xor al,al；不是只 catch int，也不恢复已写像素。
        // 这里不额外捕获/转换 Win32 SEH，非法宿主内存不是可解析输入。
        return false;
    }
}

}

bool ApplyPatternPixelFilterNativeAt442900(IGraphicsPatternFilterCalls& graphics,
    std::int32_t mode, std::span<const std::uint32_t> args,
    const GraphicsSurfaceNativeBody* destination, const GraphicsSurfaceNativeBody* source,
    const GraphicsSurfaceNativeBody* temporary, const RECT* clip)
{
    return ApplyPixelFilter(graphics, mode, args, destination, source, temporary, clip, nullptr);
}
bool ApplyPatternPixelFilterNativeAt442900(IGraphicsPatternFilterCalls& graphics,
    const PatternFilterNativeBody20h& filter, const GraphicsSurfaceNativeBody* destination,
    const GraphicsSurfaceNativeBody* source, const RECT* clip)
{
    return ApplyPixelFilter(graphics, 0, {}, destination, source, nullptr, clip, &filter);
}

bool ApplyPatternPixelFilterAt442900(IGraphicsPatternFilterApi& graphics,
    const std::int32_t mode, const std::span<const std::uint32_t> args,
    const RuntimeGraphicsSurface* const destination, const RuntimeGraphicsSurface* const source,
    const RuntimeGraphicsSurface* const temporary, const RECT* const clip)
{
    return ApplyPatternPixelFilterNativeAt442900(graphics, mode, args,
        destination ? &destination->NativeBody() : nullptr,
        source ? &source->NativeBody() : nullptr,
        temporary ? &temporary->NativeBody() : nullptr, clip);
}

bool ApplyPatternPixelFilterAt442900(IGraphicsPatternFilterApi& graphics,
    const std::int32_t mode, const std::span<const std::uint32_t> args,
    const RuntimeGraphicsSurface& destination, const RuntimeGraphicsSurface* const source,
    const RuntimeGraphicsSurface* const temporary, const RECT* const clip)
{
    return ApplyPatternPixelFilterAt442900(graphics, mode, args, &destination, source, temporary, clip);
}


}
