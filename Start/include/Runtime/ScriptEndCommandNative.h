#pragma once

#include <Runtime/ScriptFileCommandSource.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class ScrScriptRuntimeState;
class IScrVmScriptSwitchBoundary;
class IScrVmCommandErrorSink;

// 原END无命令Ref、无正文/PC读取。LoadScript保留现有显式资源适配器边界。
[[nodiscard]] std::int32_t ExecuteScriptEndAt4CD8D0(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& source, IScrVmScriptSwitchBoundary& scriptSwitch,
    IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
