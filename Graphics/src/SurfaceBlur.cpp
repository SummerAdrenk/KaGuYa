#include "SurfaceBlur.h"
#include "Surface.h"
#include "NativeArrayReference.h"
#include <Logging/Logger.h>
#include <Windows.h>
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{
[[noreturn]] void Reject(const char* reason)
{
    Logging::WriteFormat(Logging::Level::Error, "SurfaceBlur: 拒绝原危险域：%s", reason);
    throw std::out_of_range(reason);
}

SurfaceInfo& Info(Surface& surface)
{
    auto* result = surface.info.get();
    if (!result) Reject("missing Info");
    return *result;
}

RECT Region(const SurfaceInfo& source, const int address)
{
    RECT rect;
    if (address) rect = *reinterpret_cast<const RECT*>(static_cast<std::intptr_t>(address));
    else
    {
        // 原1002F356忽略BOOL，返回后F374/F379重新读尺寸；调用异常按外层当前EH状态展开。
        (void)::SetRect(&rect, 0, 0, source.width, source.height);
    }
    const int width = source.width, height = source.height;
    if (width < 0 || height < 0) Reject("negative source dimensions");
    // 原1002F330：完全在轴外时该轴两端置0，不把反向矩形擅自排序。
    if (width < rect.left || rect.right < 0) rect.left = rect.right = 0;
    else { rect.left = (std::max)(0L, rect.left); rect.right = (std::min)(LONG(width), rect.right); }
    if (height < rect.top || rect.bottom < 0) rect.top = rect.bottom = 0;
    else { rect.top = (std::max)(0L, rect.top); rect.bottom = (std::min)(LONG(height), rect.bottom); }
    return rect;
}
bool Empty(const RECT& r) noexcept { return r.right <= r.left || r.bottom <= r.top; }

std::uint32_t ArrayCount(const std::int64_t count, const unsigned stride)
{
    if (count <= 0 || count > INT32_MAX || std::uint64_t(count) * stride > UINT32_MAX)
        Reject("temporary array size overflow");
    return static_cast<std::uint32_t>(count);
}

void CheckPlane(const SurfaceInfo& owner, const int pitch, const RECT& r)
{
    if (r.left < 0 || r.top < 0 || r.right > owner.width || r.bottom > owner.height ||
        pitch < 0 || std::int64_t(pitch) < std::int64_t(r.right) * 4)
        Reject("destination too small or invalid pitch");
    if (!Empty(r))
    {
        const auto end = std::int64_t(r.bottom - 1) * pitch + std::int64_t(r.right) * 4;
        if (!owner.pixels.data() || end < 0 || std::uint64_t(end) > owner.pixels.size())
            Reject("pixel access outside allocation");
    }
}

struct Sample { std::uint32_t value = 0; bool written = false; };
int ReadPixel(SurfaceInfo& source, const int x, const int y, Sample& sample)
{
    // 原1002F590：逐次虚Lock，失败返回0且不写out，不补Unlock；无激活的异常Unlock状态。
    if (source.Lock() != 0)
    {
        if (!sample.written) Reject("first sample left uninitialized by failed Lock");
        return 0;
    }
    if (x < 0 || y < 0 || x >= source.width || y >= source.height)
    {
        source.Unlock();
        return -1;
    }
    if (source.sourcePixelFormat != 7 && source.sourcePixelFormat != 12)
    {
        sample.value = 0;
        sample.written = true;
        source.Unlock();
        return -2;
    }
    const RECT pixel{x, y, x + 1, y + 1};
    CheckPlane(source, source.pitch, pixel);
    std::memcpy(&sample.value, source.pixels.data() + std::size_t(y) * source.pitch + std::size_t(x) * 4, 4);
    if (source.sourcePixelFormat == 7) sample.value &= 0x00FFFFFFu; // 原F66F，X不是alpha=255。
    sample.written = true;
    source.Unlock();
    return 0;
}

struct Column { std::uint32_t count, alpha, red, green, blue; };
struct Intermediate { std::uint32_t alpha, red, green, blue; };
static_assert(sizeof(Column) == 0x14 && sizeof(Intermediate) == 0x10);

void Accumulate(Column& c, const std::uint32_t pixel, const bool alpha, const bool remove) noexcept
{
    const std::uint32_t sign = remove ? UINT32_MAX : 1u;
    const std::uint32_t a = alpha ? pixel >> 24 : 1u;
    c.count += sign;
    if (alpha) c.alpha += sign * a;
    c.red += sign * a * ((pixel >> 16) & 255u);
    c.green += sign * a * ((pixel >> 8) & 255u);
    c.blue += sign * a * (pixel & 255u);
}
void Accumulate(Column& a, const Column& b, const bool remove) noexcept
{
    const std::uint32_t sign = remove ? UINT32_MAX : 1u;
    a.count += sign * b.count; a.alpha += sign * b.alpha;
    a.red += sign * b.red; a.green += sign * b.green; a.blue += sign * b.blue;
}
std::uint32_t SignedDivide(const std::uint32_t a, const std::uint32_t b)
{
    const auto n = std::bit_cast<std::int32_t>(a), d = std::bit_cast<std::int32_t>(b);
    if (!d || (n == INT32_MIN && d == -1)) Reject("box IDIV undefined");
    return static_cast<std::uint32_t>(n / d);
}
std::uint32_t Pack(const std::uint32_t a, const std::uint32_t r,
    const std::uint32_t g, const std::uint32_t b) noexcept
{ return (a << 24) | ((r & 255u) << 16) | ((g & 255u) << 8) | (b & 255u); }
std::uint32_t BoxPixel(const Column& c, const bool alpha)
{
    // 原B83E/B847使用有符号IDIV；累计在32位回绕，不用uint64改变正常历史算术。
    if (alpha && c.alpha == 0) return 0;
    const auto d = alpha ? c.alpha : c.count;
    return Pack(alpha ? SignedDivide(c.alpha, c.count) : 0,
        SignedDivide(c.red, d), SignedDivide(c.green, d), SignedDivide(c.blue, d));
}
void Store(std::uint8_t* bits, const int pitch, const int x, const int y, const std::uint32_t pixel)
{ std::memcpy(bits + std::size_t(y) * pitch + std::size_t(x) * 4, &pixel, 4); }

void Normalize(Intermediate& p, const std::uint32_t weight, const bool alpha)
{
    if (alpha && !p.alpha) { p.red = p.green = p.blue = 0; return; }
    const auto divisor = alpha ? p.alpha : weight;
    if (!divisor || (alpha && !weight)) Reject("gaussian DIV undefined");
    p.red /= divisor; p.green /= divisor; p.blue /= divisor;
    if (alpha) p.alpha /= weight;
}
}

