#pragma once

#include <Runtime/RuntimeCString.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 按原EXE数据选择两段表；第二段编号接在common之后，不能重排存档/脚本的数值槽。
struct ScriptNamedValueNativeTables final
{
    std::span<const char* const> common;
    std::span<const char* const> game;
};

[[nodiscard]] ScriptNamedValueNativeTables MamatomoScriptNamedValueTables() noexcept;
[[nodiscard]] ScriptNamedValueNativeTables GameScriptNamedValueTables() noexcept;
[[nodiscard]] std::int32_t ResolveScriptNamedValueNativeAt498020(const char* name,
    ScriptNamedValueNativeTables tables = GameScriptNamedValueTables(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
