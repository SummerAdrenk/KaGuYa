#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ParamsByteArray.h>

#include <bit>
#include <cstring>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
template <typename Value>
[[nodiscard]] Value ReadLittleEndian(
    const std::span<const std::byte> bytes,
    const std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(Value))
    {
        throw StaticSurfaceResourceException(
            StaticSurfaceResourceError::HeaderTruncated,
            "Static surface resource header is truncated");
    }
    Value value{};
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

[[nodiscard]] std::int32_t ReadSignedDword(
    const std::span<const std::byte> bytes,
    const std::size_t offset)
{
    return std::bit_cast<std::int32_t>(
        ReadLittleEndian<std::uint32_t>(bytes, offset));
}

[[nodiscard]] std::size_t CheckedPayloadSize(
    const std::int32_t width,
    const std::int32_t height,
    const std::size_t bytesPerRow)
{
    if (width <= 0 || height <= 0)
        return 0;
    const std::size_t rows = static_cast<std::size_t>(height);
    if (bytesPerRow > (std::numeric_limits<std::size_t>::max)() / rows)
    {
        throw StaticSurfaceResourceException(
            StaticSurfaceResourceError::PayloadSizeOverflow,
            "Static surface resource payload size overflows size_t");
    }
    return bytesPerRow * rows;
}

void RequirePayload(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::size_t length)
{
    if (offset > bytes.size() || length > bytes.size() - offset)
    {
        throw StaticSurfaceResourceException(
            StaticSurfaceResourceError::PayloadTruncated,
            "Static surface resource payload is truncated");
    }
}
}

