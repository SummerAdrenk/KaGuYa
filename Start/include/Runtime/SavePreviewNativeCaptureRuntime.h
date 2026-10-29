#pragma once
#include <Runtime/PatternPreviewNative.h>
#include <Runtime/SavePreviewRuntimeSource.h>
#include <Runtime/SavePreviewCaptureApi.h>

namespace KaGuYa::Start::Runtime
{
// 只借实际2Ch/4Ch/80h；不拥有manager、图层或根双预览槽。
class SavePreviewNativeCaptureRuntime final:public ISavePreviewCaptureServices
{
public:
    SavePreviewNativeCaptureRuntime(const ISavePreviewRuntimeSource& source,PatternPreviewNativeServices& services) noexcept
        :source_(source),services_(services){}
    void CaptureIntoAt48DBE0(YtlReferenceSlotRuntime& targetAt1E4) override;
private:
    const ISavePreviewRuntimeSource& source_;
    PatternPreviewNativeServices& services_;
};
}
