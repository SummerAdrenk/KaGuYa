#pragma once

#include <Runtime/ScrContainer.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class ScrInstructionDecodeError : std::uint8_t
{
    ProgramCounterBeforeCode,
    ProgramCounterAfterCode,
    HeaderTruncated,
    LengthBelowHeader,
    InstructionRangeOverflow,
};

class ScrInstructionDecodeException final : public std::runtime_error
{
public:
    ScrInstructionDecodeException(
        ScrInstructionDecodeError error,
        std::uint32_t programCounter,
        const char* message);

    [[nodiscard]] ScrInstructionDecodeError Error() const noexcept;
    [[nodiscard]] std::uint32_t ProgramCounter() const noexcept;

private:
    ScrInstructionDecodeError error_;
    std::uint32_t programCounter_;
};

struct ScrInstructionView final
{
    std::uint32_t programCounter = 0;
    std::uint32_t nextProgramCounter = 0;
    std::uint16_t opcode = 0;
    std::uint16_t declaredLength = 0;
    std::span<const std::byte> body;
};

// 对应 0x4D3FF0 的公共四字节取指头。操作码业务分派不属于该结构解码入口。
[[nodiscard]] ScrInstructionView DecodeScrInstructionAt(
    const ScrContainer& container,
    std::uint32_t programCounter);

[[nodiscard]] bool IsKnownScrOpcode(std::uint16_t opcode) noexcept;
}
