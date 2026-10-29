#include <Runtime/MusicAppreciationSceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kBlackTransitionResource[] = "bg_black.bgd";
constexpr char kCgModeSceneCp932[] =
    "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char kScenePlaybackModeSceneCp932[] =
    "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6"
    "\x83\x82\x81\x5B\x83\x68";
constexpr char kTitleSceneCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B";
constexpr char kSettingSceneCp932[] =
    "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";

}

MusicAppreciationSceneRuntime::MusicAppreciationSceneRuntime(
    IMusicAppreciationSceneServices& services)
    : services_(&services)
{
    blackTransitionSource_ =
        services_->LoadTransitionSource(kBlackTransitionResource);
    if (!blackTransitionSource_)
        return;

    services_->PrepareSceneControllerAt4703D0(2);
    button_ = services_->CreateMusicAppreciationButton();
    primarySceneResource_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->StopMusicEntryAt4AA890(0);
    services_->ResetMusicEntriesAt4AA780();
    services_->ClearCurrentSceneName();
    services_->WriteRegistrySlot2DwordAt04(0u);
    services_->WriteRegistrySlot2ByteAt09(0u);
    services_->WriteRegistrySlot2ByteAt24(1u);
    services_->WriteRegistrySlot2ByteAt25(1u);
    services_->WriteRegistrySlot2ByteAt26(0u);
    services_->ClearRegistrySlot2StringAt2C();
    services_->InvokeSoundProcedureAt4383B0();
    phase_ = MusicAppreciationPhase::BeginEntryTransition;
    services_->WriteRootByteAt218(1u);
    services_->WriteRootByteAt219(1u);
}

void MusicAppreciationSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    if (!phase_)
        throw std::logic_error(
            "CModeSound cannot run after bg_black.bgd load failure");
    Update();
    // 0x5B50D0 -> 0x4B5E20：Draw 是严格空函数。
}

bool MusicAppreciationSceneRuntime::IsOperational() const noexcept
{
    return phase_.has_value();
}

std::optional<MusicAppreciationPhase>
MusicAppreciationSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::int32_t MusicAppreciationSceneRuntime::ExitRoute() const noexcept
{
    return exitRoute_;
}

const MusicAppreciationTransitionOperation&
MusicAppreciationSceneRuntime::Transition() const noexcept
{
    return transition_;
}

const MusicAppreciationButtonRef&
MusicAppreciationSceneRuntime::Button() const noexcept
{
    return button_;
}

void MusicAppreciationSceneRuntime::EnterResetAfterExternalMovieCompletion()
{
    if (!phase_ || *phase_ !=
        MusicAppreciationPhase::WaitForExternalMovieCompletion)
    {
        throw std::logic_error(
            "CModeSound external movie completion outside phase 9");
    }
    phase_ = MusicAppreciationPhase::ResetAfterMovie;
}

void MusicAppreciationSceneRuntime::InitializeTransition(
    const std::int32_t selector,
    const MusicAppreciationTransitionSourceRef& source)
{
    transition_.Reset();
    transition_.selectorAt1C = selector;
    transition_.sourceAt04 = source;
    transitionStep_ = 20;
    phase_ = static_cast<MusicAppreciationPhase>(
        static_cast<std::uint32_t>(*phase_) + 1u);
    services_->WriteRegistrySlot2DwordAt04(0u);
}

void MusicAppreciationSceneRuntime::AdvanceTransition()
{
    if (services_->ShouldAccelerateTransitionAt4AA970())
    {
        transition_.progressAt24 += 256;
        if (transition_.progressAt24 >= transition_.maximumAt28)
            transition_.progressAt24 = transition_.maximumAt28;
    }

    if (transition_.selectorAt1C == -1 ||
        transition_.progressAt24 >= transition_.maximumAt28)
    {
        phase_ = static_cast<MusicAppreciationPhase>(
            static_cast<std::uint32_t>(*phase_) + 1u);
    }

    services_->RenderTransition(transition_);
    transition_.progressAt24 += transitionStep_;
    if (transition_.progressAt24 >= transition_.maximumAt28)
        transition_.progressAt24 = transition_.maximumAt28;
    services_->CommitTransitionFrameAt4B02E0(0);
}

