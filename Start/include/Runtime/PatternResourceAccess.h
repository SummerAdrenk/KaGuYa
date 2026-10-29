#pragma once

#include <Runtime/RuntimeCStringArray.h>

namespace KaGuYa::Start::Runtime
{
// 4CB030的原始访问与显式工具检查共享执行核；此值只随调用传递，不进入原生正文。
enum class PatternResourceAccess : std::uint8_t { Checked, Native };

[[nodiscard]] inline const RuntimeCString& PatternResourceFailurePrefixAt466797(
    const RuntimeCString* prefix)
{
    // 466797先读原字符槽。通用RuntimeStringBorrow把null视为无typed来源，
    // 此处保留空槽的真实访问，不能让它转入全局manager路径；有效槽不增加读取。
    if (!prefix)
    {
        const auto characters = *reinterpret_cast<const char* const volatile*>(prefix);
        static_cast<void>(characters);
    }
    return *prefix;
}

template<PatternResourceAccess Access>
[[nodiscard]] const RuntimeCString& PatternResourceNameAt(
    const RuntimeCStringArrayReference& names, std::uint32_t index)
{
    if constexpr (Access == PatternResourceAccess::Checked) return names.At(index);
    // 4CB85C/4CC0FC：空base仍到原字段读点，不伪造空名或提前抛工具异常。
    else return *names.Resolve(index);
}
}
