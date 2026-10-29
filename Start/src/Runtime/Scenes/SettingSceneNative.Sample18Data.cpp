#include <Runtime/SettingSceneNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<std::uint32_t, 4> scene{0x4C3630, 0x4A7020, 0x4BA800, 0x4B5E20};
// 5BEE4C..5BEEDC，下一 DWORD 5BEEE0 为 RTTI；仅记录，不提供 GUI 实现。
constexpr std::array<std::uint32_t, 37> dialog{
    0x509ED0, 0x4FE220, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x5363C0,
    0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740,
    0x510B90, 0x5106B0, 0x511610, 0x50F3C0, 0x530400, 0x5094F0, 0x50AB50,
    0x51A860, 0x527220, 0x523480, 0x523370, 0x50FC80, 0x522E90, 0x510030,
    0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80,
    0x4FF130, 0x509DE0};
// PE 5F0A4C：{previousState, action}，不是平台自己的 EH 表。
constexpr std::array<std::array<std::uint32_t, 2>, 3> unwind{{
    {0xFFFFFFFF, 0x59CD60}, {0, 0x59CD68}, {1, 0x59CD73}}};
}
std::span<const std::uint32_t, 4> SettingSceneNativeOriginalVtableSample18() noexcept { return scene; }
std::span<const std::uint32_t, 37> SettingSceneNativeOriginalDialogVtableSample18() noexcept { return dialog; }
std::span<const std::array<std::uint32_t, 2>, 3> SettingSceneNativeConstructorUnwindSample18() noexcept
{ return unwind; }
}
