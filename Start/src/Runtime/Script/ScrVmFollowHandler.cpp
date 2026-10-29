#include <Runtime/ScrVmFollowHandler.h>

namespace KaGuYa::Start::Runtime
{
ScrFollowJumpInstructionHandler::ScrFollowJumpInstructionHandler(IScriptFileCommandSource& source,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : source_(source), scriptSwitch_(scriptSwitch), errors_(errors), strings_(strings) {}

std::int32_t ScrFollowJumpInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    return ExecuteScriptFollowAt4CE3E0(runtime, source_, scriptSwitch_, errors_, memory, strings_);
}
std::int32_t ScrFollowJumpInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    // 工具调用同样从绑定Params的PC借用parser，不能另走快照算法。
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}
}
