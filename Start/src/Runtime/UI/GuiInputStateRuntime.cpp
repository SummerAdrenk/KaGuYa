#include <Runtime/GuiInputStateRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
GameGuiInputStateRuntime::GameGuiInputStateRuntime(Platform::MouseView mouse,
    Platform::KeyboardView keyboard) noexcept : mouse_(mouse), keyboard_(keyboard)
{
}

GameGuiInputStateRuntime::GameGuiInputStateRuntime(const Platform::IInputRuntime& input) noexcept
    : input_(&input)
{
}

Platform::MouseView GameGuiInputStateRuntime::Mouse() const
{
    // 50A653 在查询入口才取根槽；GUI 构造早于 4507E0 的输入创建。
    const auto mouse = input_ ? input_->MouseOrNull() : mouse_;
    if (!mouse) throw std::logic_error("GameInputState mouse slot is empty");
    return *mouse;
}

Platform::KeyboardView GameGuiInputStateRuntime::Keyboard() const
{
    // 50FFC4/51008F 每次取当前键盘，不缓存旧实例，也不要求鼠标仍存在。
    const auto keyboard = input_ ? input_->KeyboardOrNull() : keyboard_;
    if (!keyboard) throw std::logic_error("GameInputState keyboard slot is empty");
    return *keyboard;
}

std::size_t GameGuiInputStateRuntime::CheckedAction(std::int32_t action,
    Platform::KeyboardView keyboard)
{
    // 原代码除 -1 外直接索引动作向量。拒绝非法内存访问，不把越界动作当未按下。
    if (action < 0 || static_cast<std::size_t>(action) >= keyboard.ActionCapacity())
        throw std::out_of_range("GameInputState action index is outside the keyboard table");
    return static_cast<std::size_t>(action);
}

bool GameGuiInputStateRuntime::ReadHeldAtVslot00(std::int32_t action) const
{
    if (action == -1) return false; // 50FFB7..50FFC0，在访问根输入对象之前短路。
    const auto& keyboard = Keyboard();
    const auto index = CheckedAction(action, keyboard);
    for (std::size_t i = 0; i < keyboard.BindingCount(index); ++i)
        if (keyboard.CurrentState(index, i) != 0) return true;
    return false;
}

bool GameGuiInputStateRuntime::ReadPressedAtVslot04(std::int32_t action) const
{
    if (action == -1) return false;
    const auto& keyboard = Keyboard();
    const auto index = CheckedAction(action, keyboard);
    // 510080 -> 47F4E0 按每个绑定分别检查边沿，不能先对整个动作合并 held。
    for (std::size_t i = 0; i < keyboard.BindingCount(index); ++i)
        if (keyboard.CurrentState(index, i) != 0 && keyboard.PreviousState(index, i) == 0)
            return true;
    return false;
}

POINT GameGuiInputStateRuntime::ReadPointerAtVslot08() const
{
    return Mouse().Position().LogicalPosition(); // 50A650: CMouseManager+18/+1C。
}

std::int32_t GameGuiInputStateRuntime::ReadWheelAtVslot0C() const
{
    // 50B905..50B90E：+2C 不为零时返回零；450AA0 将有符号 word 扩展到 +34。
    const auto& mouse = Mouse();
    return mouse.SecondaryCountdown() != 0 ? 0 : mouse.WheelSteps();
}

bool GameGuiInputStateRuntime::ReadPointerMovedAtVslot10() const
{
    return Mouse().Position().LogicalPositionChanged(); // 5101D0: +33，不是取消键。
}

bool GameGuiInputStateRuntime::ReadMousePressAtVslot14() const
{
    // 510155..510165: previousLeft==0 && currentLeft!=0 && +28==0。
    const auto& mouse = Mouse();
    return mouse.PreviousPressed(0) == 0 && mouse.Pressed(0) != 0 &&
        mouse.ReleaseSuppressionCountdown() == 0;
}

bool GameGuiInputStateRuntime::ReadMouseReleaseAtVslot18() const
{
    // 510295..51029F：这里不是 release edge，未按住或仍有抑制倒计时都返回真。
    const auto& mouse = Mouse();
    return mouse.Pressed(0) == 0 || mouse.ReleaseSuppressionCountdown() != 0;
}
}
