#include <Runtime/AutoSaveOperationsNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<AutoSaveOperationsNativeFunctionInfo, 6> infos{{
    {0x46B280, 0x5EA184, 0x5EA174, 2, 1}, {0x4883F0, 0x5E9314, 0x5E9304, 2, 1},
    {0x45B800, 0x5E921C, 0x5E9214, 1, 1}, {0x462B50, 0x5E9338, 0, 0, 5},
    {0x463A90, 0x5E9240, 0, 0, 5}, {0x4686D0, 0x5E935C, 0, 0, 5}}};
constexpr std::array<std::array<std::uint32_t, 2>, 2> entryRows{{
    {0xFFFFFFFF, 0x598A20}, {0xFFFFFFFF, 0x598A28}}};
constexpr std::array<std::array<std::uint32_t, 2>, 2> rollingRows{{
    {0xFFFFFFFF, 0x59A8F0}, {0, 0x59A8F8}}};
constexpr std::array<std::array<std::uint32_t, 2>, 1> constructorRows{{{0xFFFFFFFF, 0x596910}}};
constexpr std::array<std::uint32_t, 1> slots{0x4686D0}; // 5B2ABC唯一虚槽。
constexpr char title[] = "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75%02d";
}
std::span<const AutoSaveOperationsNativeFunctionInfo, 6> AutoSaveOperationsNativeFunctionInfos() noexcept { return infos; }
std::span<const std::array<std::uint32_t, 2>, 2> AutoSaveOperationsNativeEntryUnwindRows() noexcept { return entryRows; }
std::span<const std::array<std::uint32_t, 2>, 2> AutoSaveOperationsNativeRollingUnwindRows() noexcept { return rollingRows; }
std::span<const std::array<std::uint32_t, 2>, 1> AutoSaveOperationsNativeBaseConstructorUnwindRows() noexcept { return constructorRows; }
std::span<const std::uint32_t, 1> AutoSaveOperationsNativeFactoryOriginalSlots() noexcept { return slots; }
const char* AutoSaveOperationsNativeTitleFormat() noexcept { return title; }
}
