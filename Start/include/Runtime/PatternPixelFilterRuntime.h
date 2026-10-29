#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>

#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
class PatternFilterNativeBody20h;
// 原生调用将4182F0取根、Ref资格检查和DLL调用保持为同一次操作。
// 参数与句柄已由442900读取；此接口允许取根失败按原catch(...)边界传播。
class IGraphicsPatternFilterCalls : public IGraphicsSurfaceRawCopyApi
{
public:
    virtual ~IGraphicsPatternFilterCalls() = default;
    virtual void SurfaceFilterIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFilter2IfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFlushIfLoaded(GraphicsSurfaceHandle destination, const RECT* clip) = 0;
    virtual void SurfaceBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) = 0;
    virtual void SurfaceGaussBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) = 0;
    virtual void SurfaceAddColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceSubColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceCopyDrawIfLoaded(GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* destinationSize, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual void SurfaceClearIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceMulAlphaIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceAddSubSurfaceIfLoaded(GraphicsSurfaceHandle destination, std::uintptr_t sourceObjectAddress, const RECT* clip) = 0;
    virtual void SurfaceMosaicIfLoaded(GraphicsSurfaceHandle destination, std::int32_t size, const RECT* clip) = 0;
    virtual void SurfaceMulColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFlipIfLoaded(GraphicsSurfaceHandle destination, std::int32_t flags, const RECT* clip) = 0;
};

// 442900 的窄 Graphics 边界：仅判断模块是否存在，不要求 Initialized()。
// 原调用者忽略所有 DLL 返回值；Raw 方法因此返回 void，调用异常继续传播。
// 既有现代DLL服务和回归夹具的适配层；原生服务不经此二段式接口。
class IGraphicsPatternFilterApi : public IGraphicsPatternFilterCalls
{
public:
    virtual ~IGraphicsPatternFilterApi() = default;
    [[nodiscard]] virtual bool ModuleLoaded() const = 0;

    void SurfaceFilterIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceFilterRaw(destination, color, clip); }
    void SurfaceFilter2IfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceFilter2Raw(destination, color, clip); }
    void SurfaceFlushIfLoaded(GraphicsSurfaceHandle destination, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceFlushRaw(destination, clip); }
    void SurfaceBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceBlurRaw(destination, source, radius, clip); }
    void SurfaceGaussBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceGaussBlurRaw(destination, source, radius, clip); }
    void SurfaceAddColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceAddColorRaw(destination, color, clip); }
    void SurfaceSubColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceSubColorRaw(destination, color, clip); }
    void SurfaceCopyDrawIfLoaded(GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* destinationSize, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override
    { if (ModuleLoaded()) SurfaceCopyDrawRaw(source, x, y, destinationSize, sourceRect, clip, anchor); }
    void SurfaceClearIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceClearRaw(destination, color, clip); }
    void SurfaceMulAlphaIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceMulAlphaRaw(destination, color, clip); }
    void SurfaceAddSubSurfaceIfLoaded(GraphicsSurfaceHandle destination, std::uintptr_t sourceObjectAddress, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceAddSubSurfaceRaw(destination, sourceObjectAddress, clip); }
    void SurfaceMosaicIfLoaded(GraphicsSurfaceHandle destination, std::int32_t size, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceMosaicRaw(destination, size, clip); }
    void SurfaceMulColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceMulColorRaw(destination, color, clip); }
    void SurfaceFlipIfLoaded(GraphicsSurfaceHandle destination, std::int32_t flags, const RECT* clip) override
    { if (ModuleLoaded()) SurfaceFlipRaw(destination, flags, clip); }

    virtual void SurfaceFilterRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFilter2Raw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFlushRaw(GraphicsSurfaceHandle destination,
        const RECT* clip) = 0;
    virtual void SurfaceBlurRaw(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) = 0;
    virtual void SurfaceGaussBlurRaw(GraphicsSurfaceHandle destination,
        GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) = 0;
    virtual void SurfaceAddColorRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceSubColorRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceCopyDrawRaw(GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* destinationSize,
        const RECT* sourceRect, const RECT* clip, std::int32_t anchor) = 0;
    virtual void SurfaceClearRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceMulAlphaRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    // 44314A -> 444770 返回对象地址，4431E9 未解引用 +4，不能改传 handle。
    virtual void SurfaceAddSubSurfaceRaw(GraphicsSurfaceHandle destination,
        std::uintptr_t sourceObjectAddress, const RECT* clip) = 0;
    virtual void SurfaceMosaicRaw(GraphicsSurfaceHandle destination,
        std::int32_t size, const RECT* clip) = 0;
    virtual void SurfaceMulColorRaw(GraphicsSurfaceHandle destination,
        std::uint32_t color, const RECT* clip) = 0;
    virtual void SurfaceFlipRaw(GraphicsSurfaceHandle destination,
        std::int32_t flags, const RECT* clip) = 0;
};

