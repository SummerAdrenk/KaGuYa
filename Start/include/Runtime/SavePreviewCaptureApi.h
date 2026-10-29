#pragma once

#include <Runtime/GraphicsSurfaceReference.h>

namespace KaGuYa::Start::Runtime
{
class RuntimeGraphicsSurface;

class ISavePreviewCaptureServices
{
public:
    virtual ~ISavePreviewCaptureServices() = default;
    // 宿主先按46DC90释放两个旧Ref；此处按48DBE0创建并逐层提交新大图。
    virtual void CaptureIntoAt48DBE0(
        YtlReferenceSlotRuntime& targetAt1E4) = 0;
};
}
