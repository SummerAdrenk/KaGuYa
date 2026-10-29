#include <Runtime/ScrVmFileCallHandlers.h>

#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ScrVmFileCallStackException::ScrVmFileCallStackException(
    const ScrVmFileCallStackError error,
    const char* const message)
    : std::runtime_error(message),
      error_(error)
{
}

ScrVmFileCallStackError ScrVmFileCallStackException::Error() const noexcept
{
    return error_;
}

ScrVmFileCallStack::ScrVmFileCallStack(ScrVmParamsGenerationBinding generationSlot)
    : generationSlot_(std::move(generationSlot))
{
    if (!generationSlot_)
        throw std::invalid_argument("SCR VM file call stack params generation slot is null");
}

bool ScrVmFileCallStack::UsesParamsGenerationSlot(
    const ScrVmParamsGenerationBinding& generationSlot) const noexcept
{
    return generationSlot_ && generationSlot_ == generationSlot;
}

ScriptFileCallStorage& ScrVmFileCallStack::Frames()
{
    return generationSlot_ ? generationSlot_->Current().FileCallStackAt18() : frames_;
}

const ScriptFileCallStorage& ScrVmFileCallStack::Frames() const
{
    return generationSlot_ ? generationSlot_->Current().FileCallStackAt18() : frames_;
}

void ScrVmFileCallStack::Push(const ScrVmFileCallFrame frame)
{
    auto& frames = Frames();
    // 值入口仅是已有消费者的适配；底层始终创建原0Ch正文及18h control。
    void* allocation = DefaultEnvironmentRootMemory().AllocateBody(0x0C);
    auto* body = allocation ? ::new (allocation) ScriptStackFrameNative(frame.scriptId, frame.returnProgramCounter) : nullptr;
    YtlReferenceSlotRuntime reference{};
    CreateScriptStackReference(reference, body);
    try { frames.Append(reference); }
    catch (...) { ReleaseScriptStackReferenceAt4C9330(reference); throw; }
    ReleaseCapturedScriptStackControl(reference.controlAt08);
}

ScrVmFileCallFrame ScrVmFileCallStack::Pop()
{
    auto& frames = Frames();
    YtlReferenceSlotRuntime reference{};
    frames.PopReferenceAt4D4B40(reference);
    const auto* body = ResolveEnvironmentObjectReference<ScriptStackFrameNative>(reference);
    if (!body)
    {
        ReleaseScriptStackReferenceAt4C9330(reference);
        throw ScrVmFileCallStackException(
            ScrVmFileCallStackError::Empty,
            "SCR VM file return executed with an empty stack");
    }

    const ScrVmFileCallFrame frame{body->scriptIdAt04, body->returnPcAt08};
    ReleaseScriptStackReferenceAt4C9330(reference);
    return frame;
}

void ScrVmFileCallStack::Clear()
{
    Frames().clear();
}

std::size_t ScrVmFileCallStack::Size() const
{
    return Frames().Size();
}

ScrVmRuntimeScriptSwitchBoundary::ScrVmRuntimeScriptSwitchBoundary(
    const IScrResourceNameSource& resourceNames,
    const ResourceCatalog& catalog,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes,
    ArchiveCodecServices codecs) noexcept
    : resourceNames_(resourceNames),
      catalog_(catalog),
      transactionContext_(transactionContext),
      api_(api),
      environmentKeyBytes_(environmentKeyBytes),
      codecs_(codecs)
{
}

ScrScriptSwitchResult ScrVmRuntimeScriptSwitchBoundary::Switch(
    ScrScriptRuntimeState& runtime,
    const std::int32_t scriptId)
{
    return runtime.LoadScript(
        scriptId,
        resourceNames_,
        catalog_,
        transactionContext_,
        api_,
        environmentKeyBytes_, codecs_);
}

ScrFileCallInstructionHandler::ScrFileCallInstructionHandler(IScriptFileCommandSource& source,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : source_(source), scriptSwitch_(scriptSwitch), errors_(errors), strings_(strings) {}

std::int32_t ScrFileCallInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    return ExecuteScriptFileCallAt4CD240(runtime, source_, scriptSwitch_, errors_, memory, strings_);
}
std::int32_t ScrFileCallInstructionHandler::Execute(ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}

ScrFileReturnInstructionHandler::ScrFileReturnInstructionHandler(IScriptFileCommandSource& source,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : source_(source), scriptSwitch_(scriptSwitch), errors_(errors), strings_(strings) {}

std::int32_t ScrFileReturnInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& )
{
    return ExecuteScriptFileReturnAt4D08A0(runtime, source_, scriptSwitch_, errors_, strings_);
}
std::int32_t ScrFileReturnInstructionHandler::Execute(ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}

ScrFileJumpInstructionHandler::ScrFileJumpInstructionHandler(IScriptFileCommandSource& source,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : source_(source), scriptSwitch_(scriptSwitch), errors_(errors), strings_(strings) {}

std::int32_t ScrFileJumpInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    return ExecuteScriptFileJumpAt4CDD90(runtime, source_, scriptSwitch_, errors_, memory, strings_);
}
std::int32_t ScrFileJumpInstructionHandler::Execute(ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}

}
