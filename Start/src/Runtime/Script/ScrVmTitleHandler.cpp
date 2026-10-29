#include <Runtime/ScrVmTitleHandler.h>

#include <Runtime/ScrVmControlHandlers.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
std::int32_t ScrTitleInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    // 独立VM的显式绑定入口，不冒充原全局root。生产root可提供晚取源。
    class BoundSource final : public IScriptTitleCommandSource
    {
    public:
        explicit BoundSource(ScrScriptRuntimeState& value) noexcept : value(value) {}
        ScriptTitleRootReceiver CaptureAfterEnsureAt413BE0() override
        {
            return {&value, [](void* p) { return static_cast<ScrScriptRuntimeState*>(p)->ProgramCounter(); },
                [](void* p) -> RuntimeCString& { return static_cast<ScrScriptRuntimeState*>(p)->NativeTitleAt48CF90(); }};
        }
        ScrScriptRuntimeState& value;
    } bound(runtime);
    return ExecuteScriptTitleAt4D25A0(runtime.NativeParserAtA0(), source_ ? *source_ : bound, memory);
}

std::int32_t ScrTitleInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    if (instruction.body.empty())
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM title length byte is truncated");
    }

    const std::size_t byteLength =
        std::to_integer<std::uint8_t>(instruction.body[0]);
    if (instruction.body.size() < byteLength + 1u)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM title payload is truncated");
    }

    const auto begin = instruction.body.begin() + 1;
    const auto end = begin + static_cast<std::ptrdiff_t>(byteLength);
    const auto nul = std::find(begin, end, std::byte{0});
    std::string title;
    title.reserve(static_cast<std::size_t>(nul - begin));
    for (auto current = begin; current != nul; ++current)
        title.push_back(static_cast<char>(std::to_integer<unsigned char>(*current)));

    // 原函数只做有界 NUL 扫描和 CStringA 赋值，不执行编码转换。
    runtime.SetCurrentTitle(std::move(title));
    return 0;
}
}
