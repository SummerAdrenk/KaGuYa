#pragma once

#include <Runtime/SavePreviewCaptureApi.h>
#include <Runtime/SavePreviewRuntimeSource.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class CountDownWindowResources;
class DrawManagerLayerRuntime;
class IGraphicsSurfaceApi;
class IGraphicsWindowSurfaceApi;
class IGraphicsScreenSizeApi;
class IGraphicsPatternFilterApi;
class IPatternFilterResourceLoader;
class PatternLayerRuntime;
struct ParamsGameSystem;
class IParamsPatternSource;

struct SavePreviewCaptureDependencies final
{
    const ISavePreviewRuntimeSource& runtime;
    const IParamsPatternSource& patternResources;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenSizeApi& sizes;
    IGraphicsPatternFilterApi& filters;
    IPatternFilterResourceLoader& filterResources;
    // 4813B0 创建局部 Sprite 后调用实际4814F0，复用已有完整装载/错误显示服务。
    CountDownWindowResources& spriteResources;
};

// 48DBE0：重建各 Pattern 快照，再按 owning[1..N) 合成；不是 screen backup。
class SavePreviewCaptureRuntime final : public ISavePreviewCaptureServices
{
public:
    explicit SavePreviewCaptureRuntime(SavePreviewCaptureDependencies dependencies) noexcept;
    // 不清旧Ref；创建/合成异常交给 Root 的48DFDC catch清双Ref。
    void CaptureIntoAt48DBE0(YtlReferenceSlotRuntime& targetAt1E4) override;
    // 原局部 Ref 构造在内部catch之外；其后失败只返回空快照，不回滚 Sprite 坐标。
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> SnapshotPatternAt46EA70(PatternLayerRuntime& pattern);
private:
    SavePreviewCaptureDependencies dependencies_;
};
}