bool BoxBlurSurface(Surface& destination, Surface& source, const int radius, const int rectAddress)
{
    if (radius == 0) return false;
    if (radius < 0 || radius > INT32_MAX / 2) Reject("negative or overflowing box radius");
    auto& input = Info(source); // 原B134捕获SI，后续取样始终用这个Info。
    const RECT region = Region(input, rectAddress);
    if (Empty(region)) return true;
    NativeArrayReference<Column> columns; // 原B17E state0在分配前激活。
    const RECT full = Region(input, 0); // 原B185仍调用2F330，state0只保护尚为空的列Ref。
    const int width = full.right - full.left;
    if (region.right > width) Reject("source range changed before box allocation");
    if (std::int64_t(region.bottom) + radius > INT32_MAX) Reject("box coordinate overflow");
    const auto count = ArrayCount(std::int64_t(width) + 2ll * radius, sizeof(Column));
    columns.Allocate(count, true);
    const int pitch = Info(destination).pitch; // 原B1A5/B1B5/B1C5，不能提前到分配前。
    auto& output = Info(destination);
    auto* const bits = output.pixels.data();
    const bool alpha = Info(destination).sourcePixelFormat >= 10;
    CheckPlane(output, pitch, region);
    auto column = [&](const int x) -> Column& { return columns[std::size_t(x + radius)]; };
    Sample sample;
    for (int x = region.left - radius; x < region.right + radius; ++x)
        for (int y = region.top - radius; y <= region.top + radius; ++y)
            if (ReadPixel(input, x, y, sample) == 0) Accumulate(column(x), sample.value, alpha, false);

    for (int y = region.top; y < region.bottom; ++y)
    {
        const auto advance = [&](const int x)
        {
            if (ReadPixel(input, x, y - radius, sample) == 0) Accumulate(column(x), sample.value, alpha, true);
            if (ReadPixel(input, x, y + radius + 1, sample) == 0) Accumulate(column(x), sample.value, alpha, false);
        };
        Column sum{};
        for (int x = region.left - radius; x < region.left + radius; ++x) Accumulate(sum, column(x), false);
        for (int x = region.left; x < region.right; ++x)
        {
            Accumulate(sum, column(x + radius), false);
            Store(bits, pitch, x, y, BoxPixel(sum, alpha));
            Accumulate(sum, column(x - radius), true);
            advance(x - radius); // 原B3F3/B87F先写再取样；同图必须读到已写的值。
        }
        for (int x = region.right - radius; x < region.right + radius; ++x)
            advance(alpha ? x : region.right - radius); // 原B53F/B59C：格式7尾部不推进列。
    }
    columns.Retire();
    return true;
}

