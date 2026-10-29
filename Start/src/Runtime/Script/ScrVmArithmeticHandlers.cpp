#include <Runtime/ScrVmArithmeticHandlers.h>
#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

#include <bit>
#include <cstring>
#include <limits>
#include <span>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireAssignOperand(const ScrInstructionView& instruction)
{
    if (instruction.body.size() < 12u)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM assign operand is truncated");
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

std::int32_t FromBits(const std::uint32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(value);
}

std::uint32_t ToBits(const std::int32_t value) noexcept
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t SignedDivide(const std::int32_t lhs, const std::int32_t rhs)
{
    if (rhs == 0)
    {
        throw ScrVmArithmeticException(
            ScrVmArithmeticError::DivideByZero,
            "SCR VM assign division by zero");
    }
    if (lhs == (std::numeric_limits<std::int32_t>::min)() && rhs == -1)
    {
        throw ScrVmArithmeticException(
            ScrVmArithmeticError::SignedDivisionOverflow,
            "SCR VM assign signed division overflow");
    }
    return lhs / rhs;
}

std::int32_t SignedRemainder(const std::int32_t lhs, const std::int32_t rhs)
{
    if (rhs == 0)
    {
        throw ScrVmArithmeticException(
            ScrVmArithmeticError::DivideByZero,
            "SCR VM assign remainder by zero");
    }
    if (lhs == (std::numeric_limits<std::int32_t>::min)() && rhs == -1)
    {
        throw ScrVmArithmeticException(
            ScrVmArithmeticError::SignedDivisionOverflow,
            "SCR VM assign signed remainder overflow");
    }
    return lhs % rhs;
}

std::int32_t ArithmeticShiftRight(
    const std::int32_t value,
    const std::uint32_t count) noexcept
{
    const std::uint32_t shift = count & 31u;
    if (shift == 0u)
        return value;

    std::uint32_t result = ToBits(value) >> shift;
    if (value < 0)
        result |= ~std::uint32_t{0} << (32u - shift);
    return FromBits(result);
}
}

ScrVmArithmeticException::ScrVmArithmeticException(
    const ScrVmArithmeticError error,
    const char* const message)
    : std::runtime_error(message),
      error_(error)
{
}

ScrVmArithmeticError ScrVmArithmeticException::Error() const noexcept
{
    return error_;
}

ScrAssignInstructionHandler::ScrAssignInstructionHandler(
    ScrVmValueTable& values,
    IScrVmCommandErrorSink& errorSink, IScriptArithmeticCommandSource* source) noexcept
    : values_(values),
      errorSink_(errorSink), source_(source)
{
}

std::int32_t ScrAssignInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptArithmeticCommandSource bound(runtime, values_);
    return ExecuteScriptAssignAt4D0F40(runtime.NativeParserAtA0(), source_ ? *source_ : bound, errorSink_, memory);
}

std::int32_t ScrAssignInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    RequireAssignOperand(instruction);
    const std::uint8_t flags = std::to_integer<std::uint8_t>(instruction.body[0]);
    const std::uint8_t operation = std::to_integer<std::uint8_t>(instruction.body[1]);
    const std::uint16_t destination = ReadU16(instruction.body, 2u);
    const std::int32_t lhsRaw = ReadI32(instruction.body, 4u);
    const std::int32_t rhsRaw = ReadI32(instruction.body, 8u);

    // 原函数即使在 operation 非法或不消费 lhs 时，也先求出两个操作数。
    const std::int32_t lhs = (flags & 0x02u) != 0u
        ? values_.ReadSigned(lhsRaw)
        : (flags & 0x08u) != 0u ? 0 : lhsRaw;
    const std::int32_t rhs = (flags & 0x20u) != 0u
        ? values_.ReadSigned(rhsRaw)
        : rhsRaw;

    std::int32_t result = 0;
    switch (operation)
    {
    case 0u: result = rhs; break;
    case 1u: result = FromBits(ToBits(lhs) + ToBits(rhs)); break;
    case 2u: result = FromBits(ToBits(lhs) - ToBits(rhs)); break;
    case 3u: result = FromBits(ToBits(lhs) * ToBits(rhs)); break;
    case 4u: result = SignedDivide(lhs, rhs); break;
    case 5u: result = SignedRemainder(lhs, rhs); break;
    case 6u: result = FromBits(ToBits(lhs) & ToBits(rhs)); break;
    case 7u: result = FromBits(ToBits(lhs) | ToBits(rhs)); break;
    case 8u: result = FromBits(ToBits(lhs) ^ ToBits(rhs)); break;
    case 9u: result = FromBits(~ToBits(rhs)); break;
    case 10u:
        result = FromBits(ToBits(lhs) << (ToBits(rhs) & 31u));
        break;
    case 11u:
        result = ArithmeticShiftRight(lhs, ToBits(rhs));
        break;
    default:
        ReportScriptOperationError(errorSink_, true);
        return 2;
    }

    values_.Write(destination, result);
    return 0;
}

ScrCompareInstructionHandler::ScrCompareInstructionHandler(
    ScrVmValueTable& values,
    IScrVmCommandErrorSink& errorSink, IScriptArithmeticCommandSource* source) noexcept
    : values_(values),
      errorSink_(errorSink), source_(source)
{
}

std::int32_t ScrCompareInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptArithmeticCommandSource bound(runtime, values_);
    return ExecuteScriptCompareAt4CD640(runtime.NativeParserAtA0(), source_ ? *source_ : bound, errorSink_, memory);
}

std::int32_t ScrCompareInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    RequireAssignOperand(instruction);
    const std::uint8_t flags = std::to_integer<std::uint8_t>(instruction.body[0]);
    const std::uint8_t operation = std::to_integer<std::uint8_t>(instruction.body[1]);
    const std::uint16_t destination = ReadU16(instruction.body, 2u);
    const std::int32_t lhsRaw = ReadI32(instruction.body, 4u);
    const std::int32_t rhsRaw = ReadI32(instruction.body, 8u);

    // compare 的 0x08 位没有 assign 的“左值清零”含义。
    const std::int32_t lhs = (flags & 0x02u) != 0u
        ? values_.ReadSigned(lhsRaw)
        : lhsRaw;
    const std::int32_t rhs = (flags & 0x20u) != 0u
        ? values_.ReadSigned(rhsRaw)
        : rhsRaw;

    bool result = false;
    switch (operation)
    {
    case 0u: result = ToBits(lhs) == ToBits(rhs); break;
    case 1u: result = ToBits(lhs) != ToBits(rhs); break;
    case 2u: result = lhs < rhs; break;
    case 3u: result = lhs <= rhs; break;
    case 4u: result = lhs > rhs; break;
    case 5u: result = lhs >= rhs; break;
    case 6u: result = lhs != 0 && rhs != 0; break;
    case 7u: result = lhs != 0 || rhs != 0; break;
    default:
        ReportScriptOperationError(errorSink_, false);
        return 2;
    }

    values_.Write(destination, result ? 1 : 0);
    return 0;
}
}
