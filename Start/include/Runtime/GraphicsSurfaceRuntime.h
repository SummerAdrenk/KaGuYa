#pragma once

#include <Windows.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReference;
using GraphicsSurfaceHandle = std::int32_t;

// 441B20/4819B7 逐层传递同一 CSurface*；句柄只能在叶端提交时读取。
// 值构造保留尚未迁移的显式 handle 调用，原生调用必须使用 FromNative。
// 不提供隐式 handle 转换，防止中间接口再次冻结正文 +04。
class GraphicsSurfaceDestination final
{
public:
    GraphicsSurfaceDestination(GraphicsSurfaceHandle handle) noexcept : value_(handle) {}
    [[nodiscard]] static GraphicsSurfaceDestination FromNative(
        const GraphicsSurfaceNativeBody& body) noexcept
    {
        GraphicsSurfaceDestination result(0);
        result.body_ = &body;
        return result;
    }
    [[nodiscard]] GraphicsSurfaceHandle Handle() const noexcept
        { return body_ ? body_->handleAt04 : value_; }
    [[nodiscard]] const GraphicsSurfaceNativeBody* Body() const noexcept { return body_; }
private:
    const GraphicsSurfaceNativeBody* body_ = nullptr;
    GraphicsSurfaceHandle value_;
};

enum class StaticSurfaceResourceKind : std::uint8_t
{
    Bitmap,
    Alp,
};

enum class StaticSurfaceResourceError : std::uint8_t
{
    UnsupportedMagic,
    HeaderTruncated,
    PayloadTruncated,
    PayloadSizeOverflow,
};

class StaticSurfaceResourceException final : public std::runtime_error
{
public:
    StaticSurfaceResourceException(
        StaticSurfaceResourceError error,
        const char* message);

    [[nodiscard]] StaticSurfaceResourceError Error() const noexcept;

private:
    StaticSurfaceResourceError error_;
};

struct StaticSurfaceResourceDescriptor final
{
    StaticSurfaceResourceKind kind = StaticSurfaceResourceKind::Bitmap;
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t originX = 0;
    std::int32_t originY = 0;
    bool hasAlpha = false;
    std::uint8_t alpMode = 0;
    std::size_t bitmapInfoOffset = 0;
    std::size_t pixelOffset = 0;
};

// Start.exe:41FCB0。这里只接受该函数明确识别的 BMP 与 AP-0/1/2/3 布局。
[[nodiscard]] StaticSurfaceResourceDescriptor ParseStaticSurfaceResource(
    std::span<const std::byte> bytes);

class IGraphicsSurfaceApi
{
public:
    virtual ~IGraphicsSurfaceApi() = default;

    [[nodiscard]] virtual GraphicsSurfaceHandle CreateSurface(
        std::int32_t width,
        std::int32_t height,
        std::int32_t hasAlpha,
        std::int32_t clearOnCreate) = 0;
    virtual void ReleaseSurface(GraphicsSurfaceHandle handle) = 0;
    [[nodiscard]] virtual bool LoadBitmap(
        GraphicsSurfaceHandle handle,
        const void* fileHeader,
        const void* bitmapInfo,
        const void* pixels,
        std::uint32_t flags) = 0;
    [[nodiscard]] virtual bool LoadAlp(
        GraphicsSurfaceHandle handle,
        std::uint8_t mode,
        std::int32_t width,
        std::int32_t height,
        const void* pixels) = 0;
    virtual void LoadAnimationPixels(
        GraphicsSurfaceHandle handle,
        std::int32_t width,
        std::int32_t height,
        std::int32_t channels,
        const void* pixels) = 0;
};

class IGraphicsSurfacePixelApi
{
public:
    virtual ~IGraphicsSurfacePixelApi() = default;
    virtual std::uint32_t GetSurfacePixel(GraphicsSurfaceHandle handle,
        std::int32_t x, std::int32_t y) = 0;
};

