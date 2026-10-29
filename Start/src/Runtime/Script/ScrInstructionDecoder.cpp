#include <Runtime/ScrInstructionDecoder.h>

#include <limits>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::uint16_t ReadWord(
    const std::span<const std::byte> bytes,
    const std::size_t offset) noexcept
{
    return static_cast<std::uint16_t>(
        std::to_integer<std::uint16_t>(bytes[offset]) |
        (std::to_integer<std::uint16_t>(bytes[offset + 1u]) << 8u));
}

[[noreturn]] void Throw(
    const ScrInstructionDecodeError error,
    const std::uint32_t programCounter,
    const char* const message)
{
    throw ScrInstructionDecodeException(error, programCounter, message);
}
}

ScrInstructionDecodeException::ScrInstructionDecodeException(
    const ScrInstructionDecodeError error,
    const std::uint32_t programCounter,
    const char* const message)
    : std::runtime_error(message), error_(error), programCounter_(programCounter)
{
}

ScrInstructionDecodeError ScrInstructionDecodeException::Error() const noexcept
{
    return error_;
}

std::uint32_t ScrInstructionDecodeException::ProgramCounter() const noexcept
{
    return programCounter_;
}

ScrInstructionView DecodeScrInstructionAt(
    const ScrContainer& container,
    const std::uint32_t programCounter)
{
    const std::uint32_t codeOffset = container.CodeOffset();
    const std::uint64_t codeEnd64 =
        static_cast<std::uint64_t>(codeOffset) + container.CodeSize();
    if (codeEnd64 > (std::numeric_limits<std::uint32_t>::max)())
    {
        Throw(ScrInstructionDecodeError::InstructionRangeOverflow, programCounter,
            "SCR code range exceeds the original x86 program-counter width");
    }
    const std::uint32_t codeEnd = static_cast<std::uint32_t>(codeEnd64);
    if (programCounter < codeOffset)
    {
        Throw(ScrInstructionDecodeError::ProgramCounterBeforeCode, programCounter,
            "SCR program counter precedes the code section");
    }
    if (programCounter >= codeEnd)
    {
        Throw(ScrInstructionDecodeError::ProgramCounterAfterCode, programCounter,
            "SCR program counter is at or after the code section end");
    }
    if (codeEnd - programCounter < 4u)
    {
        Throw(ScrInstructionDecodeError::HeaderTruncated, programCounter,
            "SCR instruction header is truncated");
    }

    const std::span<const std::byte> bytes = container.Bytes();
    const std::uint16_t opcode = ReadWord(bytes, programCounter);
    const std::uint16_t declaredLength = ReadWord(bytes, programCounter + 2u);
    if (declaredLength < 4u)
    {
        Throw(ScrInstructionDecodeError::LengthBelowHeader, programCounter,
            "SCR instruction length is smaller than its four-byte header");
    }
    if (declaredLength > codeEnd - programCounter)
    {
        Throw(ScrInstructionDecodeError::InstructionRangeOverflow, programCounter,
            "SCR instruction exceeds the code section");
    }

    const std::uint32_t nextProgramCounter = programCounter + declaredLength;
    return ScrInstructionView{
        programCounter,
        nextProgramCounter,
        opcode,
        declaredLength,
        bytes.subspan(programCounter + 4u, declaredLength - 4u)};
}

bool IsKnownScrOpcode(const std::uint16_t opcode) noexcept
{
    return opcode >= 1u && opcode <= 28u;
}
}
