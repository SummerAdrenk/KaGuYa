#include <Runtime/TitleDialogRuntime.h>

#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
}

TitleDialogRuntime::TitleDialogRuntime(ITitleDialogServices& services,
    LoadAfterTransitionSourceRef blackAt78, LoadAfterTransitionSourceRef canvasAt84,
    const RECT& rectAt68) noexcept
    : services_(&services), black_(std::move(blackAt78)), canvas_(std::move(canvasAt84)), rect_(rectAt68)
{
}

void TitleDialogRuntime::InvokeVslot44()
{
    services_->ClearClickedEventsAt532200();
    services_->WriteDialogEnabledAt538660(1);
    services_->ResetInputAt510840();
    services_->ResetChildrenAt522E90();
    phase_ = 0;
    selection_ = 0xFFFFFFFFu;
}

std::uint32_t TitleDialogRuntime::SelectionResult() const
{
    if (!selection_) throw std::logic_error("TitleDialog result queried before initialization");
    return *selection_;
}

bool TitleDialogRuntime::Run()
{
    if (!phase_) throw std::logic_error("TitleDialog run before initialization");
    switch (*phase_)
    {
    case 0:
        phase_ = 1;
        transitionPhase_ = 0;
        [[fallthrough]];
    case 1: case 2:
        AdvanceTransitionAt51CC70();
        return true;
    case 3:
        ProcessSelectionAt522BD0();
        DrawDirtyAt525FB0();
        return true;
    case 4:
        phase_ = -1;
        return false;
    case 5:
        services_->ResetChildrenAt522E90();
        services_->ResetInputAt510840();
        (void)services_->OwnDirtyRectsAtVslot00().AddAtVslot08(rect_);
        phase_ = 3;
        DrawDirtyAt525FB0();
        return true;
    default:
        return false;
    }
}

void TitleDialogRuntime::AdvanceTransitionAt51CC70()
{
    switch (transitionPhase_)
    {
    case 0:
        if (*phase_ == 1)
        {
            services_->CompositeChildrenAt5231C0(canvas_, nullptr);
            transition_.Reset();
            transition_.selectorAt1C = 57;
            // 444770 返回的指针放在 +94，operation+04 以强引用表示该源。
            transition_.sourceAt04 = canvas_;
            const char* const name = services_->ReadTitleBgmNameAt61A654();
            const auto volume = services_->ReadRegistrySlot1BgmVolumeAtVslot14();
            services_->WriteRegistrySlot2BgmNameAt2C(name);
            services_->PlayBgmAt42BFE0(name, volume, 1);
            services_->ClearSoundManagerAt4384D0();
            services_->StopSoundNamedByRegistrySlot2At30();
            nextPhase_ = 3;
        }
        else
        {
            transition_.Reset();
            transition_.selectorAt1C = 58;
            transition_.sourceAt04 = black_;
            nextPhase_ = 4;
        }
        services_->WriteRegistrySlot2DwordAt04(0);
        step_ = 20;
        transitionPhase_ = 1;
        return;
    case 1:
        // 完成判断发生在输入加速之前，即使此处完成仍执行下面的渲染和提交。
        if (transition_.selectorAt1C == -1 || transition_.progressAt24 >= transition_.maximumAt28)
            transitionPhase_ = 2;
        if (services_->ReadInputAt4C3430() || services_->ReadInputAt4AA970())
        {
            transition_.progressAt24 = Add(transition_.progressAt24, 256);
            if (transition_.progressAt24 >= transition_.maximumAt28)
                transition_.progressAt24 = transition_.maximumAt28;
        }
        services_->RenderTransitionAt449DD0(transition_);
        transition_.progressAt24 = Add(transition_.progressAt24, step_);
        if (transition_.progressAt24 >= transition_.maximumAt28)
            transition_.progressAt24 = transition_.maximumAt28;
        services_->CommitTransitionFrameAt4B02E0(0);
        return;
    case 2:
        phase_ = nextPhase_;
        services_->WriteRegistrySlot2DwordAt04(4);
        return;
    default:
        return;
    }
}

void TitleDialogRuntime::ProcessSelectionAt522BD0()
{
    if (services_->ReadInputAt47F6D0())
    {
        if (services_->ReadSceneManagerAt485CE0())
        {
            selection_ = 1u;
            phase_ = 2;
            transitionPhase_ = 0;
        }
        return;
    }
    services_->UpdateGuiDialogAt518C30();
    const auto event = services_->PollClickedEventAt50FDB0(1);
    if (event < 3 || event > 8) return;
    services_->PlayConfirmSoundAt4BCD70();
    if (event == 8)
    {
        services_->PostCloseMessage();
        phase_ = 5;
        return;
    }
    if (event == 4)
    {
        services_->WriteCurrentSceneName("\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA");
        phase_ = 5;
        return;
    }
    if (event == 3 && services_->ReadObjectAt50A620DwordAt1C() != 0u &&
        services_->ReadRegistrySlot1ByteAt54() == 0u)
    {
        auto confirmation = services_->CreateConfirmationAt4E4340();
        if (!confirmation) throw std::runtime_error("Title confirmation factory returned null");
        services_->PrepareModalAt4AD230();
        if (services_->RunModalAt5071D0(*confirmation) != 6)
        {
            phase_ = 5;
            return;
        }
    }
    selection_ = static_cast<std::uint32_t>(event - 3);
    phase_ = 2;
    transitionPhase_ = 0;
}

void TitleDialogRuntime::DrawDirtyAt525FB0()
{
    services_->OwnDirtyRectsAtVslot00().PrepareAtVslot04();
    const auto count = services_->OwnDirtyRectsAtVslot00().CountAtVslot18();
    if (count == 0u) return;
    const auto rects = services_->OwnDirtyRectsAtVslot00().RectsAtVslot1C();
    if (count > rects.size()) throw std::out_of_range("Title dirty rectangle count exceeds storage");
    for (std::size_t i = 0; i < count; ++i)
    {
        services_->DrawChildrenAt5230B0(rects[i]);
        services_->InvalidatedRect(rects[i]);
    }
    services_->OwnDirtyRectsAtVslot00().ClearAtVslot00();
}

void TitleDialogRuntime::RedrawAllAt530E60()
{
    // +8C 不读取 phase；先将 +68 RECT 放入 dirty，再调用 +58。
    (void)services_->OwnDirtyRectsAtVslot00().AddAtVslot08(rect_);
    DrawDirtyAt525FB0();
}
}