bool GaussianBlurSurface(Surface& destination, Surface& source, const int radius, const int rectAddress)
{
    if (radius == 0) return false;
    // 原BC11/BC2A先32位平方再fild；不把溢出半径转换成另一套double核。
    if (radius < 0 || radius > 46340 / 3) Reject("negative or overflowing gaussian radius");
    const int kernel = radius * 3;
    NativeArrayReference<std::uint32_t> weights;
    weights.Allocate(ArrayCount(std::int64_t(kernel) + 1, 4), false);
    for (int distance = 0; distance <= kernel; ++distance)
        weights[distance] = static_cast<std::uint32_t>(std::exp(double(distance * distance) /
            double(-2 * radius * radius)) * 64.0);

    auto& input = Info(source); // 原BC97在权重创建/填充以后才取SI，即使RECT为空也一样。
    const RECT region = Region(input, rectAddress);
    if (Empty(region)) { weights.Retire(); return true; }
    // 原BD13在state0(仅权重)内查询全源；BD53之后才激活中间图state1，BD69才读目的format。
    const RECT full = Region(input, 0);
    const int width = full.right - full.left, height = full.bottom - full.top;
    if (region.right > width || region.bottom > height) Reject("source range changed before gaussian allocation");
    if (std::int64_t(width) + kernel > INT32_MAX || std::int64_t(height) + kernel > INT32_MAX)
        Reject("gaussian coordinate overflow");
    NativeArrayReference<Intermediate> intermediate; // 原BD53 state1。
    intermediate.Allocate(ArrayCount(std::int64_t(width) * height, sizeof(Intermediate)), true);
    const bool alpha = Info(destination).sourcePixelFormat >= 10; // 原BD69。
    Sample sample;
    const int top = (std::max)(0, int(region.top) - kernel);
    const int bottom = (std::min)(height, int(region.bottom) + kernel);
    for (int y = top; y < bottom; ++y)
        for (int x = region.left; x < region.right; ++x)
        {
            auto& pixel = intermediate[std::size_t(y) * width + x];
            std::uint32_t weight = 0;
            for (int sx = (std::max)(0, x - kernel); sx <= (std::min)(width - 1, x + kernel); ++sx)
            {
                // 原BE2E/C4DC不测试返回码；保留同一sample槽，不补空像素。
                (void)ReadPixel(input, sx, y, sample);
                if (!sample.written) Reject("gaussian sample never initialized");
                const auto w = weights[std::abs(sx - x)];
                const auto a = alpha ? (sample.value >> 24) * w : w;
                weight += w;
                if (alpha) pixel.alpha += a;
                pixel.red += a * ((sample.value >> 16) & 255u);
                pixel.green += a * ((sample.value >> 8) & 255u);
                pixel.blue += a * (sample.value & 255u);
            }
            Normalize(pixel, weight, alpha);
        }

    const int pitch = Info(destination).pitch; // 原C0AB/C0BB或C5E0/C5F0，水平取样完毕才查询。
    auto& output = Info(destination);
    auto* const bits = output.pixels.data();
    CheckPlane(output, pitch, region);
    for (int y = region.top; y < region.bottom; ++y)
        for (int x = region.left; x < region.right; ++x)
        {
            Intermediate sum{};
            std::uint32_t weight = 0;
            for (int sy = (std::max)(0, y - kernel); sy <= (std::min)(height - 1, y + kernel); ++sy)
            {
                const auto& pixel = intermediate[std::size_t(sy) * width + x];
                const auto w = weights[std::abs(sy - y)];
                const auto a = alpha ? pixel.alpha * w : w;
                weight += w;
                if (alpha) sum.alpha += a;
                sum.red += a * pixel.red; sum.green += a * pixel.green; sum.blue += a * pixel.blue;
            }
            Normalize(sum, weight, alpha);
            Store(bits, pitch, x, y, Pack(sum.alpha, sum.red, sum.green, sum.blue));
        }
    intermediate.Retire();
    weights.Retire();
    return true;
}
}
