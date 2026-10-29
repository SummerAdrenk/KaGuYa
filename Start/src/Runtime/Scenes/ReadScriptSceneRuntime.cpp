#include <Runtime/ReadScriptSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
void ReadScriptSceneState::SetStateAndClearSubstate(
    const std::int32_t state) noexcept
{
    mainState = state;
    substate = 0;
}

ReadScriptSceneRuntime::ReadScriptSceneRuntime(
    IReadScriptUpdateBoundary& updateBoundary,
    IReadScriptDrawBoundary& drawBoundary,
    IReadScriptErrorBoundary& errorBoundary) noexcept
    : updateBoundary_(&updateBoundary),
      drawBoundary_(&drawBoundary),
      errorBoundary_(&errorBoundary)
{
}

void ReadScriptSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);

    // 4C3630 本身不设 catch；4BA530 的 Update 内部处理错误后才调用 Draw。
    Update();
    Draw();
}

void ReadScriptSceneRuntime::Update()
{
    try
    {
        DispatchUpdate();
    }
    // 原PE 5F09E4：.PAD + HT_IsConst；char*、const char*和typed-null均保留原指针。
    catch (const char* const messageCurrentCodePage)
    {
        errorBoundary_->ShowOriginalError(messageCurrentCodePage);
        errorBoundary_->CleanupCurrentEnvironment();
    }
    catch (...)
    {
        errorBoundary_->ShowSyntaxError();
        errorBoundary_->CleanupCurrentEnvironment();
    }
}

void ReadScriptSceneRuntime::DispatchUpdate()
{
    bool continueInSameFrame = false;
    do
    {
        switch (state_.mainState)
        {
        case 0:
            continueInSameFrame = updateBoundary_->RunVmYield(state_);
            break;
        case 1:
            continueInSameFrame = updateBoundary_->RunUpdateEffect(state_);
            break;
        case 2:
            continueInSameFrame = updateBoundary_->RunSuspend(state_);
            break;
        case 3:
            continueInSameFrame = updateBoundary_->RunTextInteraction(state_);
            break;
        case 4:
            continueInSameFrame = updateBoundary_->RunMenuInteraction(state_);
            break;
        case 5:
            continueInSameFrame = updateBoundary_->RunWait(state_);
            break;
        case 7:
            // 4BA5B9 不重写 CL。直接进入为 false；由 true 分支进入会保持原忙循环。
            break;
        default:
            // 4BA619：状态 6 与其余未知值均抛整数 1，再由 Update 的 catch-all 处理。
            throw 1;
        }

    } while (continueInSameFrame);
}

void ReadScriptSceneRuntime::Draw()
{
    if (!state_.drawEnabled)
        return;

    drawBoundary_->PrepareRootOwners();
    drawBoundary_->SubmitCurrentDirtyRect();
    drawBoundary_->SubmitDrawRecords();
    drawBoundary_->CompleteRootOwnerDraw();
}

ReadScriptSceneState& ReadScriptSceneRuntime::State() noexcept
{
    return state_;
}

const ReadScriptSceneState& ReadScriptSceneRuntime::State() const noexcept
{
    return state_;
}

}
