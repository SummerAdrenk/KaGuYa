#include "TexturePixelConversion.h"

#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>

namespace KaGuYa::RenderDX
{
namespace
{
struct PixelLayout
{
    D3DFORMAT format;
    std::uint8_t bytes;
    std::uint8_t redBits;
    std::uint8_t greenBits;
    std::uint8_t blueBits;
    std::uint8_t redShift;
    std::uint8_t greenShift;
    std::uint8_t blueShift;
    std::uint64_t opaqueAlpha;
};

// 原 100137CB 构造分派、1003F580..1003F678 虚表及各 write 的移位/整字写入。
// 10011446 读取源 XRGB 时固定 A=1；目标 X 位不从源继承，只有 22 走原样复制。
// 34 的 write=1000D772 只取 R/G，不能擅自补 B 或改为灰度。
constexpr std::array<PixelLayout, 15> kLayouts = {{
    // format                  bytes  R   G   B   Rsh Gsh Bsh  alpha
    {D3DFMT_A8R8G8B8,             4,  8,  8,  8,  16,  8,  0, 0xFF000000ULL}, // 1000ADA4
    {D3DFMT_X8R8G8B8,             4,  8,  8,  8,  16,  8,  0, 0},             // 100064B8
    {D3DFMT_R5G6B5,               2,  5,  6,  5,  11,  5,  0, 0},             // 1000B469
    {D3DFMT_X1R5G5B5,             2,  5,  5,  5,  10,  5,  0, 0},             // 1000B76C
    {D3DFMT_A1R5G5B5,             2,  5,  5,  5,  10,  5,  0, 0x8000ULL},     // 1000BA6B
    {D3DFMT_A4R4G4B4,             2,  4,  4,  4,   8,  4,  0, 0xF000ULL},     // 1000BDFB
    {D3DFMT_R3G3B2,               1,  3,  3,  2,   5,  2,  0, 0},             // 1000C18B
    {D3DFMT_A8R3G3B2,             2,  3,  3,  2,   5,  2,  0, 0xFF00ULL},     // 1000C661
    {D3DFMT_X4R4G4B4,             2,  4,  4,  4,   8,  4,  0, 0},             // 1000C9F9
    {D3DFMT_A2B10G10R10,          4, 10, 10, 10,   0, 10, 20, 0xC0000000ULL}, // 1000CCF8
    {D3DFMT_A8B8G8R8,             4,  8,  8,  8,   0,  8, 16, 0xFF000000ULL}, // 1000D0AD
    {D3DFMT_X8B8G8R8,             4,  8,  8,  8,   0,  8, 16, 0},             // 1000D45C
    {D3DFMT_G16R16,               4, 16, 16,  0,   0, 16,  0, 0},             // 1000D772
    {D3DFMT_A2R10G10B10,          4, 10, 10, 10,  20, 10,  0, 0xC0000000ULL}, // 1000D9E7
    {D3DFMT_A16B16G16R16,         8, 16, 16, 16,   0, 16, 32, 0xFFFF000000000000ULL}, // 1000DD9C
}};

const PixelLayout* FindLayout(D3DFORMAT format) noexcept
{
    for (const auto& layout : kLayouts)
    {
        if (layout.format == format)
        {
            return &layout;
        }
    }
    return nullptr;
}

std::uint32_t Quantize(std::uint8_t channel, std::uint8_t bits) noexcept
{
    // 原 read: float(c * float[1003F4C4])；write: 乘满量程、加 0.5、向零取整。
    // 仅在本表的 8 位源域成立的整数等价式，误差界见证据文档；不是高位截断。
    // n=10 不能用 (c<<2)|(c>>6)；n=16 的结果恰为 c*257；n=0 丢弃该通道。
    const std::uint32_t maximum = (std::uint32_t{1} << bits) - 1u;
    return (static_cast<std::uint32_t>(channel) * maximum + 127u) / 255u;
}

struct FloatRgba
{
    float red;
    float green;
    float blue;
    float alpha;
};
static_assert(sizeof(FloatRgba) == 16);
static_assert(std::is_trivially_destructible_v<FloatRgba>);

struct FormatView;

struct FormatOperations
{
    void (*destroy)(FormatView*) noexcept;
    void (*readRow)(const FormatView&, std::uint32_t, FloatRgba*) noexcept;
    void (*writeRow)(FormatView&, std::uint32_t, const FloatRgba*) noexcept;
    const PixelLayout* layout;
};

constexpr auto kRoundingBias = []
{
    std::array<float, 32> values{};
    for (auto& value : values) value = 0.5f;
    return values;
}();

struct PixelBox
{
    std::uint32_t left;
    std::uint32_t top;
    std::uint32_t right;
    std::uint32_t bottom;
    std::uint32_t front;
    std::uint32_t back;
};

// 1000A299 的 0x80 字节视图。目的filter=1明确清零可选模式；
// 原04833遗漏源描述+40/+44初始化，本版明确置零，不复制垃圾标志触发的无用分配。
struct FormatView
{
    const FormatOperations* operations;
    D3DFORMAT format;
    std::uint32_t colorClass;
    std::uint32_t byteAddressable;
    std::uint32_t srgb;
    std::uint32_t pairedSrgb;
    std::uint32_t colorKey;
    std::uint32_t indexed;
    const std::uint8_t* pixels;
    FloatRgba keyColor;
    const float* roundingBias;
    FloatRgba* palette;
    PixelBox box;
    std::uint32_t sourceClass;
    FloatRgba* transformRow;
    FloatRgba* diffusionRow;
    std::uint32_t pitch;
    std::uint32_t slicePitch;
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t depth;
    std::uint32_t rowBytes;
    std::uint32_t pixelBytes;
    std::uint32_t savedControlWord;

