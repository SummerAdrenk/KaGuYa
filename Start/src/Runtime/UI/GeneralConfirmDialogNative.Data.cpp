#include "GeneralConfirmDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原18 vtable/EH仅作历史证据；运行数据由编译profile提供。
constexpr std::array<std::uint32_t, 38> slots{{
    0x509ED0,0x4FCAC0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,0x536500,
    0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,0x510BA0,0x5106D0,
    0x511610,0x50CF50,0x52DBC0,0x508E80,0x50AB70,0x518C30,0x5267E0,0x5231C0,
    0x5230B0,0x50FC80,0x522E90,0x510030,0x5100E0,0x50A6A0,0x50B920,0x510220,
    0x510170,0x5102B0,0x518A80,0x5086B0,0x524E60,0x51C100,
}};
// 5F49E4：state3/4/8/10/11/12/14是正常尾terminate态，绝非可重试清理。
constexpr std::array<std::array<std::uint32_t, 2>, 15> unwind{{
    {{0xFFFFFFFF,0x5A18A0}},{{0,0x5A18A8}},{{1,0x5A18B0}},{{1,0x541E55}},{{0,0x541E55}},
    {{0,0x5A18B8}},{{5,0x5A18C0}},{{6,0x5A18C8}},{{6,0x541E55}},{{6,0x5A18D0}},
    {{6,0x541E55}},{{5,0x541E55}},{{0,0x541E55}},{{0,0x5A18D8}},{{0,0x541E55}},
}};
}
std::span<const std::uint32_t, 38> GeneralConfirmDialogNativeOriginalSlots() noexcept { return slots; }
std::span<const GuiStaticDescriptorNative24h> GeneralConfirmDialogNativeStaticDescriptors()
{ return CurrentGeneralConfirmResourceProfile().statics; }
std::span<const GuiButtonDescriptorNative2Ch> GeneralConfirmDialogNativeButtonDescriptors()
{ return CurrentGeneralConfirmResourceProfile().yesNoButtons; }
std::span<const GeneralConfirmModeRow> GeneralConfirmDialogNativeModeRows()
{ return CurrentGeneralConfirmResourceProfile().modes; }
std::span<const std::array<std::uint32_t, 2>, 15> GeneralConfirmDialogNativeCtorUnwindRows() noexcept { return unwind; }
}