// 419D70的原生入口直接修改24h正文；不创建现代Surface，也不保留reader或像素owner。
// 原Create的alpha高24位来自reader槽地址；现代方法另保留其bool归一化边界。
[[nodiscard]] bool LoadGraphicsSurfaceArchiveReaderAt419D70(GraphicsSurfaceNativeBody& destination,
    const ArchiveReaderReference& reader, IGraphicsSurfaceApi& surfaces,
    std::uint32_t bitmapFlags = 0);

// ColorTransWindow/消息框的原导出边界。只按模块门控，不附加 Initialized 条件。
class IGraphicsWindowSurfaceApi
{
public:
    virtual ~IGraphicsWindowSurfaceApi() = default;
    [[nodiscard]] virtual std::byte* GetSurfacePointerRaw(GraphicsSurfaceHandle handle) = 0;
    [[nodiscard]] virtual std::int32_t GetSurfacePitchRaw(GraphicsSurfaceHandle handle) = 0;
    virtual void SurfaceClearRaw(GraphicsSurfaceHandle handle, std::uint32_t color,
        const RECT* rect) = 0;
    [[nodiscard]] virtual std::int32_t SurfaceDrawRaw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) = 0;
    [[nodiscard]] virtual std::int32_t SurfaceBlitRaw(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
};

// Start.exe:4143A0/4425C0 使用的绘制导出边界，与图片装载接口分开，
// 避免只需要 decoder 的测试对象被迫伪造合成行为。
class IGraphicsSurfaceCompositionApi
{
public:
    virtual ~IGraphicsSurfaceCompositionApi() = default;

    virtual void ClearSurface(
        GraphicsSurfaceHandle handle,
        std::uint32_t color,
        const RECT* rect) = 0;
    [[nodiscard]] virtual bool DrawSurface(
        GraphicsSurfaceHandle source,
        std::int32_t destinationX,
        std::int32_t destinationY,
        const SIZE* destinationSize,
        const RECT* sourceRect,
        const RECT* destinationClip,
        std::int32_t anchor) = 0;
    [[nodiscard]] virtual bool BlitSurface(
        GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source,
        std::int32_t destinationX,
        std::int32_t destinationY,
        const SIZE* destinationSize,
        const RECT* sourceRect,
        const RECT* destinationClip,
        std::int32_t anchor) = 0;
};

// 47CC00：每次查询 DLL 的逻辑尺寸，空模块返回零 SIZE。
class IGraphicsScreenSizeApi
{
public:
    virtual ~IGraphicsScreenSizeApi() = default;
    [[nodiscard]] virtual SIZE ReadScreenSizeAt47CC00() = 0;
};

// 单次调用负责取得当前Graphics根及模块资格；原生LayerList无需预取模块。
class IGraphicsScreenCalls
{
public:
    virtual ~IGraphicsScreenCalls() = default;
    virtual void ClearScreenIfLoaded(const RECT* rect) = 0;
    virtual void InvalidatedRectIfLoaded(const RECT* rect) = 0;
};

// 既有4C3D50现代宿主的显式模块查询；原生调用链不以noexcept查询代替4182F0。
class IGraphicsScreenUpdateApi : public IGraphicsScreenCalls
{
public:
    virtual ~IGraphicsScreenUpdateApi() = default;
    [[nodiscard]] virtual bool ModuleLoaded() const = 0;
};

// 443A00只检查模块引用，忽略DLL返回值；不能借用下方要求初始化的直接Copy接口。
class IGraphicsSurfaceRawCopyApi
{
public:
    virtual ~IGraphicsSurfaceRawCopyApi() = default;
    virtual void SurfaceCopyBlitRaw(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
};

class IGraphicsSurfaceCopyApi
{
public:
    virtual ~IGraphicsSurfaceCopyApi() = default;

