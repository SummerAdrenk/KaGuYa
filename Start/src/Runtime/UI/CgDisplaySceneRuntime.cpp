#include <Runtime/CgDisplaySceneRuntime.h>

#include <algorithm>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kBlackTransitionResource[] = "bg_black.bgd";
constexpr char kSettingSceneCp932[] =
    "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char kScenePlaybackModeCp932[] =
    "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6"
    "\x83\x82\x81\x5B\x83\x68";
constexpr char kMusicAppreciationModeCp932[] =
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
constexpr char kTitleSceneCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B";
}

void CgDisplaySelectionState::Clear()
{
    countAt18 = 0;
    valueAt1C = 0;
    xAt20 = 0;
    yAt24 = 0;
    selectedMappingIndexAt28 = 0;
    mappingPairsAt2C.clear();
    availability.clear();
}

CgDisplaySceneRuntime::CgDisplaySceneRuntime(
    ICgDisplaySceneServices& services)
    : services_(&services)
{
    try
    {
        services_->ClearCurrentSceneName();
        blackTransitionSource_ =
            services_->LoadTransitionSource(kBlackTransitionResource);
        if (!blackTransitionSource_)
            throw 1;

        auxiliaryListSource_ = services_->CreateAuxiliaryListSourceAt40C2E0();
        if (!services_->InitializeAuxiliaryListSourceAt415010(
                auxiliaryListSource_))
        {
            throw 1;
        }
        services_->InvokeAt4703D0WithNull();

        buttons_.reserve(5u);
        for (std::int32_t index = 0; index < 5; ++index)
            buttons_.push_back(services_->CreateCgDisplayButton(index));

        displayEnabled_ = false;
        currentButtonIndex_ = 0;
        displayWindow_ = services_->CreateCgDisplayWindow();
        operationWindow_ = services_->CreateCgOperationWindow();

        services_->WriteRegistrySlot2DwordAt04(0u);
        services_->WriteRegistrySlot2ByteAt09(0u);
        services_->WriteRegistrySlot2ByteAt24(0u);
        services_->WriteRegistrySlot2ByteAt25(1u);
        services_->WriteRegistrySlot2ByteAt26(1u);
        services_->WriteRegistrySlot2ByteAt27(1u);
        phase_ = CgDisplayPhase::ResetToList;
        services_->WriteRootByteAt218(1u);
        services_->WriteRootByteAt219(1u);
    }
    catch (...)
    {
        // 49EC22 的 catch handler 只排入标题，不恢复此前 registry/root 副作用。
        services_->QueueTitleAfterConstructionFailureAt47E180();
    }
}

CgDisplaySceneRuntime::~CgDisplaySceneRuntime()
{
    // 0x4A4720 固定先拆 CGOpeWin，再拆 CCGDispWin；随后依次释放
    // auxiliary、captured、bg_black，最后按 0..4 释放按钮与选择表。
    operationWindow_.reset();
    displayWindow_.reset();

    // 当前强类型 transition 会额外持有同一 source；原对象没有比三个
    // 场景成员更晚释放的副本，因此必须先清掉该引用。
    transition_.Reset();
    auxiliaryListSource_.reset();
    capturedSceneSource_.reset();
    blackTransitionSource_.reset();

    for (auto& button : buttons_)
        button.reset();
    buttons_.clear();
    selection_.Clear();
}

void CgDisplaySceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    // 0x4B6A97 将状态作为无符号值与 0x10 比较；构造失败留下的
    // 0xFFFFFFFF 直接进入 default 返回。随后 0x4B5970 也会因 +0x0C 为 0
    // 跳过绘制，因此这里保持可调用的空操作语义。
    Update();
    Draw();
}

bool CgDisplaySceneRuntime::IsOperational() const noexcept
{
    return phase_.has_value();
}

std::optional<CgDisplayPhase> CgDisplaySceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::int32_t CgDisplaySceneRuntime::CurrentButtonIndex() const noexcept
{
    return currentButtonIndex_;
}

std::int32_t CgDisplaySceneRuntime::ExitRoute() const noexcept
{
    return exitRoute_;
}

bool CgDisplaySceneRuntime::DisplayEnabled() const noexcept
{
    return displayEnabled_;
}

const CgDisplaySelectionState&
CgDisplaySceneRuntime::SelectionState() const noexcept
{
    return selection_;
}

