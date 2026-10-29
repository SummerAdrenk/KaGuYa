#pragma once

#include <Runtime/CgDisplaySceneRuntime.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ParamsRuntimeProfile.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 合并 0x4AD780 的四项输出与 0x4B74A0..0x4B7606 的候选遍历。
// category/item 越界是原 0x4AD780 的受控 false；空列表会成功返回 count=0，
// 由 0x4B74A0 调用方处理。非法 flag index 属于不一致对象图，必须显式暴露。
[[nodiscard]] bool BuildCgDisplaySelectionAt4AD780And4B74A0(
    const SecretCgArchive& secretCg,
    const ParamsGameSystem& gameSystem,
    std::int32_t categoryIndex,
    std::int32_t itemIndex,
    CgDisplaySelectionState& state);
}
