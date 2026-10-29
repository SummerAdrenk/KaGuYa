#include <Runtime/ScrVmWaitHandler.h>
#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScriptControlCommandNative.h>

#include <cstring>
#include <span>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint16_t ReadU16(const std::span<const std::byte> bytes, const std::size_t offset)
{
    std::uint16_t value = 0;
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

void RequireWaitOperand(
    const ScrInstructionView& instruction,
    const std::size_t required)
{
    if (instruction.body.size() < required)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM wait operand is truncated");
    }
}
}

ScrWaitInstructionHandler::ScrWaitInstructionHandler(
    ScrVmValueTable& values,
    ScrVmScriptStorage& storage, IScriptControlCommandSource* source) noexcept
    : values_(values),
      storage_(storage), source_(source)
{
}

std::int32_t ScrWaitInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    BoundScriptControlCommandSource bound(runtime, &values_);
    return ExecuteScriptWaitAt4D11D0(runtime.NativeParserAtA0(), source_ ? *source_ : bound, storage_, memory);
}

std::int32_t ScrWaitInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    RequireWaitOperand(instruction, 3u);
    const std::uint8_t flags = std::to_integer<std::uint8_t>(instruction.body[0]);
    const std::uint16_t primaryRaw = ReadU16(instruction.body, 1u);

    storage_.WriteWaitValue(storage_.WaitState(), 2, 0);
    if ((flags & 0x40u) != 0u)
    {
        storage_.WriteWaitValue(storage_.WaitState(), 0, 1);
    }
    else if ((flags & 0x80u) != 0u)
    {
        storage_.WriteWaitValue(storage_.WaitState(), 0, 3);
    }
    else if ((flags & 0x20u) != 0u)
    {
        storage_.WriteWaitValue(storage_.WaitState(), 0, 4);
    }
    else if ((flags & 0x10u) != 0u)
    {
        storage_.WriteWaitValue(storage_.WaitState(), 0, 5);
        // 4D1421先发布mode，再由4D1431读取额外WORD；安全拒绝也保留此前写入。
        RequireWaitOperand(instruction, 5u);
        storage_.WriteWaitValue(storage_.WaitState(), 2, ReadU16(instruction.body, 3u));
    }
    else
    {
        storage_.WriteWaitValue(storage_.WaitState(), 0, 2);
    }

    // 4D1474..4D1493先捕获wait正文，4D14A3取值后仍写该正文，不随槽换代改目标。
    auto& target = storage_.WaitState();
    const auto value = (flags & 0x02u) != 0u
        ? values_.Read(primaryRaw)
        : static_cast<std::int32_t>(primaryRaw);
    storage_.WriteWaitValue(target, 1, value);
    return 7;
}
}