const CgDisplayTransitionOperation&
CgDisplaySceneRuntime::Transition() const noexcept
{
    return transition_;
}

CgDisplayButtonRef& CgDisplaySceneRuntime::CurrentButton()
{
    if (currentButtonIndex_ < 0 ||
        static_cast<std::size_t>(currentButtonIndex_) >= buttons_.size())
    {
        throw std::out_of_range("CModeCGdisp current button index is out of range");
    }
    auto& button = buttons_[static_cast<std::size_t>(currentButtonIndex_)];
    if (!button)
        throw std::logic_error("CModeCGdisp CCGdispBtn is null");
    return button;
}

const CgDisplayButtonRef& CgDisplaySceneRuntime::CurrentButton() const
{
    if (currentButtonIndex_ < 0 ||
        static_cast<std::size_t>(currentButtonIndex_) >= buttons_.size())
    {
        throw std::out_of_range("CModeCGdisp current button index is out of range");
    }
    const auto& button = buttons_[static_cast<std::size_t>(currentButtonIndex_)];
    if (!button)
        throw std::logic_error("CModeCGdisp CCGdispBtn is null");
    return button;
}

void CgDisplaySceneRuntime::InitializeTransition(
    const std::int32_t selector,
    const CgDisplayTransitionSourceRef& source)
{
    transition_.Reset();
    transition_.selectorAt1C = selector;
    transition_.sourceAt04 = source;
    transitionStep_ = 20;
    phase_ = static_cast<CgDisplayPhase>(
        static_cast<std::uint32_t>(*phase_) + 1u);
}

void CgDisplaySceneRuntime::AdvanceTransition()
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
        phase_ = static_cast<CgDisplayPhase>(
            static_cast<std::uint32_t>(*phase_) + 1u);
    }

    services_->RenderTransition(transition_);
    transition_.progressAt24 += transitionStep_;
    if (transition_.progressAt24 >= transition_.maximumAt28)
        transition_.progressAt24 = transition_.maximumAt28;
    services_->CommitTransitionFrameAt4B02E0(0);
}

std::optional<std::int32_t> CgDisplaySceneRuntime::ResolveReturnRoute() const
{
    // 0x4B768A 与 0x4B76DC 分别重新读取当前来源场景，不共享缓存值。
    if (services_->ReadReturnSourceSceneName() == kScenePlaybackModeCp932)
        return 1;
    if (services_->ReadReturnSourceSceneName() == kMusicAppreciationModeCp932)
        return 2;

    const auto fallback = services_->ReadReturnFallbackSceneName();
    if (fallback == kTitleSceneCp932 ||
        services_->ReadTitleReturnGateAt4B0740())
    {
        return 3;
    }
    return std::nullopt;
}

void CgDisplaySceneRuntime::PollListInteraction()
{
    if (services_->ReadCurrentSceneName() == kSettingSceneCp932)
    {
        services_->ClearCurrentSceneName();
        services_->PushActiveSceneAt4A9030(kSettingSceneCp932);
        return;
    }

    auto& button = CurrentButton();
    const auto result = button->ProcessSelectionAtVslot04();
    const auto itemCount = button->ItemCountAt3C();
    if (result >= itemCount)
    {
        const auto route = result - itemCount;
        const auto routeCount = services_->ReadExitRouteCountAt47B7B0();
        if (route >= routeCount)
        {
            const auto nextButton = result - routeCount - itemCount;
            if (currentButtonIndex_ != nextButton)
            {
                currentButtonIndex_ = nextButton;
                static_cast<void>(CurrentButton());
                CurrentButton()->ResetGroupAt4C1260(1);
            }
        }
        else
        {
            exitRoute_ = route;
            phase_ = CgDisplayPhase::BeginExitTransition;
        }
        CurrentButton()->RenderDirtyRegionsAtVslot34();
        return;
    }

    if (result < 0)
    {
        if (const auto route = ResolveReturnRoute())
        {
            services_->ClearCurrentSceneName();
            exitRoute_ = *route;
            phase_ = CgDisplayPhase::BeginExitTransition;
        }
        button->RenderDirtyRegionsAtVslot34();
        return;
    }

    if (!operationWindow_)
        throw std::logic_error("CModeCGdisp CGOpeWin is null");
    operationWindow_->BeginSelectionAt502110();
    selection_.Clear();
    if (!services_->BuildCgSelectionAt4AD780(
            currentButtonIndex_, result, selection_))
    {
        return;
    }
    for (const bool available : selection_.availability)
        operationWindow_->AppendAvailabilityAt4FEA00(available);
    services_->FinalizeCgSelectionAt47E940();
    services_->WriteCgSelectionOwnerByteAt0C(0u);
    phase_ = CgDisplayPhase::BeginSelectedCgTransition;
    button->RenderDirtyRegionsAtVslot34();
}

