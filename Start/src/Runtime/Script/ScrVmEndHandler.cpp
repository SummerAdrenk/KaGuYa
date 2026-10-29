#include <Runtime/ScrVmEndHandler.h>

namespace KaGuYa::Start::Runtime
{
ScrEndInstructionHandler::ScrEndInstructionHandler(IScriptFileCommandSource& source,
    IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : source_(source), scriptSwitch_(scriptSwitch), errors_(errors), strings_(strings) {}

std::int32_t ScrEndInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory&)
{
    return ExecuteScriptEndAt4CD8D0(runtime, source_, scriptSwitch_, errors_, strings_);
}

std::int32_t ScrEndInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime, const ScrInstructionView&)
{
    // END没有操作数；工具入口也不得沿用调用者的指令快照或环境ID。
    return ExecuteRuntimeEntry(runtime, DefaultEnvironmentRootMemory());
}
}