    [[nodiscard]] virtual bool CopyBlitSurface(
        GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source,
        std::int32_t destinationX,
        std::int32_t destinationY,
        const SIZE* destinationSize,
        const RECT* sourceRect,
        const RECT* destinationClip,
        std::int32_t anchor) = 0;
};

// 4C3530/44ED00/44EC80/44ED80 使用的导出包装；模块为空时原包装无操作。
class IGraphicsSurfaceTransitionApi
{
public:
    virtual ~IGraphicsSurfaceTransitionApi() = default;
    virtual void BackupScreenToSurfaceIfLoaded(GraphicsSurfaceHandle destination) = 0;
    virtual void CopyDrawSurfaceIfLoaded(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const RECT* sourceRect) = 0;
    virtual void BlendDrawSurfaceIfLoaded(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, std::int32_t alpha) = 0;
    virtual void UniversalBlitSurfaceIfLoaded(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle mask, const std::int32_t* thresholds) = 0;
};

// 对应 Start.exe 中被 CSprite 持有的 CSurface 状态。api 必须比本对象存活更久。
class RuntimeGraphicsSurface final
{
public:
    explicit RuntimeGraphicsSurface(IGraphicsSurfaceApi& api);
    ~RuntimeGraphicsSurface();

    RuntimeGraphicsSurface(const RuntimeGraphicsSurface&) = delete;
    RuntimeGraphicsSurface& operator=(const RuntimeGraphicsSurface&) = delete;
    RuntimeGraphicsSurface(RuntimeGraphicsSurface&& other) noexcept;
    RuntimeGraphicsSurface& operator=(RuntimeGraphicsSurface&& other) noexcept;

    void Reset();

    // 45B120 -> 415010：PatternLayer 的主画布直接使用 GetScreenSize 返回的宽高创建。
    [[nodiscard]] bool Create(
        std::int32_t width,
        std::int32_t height,
        bool hasAlpha,
        bool clearOnCreate);

    // 419D70：尺寸/alpha 相同则复用句柄；仅重建句柄时更新 AP 原点。
    [[nodiscard]] bool LoadStatic(
        std::span<const std::byte> bytes,
        std::uint32_t bitmapFlags);
    // 真reader入口：Ready/解析借用/上传后Name分别按419D70重新取得来源。
    [[nodiscard]] bool LoadArchiveReaderAt419D70(
        const ArchiveReaderReference& reader, std::uint32_t bitmapFlags);

    // 445E5B..44604B：动画帧一律创建 alpha Surface，原调用者不检查 SurfaceLoadANM 返回值。
    [[nodiscard]] bool LoadAnimationFrame(
        std::int32_t originX,
        std::int32_t originY,
        std::int32_t width,
        std::int32_t height,
        std::int32_t channels,
        const void* pixels);

    // 447F60 的 PL reader 会按版本决定 Surface 是否带 alpha，随后仍统一调用 SurfaceLoadANM。
    [[nodiscard]] bool LoadRawFrame(
        std::int32_t originX,
        std::int32_t originY,
        std::int32_t width,
        std::int32_t height,
        std::int32_t channels,
        bool createWithAlpha,
        const void* pixels);

    // 4B0840 合并 FileGroup 时直接改写 CSurface+16/+20，不重新上传像素。
    void SetOrigin(std::int32_t x, std::int32_t y) noexcept;
    void SetClearColor(std::uint32_t color) noexcept;

    [[nodiscard]] GraphicsSurfaceHandle Handle() const noexcept;
    [[nodiscard]] std::int32_t Width() const noexcept;
    [[nodiscard]] std::int32_t Height() const noexcept;
    [[nodiscard]] std::int32_t OriginX() const noexcept;
    [[nodiscard]] std::int32_t OriginY() const noexcept;
    [[nodiscard]] bool HasAlpha() const noexcept;
    [[nodiscard]] std::uint32_t ClearColor() const noexcept;
    [[nodiscard]] const char* ResourceName() const noexcept { return body_.nameAt1C.GetString(); }
    [[nodiscard]] GraphicsSurfaceNativeBody& NativeBody() noexcept { return body_; }
    [[nodiscard]] const GraphicsSurfaceNativeBody& NativeBody() const noexcept { return body_; }

private:
    [[nodiscard]] bool LoadStaticDescriptor(std::span<const std::byte> bytes,
        const StaticSurfaceResourceDescriptor& descriptor, std::uint32_t bitmapFlags);
    IGraphicsSurfaceApi* api_ = nullptr;
    // 现代适配器仅借用API，所有业务字段只存于同一个24h正文；适配器本身不是原ABI。
    union { GraphicsSurfaceNativeBody body_; };
};
}
