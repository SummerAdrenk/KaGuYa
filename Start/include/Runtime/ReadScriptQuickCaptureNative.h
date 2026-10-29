#pragma once

#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IReadScriptQuickCaptureNativeSource
{
public:
    virtual ~IReadScriptQuickCaptureNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual DrawLayersView LayersAt4175A0(void* capturedRoot) = 0;
    virtual YtlReferenceSlotRuntime& FrameAt1F0(void* capturedRoot) = 0;
};

struct ReadScriptQuickCaptureNativeServices final
{
    IReadScriptQuickCaptureNativeSource& source;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenSizeApi& sizes;
};

// 4B2730/4B11B0 的 QuickSave/QuickLoad 共用前缀；两次 Root 各自捕获。
// source 只借用指定 Root；空 Draw 不提前拒绝。surfaces 覆盖新旧帧的删除服务。
// 不含 Tool/WaitMark 恢复、Quick 业务、RootHost 装配或异常回滚。
void CaptureReadScriptQuickNativeAt4C1C60(ReadScriptQuickCaptureNativeServices& services);
}
