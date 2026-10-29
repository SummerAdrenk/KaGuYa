#pragma once

#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;

// 返回槽尚未拥有资源；451500完成后才取得异常展开资格。
void CaptureGraphicsScreenAt4ACDD0(YtlReferenceSlotRuntime& out,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
    IGraphicsSurfaceTransitionApi& transition);
void CompositeGraphicsFrameAt4A8340(YtlReferenceSlotRuntime& out,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
    DrawLayersView manager, std::int32_t alpha);

// 444470：fresh返回槽；复制后重读out与源坐标，CopyBlt的返回值不参与提交。
// 原局部alpha只定义低byte；此入口零扩展该byte，不传播未初始化的栈高24位。
void CloneGraphicsSurfaceAt444470(YtlReferenceSlotRuntime& out,
    const GraphicsSurfaceNativeBody& source, std::uint8_t copyPixels,
    IGraphicsSurfaceApi& surfaces, IGraphicsSurfaceRawCopyApi& copy,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 模态生产链只接原生拥有引用，不把尚未迁移的effects shared_ptr当作Ref12。
class INativeScreenCaptureServices
{
public:
    virtual ~INativeScreenCaptureServices() = default;
    virtual GraphicsSurfaceReference CaptureScreenNativeAt4ACDD0() = 0;
};
}
