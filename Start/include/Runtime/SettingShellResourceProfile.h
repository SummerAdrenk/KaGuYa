#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 页号属于共享控制器；原游戏按钮ID仅通过表绑定，不直接充当页号。
enum class SettingShellPage : std::int32_t { Text = 0, Display = 1, Sound = 2, Other = 3 };
enum class SettingShellAction { Close, ExitGame, ReturnToTitle, RestoreDefaults };
struct SettingShellTab final
{
    int button;
    SettingShellPage page;
};
struct SettingShellCommand final
{
    int button;
    SettingShellAction action;
};
struct SettingShellResourceProfile final
{
    std::span<const GuiStaticDescriptorNative24h> backgrounds;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    std::span<const SettingShellTab> tabs;
    std::span<const SettingShellCommand> commands;
    std::span<const SettingShellPage> pageVisitOrder;
    // -1表示本款没有该读取点，不能把空闲事件-1当作一个命令。
    int modeGateButton = -1;
    // 姓名确认来自Other子页结果；01-10无该父分派，不能默认接受18的239。
    int nameResult = -1;
};
const SettingShellResourceProfile& CurrentSettingShellProfile();
}
