#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScriptFollowCommandNative.h>

namespace KaGuYa::Start::Runtime
{
// 对应opcode 26 / 4CE3E0，不经过通用Decode预读正文。
class ScrFollowJumpInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrFollowJumpInstructionHandler(IScriptFileCommandSource& source,
        IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime, const ScrInstructionView& instruction) override;
private:
    IScriptFileCommandSource& source_;
    IScrVmScriptSwitchBoundary& scriptSwitch_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
}
