#include <Runtime/LogoSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kLogoResourceNameCp932[] = "\x83\x8D\x83\x53";
constexpr char kTitleSceneNameCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B";
}

LogoSceneRuntime::LogoSceneRuntime(
    ILogoDemoSequenceRuntime& sequence,
    ILogoSceneRuntimeServices& services, IDemoParamsSource& params)
    : sequence_(&sequence), services_(&services)
{
    // sequence 已构造完成，4AB01E 才取本次 GS；lookup 成功才取得局部 Ref 清理资格。
    YtlReferenceSlotRuntime found;
    FindParamsDemoAt4ACC40(params.CaptureGameSystemAt418480(), kLogoResourceNameCp932, found);
    try { sequence_->BindDemoDataAt4C2220(found); }
    catch (...)
    {
        ReleaseParamsDemoLookupAt4A32A0(found);
        throw;
    }
    // 正常与展开释放都有 terminate 边界；释放返回以后才清 descriptor2。
    ReleaseParamsDemoLookupAt4A32A0(found);
    services_->ClearGlobalDescriptorTarget(2u);
}

void LogoSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
}

void LogoSceneRuntime::SetInputCompletionCheckEnabled(const bool enabled) noexcept
{
    inputCompletionCheckEnabled_ = enabled;
}

std::uint32_t LogoSceneRuntime::State() const noexcept
{
    return state_;
}

bool LogoSceneRuntime::CompletionRequested() const noexcept
{
    return completionRequested_;
}

bool LogoSceneRuntime::InputCompletionCheckEnabled() const noexcept
{
    return inputCompletionCheckEnabled_;
}

void LogoSceneRuntime::Update()
{
    // 0x4B5FC4..0x4B6043：三项短路条件命中后，本帧直接转入结束状态。
    if (inputCompletionCheckEnabled_ && !completionRequested_ && state_ != 2u &&
        services_->ShouldRequestCompletion())
    {
        completionRequested_ = true;
        state_ = 2u;
    }

    switch (state_)
    {
    case 0u:
        switch (sequence_->Run(completionRequested_ ? 1u : 0u))
        {
        case -1:
            // 异常结束路径在同一帧替换 active 场景，不执行正常结束清理。
            services_->QueueScene(kTitleSceneNameCp932);
            break;

        case 0:
            // 正常结束只改状态；0x4B5EF0 要到下一帧才执行。
            state_ = 2u;
            break;

        case 1:
            state_ = 1u;
            break;

        case 2:
            sequence_->UpdateSpritesAndRedraw();
            break;

        default:
            break;
        }
        break;

    case 1u:
        if (sequence_->AdvanceTransition())
        {
            state_ = 0u;
            if (completionRequested_)
                state_ = 2u;
        }
        break;

    case 2u:
        Finish();
        break;

    default:
        break;
    }
}

void LogoSceneRuntime::Finish()
{
    services_->ClearRootObjectAt24Binding();
    services_->QueueScene(kTitleSceneNameCp932);
}
}
