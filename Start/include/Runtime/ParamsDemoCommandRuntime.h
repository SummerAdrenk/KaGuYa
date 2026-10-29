#pragma once

#include <Runtime/ParamsRuntimeProfile.h>

#include <variant>

namespace KaGuYa::Start::Runtime
{
struct DemoEndCommand final {};
struct DemoNextCommand final {};
struct DemoWaitCommand final
{
    std::uint8_t mode = 0;
    std::int32_t ticks = 0;
};
struct DemoSoundCommand final
{
    std::uint8_t mode = 0;
    std::uint8_t valueAt05 = 0; // 419EB0 保留此 byte，4BF5A0 不读取它。
    ParamsText resourceName;
};
struct DemoLoadCommand final
{
    std::uint32_t slot = 0;
    ParamsText resourceName;
};
struct DemoTransitionCommand final
{
    ParamsText effectName;
    std::int32_t step = 0;
    ParamsText maskName;
};
struct DemoVisibilityCommand final
{
    std::int32_t slot = 0;
    bool visible = false;
};
struct DemoUpdateCommand final {};
struct DemoMoveCommand final
{
    std::uint32_t slot = 0;
    std::int32_t duration = 0;
    std::int32_t startX = 0;
    std::int32_t startY = 0;
    std::int32_t endX = 0;
    std::int32_t endY = 0;
};
struct DemoPositionCommand final
{
    std::uint32_t slot = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

using DecodedParamsDemoCommand = std::variant<
    DemoEndCommand, DemoNextCommand, DemoWaitCommand, DemoSoundCommand,
    DemoLoadCommand, DemoTransitionCommand, DemoVisibilityCommand, DemoUpdateCommand,
    DemoMoveCommand, DemoPositionCommand>;

// 419EB0：从保留的 payload 还原运行时命令字段；复用 params 的严格边界与文本读取器。
[[nodiscard]] DecodedParamsDemoCommand DecodeParamsDemoCommand(
    const ParamsDemoCommand& command);
}
