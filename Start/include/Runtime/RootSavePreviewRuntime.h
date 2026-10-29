#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SaveLoadFileFactoryRuntime.h>

#include <string>

namespace KaGuYa::Start::Runtime
{
class IStartFileApi;
class IGraphicsSurfaceSaveApi;
class ISavePreviewCaptureServices;

// 借用根双Ref；+1D8是本款缩略图，+1E4是重新合成的大图，不借用转场+1F0。
// 本服务与依赖不保活根，析构不清槽；根装配必须另行显式登记同一Graphics API。
class RootSavePreviewRuntime final : public ISaveLoadPreviewServices
{
public:
    RootSavePreviewRuntime(RootSavePreviewSlots& slots, IGraphicsSurfaceApi& surfaces,
        IGraphicsWindowSurfaceApi& pixels, IGraphicsSurfaceSaveApi& saver,
        IStartFileApi& files, ISavePreviewCaptureServices& capture, SIZE thumbnailSize) noexcept;
    ~RootSavePreviewRuntime() override = default;
    RootSavePreviewRuntime(const RootSavePreviewRuntime&) = delete;
    RootSavePreviewRuntime& operator=(const RootSavePreviewRuntime&) = delete;

    void ReleaseAt46DC90();
    void SetCaptureAt48CFF0(const YtlReferenceSlotRuntime& source);
    void CaptureForSaveAt48DBE0() override;
    void GenerateThumbnailAt475FA0();
    // out须为不拥有资源的返回槽；保留原隐藏返回缓冲地址及Ref12复制语义。
    [[nodiscard]] YtlReferenceSlotRuntime* PreviewAt47D740(YtlReferenceSlotRuntime& out);
    [[nodiscard]] GraphicsSurfaceReference PreviewAt47D740();
    [[nodiscard]] bool SavePreviewAt489420(const std::string& fullPath) override;
    [[nodiscard]] bool SavePreviewAt489420(const char* fullPath);
    [[nodiscard]] GraphicsSurfaceReference ThumbnailAt1D8() const noexcept;
    [[nodiscard]] GraphicsSurfaceReference CaptureAt1E4() const noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& RawThumbnailAt1D8() const noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& RawCaptureAt1E4() const noexcept;

private:
    RootSavePreviewSlots& slots_;
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsWindowSurfaceApi& pixels_;
    IGraphicsSurfaceSaveApi& saver_;
    IStartFileApi& files_;
    ISavePreviewCaptureServices& capture_;
    SIZE thumbnailSize_;
};
}
