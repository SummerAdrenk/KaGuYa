#include <Runtime/EndingSceneRuntime.h>

#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kTitleSceneNameCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B";

class CapturedEndingName final
{
public:
    explicit CapturedEndingName(ATL::CStringData* header) noexcept : header_(header) {}
    ~CapturedEndingName() noexcept
    {
        if (header_) ReleaseRuntimeCStringHeader(header_);
    }
    const char* Chars() const noexcept { return reinterpret_cast<const char*>(header_ + 1); }
    void ReleaseNormal()
    {
        // 49DB0A 先退 state，再 Free 缓存 Clone header；Free 抛出不能再次减持。
        auto* const header = header_;
        header_ = nullptr;
        ReleaseRuntimeCStringHeader(header);
    }
private:
    ATL::CStringData* header_;
};
}

EndingSceneRuntime::EndingSceneRuntime(
    IEndingDemoSequenceRuntime& sequence,
    IEndingSceneRuntimeServices& services, IDemoParamsSource& params)
    : sequence_(&sequence), services_(&services)
{
    // 49DA53..49DC97：原构造函数逐步提交副作用，后续异常不回滚前序动作。
    const auto* const source = services_->CaptureDemoNameSlotAt49DA53();
    if (!source) _invalid_parameter_noinfo_noreturn();
    auto* const sourceHeader = reinterpret_cast<ATL::CStringData*>(
        const_cast<char*>(source->GetString())) - 1;
    // 49DA85 Clone 必须先于 49DA97 Params getter；不 GetNil、不构造名字副本。
    CapturedEndingName name(CloneRuntimeCStringDataAt414540(sourceHeader));
    YtlReferenceSlotRuntime found;
    FindParamsDemoAt4ACC40(params.CaptureGameSystemAt418480(), name.Chars(), found);
    try { sequence_->BindDemoDataAt4C2220(found); }
    catch (...)
    {
        ReleaseParamsDemoLookupAt4A32A0(found);
        throw;
    }
    ReleaseParamsDemoLookupAt4A32A0(found);
    name.ReleaseNormal();
    services_->StopRootSlot24Sounds();
    services_->StopRootSlot24SoundsUsingDescriptor2ListAt30();
    services_->WriteDescriptor2DwordAt04(0u);
    services_->WriteDescriptor2DwordAt14(1u);
}

void EndingSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
}

void EndingSceneRuntime::SetInputCompletionCheckEnabled(const bool enabled) noexcept
{
    inputCompletionCheckEnabled_ = enabled;
}

std::uint32_t EndingSceneRuntime::State() const noexcept
{
    return state_;
}

bool EndingSceneRuntime::CompletionRequested() const noexcept
{
    return completionRequested_;
}

bool EndingSceneRuntime::InputCompletionCheckEnabled() const noexcept
{
    return inputCompletionCheckEnabled_;
}

void EndingSceneRuntime::Update()
{
    // 4B5FC4..4B6043：完成请求命中时，本帧直接进入状态 2。
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
            services_->QueueScene(kTitleSceneNameCp932);
            break;

        case 0:
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

void EndingSceneRuntime::Finish()
{
    // 4B5E30：所有调用均为顺序提交；任一步抛出时不撤销此前写入。
    services_->WriteDescriptor2DwordAt04(1u);
    services_->ClearScreen(0);
    services_->InvalidateScreenRect(nullptr);
    services_->AdvanceActiveSceneFromPending();
}
}
