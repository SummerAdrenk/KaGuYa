#pragma once
#include <string_view>

namespace KaGuYa::Sound
{
inline std::wstring_view SoundFileNamePart(const std::wstring_view path) noexcept
{
    // 原B4B0先倒查CPath+6反斜杠，仅未找到时才查+4正斜杠，不是两者取最后。
    auto separator = path.rfind(L'\\');
    if (separator == std::wstring_view::npos) separator = path.rfind(L'/');
    return separator == std::wstring_view::npos ? path : path.substr(separator + 1);
}
}
