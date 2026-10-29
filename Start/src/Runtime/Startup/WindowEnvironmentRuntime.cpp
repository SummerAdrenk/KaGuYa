#include <Runtime/WindowEnvironmentRuntime.h>

#include <Runtime/ParamsRuntimeProfile.h>

#include <bit>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
WindowEnvironmentRuntime BuildSample18WindowEnvironment(const ParamsRuntimeProfile& params)
{
    static_assert(sizeof(int) == sizeof(std::int32_t));

    WindowEnvironmentRuntime environment;
    // 4963C0..4963CE 原样复制 DWORD，不截断、钳制或拒绝高位尺寸。
    environment.gameProfile.logicalWidth = std::bit_cast<std::int32_t>(params.gameSystem.width);
    environment.gameProfile.logicalHeight = std::bit_cast<std::int32_t>(params.gameSystem.height);
    // 此显式工具快照保留CString长度；不是4344D5从环境根Clone标题的生产拥有链。
    environment.gameProfile.gameTitle = params.gameSystem.gameTitle.View();
    environment.gameProfile.appendTitle = params.gameSystem.appendTitle.View();

    // 4963D6/4963E2/4963F4：环境原值与创建窗口时的零值替代项分别保存。
    environment.executableProfile.configuredStyle = 0x02CB0000u;
    environment.executableProfile.configuredExStyle = 0x00000200u;
    environment.fullscreenStyle = 0x12000000u;
    // 434544/43454E：仅供 CreateWindowExA 前的零值替代，不回灌原 style。
    environment.executableProfile.fallbackStyle = 0x06CF0000u;
    environment.executableProfile.fallbackExStyle = 0x00040100u;
    // 48AD90 的帧率请求与 601258 的图标 ID；此处不执行计时器或图标初始化。
    environment.executableProfile.targetFramesPerSecond = 30u;
    environment.executableProfile.iconResourceId = 128u;
    return environment;
}
}