void CgDisplaySceneRuntime::QueueExitScene()
{
    switch (exitRoute_)
    {
    case 1:
        services_->QueuePendingSceneAt47F920(kScenePlaybackModeCp932);
        return;
    case 2:
        services_->QueuePendingSceneAt47F920(kMusicAppreciationModeCp932);
        return;
    default:
        services_->QueuePendingSceneAt47F920(kTitleSceneCp932);
        return;
    }
}

void CgDisplaySceneRuntime::BeginCapturedBlackTransition()
{
    displayEnabled_ = false;
    capturedSceneSource_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->WriteRegistrySlot2DwordAt04(0u);
    InitializeTransition(0, blackTransitionSource_);
    transition_.valueAt08 =
        services_->ReadTransitionSourceValueAt04(capturedSceneSource_);
}

void CgDisplaySceneRuntime::PrepareSelectedCg()
{
    if (!services_->PrepareSelectedCgAt4A84E0(selection_, displayWindow_))
        return;

    displayEnabled_ = false;
    capturedSceneSource_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    transition_.Reset();
    transition_.selectorAt1C = 0;
    transition_.valueAt08 =
        services_->ReadTransitionSourceValueAt04(capturedSceneSource_);
    transitionStep_ = 20;
    displayWindow_->InvokeAt4C17F0(40);
    services_->SubmitCgDisplayWindowRectToParentDirtyRects(displayWindow_);
    services_->CompositeAuxiliaryListSource(auxiliaryListSource_);
    transition_.sourceAt04 = auxiliaryListSource_;
    services_->ClearDirtyRectsAt4AA780();
    phase_ = CgDisplayPhase::AnimateCgPresentationTransition;
}

void CgDisplaySceneRuntime::PollCgOperation()
{
    if (!displayWindow_ || !operationWindow_)
        throw std::logic_error("CModeCGdisp CG window is null");

    switch (operationWindow_->PollCommandAt4BD590())
    {
    case 1:
        services_->WriteRegistrySlot2DwordAt04(0u);
        displayWindow_->BeginAt4AA860();
        return;

    case 3:
        if (displayWindow_->ActiveByteAt54() == 0u)
        {
            operationWindow_->SubmitWindowVisibilityAt4C22B0(0u);
            phase_ = CgDisplayPhase::WaitForCgOperationClose;
            services_->WriteRegistrySlot2DwordAt04(0u);
        }
        else
        {
            displayWindow_->UpdateAt4BD050();
            return;
        }
        break;

    case 4:
        services_->WriteRegistrySlot2DwordAt04(0u);
        phase_ = CgDisplayPhase::BeginCgReturnTransition;
        services_->InvokeCgOperationCommand4At4BCD70();
        return;

    case 5:
        services_->WriteRegistrySlot2DwordAt04(5u);
        if (!operationWindow_->PollDialogCompletionAt4C2690())
            displayWindow_->InvokeAt4C13A0(0);
        return;

    case 6:
    {
        // 4B7A42 从 4A5240 返回的 CGOpeWin 内部 Dialog 读取 +0x7C 选择值。
        const auto selectionValue =
            operationWindow_->DialogSelectionValueAt7C();
        const auto found = std::find_if(
            selection_.mappingPairsAt2C.begin(),
            selection_.mappingPairsAt2C.end(),
            [selectionValue](const auto& pair) {
                return pair.first == selectionValue;
            });
        if (found == selection_.mappingPairsAt2C.end())
            return;
        const auto index = static_cast<std::int32_t>(
            std::distance(selection_.mappingPairsAt2C.begin(), found));
        if (selection_.selectedMappingIndexAt28 != index)
        {
            selection_.selectedMappingIndexAt28 = index;
            services_->WriteRegistrySlot2DwordAt04(0u);
            displayWindow_->BeginAt4AA860();
            phase_ = CgDisplayPhase::PrepareSelectedCg;
        }
        return;
    }

    default:
        break;
    }

    if (displayWindow_->ActiveByteAt54() != 0u ||
        !operationWindow_->PollDialogCompletionAt4C2690())
    {
        displayWindow_->UpdateAt4BD050();
    }
}