    FormatView(const FormatOperations& ops, const std::uint8_t* data,
        std::uint32_t rowPitch, std::uint32_t regionWidth, std::uint32_t regionHeight) noexcept
        : operations(&ops), format(ops.layout->format), colorClass(1), byteAddressable(1),
          srgb(0), pairedSrgb(0), colorKey(0), indexed(0), pixels(data), keyColor{},
          roundingBias(kRoundingBias.data()), palette(nullptr),
          box{0, 0, regionWidth, regionHeight, 0, 1}, sourceClass(0),
          transformRow(nullptr), diffusionRow(nullptr), pitch(rowPitch), slicePitch(0),
          width(box.right - box.left), height(box.bottom - box.top), depth(box.back - box.front),
          rowBytes(width * ops.layout->bytes), pixelBytes(ops.layout->bytes), savedControlWord(0)
    {
    }

    ~FormatView() noexcept
    {
        // 100091D7/DF/E7：palette、变换行、误差扩散行，最后才释放对象本体。
        delete[] palette;
        delete[] transformRow;
        delete[] diffusionRow;
    }

    FormatView(const FormatView&) = delete;
    FormatView& operator=(const FormatView&) = delete;

    const std::uint8_t* Row(std::uint32_t y) const noexcept
    {
        return pixels + static_cast<std::size_t>(y) * pitch;
    }
};
static_assert(sizeof(void*) == 4, "RenderDX conversion views require Win32.");
static_assert(std::is_standard_layout_v<FormatView>);
static_assert(sizeof(FormatView) == 0x80);
static_assert(offsetof(FormatView, pixels) == 0x20);
static_assert(offsetof(FormatView, palette) == 0x38);
static_assert(offsetof(FormatView, box) == 0x3C);
static_assert(offsetof(FormatView, transformRow) == 0x58);
static_assert(offsetof(FormatView, pitch) == 0x60);
static_assert(offsetof(FormatView, width) == 0x68);
static_assert(offsetof(FormatView, rowBytes) == 0x74);
static_assert(offsetof(FormatView, savedControlWord) == 0x7C);

void DestroyView(FormatView* view) noexcept
{
    delete view;
}

void ReadXrgbRow(const FormatView& view, std::uint32_t y, FloatRgba* output) noexcept
{
    // 10011446：R/G/B 乘原 binary32 常量，X 不参与 alpha。
    constexpr float scale = std::bit_cast<float>(std::uint32_t{0x3B808081u});
    const auto* input = view.Row(y);
    for (std::uint32_t x = 0; x < view.width; ++x)
    {
        output[x] = {input[2] * scale, input[1] * scale, input[0] * scale, 1.0f};
        input += view.pixelBytes;
    }
}

std::uint8_t RecoverXrgbChannel(float channel, float bias) noexcept
{
    // 本行只来自 8 位源 read；反量化误差远小于 0.5，复用已验整数 Quantize。
    const float value = channel * 255.0f + bias;
    if (value <= 0.0f) return 0;
    if (value >= 255.0f) return 255;
    return static_cast<std::uint8_t>(value);
}

void WriteRgbaRow(FormatView& view, std::uint32_t y, const FloatRgba* input) noexcept
{
    const auto& layout = *view.operations->layout;
    const auto alphaBits = static_cast<std::uint8_t>(std::popcount(layout.opaqueAlpha));
    const auto alphaShift = layout.opaqueAlpha != 0 ? std::countr_zero(layout.opaqueAlpha) : 0;
    const float bias = view.roundingBias[(y & 3u) * 8u];
    // 仅目的视图走 write；其基址来自 API 的可写 destination，不写 source 视图。
    auto* output = const_cast<std::uint8_t*>(view.Row(y));
    for (std::uint32_t x = 0; x < view.width; ++x)
    {
        const auto red = static_cast<std::uint64_t>(Quantize(
            RecoverXrgbChannel(input[x].red, bias), layout.redBits));
        const auto green = static_cast<std::uint64_t>(Quantize(
            RecoverXrgbChannel(input[x].green, bias), layout.greenBits));
        const auto blue = static_cast<std::uint64_t>(Quantize(
            RecoverXrgbChannel(input[x].blue, bias), layout.blueBits));
        const auto alpha = static_cast<std::uint64_t>(Quantize(
            RecoverXrgbChannel(input[x].alpha, bias), alphaBits));
        std::uint64_t pixel = (alpha << alphaShift) | (red << layout.redShift)
            | (green << layout.greenShift) | (blue << layout.blueShift);
        for (std::uint32_t byte = 0; byte < view.pixelBytes; ++byte)
        {
            output[byte] = static_cast<std::uint8_t>(pixel);
            pixel >>= 8;
        }
        output += view.pixelBytes;
    }
}

constexpr auto kOperations = []
{
    std::array<FormatOperations, kLayouts.size()> operations{};
    for (std::size_t index = 0; index < kLayouts.size(); ++index)
    {
        operations[index] = {DestroyView,
            kLayouts[index].format == D3DFMT_X8R8G8B8 ? ReadXrgbRow : nullptr,
            WriteRgbaRow, &kLayouts[index]};
    }
    return operations;
}();

const FormatOperations* FindOperations(D3DFORMAT format) noexcept
{
    for (const auto& operations : kOperations)
    {
        if (operations.layout->format == format) return &operations;
    }
    return nullptr;
}

struct FormatOwners
{
    FormatView* destination = nullptr;
    FormatView* source = nullptr;

