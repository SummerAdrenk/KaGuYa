#pragma once

#include <string>

namespace KaGuYa::Start::Runtime
{
struct ParamsText final
{
    bool operator==(const ParamsText&) const = default;
    std::u16string utf16;
    // 历史字段名：严格工具入口固定 CP932；原位运行时入口按 4102D0 使用线程 ACP。
    std::string cp932;
};
}