std::optional<std::int32_t>
MusicAppreciationSceneRuntime::ResolveReturnRoute() const
{
    const auto source = services_->ReadReturnSourceSceneName();
    if (source == kCgModeSceneCp932)
        return 0;
    if (source == kScenePlaybackModeSceneCp932)
        return 1;

    const auto fallback = services_->ReadReturnFallbackSceneName();
    if (fallback == kTitleSceneCp932 ||
        services_->ReadTitleReturnGateAt4B0740())
    {
        return 3;
    }
    return std::nullopt;
}

void MusicAppreciationSceneRuntime::PollInteraction()
{
    if (services_->ReadCurrentSceneName() == kSettingSceneCp932)
    {
        services_->ClearCurrentSceneName();
        services_->PushActiveSceneAt4A9030(kSettingSceneCp932);
        return;
    }

    if (!button_)
        throw std::logic_error("CModeSound CSoundBtn is null");

    const auto result = button_->ProcessSelectionAtVslot04();
    const auto itemCount = button_->ItemCountAt3C();
    if (result >= itemCount)
    {
        exitRoute_ = result - itemCount;
        phase_ = MusicAppreciationPhase::BeginExitTransition;
    }
    else if (button_->MovieSelectionPendingAt6C() != 0u)
    {
        phase_ = MusicAppreciationPhase::BeginMovieTransition;
    }
    else if (const auto route = ResolveReturnRoute())
    {
        exitRoute_ = *route;
        phase_ = MusicAppreciationPhase::BeginExitTransition;
    }

    services_->SynchronizeButtonAt4ABB40(button_);
}

void MusicAppreciationSceneRuntime::QueueExitScene()
{
    switch (exitRoute_)
    {
    case 0:
        services_->QueuePendingSceneAt47F920(kCgModeSceneCp932);
        return;
    case 1:
        services_->QueuePendingSceneAt47F920(kScenePlaybackModeSceneCp932);
        return;
    default:
        services_->QueuePendingSceneAt47F920(kTitleSceneCp932);
        return;
    }
}

void MusicAppreciationSceneRuntime::ResetAfterMovie()
{
    if (!button_)
        throw std::logic_error("CModeSound CSoundBtn is null");

    const auto selectedItem = button_->SelectedItemAt68();
    if (selectedItem != -1)
        button_->ClearSelectedItemStateAt04And20(selectedItem);
    button_->WriteSelectedItemAt68(-1);

    std::int32_t width = 0;
    std::int32_t height = 0;
    services_->ReadGraphicsTargetSize(width, height);
    const RECT fullTarget{0, 0, width, height};
    button_->InvalidateAtVslot08(fullTarget);
    services_->SynchronizeButtonAt4ABB40(button_);
    button_->WriteMovieSelectionPendingAt6C(0u);

    primarySceneResource_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->StopMusicEntryAt4AA890(0);
    services_->ResetMusicEntriesAt4AA780();
    phase_ = MusicAppreciationPhase::BeginEntryTransition;
}

void MusicAppreciationSceneRuntime::Update()
{
    while (phase_)
    {
        switch (*phase_)
        {
        case MusicAppreciationPhase::BeginEntryTransition:
            InitializeTransition(57, primarySceneResource_);
            continue;

        case MusicAppreciationPhase::AnimateEntryTransition:
        case MusicAppreciationPhase::AnimateExitTransition:
        case MusicAppreciationPhase::AnimateMovieTransition:
            AdvanceTransition();
            return;

        case MusicAppreciationPhase::EnableInteraction:
            services_->WriteRegistrySlot2DwordAt04(3u);
            phase_ = MusicAppreciationPhase::PollInteraction;
            continue;

        case MusicAppreciationPhase::PollInteraction:
            PollInteraction();
            return;

        case MusicAppreciationPhase::BeginExitTransition:
        case MusicAppreciationPhase::BeginMovieTransition:
            InitializeTransition(58, blackTransitionSource_);
            continue;

        case MusicAppreciationPhase::QueueExitScene:
            QueueExitScene();
            return;

        case MusicAppreciationPhase::WaitForExternalMovieCompletion:
            return;

        case MusicAppreciationPhase::ResetAfterMovie:
            ResetAfterMovie();
            continue;
        }
    }
}
}
