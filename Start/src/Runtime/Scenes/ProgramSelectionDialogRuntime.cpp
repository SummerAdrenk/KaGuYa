#include <Runtime/ProgramSelectionDialogRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t WrapAdd(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) +
        std::bit_cast<std::uint32_t>(right));
}
}

void ProgramSelectionDialogTransitionOperation::Reset() noexcept
{
    valueAt00 = 0;
    sourceAt04 = 0;
    valueAt08 = 0;
    valueAt0C = 0;
    valueAt10 = 0;
    valueAt14 = 0;
    valueAt18 = 0;
    selectorAt1C = -1;
    valueAt20 = 0;
    progressAt24 = 0;
    maximumAt28 = 255;
    valueAt2C = 0;
}

ProgramSelectionDialogRuntime::ProgramSelectionDialogRuntime(
    const ProgramSelectionDialogMode mode,
    IProgramSelectionDialogCoreServices& services)
    : mode_(mode), services_(&services)
{
    if (mode != ProgramSelectionDialogMode::Mode00 &&
        mode != ProgramSelectionDialogMode::Mode01)
    {
        throw 1;
    }
}

void ProgramSelectionDialogRuntime::BeginAtVslot44()
{
    // 0x50E770：启用、重置基类 child、启动注册 child，最后才清结果和状态。
    services_->WriteDialogEnabledAtVslot24(1);
    services_->ResetBaseChildrenAt522E90();
    services_->BeginRegisteredChildrenAtVslot44();
    state_.resultAt94 = -1;
    state_.stateAt68 = 0;
}

ProgramSelectionReference ProgramSelectionDialogRuntime::
ReadReferenceAtVslot9C()
{
    // 0x5038A0/0x5039A0 的调用方固定传入 1，不执行 child 0 的临时禁用。
    return services_->CreateSelectionReferenceFromChildZeroAt503AA0();
}

bool ProgramSelectionDialogRuntime::ReadBusyAtVslot48() const
{
    // 0x52F6E0 的 case 0 会在同一次调用中继续执行 +0x98。
    switch (state_.stateAt68)
    {
    case 0:
        state_.stateAt68 = 1;
        state_.substateAt6C = 0;
        [[fallthrough]];
    case 1:
    case 2:
        AdvanceTransitionAtVslot98();
        return true;
    case 3:
        CompleteSelectedChildAtVslot94();
        services_->ProcessDialogAtVslot58();
        return true;
    case 4:
        state_.stateAt68 = -1;
        return false;
    default:
        return false;
    }
}

void ProgramSelectionDialogRuntime::CloseAtVslot4C()
{
    // 0x509340：先关闭窗口，再把 +0x94 写入 prog_result。
    services_->WriteDialogEnabledAtVslot24(0);
    services_->WriteProgramResultAt509340(state_.resultAt94);
}

void ProgramSelectionDialogRuntime::ReopenAtVslotA4()
{
    // 0x530E80/0x531010 仅有目标 child ID 不同：模式 00 为 55，模式 01 为 30。
    const std::int32_t childId =
        mode_ == ProgramSelectionDialogMode::Mode00 ? 55 : 30;
    const std::int32_t childValue = static_cast<std::int32_t>(
        (services_->ReadSystemInformationSlot2DwordAt14() >> 1u) & 1u);
    services_->WriteChildDwordAt5C(childId, childValue);
    services_->InvokeChildAtVslot44(childId, 0);
    services_->ApplyDialogRectToRootAt530E40();
    services_->ProcessDialogAtVslot58();
}

ProgramSelectionDialogMode ProgramSelectionDialogRuntime::Mode() const noexcept
{
    return mode_;
}

const ProgramSelectionDialogState& ProgramSelectionDialogRuntime::
State() const noexcept
{
    return state_;
}

void ProgramSelectionDialogRuntime::AdvanceTransitionAtVslot98() const
{
    // 0x51C120 与 0x51C4E0 逐字节同构；模式差异仅来自各自持有的对象图。
    switch (state_.substateAt6C)
    {
    case 0:
    {
        const bool entering = state_.stateAt68 == 1;
        if (entering)
            services_->CompositeFrameAtA4AtVslot5C(0);

        auto& operation = state_.transitionAtB0;
        operation.Reset();
        operation.selectorAt1C = 0;
        operation.sourceAt04 = services_->ResolveFrameSourceAt444770(
            entering ? ProgramSelectionDialogFrameSlot::AtA4 :
                       ProgramSelectionDialogFrameSlot::At98,
            0);
        state_.pendingStateAt70 = entering ? 3 : 4;

        if (entering)
        {
            std::uint32_t time = static_cast<std::uint32_t>(
                services_->ReadRegistryTimeAt51C120());
            if (time >= 3u)
                time = 0;

            const std::string soundName =
                services_->ReadTimeSoundNameAt61A664(time);
            const std::int32_t value =
                services_->ReadProgramSelectionValueAtVslot14();
            services_->WriteScriptSlot2StringAt2C(soundName);
            services_->ApplyTimeSoundAt42BFE0(soundName, value, 1);
            services_->StopProgramSelectionSoundsAt4384D0();
            services_->ApplyScriptSlot2SoundStopListAt413270();
        }

        services_->WriteScriptSlot2DwordAt04(0);
        state_.transitionStepAtE0 =
            services_->ResolveTransitionStepAt497900();
        state_.substateAt6C = 1;
        return;
    }

    case 1:
    {
        auto& operation = state_.transitionAtB0;
        if (operation.selectorAt1C == -1 ||
            operation.progressAt24 >= operation.maximumAt28)
        {
            state_.substateAt6C = 2;
        }

        if (services_->PollTransitionAccelerationAt4C3430() ||
            services_->PollTransitionAccelerationAt4AA970())
        {
            operation.progressAt24 = WrapAdd(operation.progressAt24, 256);
            if (operation.progressAt24 >= operation.maximumAt28)
                operation.progressAt24 = operation.maximumAt28;
        }

        services_->SynchronizeRuntimeAt413BE0();
        services_->RenderTransitionAt449DD0(operation);
        operation.progressAt24 = WrapAdd(
            operation.progressAt24,
            state_.transitionStepAtE0);
        if (operation.progressAt24 >= operation.maximumAt28)
            operation.progressAt24 = operation.maximumAt28;
        services_->CommitTransitionAt4B02E0(0);
        return;
    }

    case 2:
        state_.stateAt68 = state_.pendingStateAt70;
        services_->WriteScriptSlot2DwordAt04(1);
        return;

    default:
        return;
    }
}

void ProgramSelectionDialogRuntime::CompleteSelectedChildAtVslot94() const
{
    // 0x521FD0/0x5221A0：child 0 只负责自身推进，child 1 才提交选择结果。
    if (services_->ReadChildAtVslot20(0))
        services_->InvokeChildAtVslot48(0);

    if (!services_->ReadChildAtVslot28(1) ||
        services_->ReadChildAtVslot48(1))
    {
        return;
    }

    if (mode_ == ProgramSelectionDialogMode::Mode00)
        services_->InvokeChildAtVslot4C(1);

    state_.stateAt68 = 2;
    state_.substateAt6C = 0;
    state_.resultAt94 =
        mode_ == ProgramSelectionDialogMode::Mode00 ?
            services_->ReadChildDwordAt90(1) :
            services_->ReadChildDwordAt80(1);
}
}
