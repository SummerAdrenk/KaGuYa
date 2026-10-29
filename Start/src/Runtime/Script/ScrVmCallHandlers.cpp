#include <Runtime/ScrVmCallHandlers.h>
#include <Runtime/ScrVmControlHandlers.h>

#include <bit>
#include <cstring>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ScrVmCallStackException::ScrVmCallStackException(
    const ScrVmCallStackError error,
    const char* const message)
    : std::runtime_error(message),
      error_(error)
{
}

ScrVmCallStackError ScrVmCallStackException::Error() const noexcept
{
    return error_;
}

ScrVmCallStack::ScrVmCallStack(ScrVmParamsGenerationBinding generationSlot)
    : generationSlot_(std::move(generationSlot))
{
    if (!generationSlot_)
        throw std::invalid_argument("SCR VM call stack params generation slot is null");
}

bool ScrVmCallStack::UsesParamsGenerationSlot(
    const ScrVmParamsGenerationBinding& generationSlot) const noexcept
{
    return generationSlot_ && generationSlot_ == generationSlot;
}

ScriptLocalCallStorage& ScrVmCallStack::Entries()
{
    return generationSlot_ ? generationSlot_->Current().LocalCallStackAt24() : entries_;
}

const ScriptLocalCallStorage& ScrVmCallStack::Entries() const
{
    return generationSlot_ ? generationSlot_->Current().LocalCallStackAt24() : entries_;
}

void ScrVmCallStack::Push(const std::uint32_t programCounter)
{
    Entries().Push(programCounter);
}

std::uint32_t ScrVmCallStack::Pop()
{
    auto& entries = Entries();
    if (!entries.Size())
    {
        throw ScrVmCallStackException(
            ScrVmCallStackError::Empty,
            "SCR VM return executed with an empty call stack");
    }
    // 空栈的现代明确拒绝保留；非空按4D0AA8原序读取末项、回退end。
    const auto value = entries.storage.end[-1];
    --entries.storage.end;
    return value;
}

void ScrVmCallStack::Clear()
{
    Entries().clear();
}

std::size_t ScrVmCallStack::Size() const
{
    return Entries().Size();
}

ScrCallInstructionHandler::ScrCallInstructionHandler(
    ScrVmCallStack& callStack, IScriptControlCommandSource* source) noexcept
    : callStack_(callStack), source_(source)
{
}

std::int32_t ScrCallInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, nullptr, &callStack_);
    return ExecuteScriptCallAt4CEC40(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrCallInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    if (instruction.body.size() < 4u)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM call operand is truncated");
    }

    callStack_.Push(runtime.StagedProgramCounter());
    std::int32_t target = 0;
    std::memcpy(&target, instruction.body.data(), sizeof(target));
    ApplyScrVmProgramCounterTarget(runtime, target);
    return 0;
}

ScrReturnInstructionHandler::ScrReturnInstructionHandler(
    ScrVmCallStack& callStack, IScriptControlCommandSource* source) noexcept
    : callStack_(callStack), source_(source)
{
}

std::int32_t ScrReturnInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory&)
{
    BoundScriptControlCommandSource bound(runtime, nullptr, &callStack_);
    return ExecuteScriptReturnAt4D0A90(runtime.NativeParserAtA0(), source_ ? *source_ : bound);
}

std::int32_t ScrReturnInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView&)
{
    runtime.SetStagedProgramCounter(callStack_.Pop());
    return 0;
}
}
