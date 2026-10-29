#pragma once

#include <Runtime/ScriptEndCommandNative.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4CE3E0：file_jump_cmd借用正文、返回环境写入、切换及双CString错误链。
[[nodiscard]] std::int32_t ExecuteScriptFollowAt4CE3E0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