StaticSurfaceResourceException::StaticSurfaceResourceException(
    const StaticSurfaceResourceError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

StaticSurfaceResourceError StaticSurfaceResourceException::Error() const noexcept
{
    return error_;
}

StaticSurfaceResourceDescriptor ParseStaticSurfaceResource(
    const std::span<const std::byte> bytes)
{
    if (bytes.size() < 2u)
    {
        throw StaticSurfaceResourceException(
            StaticSurfaceResourceError::HeaderTruncated,
            "Static surface resource has no magic");
    }

    const std::uint16_t magic = ReadLittleEndian<std::uint16_t>(bytes, 0u);
    if (magic == 0x4D42u) // "BM"
    {
        if (bytes.size() < 54u)
        {
            throw StaticSurfaceResourceException(
                StaticSurfaceResourceError::HeaderTruncated,
                "BMP resource is shorter than the original 0x36-byte check");
        }

        StaticSurfaceResourceDescriptor result;
        result.kind = StaticSurfaceResourceKind::Bitmap;
        result.width = ReadSignedDword(bytes, 18u);
        result.height = ReadSignedDword(bytes, 22u);
        result.bitmapInfoOffset = 14u;
        result.pixelOffset = ReadLittleEndian<std::uint32_t>(bytes, 10u);
        RequirePayload(bytes, result.pixelOffset, 0u);
        return result;
    }

    if (magic != 0x5041u) // "AP"
    {
        throw StaticSurfaceResourceException(
            StaticSurfaceResourceError::UnsupportedMagic,
            "Static surface resource is neither BMP nor AP");
    }

    const std::uint16_t versionOrWidthLow =
        ReadLittleEndian<std::uint16_t>(bytes, 2u);
    StaticSurfaceResourceDescriptor result;
    result.kind = StaticSurfaceResourceKind::Alp;
    result.hasAlpha = true;

    if (versionOrWidthLow == 0x302Du) // "-0"
    {
        result.alpMode = 0u;
        result.width = ReadSignedDword(bytes, 4u);
        result.height = ReadSignedDword(bytes, 8u);
        result.pixelOffset = 12u;
    }
    else if (versionOrWidthLow == 0x322Du || versionOrWidthLow == 0x332Du)
    {
        result.alpMode = versionOrWidthLow == 0x322Du ? 2u : 3u;
        result.hasAlpha = result.alpMode != 3u;
        result.originX = ReadSignedDword(bytes, 4u);
        result.originY = ReadSignedDword(bytes, 8u);
        result.width = ReadSignedDword(bytes, 12u);
        result.height = ReadSignedDword(bytes, 16u);
        // 41FCB0 明确跳过 20..23 的保留 DWORD，像素从 +24 开始。
        static_cast<void>(ReadLittleEndian<std::uint32_t>(bytes, 20u));
        result.pixelOffset = 24u;
    }
    else
    {
        // AP-1 没有版本字符串；bytes +2 直接就是 width。
        result.alpMode = 1u;
        result.width = ReadSignedDword(bytes, 2u);
        result.height = ReadSignedDword(bytes, 6u);
        static_cast<void>(ReadLittleEndian<std::uint16_t>(bytes, 10u));
        result.pixelOffset = 12u;
    }

    std::size_t rowBytes = 0u;
    if (result.width > 0)
    {
        const std::size_t width = static_cast<std::size_t>(result.width);
        if (result.alpMode == 0u)
            rowBytes = width;
        else if (result.alpMode == 3u)
        {
            if (width > ((std::numeric_limits<std::size_t>::max)() - 3u) / 3u)
            {
                throw StaticSurfaceResourceException(
                    StaticSurfaceResourceError::PayloadSizeOverflow,
                    "AP-3 row size overflows size_t");
            }
            rowBytes = (width * 3u + 3u) & ~std::size_t{3u};
        }
        else
        {
            if (width > (std::numeric_limits<std::size_t>::max)() / 4u)
            {
                throw StaticSurfaceResourceException(
                    StaticSurfaceResourceError::PayloadSizeOverflow,
                    "AP row size overflows size_t");
            }
            rowBytes = width * 4u;
        }
    }
    RequirePayload(bytes, result.pixelOffset,
        CheckedPayloadSize(result.width, result.height, rowBytes));
    return result;
}

RuntimeGraphicsSurface::RuntimeGraphicsSurface(IGraphicsSurfaceApi& api)
    : api_(&api)
{
    ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(api);
}

RuntimeGraphicsSurface::~RuntimeGraphicsSurface()
{
    if (body_.nameAt1C.GetString())
    {
        GraphicsSurfaceNativeScope scope(*api_);
        body_.~GraphicsSurfaceNativeBody();
    }
}

RuntimeGraphicsSurface::RuntimeGraphicsSurface(RuntimeGraphicsSurface&& other) noexcept
    : api_(other.api_)
{
    // 仅现代vector使用转移；不Clone/GetNil，不为源重构造原生对象。
    ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(GraphicsSurfaceNativeBody::ModernMoveTag{}, other.body_);
}

RuntimeGraphicsSurface& RuntimeGraphicsSurface::operator=(
    RuntimeGraphicsSurface&& other) noexcept
{
    if (this != &other)
    {
        if (body_.nameAt1C.GetString())
        {
            GraphicsSurfaceNativeScope scope(*api_);
            body_.~GraphicsSurfaceNativeBody();
        }
        ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(GraphicsSurfaceNativeBody::ModernMoveTag{}, other.body_);
        api_ = other.api_;
    }
    return *this;
}

void RuntimeGraphicsSurface::Reset()
{
    // moved-from仅存在于现代vector适配，重用时重新建立唯一正文，不进入原生Ref路径。
    if (!body_.nameAt1C.GetString()) ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(*api_);
    else body_.ResetAt432670(*api_);
}

bool RuntimeGraphicsSurface::Create(
    const std::int32_t width,
    const std::int32_t height,
    const bool hasAlpha,
    const bool clearOnCreate)
{
    if (!body_.nameAt1C.GetString()) ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(*api_);
    return body_.CreateAt415010(*api_, width, height, hasAlpha, clearOnCreate);
}

bool RuntimeGraphicsSurface::LoadStatic(
    const std::span<const std::byte> bytes,
    const std::uint32_t bitmapFlags)
{
    const StaticSurfaceResourceDescriptor descriptor =
        ParseStaticSurfaceResource(bytes);
    return LoadStaticDescriptor(bytes, descriptor, bitmapFlags);
}

namespace
{
bool LoadNativeDescriptor(GraphicsSurfaceNativeBody& body, IGraphicsSurfaceApi& api,
    const std::span<const std::byte> bytes, const StaticSurfaceResourceDescriptor& descriptor,
    const std::uint32_t bitmapFlags, const std::uint32_t alphaHighBits)
{
    const auto alpha = static_cast<std::uint8_t>(descriptor.hasAlpha);
    // 419E31..3E只比较规格；handle==0不是额外重建条件。
    if (body.widthAt08 != descriptor.width || body.heightAt0C != descriptor.height || body.alphaAt18 != alpha)
    {
        if (!body.CreateAt415010(api, descriptor.width, descriptor.height,
            std::bit_cast<std::int32_t>(alphaHighBits | alpha), 0)) return false;
        body.originXAt10 = descriptor.originX;
        body.originYAt14 = descriptor.originY;
    }
    // 432930入口固定上传handle，API仍是显式外部服务边界。
    const auto handle = body.handleAt04;
    if (descriptor.kind == StaticSurfaceResourceKind::Bitmap)
        return api.LoadBitmap(handle, bytes.data(), bytes.data() + descriptor.bitmapInfoOffset,
            bytes.data() + descriptor.pixelOffset, bitmapFlags);
    return api.LoadAlp(handle, descriptor.alpMode, descriptor.width, descriptor.height,
        bytes.data() + descriptor.pixelOffset);
}

bool LoadNativeReader(GraphicsSurfaceNativeBody& body, IGraphicsSurfaceApi& api,
    const ArchiveReaderReference& reference, const std::uint32_t bitmapFlags, const bool modernAdapter)
{
    auto* reader = reference.Get();
    if (!reader) throw ATL::CAtlException(E_INVALIDARG);
    if (!reader->ReadyAt419360()) return false;
    // Ready回调可换outer；419DCF在此重取，再复制byte Ref给41FCB0。
    reader = reference.Get();
    if (!reader) throw ATL::CAtlException(E_INVALIDARG);
    auto* const source = reader->BytesAt4172C0();
    if (!source) throw ATL::CAtlException(E_INVALIDARG);
    StaticSurfaceResourceDescriptor descriptor;
    std::span<const std::byte> bytes;
    {
        ByteArrayReferenceRuntime argument(*source);
        bytes = std::as_bytes(BorrowBoundedByteArrayCopySpan(argument));
        try { descriptor = ParseStaticSurfaceResource(bytes); }
        catch (const StaticSurfaceResourceException&) { return false; }
        // 41FE43..67在parser返回前释放参数Ref，不延长到SurfaceCreate/上传之后。
    }
    if (descriptor.kind == StaticSurfaceResourceKind::Bitmap)
    {
        // BMP parser保留header指针，419E0C在参数release后才读尺寸；AP尺寸已经复制。
        descriptor.width = ReadSignedDword(bytes, 18u);
        descriptor.height = ReadSignedDword(bytes, 22u);
    }
    // moved-from仅属于现代适配器；原生入口没有隐式重构造或alpha归一化。
    if (modernAdapter && !body.nameAt1C.GetString())
        ::new(static_cast<void*>(&body)) GraphicsSurfaceNativeBody(api);
    const auto alphaHighBits = modernAdapter ? 0u :
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&reference.Slot())) & 0xFFFFFF00u;
    if (!LoadNativeDescriptor(body, api, bytes, descriptor, bitmapFlags, alphaHighBits)) return false;
    reader = reference.Get(); // 419E78..8A：上传成功后重新取得名字。
    if (!reader) throw ATL::CAtlException(E_INVALIDARG);
    const char* const name = reader->NameAt4179F0();
    const auto length = name ? std::strlen(name) : 0u;
    if (length > 0x7FFFFFFFu) throw ATL::CAtlException(E_INVALIDARG);
    SetRuntimeCStringAt437680(body.nameAt1C, name, static_cast<int>(length));
    return true;
}
}

