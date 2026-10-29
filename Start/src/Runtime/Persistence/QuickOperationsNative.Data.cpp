#include <Runtime/QuickOperationsNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<QuickOperationsNativeFunctionInfo, 4> infos{{
    {0x485CE0, 0x5EA128, 0x5EA14C, 5}, {0x485FC0, 0x5EA0DC, 0x5EA100, 5},
    {0x4888C0, 0x5E9298, 0x5E9288, 2}, {0x45B800, 0x5E921C, 0x5E9214, 1}}};
constexpr std::array<std::array<std::uint32_t, 2>, 5> loadRows{{
    {0xFFFFFFFF, 0x59A6C0}, {0xFFFFFFFF, 0x59A6D2}, {0xFFFFFFFF, 0x541E55},
    {0xFFFFFFFF, 0x59A6DA}, {0xFFFFFFFF, 0x59A6E2}}};
constexpr std::array<std::array<std::uint32_t, 2>, 5> saveRows{{
    {0xFFFFFFFF, 0x59A710}, {0xFFFFFFFF, 0x59A722}, {0xFFFFFFFF, 0x541E55},
    {0xFFFFFFFF, 0x59A72A}, {0xFFFFFFFF, 0x59A732}}};
constexpr std::array<std::array<std::uint32_t, 2>, 2> rollingRows{{
    {0xFFFFFFFF, 0x59A950}, {0, 0x59A958}}};
constexpr std::array<std::uint32_t, 1> factorySlots{0x469030}; // 5B2A98唯一虚槽。
constexpr char title[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75%02d";
constexpr char noSave[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"
    "\x82\xB3\x82\xEA\x82\xBD\x83\x66\x81\x5B\x83\x5E\x82\xAA"
    "\x91\xB6\x8D\xDD\x82\xB5\x82\xDC\x82\xB9\x82\xF1\x81\x49";
constexpr char loadError[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"
    "\x82\xB3\x82\xEA\x82\xBD\x83\x66\x81\x5B\x83\x5E\x82\xCC"
    "\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5"
    "\x82\xDC\x82\xB5\x82\xBD\x81\x49";
}
std::span<const QuickOperationsNativeFunctionInfo, 4> QuickOperationsNativeFunctionInfos() noexcept { return infos; }
std::span<const std::array<std::uint32_t, 2>, 5> QuickOperationsNativeLoadUnwindRows() noexcept { return loadRows; }
std::span<const std::array<std::uint32_t, 2>, 5> QuickOperationsNativeSaveUnwindRows() noexcept { return saveRows; }
std::span<const std::array<std::uint32_t, 2>, 2> QuickOperationsNativeRollingUnwindRows() noexcept { return rollingRows; }
std::span<const std::uint32_t, 1> QuickOperationsNativeFactoryOriginalSlots() noexcept { return factorySlots; }
const char* QuickOperationsNativeRollingTitleFormat() noexcept { return title; }
const char* QuickOperationsNativeNoSaveMessage() noexcept { return noSave; }
const char* QuickOperationsNativeLoadErrorMessage() noexcept { return loadError; }
}
