#pragma once

#include <Runtime/ScriptFollowCommandNative.h>

namespace KaGuYa::Start::Runtime
{
[[nodiscard]] std::int32_t ExecuteScriptFileCallAt4CD240(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] std::int32_t ExecuteScriptFileReturnAt4D08A0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] std::int32_t ExecuteScriptFileJumpAt4CDD90(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
