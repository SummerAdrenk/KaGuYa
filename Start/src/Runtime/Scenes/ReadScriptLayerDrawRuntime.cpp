#include <Runtime/ReadScriptLayerDrawRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ReadScriptLayerDrawRuntime::ReadScriptLayerDrawRuntime(
    IDrawManagerLayerSource& source, Platform::KeyboardView keyboard,
    const UnSaveDataRuntime& unsaved, const SaveDataEnvironmentArchive& saved,
    IGraphicsScreenUpdateApi& screen) noexcept
    : source_(source), unsaved_(unsaved), saved_(saved),
      input_(keyboard, unsaved, saved), screen_(screen) {}

bool ReadScriptLayerDrawRuntime::SuppressFrameAdvanceAt4C3350() const
{
    if (saved_.bytesAt6D[0] == 0 || (unsaved_.FlagsAt1C() & 2u) != 0)
        return true;
    // 4C33C8 跳到 4C3413 后直接返回 saved+6E 判定，不再读取键盘和 +14。
    if (unsaved_.BytesAt24()[5] != 0)
        return saved_.bytesAt6D[1] == 0;
    return input_.AnyActionSixCurrentState() || (unsaved_.FlagsAt14() & 2u) != 0;
}

DrawWindowView ReadScriptLayerDrawRuntime::WindowZero() const
{
    return source_.CaptureDrawLayersAt413BE0().WindowZero();
}

void ReadScriptLayerDrawRuntime::PrepareRootOwners()
{
    if (!SuppressFrameAdvanceAt4C3350())
        source_.CaptureDrawLayersAt413BE0().AdvanceFramesAt46B240();
}

void ReadScriptLayerDrawRuntime::SubmitCurrentDirtyRect()
{
    WindowZero().AdvanceEffectAndQueueDirtyAt4B5AC5();
}

void ReadScriptLayerDrawRuntime::SubmitDrawRecords()
{
    // 4B5B4C：效果回调可能更换整个根，不只是同一manager内换Window[0]。
    WindowZero().PropagateDirtyRectsAt4C4250();
}

void ReadScriptLayerDrawRuntime::CompleteRootOwnerDraw()
{
    // 4B5B8B：最后一次Ensure与前一阶段独立，绘制过程中仍固定本次receiver。
    source_.CaptureDrawLayersAt413BE0().RegisteredLayers().RenderDirtyRects(screen_);
}
}