// 442900：args 数量要求为 1/4/8/9:3，3:2，5/7/11:1；2/6/默认不检查。
// source 只供 mode3，temporary 只供 mode6。画布复制、临时图装载和 mode11
// 矩形调整由 443B00 的调用者负责；此处原样传 clip，不创建或修改 Surface 所有权。
// 5E40C4 -> 5E40B0 -> 5E40A0 是 catch(...)，异常返回 false，保留已发生的副作用。
// 默认分支（含 0/10）、有效对象下空模块和 DLL 返回失败均正常返回 true。
// 46EA70 的 clone 允许空 Ref：在原分支读取目标+4的位置显式失败，而不是解引用空指针。
// 参数/source/temp 预检在先，目标句柄读取在模块门控之前；mode4零颜色/default不读目标。
// 原生调用借用同一24h正文，句柄仅在各分支原读点读取；mode6传temp正文地址。
// 不构造现代Surface、不增持或拷贝资源；本入口仍保留上述显式非法输入边界。
[[nodiscard]] bool ApplyPatternPixelFilterNativeAt442900(
    IGraphicsPatternFilterCalls& graphics,
    std::int32_t mode,
    std::span<const std::uint32_t> args,
    const GraphicsSurfaceNativeBody* destination,
    const GraphicsSurfaceNativeBody* source,
    const GraphicsSurfaceNativeBody* temporary,
    const RECT* clip);

// 真实Filter receiver：mode4在AddColor回调后重取+08，且不再次检查参数count。
// mode只读一次；参数仅在实际分支读取，1/8/9先读参数0/1、目标句柄、参数2。
// mode6先检查temp.base，再读目标句柄、解析temp；解析地址即使为0也照传，不增持Ref。
// 原生调用不把非法目标指针转换为可捕获C++异常；旧mode/span入口保持其显式边界。
// Graphics模块/导出名CString由借用服务在每次实际调用时取源，不缓存第二套根。
[[nodiscard]] bool ApplyPatternPixelFilterNativeAt442900(
    IGraphicsPatternFilterCalls& graphics, const PatternFilterNativeBody20h& filter,
    const GraphicsSurfaceNativeBody* destination, const GraphicsSurfaceNativeBody* source,
    const RECT* clip);

[[nodiscard]] bool ApplyPatternPixelFilterAt442900(
    IGraphicsPatternFilterApi& graphics,
    std::int32_t mode,
    std::span<const std::uint32_t> args,
    const RuntimeGraphicsSurface* destination,
    const RuntimeGraphicsSurface* source,
    const RuntimeGraphicsSurface* temporary,
    const RECT* clip);

// 保留既有调用者的引用入口，委托同一个分支实现。
[[nodiscard]] bool ApplyPatternPixelFilterAt442900(
    IGraphicsPatternFilterApi& graphics,
    std::int32_t mode,
    std::span<const std::uint32_t> args,
    const RuntimeGraphicsSurface& destination,
    const RuntimeGraphicsSurface* source,
    const RuntimeGraphicsSurface* temporary,
    const RECT* clip);
}
