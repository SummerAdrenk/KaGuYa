#include <Runtime/EnvironmentPreSceneUpdateState.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentPreSceneUpdateState::EnvironmentPreSceneUpdateState(
    EnvironmentRegistryRuntime& registry, Platform::IInputRuntime& input) noexcept
    : registry_(registry), input_(input) {}

std::uint32_t EnvironmentPreSceneUpdateState::ConfigDescriptor2Field14() const
{
    return registry_.UnSaveData().FlagsAt14();
}

void EnvironmentPreSceneUpdateState::MaskConfigDescriptor2Field14(std::uint32_t mask)
{
    registry_.UnSaveData().FlagsAt14() &= mask;
}

Platform::MouseView EnvironmentPreSceneUpdateState::CaptureMouse() const
{
    const auto mouse = input_.MouseOrNull();
    if (!mouse) throw std::logic_error("46D420 accesses absent mouse manager (modern safety boundary)");
    return *mouse;
}

bool EnvironmentPreSceneUpdateState::InputDescriptor0Byte13() const
{
    // 450C6B/450CA8把raw+07/+08两字节复制到manager+13/+14。
    return CaptureMouse().DownLatched(0) != 0;
}

bool EnvironmentPreSceneUpdateState::InputDescriptor0Byte14() const
{
    return CaptureMouse().DownLatched(1) != 0;
}

bool EnvironmentPreSceneUpdateState::InputDescriptor2Action(std::uint32_t action) const
{
    return GameGuiInputStateRuntime(input_).ReadPressedAtVslot04(std::bit_cast<std::int32_t>(action));
}

void EnvironmentPreSceneUpdateState::SetInputDescriptor0Byte14(std::uint8_t value)
{
    CaptureMouse().SetDownLatched(1, value);
}

void EnvironmentPreSceneUpdateState::SetInputDescriptor0Byte13(std::uint8_t value)
{
    CaptureMouse().SetDownLatched(0, value);
}

void EnvironmentPreSceneUpdateState::SetInputDescriptor0Field28(std::uint32_t value)
{
    CaptureMouse().SetReleaseSuppressionCountdown(value);
}

bool EnvironmentPreSceneUpdateState::ConfigDescriptor1Field38Bit20() const
{
    return (registry_.SaveData().bytesAt34[4] & 0x20u) != 0;
}
}