bool LoadGraphicsSurfaceArchiveReaderAt419D70(GraphicsSurfaceNativeBody& destination,
    const ArchiveReaderReference& reference, IGraphicsSurfaceApi& surfaces, const std::uint32_t bitmapFlags)
{
    return LoadNativeReader(destination, surfaces, reference, bitmapFlags, false);
}

bool RuntimeGraphicsSurface::LoadArchiveReaderAt419D70(
    const ArchiveReaderReference& reference, const std::uint32_t bitmapFlags)
{
    return LoadNativeReader(body_, *api_, reference, bitmapFlags, true);
}

bool RuntimeGraphicsSurface::LoadStaticDescriptor(const std::span<const std::byte> bytes,
    const StaticSurfaceResourceDescriptor& descriptor, const std::uint32_t bitmapFlags)
{
    if (!body_.nameAt1C.GetString()) ::new(static_cast<void*>(&body_)) GraphicsSurfaceNativeBody(*api_);
    return LoadNativeDescriptor(body_, *api_, bytes, descriptor, bitmapFlags, 0);
}

bool RuntimeGraphicsSurface::LoadAnimationFrame(
    const std::int32_t originX,
    const std::int32_t originY,
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t channels,
    const void* const pixels)
{
    return LoadRawFrame(
        originX, originY, width, height, channels, true, pixels);
}

bool RuntimeGraphicsSurface::LoadRawFrame(
    const std::int32_t originX,
    const std::int32_t originY,
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t channels,
    const bool createWithAlpha,
    const void* const pixels)
{
    if (!Create(width, height, createWithAlpha, false))
        return false;
    body_.originXAt10 = originX;
    body_.originYAt14 = originY;
    api_->LoadAnimationPixels(body_.handleAt04, width, height, channels, pixels);
    return true;
}

void RuntimeGraphicsSurface::SetOrigin(
    const std::int32_t x,
    const std::int32_t y) noexcept
{
    body_.originXAt10 = x;
    body_.originYAt14 = y;
}

void RuntimeGraphicsSurface::SetClearColor(const std::uint32_t color) noexcept
{
    body_.clearColorAt20 = color;
}

GraphicsSurfaceHandle RuntimeGraphicsSurface::Handle() const noexcept { return body_.handleAt04; }
std::int32_t RuntimeGraphicsSurface::Width() const noexcept { return body_.widthAt08; }
std::int32_t RuntimeGraphicsSurface::Height() const noexcept { return body_.heightAt0C; }
std::int32_t RuntimeGraphicsSurface::OriginX() const noexcept { return body_.originXAt10; }
std::int32_t RuntimeGraphicsSurface::OriginY() const noexcept { return body_.originYAt14; }
bool RuntimeGraphicsSurface::HasAlpha() const noexcept { return body_.HasAlpha(); }
std::uint32_t RuntimeGraphicsSurface::ClearColor() const noexcept { return body_.clearColorAt20; }
}
