#pragma once

#include <Runtime/GraphicsRootNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct GraphicsSurfaceNativeBody;
// 稳定的非拥有代理；只借三项服务，不保存根、module、handle或导出地址。
// 每个业务入口各自4182F0一次；24h调用者的handle门控不在这里重复。
class GraphicsSurfaceNativeApi final
    : public IGraphicsSurfaceApi,
      public IGraphicsWindowSurfaceApi,
      public IGraphicsSurfaceRawCopyApi
{
public:
    GraphicsSurfaceNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    ~GraphicsSurfaceNativeApi() override = default;
    GraphicsSurfaceNativeApi(const GraphicsSurfaceNativeApi&) = delete;
    GraphicsSurfaceNativeApi& operator=(const GraphicsSurfaceNativeApi&) = delete;

    // 415010/432670/432930/4486D0中的原导出叶，不修改24h正文。
    [[nodiscard]] GraphicsSurfaceHandle CreateSurface(std::int32_t width,
        std::int32_t height, std::int32_t hasAlpha, std::int32_t clearOnCreate) override;
    void ReleaseSurface(GraphicsSurfaceHandle handle) override;
    [[nodiscard]] bool LoadBitmap(GraphicsSurfaceHandle handle, const void* fileHeader,
        const void* bitmapInfo, const void* pixels, std::uint32_t flags) override;
    [[nodiscard]] bool LoadAlp(GraphicsSurfaceHandle handle, std::uint8_t mode,
        std::int32_t width, std::int32_t height, const void* pixels) override;
    void LoadAnimationPixels(GraphicsSurfaceHandle handle, std::int32_t width,
        std::int32_t height, std::int32_t channels, const void* pixels) override;

    // 47CA10/47C680/443910/441980/4425C0；只按Ref.base门控，不加Initialized条件。
    [[nodiscard]] std::byte* GetSurfacePointerRaw(GraphicsSurfaceHandle handle) override;
    [[nodiscard]] std::int32_t GetSurfacePitchRaw(GraphicsSurfaceHandle handle) override;
    void SurfaceClearRaw(GraphicsSurfaceHandle handle, std::uint32_t color,
        const RECT* rect) override;
    // 46F73F的handle门控由调用者负责；此叶先Capture，后读固定24h正文的color/handle。
    void ClearSurfaceBodyAt46F745(const GraphicsSurfaceNativeBody& body);
    // 441A75/4426B9只传AL；现int32_t接口零扩展该byte，不把高位当bool，也不归一非零值。
    [[nodiscard]] std::int32_t SurfaceDrawRaw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) override;
    [[nodiscard]] std::int32_t SurfaceBlitRaw(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;

    // 443A00不消费DLL返回值。
    void SurfaceCopyBlitRaw(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) override;

private:
    // false仅表示base=0；base非零而bias计算出null仍返回true，随后保持原无效访问。
    [[nodiscard]] bool CaptureModule(GraphicsModuleNativeBody8h*& module);
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
