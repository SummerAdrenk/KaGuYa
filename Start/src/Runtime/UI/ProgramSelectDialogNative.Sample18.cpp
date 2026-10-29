#include <Runtime/ProgramSelectDialogNative.h>
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<std::uint32_t, 42> baseSlots{
    0x509FF0,0x4FDBD0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x5363C0,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50E770,0x52F6E0,0x509340,0x50AB50,
    0x518C30,0x525FB0,0x524740,0x524720,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80,
    0x4FF130,0x509DE0,0x541C5F,0x541C5F,0x503AA0,0x530E40,0x541C5F};
constexpr auto TopSlots(bool mode01) noexcept
{
    auto result = baseSlots;
    result[1] = mode01 ? 0x4FC280 : 0x4FC140;
    result[37] = mode01 ? 0x5221A0 : 0x521FD0;
    result[38] = mode01 ? 0x51C4E0 : 0x51C120;
    result[39] = mode01 ? 0x5039A0 : 0x5038A0;
    result[41] = mode01 ? 0x531010 : 0x530E80;
    return result;
}
constexpr auto slots00 = TopSlots(false), slots01 = TopSlots(true);
constexpr std::int32_t constructorPrevious[]{-1,0,0,0,0,0,0,0};
constexpr std::uint32_t constructor00Actions[]{0x5A1030,0x5A1038,0x5A104A,0x541E55,0x5A1052,0x5A1061,0x541E55,0x5A1069};
constexpr std::uint32_t constructor01Actions[]{0x5A11C0,0x5A11C8,0x5A11DA,0x541E55,0x5A11E2,0x5A11F1,0x541E55,0x5A11F9};
constexpr std::int32_t destructorPrevious[]{-1,-1};
constexpr std::uint32_t destructorActions[]{0x541E55,0x541E55};
constexpr std::int32_t capturePrevious[]{-1,0};
constexpr std::uint32_t captureActions[]{0x5A53BF,0x5A53B0};
constexpr std::int32_t wrapperPrevious[]{-1,0,0};
constexpr std::uint32_t wrapper00Actions[]{0x5A5338,0x5A5330,0x541E55};
constexpr std::uint32_t wrapper01Actions[]{0x5A5378,0x5A5370,0x541E55};
constexpr std::uint32_t compositeActions[]{0x5A7D10,0x541E55};
constexpr std::int32_t updatePrevious[]{-1,-1,-1,-1};
constexpr std::uint32_t update00Actions[]{0x5A79C0,0x541E55,0x5A79C8,0x541E55};
constexpr std::uint32_t update01Actions[]{0x5A79F0,0x541E55,0x5A79F8,0x541E55};
constexpr std::int32_t reopenPrevious[]{-1,0,0,-1};
constexpr std::uint32_t reopen00Actions[]{0x5A88D0,0x5A88D8,0x541E55,0x541E55};
constexpr std::uint32_t reopen01Actions[]{0x5A8900,0x5A8908,0x541E55,0x541E55};
const ProgramSelectDialogNativeEhEvidence evidence[]{
    {0x4DE1E0,0x5FADEC,1,constructorPrevious,constructor00Actions},
    {0x4DF0E0,0x5FB808,1,constructorPrevious,constructor01Actions},
    {0x4F8D50,0x5FAD1C,5,destructorPrevious,destructorActions},
    {0x503AA0,0x5FAD84,1,capturePrevious,captureActions},
    {0x5038A0,0x5FAEAC,1,wrapperPrevious,wrapper00Actions},
    {0x5039A0,0x5FB8C8,1,wrapperPrevious,wrapper01Actions},
    {0x524740,0x5FAD50,1,destructorPrevious,compositeActions},
    {0x521FD0,0x5FADC8,1,updatePrevious,update00Actions},
    {0x5221A0,0x5FB7E4,1,updatePrevious,update01Actions},
    {0x530E80,0x5FAE70,1,reopenPrevious,reopen00Actions},
    {0x531010,0x5FB88C,1,reopenPrevious,reopen01Actions}};
}
std::span<const std::uint32_t, 42> ProgramSelectDialogNativeOriginalSlotsSample18(
    std::int32_t mode) noexcept
{
    switch (mode)
    {
    case -1: return baseSlots;
    case 0: return slots00;
    case 1: return slots01;
    default: std::terminate();
    }
}
std::span<const ProgramSelectDialogNativeEhEvidence> ProgramSelectDialogNativeEhSample18() noexcept
{ return evidence; }
}
