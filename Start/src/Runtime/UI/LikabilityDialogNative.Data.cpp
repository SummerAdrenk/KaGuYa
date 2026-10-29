#include "LikabilityDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 608F9C/609154/60915C，原18号CP932字节；没有Unicode重编码或后备名字。
#define LK_STATUS "\x83\x58\x83\x65\x81\x5B\x83\x5E\x83\x58"
#define LK_SCREEN "\x89\xE6\x96\xCA"
#define LK_BACK "\x96\xDF\x82\xE9"
#define LK_BUTTON "\x83\x7B\x83\x5E\x83\x93"
#define LK_BASE "\x83\x78\x81\x5B\x83\x58"
#define LK_DIGIT "\x90\x94\x8E\x9A"
const char target[] = "vrm:\\" LK_STATUS LK_SCREEN "_" LK_BACK LK_BUTTON "_target.alp";
const GuiButtonDescriptorNative2Ch button{
    {1, {"vrm:\\" LK_STATUS LK_SCREEN "_" LK_BACK LK_BUTTON "_on.alp", nullptr, nullptr,
        target, target, "vrm:\\" LK_STATUS LK_SCREEN "_" LK_BACK LK_BUTTON "_click.alp", nullptr}, nullptr}, 0, -1};
const char* const backgrounds[2]{
    "vrm:\\" LK_STATUS LK_SCREEN "_" LK_BASE ".bmp",
    "vrm:\\" LK_STATUS LK_SCREEN "_" LK_BASE "_\x96\xA2\x8A\x4A\x95\xFA" ".bmp"};
const char* const digits[11]{
    "vrm:\\" LK_STATUS "_" LK_DIGIT "0.alp", "vrm:\\" LK_STATUS "_" LK_DIGIT "1.alp",
    "vrm:\\" LK_STATUS "_" LK_DIGIT "2.alp", "vrm:\\" LK_STATUS "_" LK_DIGIT "3.alp",
    "vrm:\\" LK_STATUS "_" LK_DIGIT "4.alp", "vrm:\\" LK_STATUS "_" LK_DIGIT "5.alp",
    "vrm:\\" LK_STATUS "_" LK_DIGIT "6.alp", "vrm:\\" LK_STATUS "_" LK_DIGIT "7.alp",
    "vrm:\\" LK_STATUS "_" LK_DIGIT "8.alp", "vrm:\\" LK_STATUS "_" LK_DIGIT "9.alp",
    "vrm:\\" LK_STATUS "_" LK_DIGIT "\x81\x7C" ".alp"};
#define LK_SPACE "\x81\x40"
#define LK_WEIGHT "\x91\xCC\x8F\x64"
#define LK_INTIMACY "\x90\x65\x96\xA7\x93\x78"
#define LK_DEPEND "\x88\xCB\x91\xB6\x93\x78"
#define LK_SHINYA "\x90\x53\x8D\xD8"
#define LK_YUKIA "\x90\xE1\x88\xA4"
#define LK_BRUETTE "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67"
#define LK_KAYOKO "\x89\xC4\x91\xE3\x8E\x71"
const char* const variables[3][3]{
    {LK_SHINYA LK_SPACE LK_WEIGHT, LK_SHINYA LK_SPACE LK_INTIMACY, LK_SHINYA LK_SPACE LK_DEPEND},
    {LK_YUKIA LK_SPACE LK_WEIGHT, LK_YUKIA LK_SPACE LK_INTIMACY, LK_YUKIA LK_SPACE LK_DEPEND},
    {LK_BRUETTE LK_SPACE LK_WEIGHT, LK_BRUETTE LK_SPACE LK_INTIMACY, LK_BRUETTE LK_SPACE LK_DEPEND}};
const char* const extras[2]{LK_KAYOKO LK_SPACE "\x8F\x6F\x8C\xBB\x83\x74\x83\x89\x83\x4F", LK_KAYOKO LK_SPACE LK_INTIMACY};
const POINT positions[3][9]{
    {{304,363},{263,363},{241,378},{297,483},{265,483},{238,483},{297,613},{265,613},{238,613}},
    {{545,363},{504,363},{482,378},{538,483},{506,483},{479,483},{538,613},{506,613},{479,613}},
    {{784,363},{743,363},{721,378},{778,483},{746,483},{719,483},{778,613},{746,613},{719,613}}};
const POINT extraPositions[3]{{1017,483},{985,483},{958,483}};
constexpr std::uint32_t slots[35]{
    0x509F00,0x4FC750,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,0x536500,
    0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,0x510B90,0x5106B0,
    0x511610,0x50C950,0x52D4A0,0x508D40,0x50AB50,0x518F30,0x5261A0,0x5231C0,
    0x5230B0,0x50FC80,0x522E90,0x510030,0x5100E0,0x50A6A0,0x50B920,0x510220,
    0x510170,0x5102B0,0x518A80};
constexpr LikabilityNativeDetail::UnwindEntry ctorUnwind[14]{
    {-1,0x5A14A0},{0,0x5A14A8},{1,0x5A14B3},{2,0x5A14BE},{3,0x5A14CC},
    {3,0x5A14DB},{5,0x5A14E3},{5,0x5A14F2},{5,0x541E55},{3,0x541E55},
    {3,0x5A14FA},{10,0x5A1502},{10,0x541E55},{3,0x541E55}};
constexpr LikabilityNativeDetail::UnwindEntry refreshUnwind[14]{
    {-1,0x5A81F0},{0,0x5A81F8},{1,0x5A8200},{0,0x541E55},{0,0x5A820F},
    {0,0x541E55},{0,0x5A8217},{0,0x5A821F},{0,0x5A8227},{0,0x5A822F},
    {0,0x541E55},{0,0x5A8237},{0,0x5A823F},{0,0x5A8247}};
#undef LK_STATUS
#undef LK_SCREEN
#undef LK_BACK
#undef LK_BUTTON
#undef LK_BASE
#undef LK_DIGIT
#undef LK_SPACE
#undef LK_WEIGHT
#undef LK_INTIMACY
#undef LK_DEPEND
#undef LK_SHINYA
#undef LK_YUKIA
#undef LK_BRUETTE
#undef LK_KAYOKO
}
std::span<const std::uint32_t,35> LikabilityDialogNativeOriginalSlotsSample18() noexcept { return slots; }
const GuiButtonDescriptorNative2Ch& LikabilityDialogNativeButtonSample18() noexcept { return button; }
std::span<const char* const,2> LikabilityDialogNativeBackgroundNamesSample18() noexcept { return backgrounds; }
std::span<const char* const,11> LikabilityDialogNativeDigitNamesSample18() noexcept { return digits; }
namespace LikabilityNativeDetail
{
const char* const* VariableNames(unsigned row) noexcept { return variables[row]; }
const char* const* ExtraNames() noexcept { return extras; }
const POINT* Positions(unsigned row) noexcept { return positions[row]; }
const POINT* ExtraPositions() noexcept { return extraPositions; }
std::span<const UnwindEntry,14> ConstructorUnwind() noexcept { return ctorUnwind; }
std::span<const UnwindEntry,14> RefreshUnwind() noexcept { return refreshUnwind; }
}
}
