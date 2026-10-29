#pragma once
#include <Runtime/PatternLayerNative.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/ParamsPatternSource.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsScreenSizeApi;
struct PatternPreviewNativeServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenSizeApi& sizes;
    IGraphicsPatternFilterCalls& filters;
    const IParamsPatternSource& pattern;
    const IScrResourceNameRootSource& names;
    ISpriteNativeResourceSource& sprites;
    IGuiControlResourceLoadServices& resources;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 46EA70：fresh返回Ref；初始CSurface/Ref构造在业务catch之外，失败只清本地快照。
// 不使用processed画布、现代Sprite/Surface、std::string名称或第二份Pattern状态。
void SnapshotPatternPreviewNativeAt46EA70(YtlReferenceSlotRuntime& output,
    PatternLayerNativeBody80h& pattern,PatternPreviewNativeServices&);
}
