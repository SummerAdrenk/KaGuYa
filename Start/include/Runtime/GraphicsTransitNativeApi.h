#pragma once

#include <Runtime/GraphicsRootNative.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 原转场导出的稳定非拥有代理；每个入口一次4182F0，不保存root/module/导出地址。
// 六个绘制入口完整保留AL，Universal/Mosaic按原调用者丢弃返回值。
class GraphicsTransitNativeApi final : public IGraphicsTransitApi
{
public:
    GraphicsTransitNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    ~GraphicsTransitNativeApi() override = default;
    GraphicsTransitNativeApi(const GraphicsTransitNativeApi&) = delete;
    GraphicsTransitNativeApi& operator=(const GraphicsTransitNativeApi&) = delete;

    // 443A00：内联显式长度名称，resolver后释放缓存的名称header。
    std::uint8_t TransitCopyBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    // 44ED00/44EC00/44EC80：40B530构造名称，resolver后重读本地CString槽。
    std::uint8_t TransitCopyDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    std::uint8_t TransitBlendBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        std::int32_t alpha, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) override;
    std::uint8_t TransitBlendDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, std::int32_t alpha, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;

    // 44ED80的最终绘制叶，同名Surface导出不代表与441980/4425C0的名称路径相同。
    std::uint8_t TransitBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    std::uint8_t TransitDraw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    // 44EE45/44DCBA：阈值仅借用，size按完整DWORD传递，不在代理重算或钳位。
    void TransitUniversalBlit(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle mask, const std::int32_t* thresholds,
        std::int32_t x, std::int32_t y, const SIZE* size,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    void TransitMosaic(GraphicsSurfaceHandle destination,
        std::uint32_t size, const RECT* clip) override;

private:
    // false仅表示Ref.base为0；非零base解析成null不改为空模块分支。
    [[nodiscard]] bool CaptureModule(GraphicsModuleNativeBody8h*& module);
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};
}
