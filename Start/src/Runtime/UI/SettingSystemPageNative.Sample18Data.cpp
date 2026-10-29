#include <Runtime/SettingSystemPageNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 5BCC80..5BCEE4：原CP932逐字节，非运行时PE地址。三个组的position指针均为null。
#define SET18_SCREEN "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA"
#define SET18_TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define SET18_CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define SET18_OFF "\x83\x49\x83\x74"
constexpr char frame[] = SET18_SCREEN ".vrm";
constexpr char exitTarget[] = SET18_SCREEN SET18_TARGET "\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9.vrm";
constexpr char exitClick[] = SET18_SCREEN SET18_CLICK "\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9.vrm";
constexpr char cancelTarget[] = SET18_SCREEN SET18_TARGET "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B.vrm";
constexpr char cancelClick[] = SET18_SCREEN SET18_CLICK "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B.vrm";
constexpr char defaultsTarget[] = SET18_SCREEN SET18_TARGET "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x96\xDF\x82\xB7.vrm";
constexpr char defaultsClick[] = SET18_SCREEN SET18_CLICK "\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xC9\x96\xDF\x82\xB7.vrm";
constexpr char systemOff[] = SET18_SCREEN SET18_OFF "SYSTEM.vrm";
constexpr char systemTarget[] = SET18_SCREEN SET18_TARGET "SYSTEM.vrm";
constexpr char systemClick[] = SET18_SCREEN SET18_CLICK "SYSTEM.vrm";
constexpr char system2Off[] = SET18_SCREEN SET18_OFF "SYSTEM2.vrm";
constexpr char system2Target[] = SET18_SCREEN SET18_TARGET "SYSTEM2.vrm";
constexpr char system2Click[] = SET18_SCREEN SET18_CLICK "SYSTEM2.vrm";
constexpr char system3Off[] = SET18_SCREEN SET18_OFF "SYSTEM3.vrm";
constexpr char system3Target[] = SET18_SCREEN SET18_TARGET "SYSTEM3.vrm";
constexpr char system3Click[] = SET18_SCREEN SET18_CLICK "SYSTEM3.vrm";
constexpr char soundOff[] = SET18_SCREEN SET18_OFF "\x83\x54\x83\x45\x83\x93\x83\x68.vrm";
constexpr char soundTarget[] = SET18_SCREEN SET18_TARGET "\x83\x54\x83\x45\x83\x93\x83\x68.vrm";
constexpr char soundClick[] = SET18_SCREEN SET18_CLICK "\x83\x54\x83\x45\x83\x93\x83\x68.vrm";
#undef SET18_SCREEN
#undef SET18_TARGET
#undef SET18_CLICK
#undef SET18_OFF
constexpr GuiStaticDescriptorNative24h staticDescriptor{
    0x1A, {frame, nullptr, nullptr, frame, frame, nullptr, nullptr}, nullptr};
constexpr std::array<GuiButtonDescriptorNative2Ch, 7> buttons{{
    {{0x1F, {nullptr, nullptr, nullptr, exitTarget, exitTarget, exitClick, nullptr}, nullptr}, 3, -1},
    {{0x21, {nullptr, nullptr, nullptr, cancelTarget, cancelTarget, cancelClick, nullptr}, nullptr}, 3, -1},
    {{0x22, {nullptr, nullptr, nullptr, defaultsTarget, defaultsTarget, defaultsClick, nullptr}, nullptr}, 3, -1},
    {{0x23, {nullptr, systemOff, nullptr, systemTarget, systemTarget, systemClick, nullptr}, nullptr}, 3, -1},
    {{0x24, {nullptr, system2Off, nullptr, system2Target, system2Target, system2Click, nullptr}, nullptr}, 3, -1},
    {{0x26, {nullptr, system3Off, nullptr, system3Target, system3Target, system3Click, nullptr}, nullptr}, 3, -1},
    {{0x25, {nullptr, soundOff, nullptr, soundTarget, soundTarget, soundClick, nullptr}, nullptr}, 3, -1}}};
constexpr std::array<std::uint32_t, 9> rawStatic{
    0x1A,0x5BCC80,0,0,0x5BCC80,0x5BCC80,0,0,0};
constexpr std::array<std::array<std::uint32_t, 11>, 7> rawButtons{{
    {0x1F,0,0,0,0x5BCC94,0x5BCC94,0x5BCCBC,0,0,3,0xFFFFFFFF},
    {0x21,0,0,0,0x5BCCE0,0x5BCCE0,0x5BCD08,0,0,3,0xFFFFFFFF},
    {0x22,0,0,0,0x5BCD2C,0x5BCD2C,0x5BCD58,0,0,3,0xFFFFFFFF},
    {0x23,0,0x5BCD80,0,0x5BCD9C,0x5BCD9C,0x5BCDC0,0,0,3,0xFFFFFFFF},
    {0x24,0,0x5BCDE0,0,0x5BCDFC,0x5BCDFC,0x5BCE20,0,0,3,0xFFFFFFFF},
    {0x26,0,0x5BCE40,0,0x5BCE5C,0x5BCE5C,0x5BCE80,0,0,3,0xFFFFFFFF},
    {0x25,0,0x5BCEA0,0,0x5BCEC0,0x5BCEC0,0x5BCEE4,0,0,3,0xFFFFFFFF}}};
constexpr std::array<std::uint32_t, 35> original{
    0x509ED0,0x4FE410,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50F8A0,0x5309F0,0x509540,0x50AC50,
    0x518C30,0x525FB0,0x5231C0,0x5230B0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80};
constexpr std::array<std::array<std::uint32_t, 2>, 14> unwind{{
    {0xFFFFFFFF,0x5A3030},{0,0x5A303B},{1,0x5A3049},{2,0x5A3051},
    {3,0x5A3059},{4,0x5A3061},{4,0x541E55},{4,0x5A306C},
    {4,0x541E55},{4,0x5A3074},{4,0x541E55},{3,0x541E55},
    {2,0x541E55},{1,0x541E55}}};
}
const GuiStaticDescriptorNative24h& SettingSystemPageNativeStaticSample18() noexcept { return staticDescriptor; }
std::span<const GuiButtonDescriptorNative2Ch, 7> SettingSystemPageNativeButtonsSample18() noexcept { return buttons; }
std::span<const std::uint32_t, 35> SettingSystemPageNativeOriginalVtableSample18() noexcept { return original; }
std::span<const std::uint32_t, 9> SettingSystemPageNativeRawStaticSample18() noexcept { return rawStatic; }
std::span<const std::array<std::uint32_t, 11>, 7> SettingSystemPageNativeRawButtonsSample18() noexcept { return rawButtons; }
std::span<const std::array<std::uint32_t, 2>, 14> SettingSystemPageNativeUnwindSample18() noexcept { return unwind; }
}
