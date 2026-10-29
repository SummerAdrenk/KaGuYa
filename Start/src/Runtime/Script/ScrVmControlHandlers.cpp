#include <Runtime/ScrVmControlHandlers.h>

#include <cstring>
#include <span>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireOperandBytes(
    const ScrInstructionView& instruction,
    const std::size_t required)
{
    if (instruction.body.size() < required)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM instruction operand is truncated");
    }
}

std::uint16_t ReadU16(const std::span<const std::byte> bytes, const std::size_t offset)
{
    std::uint16_t value = 0;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

std::int32_t ReadI32(const std::span<const std::byte> bytes, const std::size_t offset)
{
    std::int32_t value = 0;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

std::int32_t ResolveCondition(
    const ScrInstructionView& instruction,
    const ScrVmValueTable& values)
{
    RequireOperandBytes(instruction, 7u);
    const std::uint8_t flags = std::to_integer<std::uint8_t>(instruction.body[0]);
    const std::uint16_t raw = ReadU16(instruction.body, 1u);
    return (flags & 2u) != 0u
        ? values.Read(raw)
        : static_cast<std::int32_t>(raw);
}
}

void ApplyScrVmProgramCounterTarget(
    ScrScriptRuntimeState& runtime,
    const std::int32_t target)
{
    runtime.ApplyProgramCounterTargetAt4D6E90(target);
}

ScrVmOperandException::ScrVmOperandException(
    const ScrVmOperandError error,
    const std::uint16_t opcode,
    const char* const message)
    : std::runtime_error(message),
      error_(error),
      opcode_(opcode)
{
}

ScrVmOperandError ScrVmOperandException::Error() const noexcept
{
    return error_;
}

std::uint16_t ScrVmOperandException::Opcode() const noexcept
{
    return opcode_;
}

ScrSetFlagInstructionHandler::ScrSetFlagInstructionHandler(
    ScrVmValueTable& values, IScriptControlCommandSource* source) noexcept
    : values_(values), source_(source)
{
}

std::int32_t ScrSetFlagInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, &values_);
    return ExecuteScriptSetFlagAt4CE2A0(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrSetFlagInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    RequireOperandBytes(instruction, 2u);
    values_.Write(ReadU16(instruction.body, 0u), 1);
    return 0;
}

ScrClearFlagInstructionHandler::ScrClearFlagInstructionHandler(
    ScrVmValueTable& values, IScriptControlCommandSource* source) noexcept
    : values_(values), source_(source)
{
}

std::int32_t ScrClearFlagInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, &values_);
    return ExecuteScriptClearFlagAt4CE160(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrClearFlagInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    RequireOperandBytes(instruction, 2u);
    values_.Write(ReadU16(instruction.body, 0u), 0);
    return 0;
}

std::int32_t ScrJumpInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime);
    return ExecuteScriptJumpAt4CE7B0(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrJumpInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    RequireOperandBytes(instruction, 4u);
    ApplyScrVmProgramCounterTarget(runtime, ReadI32(instruction.body, 0u));
    return 0;
}

ScrIfTrueInstructionHandler::ScrIfTrueInstructionHandler(
    const ScrVmValueTable& values, IScriptControlCommandSource* source) noexcept
    : values_(values), source_(source)
{
}

std::int32_t ScrIfTrueInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, values_);
    return ExecuteScriptIfTrueAt4CEA90(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrIfTrueInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    if (ResolveCondition(instruction, values_) != 0)
        ApplyScrVmProgramCounterTarget(runtime, ReadI32(instruction.body, 3u));
    return 0;
}

ScrIfFalseInstructionHandler::ScrIfFalseInstructionHandler(
    const ScrVmValueTable& values, IScriptControlCommandSource* source) noexcept
    : values_(values), source_(source)
{
}

std::int32_t ScrIfFalseInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, values_);
    return ExecuteScriptIfFalseAt4CE8E0(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrIfFalseInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    if (ResolveCondition(instruction, values_) == 0)
        ApplyScrVmProgramCounterTarget(runtime, ReadI32(instruction.body, 3u));
    return 0;
}
}