void CgDisplaySceneRuntime::WaitForCgOperationClose()
{
    if (!displayWindow_ || !operationWindow_)
        throw std::logic_error("CModeCGdisp CG window is null");

    displayWindow_->UpdateAt4BD050();
    if (displayWindow_->ActiveByteAt54() != 0u)
        return;
    if (!services_->ReadCgCloseInputAt47F4E0(0) &&
        !services_->ReadTitleReturnGateAt4B0740())
    {
        return;
    }
    operationWindow_->ReactivateWindowAndLayerAt4B7C24();
    services_->WriteRegistrySlot2DwordAt04(5u);
    phase_ = CgDisplayPhase::PollCgOperation;
}

void CgDisplaySceneRuntime::ResetToList()
{
    capturedSceneSource_.reset();
    services_->InvokeCgOwnerVslot08();
    CurrentButton()->ResetGroupAt4C1260(0);
    CurrentButton()->CompositeIntoAtVslot38(auxiliaryListSource_, nullptr);
    phase_ = CgDisplayPhase::BeginEntryTransition;
    services_->WriteRootByteAt219(1u);
}

void CgDisplaySceneRuntime::Update()
{
    while (phase_)
    {
        switch (*phase_)
        {
        case CgDisplayPhase::BeginEntryTransition:
            InitializeTransition(57, auxiliaryListSource_);
            services_->InitializeEntrySubmissionAt4BCCC0();
            continue;

        case CgDisplayPhase::AnimateEntryTransition:
        case CgDisplayPhase::AnimateExitTransition:
        case CgDisplayPhase::AnimateSelectedCgTransition:
        case CgDisplayPhase::AnimateCgPresentationTransition:
        case CgDisplayPhase::AnimateCgReturnTransition:
            AdvanceTransition();
            return;

        case CgDisplayPhase::EnableListInteraction:
            services_->WriteRegistrySlot2DwordAt04(3u);
            phase_ = CgDisplayPhase::PollListInteraction;
            continue;

        case CgDisplayPhase::PollListInteraction:
            PollListInteraction();
            return;

        case CgDisplayPhase::BeginExitTransition:
            InitializeTransition(58, blackTransitionSource_);
            continue;

        case CgDisplayPhase::QueueExitScene:
            QueueExitScene();
            return;

        case CgDisplayPhase::BeginSelectedCgTransition:
            if (!displayWindow_)
                throw std::logic_error("CModeCGdisp CCGDispWin is null");
            displayWindow_->WritePositionAt04And08(
                selection_.xAt20,
                selection_.yAt24);
            services_->WriteRootByteAt219(0u);
            BeginCapturedBlackTransition();
            continue;

        case CgDisplayPhase::PrepareSelectedCg:
            PrepareSelectedCg();
            if (*phase_ == CgDisplayPhase::PrepareSelectedCg)
                return;
            continue;

        case CgDisplayPhase::EnableCgOperation:
            capturedSceneSource_.reset();
            displayEnabled_ = true;
            phase_ = CgDisplayPhase::PollCgOperation;
            services_->WriteRegistrySlot2DwordAt04(5u);
            continue;

        case CgDisplayPhase::PollCgOperation:
            PollCgOperation();
            return;

        case CgDisplayPhase::WaitForCgOperationClose:
            WaitForCgOperationClose();
            return;

        case CgDisplayPhase::BeginCgReturnTransition:
            BeginCapturedBlackTransition();
            continue;

        case CgDisplayPhase::ResetToList:
            ResetToList();
            continue;
        }
    }
}

void CgDisplaySceneRuntime::Draw()
{
    if (!displayEnabled_)
        return;
    if (services_->ReadRegistrySlot1ByteAt6D())
    {
        if (!displayWindow_)
            throw std::logic_error("CModeCGdisp CCGDispWin is null");
        displayWindow_->UpdateAnimationAndDirtyRectsAtVslot14();
    }
    services_->DrawCgGlobalLayerAtVslot18();
}
}
