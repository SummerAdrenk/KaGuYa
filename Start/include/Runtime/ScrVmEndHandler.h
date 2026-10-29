#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScriptEndCommandNative.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 对应 opcode 4 / 0x4CD8D0。
class ScrEndInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrEndInstructionHandler(IScriptFileCommandSource& source,
        IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    IScriptFileCommandSource& source_;
    IScrVmScriptSwitchBoundary& scriptSwitch_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
}