    FormatOwners() = default;
    FormatOwners(const FormatOwners&) = delete;
    FormatOwners& operator=(const FormatOwners&) = delete;

    ~FormatOwners() noexcept
    {
        // 10008B0C/10008B1C 正常返回先目的后源；异常也安全退真实拥有，不吞异常。
        if (destination) destination->operations->destroy(destination);
        if (source) source->operations->destroy(source);
    }
};

bool RegionAddressable(std::ptrdiff_t pitch, std::size_t rowBytes, std::size_t height) noexcept
{
    if (pitch <= 0 || rowBytes > static_cast<std::size_t>(pitch)) return false;
    constexpr auto maximumOffset = static_cast<std::size_t>((std::numeric_limits<std::ptrdiff_t>::max)());
    return height - 1u <= (maximumOffset - rowBytes) / static_cast<std::size_t>(pitch);
}
}

std::size_t TexturePixelBytes(D3DFORMAT format) noexcept
{
    const auto* layout = FindLayout(format);
    return layout != nullptr ? layout->bytes : 0;
}

bool ConvertXrgbPixels(D3DFORMAT destinationFormat, const std::uint8_t* source,
    std::uint8_t* destination, std::size_t pixelCount) noexcept
{
    const auto* layout = FindLayout(destinationFormat);
    if (layout == nullptr)
    {
        return false;
    }
    if (pixelCount == 0)
    {
        return true;
    }
    constexpr auto maximumSize = (std::numeric_limits<std::size_t>::max)();
    if (source == nullptr || destination == nullptr || pixelCount > maximumSize / 4u
        || pixelCount > maximumSize / layout->bytes)
    {
        return false;
    }

    if (destinationFormat == D3DFMT_X8R8G8B8)
    {
        // 100064C8..10006510 同格式/等尺寸条件；10006584/1000658E 原样复制，保留 X。
        // 不能改走 1000B153 的通用 write：后者会把 X 清零。
        if (source != destination)
        {
            std::memcpy(destination, source, pixelCount * 4u);
        }
        return true;
    }

    for (std::size_t index = 0; index < pixelCount; ++index)
    {
        const auto red = static_cast<std::uint64_t>(Quantize(source[2], layout->redBits));
        const auto green = static_cast<std::uint64_t>(Quantize(source[1], layout->greenBits));
        const auto blue = static_cast<std::uint64_t>(Quantize(source[0], layout->blueBits));
        std::uint64_t pixel = layout->opaqueAlpha | (red << layout->redShift)
            | (green << layout->greenShift) | (blue << layout->blueShift);

        // 原 x86 的小端整字写入；逐字节存储不要求锁返回地址按 WORD/DWORD 对齐。
        for (std::size_t byte = 0; byte < layout->bytes; ++byte)
        {
            destination[byte] = static_cast<std::uint8_t>(pixel);
            pixel >>= 8;
        }
        source += 4;
        destination += layout->bytes;
    }
    return true;
}

bool ConvertXrgbRegion(D3DFORMAT destinationFormat, const std::uint8_t* source,
    std::ptrdiff_t sourcePitch, std::uint8_t* destination, std::ptrdiff_t destinationPitch,
    std::size_t width, std::size_t height)
{
    const auto* destinationOperations = FindOperations(destinationFormat);
    constexpr auto maximumOffset = static_cast<std::size_t>((std::numeric_limits<std::ptrdiff_t>::max)());
    if (!destinationOperations || !source || !destination || width == 0 || height == 0
        || width > maximumOffset / 4u || height > maximumOffset)
        return false;
    const auto destinationBytes = destinationOperations->layout->bytes;
    if (width > maximumOffset / destinationBytes
        || !RegionAddressable(sourcePitch, width * 4u, height)
        || !RegionAddressable(destinationPitch, width * destinationBytes, height)
        || (destinationFormat != D3DFMT_X8R8G8B8 && width > maximumOffset / sizeof(FloatRgba)))
        return false;

    // 10008A5B/10008A6C：同格式也独立分配两个 0x80 对象，new 失败原样传播。
    FormatOwners owners;
    owners.destination = new FormatView(*destinationOperations, destination,
        static_cast<std::uint32_t>(destinationPitch), static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    owners.source = new FormatView(*FindOperations(D3DFMT_X8R8G8B8), source,
        static_cast<std::uint32_t>(sourcePitch), static_cast<std::uint32_t>(width),
        static_cast<std::uint32_t>(height));
    auto& targetView = *owners.destination;
    const auto& sourceView = *owners.source;
    if (targetView.format == sourceView.format)
    {
        for (std::uint32_t row = 0; row < targetView.height; ++row)
        {
            const auto* from = sourceView.Row(row);
            auto* to = const_cast<std::uint8_t*>(targetView.Row(row));
            if (from != to) std::memcpy(to, from, targetView.rowBytes);
        }
        return true;
    }

    // 100065FC/1000665A/10006667：同一条实际 float 行由 read 填充，再由 write 消费。
    // 原 1002EC72 不返回 null；不加入改写 allocator 契约后才可达的后继滤镜。
    std::unique_ptr<FloatRgba[]> row(new FloatRgba[sourceView.width]);
    for (std::uint32_t y = 0; y < targetView.height; ++y)
    {
        sourceView.operations->readRow(sourceView, y, row.get());
        targetView.operations->writeRow(targetView, y, row.get());
    }
    return true;
}
}
