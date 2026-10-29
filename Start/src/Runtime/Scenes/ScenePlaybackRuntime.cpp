#include <Runtime/ScenePlaybackRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kCg[] = "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char kMusic[] = "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
constexpr char kTitle[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B";
constexpr char kSettings[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char kScript[] = "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
// 原 PE 5B52EC / 5B530C，不用 UTF-8 字面值传给 MessageBoxA。
constexpr char kMissingReplay[] = "\x83\x56\x83\x69\x83\x8A\x83\x49\x8D\xC4\x90\xB6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
constexpr char kInvalidReplay[] = "\x83\x56\x83\x69\x83\x8A\x83\x49\x8D\xC4\x90\xB6\x82\xCC\x90\xDD\x92\xE8\x82\xAA\x82\xA8\x82\xA9\x82\xB5\x82\xA2\x81\x49";

std::int32_t Add(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Subtract(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
}

ScenePlaybackRuntime::ScenePlaybackRuntime(IScenePlaybackServices& services)
    : ScenePlaybackRuntime(services, services)
{
}

ScenePlaybackRuntime::ScenePlaybackRuntime(IScenePlaybackServices& services,
    IScenePlaybackScriptServices& scriptServices)
    : ScenePlaybackRuntime(services, services, scriptServices)
{
}

ScenePlaybackRuntime::ScenePlaybackRuntime(IScenePlaybackHostServices& services,
    IScenePlaybackButtonFactory& buttons, IScenePlaybackScriptServices& scriptServices)
    : services_(&services), scriptServices_(&scriptServices)
{
    try
    {
        black_ = services_->LoadTransitionSource("bg_black.bgd");
        if (!black_) throw 1;
        auxiliary_ = services_->CreateAuxiliarySourceAt40C2E0();
        if (!services_->InitializeAuxiliarySourceAt415010(auxiliary_)) throw 1;
        services_->InvokeAt4703D0(1);
        for (std::int32_t i = 0; i < 5; ++i)
            buttons_.push_back(buttons.CreateButtonAt49F270(i));
        group_ = 0;
        services_->WriteRegistrySlot2DwordAt04(0);
        services_->WriteRegistrySlot2ByteAt09(0);
        services_->WriteRegistrySlot2ByteAt24(1);
        services_->WriteRegistrySlot2ByteAt25(0);
        services_->WriteRegistrySlot2ByteAt26(1);
        services_->WriteRegistrySlot2ByteAt27(1);
        phase_ = 14;
        services_->WriteRootByteAt218(1);
        services_->WriteRootByteAt219(1);
        services_->WriteSceneControllerDwordAt40(1);
    }
    catch (...)
    {
        // 5F0450 的 EH 表：type=0、handler=49F20B。原 handler 不回滚部分构造。
        services_->QueueTitleAfterConstructionFailureAt47E180();
    }
}

ScenePlaybackRuntime::~ScenePlaybackRuntime()
{
    // 操作块原本只借用指针，不得延长源对象到按钮释放之后。
    operation_.Reset();
    auxiliary_.reset();
    captured_.reset();
    black_.reset();
    for (auto& button : buttons_) button.reset();
}

IScenePlaybackButton& ScenePlaybackRuntime::CurrentButton()
{
    if (group_ < 0 || static_cast<std::size_t>(group_) >= buttons_.size())
        throw std::out_of_range("CModeReplay button group is out of range");
    const auto& button = buttons_[static_cast<std::size_t>(group_)];
    if (!button) throw std::logic_error("CModeReplay button is null");
    return *button;
}

void ScenePlaybackRuntime::AdvanceTransition()
{
    // 4B81D5..4B82BE：完成判定先于绘制，步进后才提交 dirty rect。
    if (services_->ShouldAccelerateTransitionAt4AA970())
    {
        operation_.progressAt24 = Add(operation_.progressAt24, 256);
        if (operation_.progressAt24 >= operation_.maximumAt28)
            operation_.progressAt24 = operation_.maximumAt28;
    }
    if (operation_.selectorAt1C == -1 ||
        operation_.progressAt24 >= operation_.maximumAt28)
        ++*phase_;
    services_->RenderTransition(operation_);
    operation_.progressAt24 = Add(operation_.progressAt24, transitionStep_);
    if (operation_.progressAt24 >= operation_.maximumAt28)
        operation_.progressAt24 = operation_.maximumAt28;
    services_->CommitTransitionFrameAt4B02E0(0);
}

void ScenePlaybackRuntime::BeginListTransition()
{
    operation_.Reset();
    operation_.selectorAt1C = 57;
    operation_.sourceAt04 = auxiliary_;
    transitionStep_ = 20;
    services_->InitializeEntrySubmissionAt4BCCC0();
    services_->ClearSoundManagerEntriesAt4384D0();
    services_->StopSoundEntryNamedByRegistrySlot2At30();
    services_->WriteRegistrySlot2DwordAt04(0);
    ++*phase_;
    AdvanceTransition();
}

void ScenePlaybackRuntime::PollList()
{
    if (services_->ReadCurrentSceneName() == kSettings)
    {
        services_->ClearCurrentSceneName();
        services_->PushActiveSceneAt4A9030(kSettings);
        return;
    }
    const auto result = CurrentButton().ProcessSelectionAtVslot04();
    selectedItem_ = result;
    if (result < CurrentButton().ItemCountAt3C())
    {
        if (result >= 0)
        {
            phase_ = 7;
            segment_ = 0;
            services_->WriteSceneControllerDwordAt40(0);
            CurrentButton().RenderDirtyRegionsAtVslot34();
            return;
        }
        std::optional<std::int32_t> route;
        if (services_->ReadCurrentSceneName() == kCg) route = 0;
        else if (services_->ReadCurrentSceneName() == kMusic) route = 2;
        else if (services_->ReadReturnFallbackSceneNameAt4ADAC0() == kTitle ||
                 services_->ReadTitleReturnGateAt4B0740()) route = 3;
        if (!route)
        {
            CurrentButton().RenderDirtyRegionsAtVslot34();
            return;
        }
        services_->ClearCurrentSceneName();
        exitRoute_ = *route;
    }
    else
    {
        const auto route = Subtract(result, CurrentButton().ItemCountAt3C());
        if (route >= services_->ReadExitRouteCountAt47B7B0())
        {
            // 4B84E2..4B8538 再读 itemCount 和 routeCount，先提交 group 再 Reset。
            const auto items = CurrentButton().ItemCountAt3C();
            const auto routes = services_->ReadExitRouteCountAt47B7B0();
            const auto next = Subtract(Subtract(selectedItem_, routes), items);
            if (group_ != next)
            {
                group_ = next;
                CurrentButton().ResetGroupAt4C1260(1);
            }
            CurrentButton().RenderDirtyRegionsAtVslot34();
            return;
        }
        exitRoute_ = route;
    }
    phase_ = 4;
    CurrentButton().RenderDirtyRegionsAtVslot34();
}

void ScenePlaybackRuntime::BeginCapturedBlackTransition()
{
    captured_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->WriteRegistrySlot2DwordAt04(0);
    operation_.Reset();
    operation_.selectorAt1C = 0;
    operation_.sourceAt04 = black_;
    const auto target = services_->ReadTransitionSourceValueAt04(captured_);
    ++*phase_;
    operation_.valueAt08 = target;
    transitionStep_ = 16;
}

void ScenePlaybackRuntime::PrepareReplay()
{
    services_->StopSoundManagerEntryNamedAt10();
    services_->ClearSoundManagerEntriesAt4384D0();
    services_->StopSoundEntryNamedByRegistrySlot2At30();
    const auto selection = scriptServices_->FindSelectionAt4ADAE0(group_, selectedItem_, segment_);
    if (!selection)
    {
        services_->ShowReplayError(kMissingReplay);
        phase_ = 14;
        return;
    }
    const auto name = scriptServices_->ResolveScriptNameAt4AD1B0(*selection);
    scriptServices_->LoadReplayScriptAt4D53D0(name.c_str(), selection->entry);
    if (scriptServices_->ExecuteScriptAt4D3FF0() != 3)
    {
        services_->ShowReplayError(kInvalidReplay);
        phase_ = 14;
        return;
    }
    scriptServices_->WriteScriptDwordAt04(0);
    scriptServices_->WriteScriptByteAt08(0);
    services_->InvokeGroup0At4C4330();
    services_->InvokeGroup0At4C4250();
    services_->InvokeRootAt4AC490();
    captured_ = services_->CapturePrimarySceneResourceAt4ACDD0();
    services_->InvokeAt4AA890(0);
    services_->ClearCurrentSceneName();
    operation_.Reset();
    operation_.selectorAt1C = 0;
    operation_.sourceAt04 = captured_;
    operation_.valueAt08 = services_->ReadTransitionSourceValueAt04(black_);
    transitionStep_ = 16;
    savedRegistryValue_ = services_->ReadRegistrySlot2DwordAt04();
    services_->WriteRegistrySlot2DwordAt04(0);
    ++*phase_;
    // phase 9 在此返回；不同于 phase 7/12，不在本帧推进转场。
}

bool ScenePlaybackRuntime::ResetOrContinueReplay()
{
    if (services_->ReadSceneControllerDwordAt40() == 0)
    {
        segment_ = Add(segment_, 1);
        const auto next = scriptServices_->FindSelectionAt4ADAE0(group_, selectedItem_, segment_);
        if (next)
        {
            phase_ = 7;
            return true;
        }
        services_->WriteSceneControllerDwordAt40(1);
    }
    CurrentButton().ResetGroupAt4C1260(0);
    CurrentButton().CompositeIntoAtVslot38(auxiliary_, nullptr);
    phase_ = 0;
    services_->WriteRootByteAt218(1);
    BeginListTransition();
    return false;
}

void ScenePlaybackRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    // 构造失败尚未写 phase 时不能读取未初始化内存，明确保留未装配状态。
    while (phase_)
    {
        switch (*phase_)
        {
        case 0: BeginListTransition(); return;
        case 1: case 5: case 8: case 10: case 13: AdvanceTransition(); return;
        case 2:
            services_->WriteRegistrySlot2DwordAt04(3);
            services_->WriteRegistrySlot2DwordAt14(1);
            phase_ = 3;
            [[fallthrough]];
        case 3: PollList(); return;
        case 4:
            operation_.Reset();
            operation_.selectorAt1C = 58;
            operation_.sourceAt04 = black_;
            transitionStep_ = 20;
            phase_ = 5;
            AdvanceTransition();
            return;
        case 6:
            services_->QueuePendingSceneAt47F920(
                exitRoute_ == 0 ? kCg : exitRoute_ == 2 ? kMusic : kTitle);
            return;
        case 7: case 12:
            BeginCapturedBlackTransition();
            continue;
        case 9: PrepareReplay(); return;
        case 11:
            services_->WriteRootServiceDwordAt10(1);
            services_->WriteRegistrySlot2DwordAt04(savedRegistryValue_);
            services_->PushActiveSceneAt4A9030(kScript);
            phase_ = 12;
            scriptServices_->WriteScriptByteAt08(1);
            services_->WriteRootByteAt218(0);
            services_->WriteRootByteAt219(1);
            return;
        case 14:
            if (ResetOrContinueReplay()) continue;
            return;
        default: return;
        }
    }
}
}
