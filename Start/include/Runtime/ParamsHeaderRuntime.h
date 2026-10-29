#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/ParamsInputBytesBorrow.h>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 43499D等调用点的414540按值复制，以及413B30比较/清理。
// 输入是已取得的vector对象，不是缓存的data；不读取CSerialize标量游标。
[[nodiscard]] bool MatchParamsHeaderAt413B30(const RuntimeCString& globalHeader,
    ParamsInputBytesBorrow input, std::uint32_t& matchedBytes);
}
