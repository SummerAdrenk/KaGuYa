#pragma once

#include <Runtime/FreeScrollNative.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceApi;
class IGraphicsWindowSurfaceApi;
class IGraphicsPatternFilterCalls;
class IGraphicsScreenCalls;
class ISpriteAnimationInitializationSource;
class GraphicsRootOwner;
class IGraphicsModuleNativeWin32Api;
struct GuiDirtyRectNativeBody80h;
class IFreeScrollNativeCanvasCopy;

// 仅借用调用期服务；这些引用均不进入原 50h/24h 正文。
struct FreeScrollNativeDrawServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsPatternFilterCalls& filters;
    IRuntimeStringManagerSource& strings;
    IFreeScrollNativeCanvasCopy& processedCopy;
};

// 4AEFED 使用 418680/40B530 名字协议，不是显式长度的 443A00 CopyBlt。
class IFreeScrollNativeCanvasCopy
{
public:
    virtual ~IFreeScrollNativeCanvasCopy() = default;
    virtual void CopyProcessedAt4AEFED(std::int32_t destination, std::int32_t source,
        std::int32_t x, std::int32_t y, const RECT* sourceRect) = 0;
};
class FreeScrollNativeCanvasCopy final : public IFreeScrollNativeCanvasCopy
{
public:
    FreeScrollNativeCanvasCopy(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32) noexcept;
    void CopyProcessedAt4AEFED(std::int32_t destination, std::int32_t source,
        std::int32_t x, std::int32_t y, const RECT* sourceRect) override;
private:
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
};

void CollectFreeScrollDirtyAt4A7A30(FreeScrollEntryNativeBody24h& entry,
    GuiDirtyRectNativeBody80h& dirty);
void RedrawFreeScrollEntryAt4C43D0(FreeScrollEntryNativeBody24h& entry,
    const RECT* clip, FreeScrollNativeDrawServices& services);
void RenderFreeScrollAt4AF200(FreeScrollNativeBody50h& body, std::uint32_t changed,
    FreeScrollNativeDrawServices& services,
    const ISpriteAnimationInitializationSource& registry, IGraphicsScreenCalls& screen);
// 仅处理 4AE574..4AF194 的一个 entry；外层实时 end 遍历属于 Begin。
void CreateFreeScrollCanvasAt4AE574(FreeScrollNativeBody50h& body,
    YtlReferenceSlotRuntime& entrySlot, FreeScrollNativeDrawServices& services);
}
