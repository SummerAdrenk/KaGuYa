#pragma once

#include <Runtime/ScriptFileCommandSource.h>
#include <Runtime/ScriptInvalidCommandNative.h>
#include <Runtime/ScrVmUpdateHandler.h>

namespace KaGuYa::Start::Runtime
{
// 4D2CAD..4D2D4C：index未初始化，先清两坐标；分配control之后才从命令读取三字段。
struct ScriptUpdateResultNative final
{
    std::int32_t index;
    std::int32_t first;
    std::int32_t second;
};
static_assert(sizeof(ScriptUpdateResultNative) == 12);

[[nodiscard]] std::int32_t ExecuteScriptUpdateAt4D2800(ScrScriptRuntimeState& runtime,
    IScriptFileCommandSource& pcSource, IScrVmUpdateValueSource& values,
    const IScrResourceNameSource& names, IScrVmUpdateRuntimeBoundary& layers,
    ScrVmScriptStorage& storage, IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
