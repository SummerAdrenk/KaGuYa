#include "ProgramSystemDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE的CP932字节；只做静态重定位，运行时不读PE、不做编码猜测。
#define P "vrm:\\" "\x83\x52\x83\x7D\x83\x93\x83\x68"
#define ON "\x83\x49\x83\x93"
#define OFF "\x83\x49\x83\x74"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define RANGE "\x94\xCD\x88\xCD"
#define PLAY "\x83\x76\x83\x8C\x83\x43"
#define QLOAD "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"
#define QSAVE "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"
#define LOAD "\x83\x8D\x81\x5B\x83\x68"
#define SAVE "\x83\x5A\x81\x5B\x83\x75"
#define VOICE "\x83\x7B\x83\x43\x83\x58"
#define OPACITY "\x93\xA7\x96\xBE\x93\x78"
#define LIKE "\x8D\x44\x8A\xB4\x93\x78"
#define TITLE "\x83\x5E\x83\x43\x83\x67\x83\x8B"
#define AUTO "\x83\x49\x81\x5B\x83\x67"
#define SKIP "\x83\x58\x83\x4C\x83\x62\x83\x76"
#define SCREEN "\x89\xE6\x96\xCA"
#define NAMES(X) \
    X(5B9BF0, "vrm:\\" "\x91\x49\x91\xF0" SCREEN "_\x89\xBA\x92\x6E.alp") \
    X(5B9C08, P ON QLOAD ".alp") \
    X(5B9C2C, P OFF QLOAD ".alp") \
    X(5B9C50, P TARGET QLOAD ".alp") \
    X(5B9C7C, P CLICK QLOAD ".alp") \
    X(5B9CA4, P RANGE QLOAD ".alp") \
    X(5B9CC8, P ON QSAVE ".alp") \
    X(5B9CEC, P OFF QSAVE ".alp") \
    X(5B9D10, P TARGET QSAVE ".alp") \
    X(5B9D3C, P CLICK QSAVE ".alp") \
    X(5B9D64, P RANGE QSAVE ".alp") \
    X(5B9D88, P ON LOAD ".alp") \
    X(5B9DA4, P OFF LOAD ".alp") \
    X(5B9DC0, P TARGET LOAD ".alp") \
    X(5B9DE4, P CLICK LOAD ".alp") \
    X(5B9E04, P RANGE LOAD ".alp") \
    X(5B9E20, P ON SAVE ".alp") \
    X(5B9E3C, P OFF SAVE ".alp") \
    X(5B9E58, P TARGET SAVE ".alp") \
    X(5B9E7C, P CLICK SAVE ".alp") \
    X(5B9E9C, P RANGE SAVE ".alp") \
    X(5B9EB8, P ON VOICE ".alp") \
    X(5B9ED4, P OFF VOICE ".alp") \
    X(5B9EF0, P TARGET VOICE ".alp") \
    X(5B9F14, P CLICK VOICE ".alp") \
    X(5B9F34, P RANGE VOICE ".alp") \
    X(5B9F50, P ON OPACITY ".alp") \
    X(5B9F6C, P OFF OPACITY ".alp") \
    X(5B9F88, P TARGET OPACITY ".alp") \
    X(5B9FAC, P RANGE OPACITY ".alp") \
    X(5B9FC8, P ON LIKE ".alp") \
    X(5B9FE4, P OFF LIKE ".alp") \
    X(5BA000, P TARGET LIKE ".alp") \
    X(5BA024, P CLICK LIKE ".alp") \
    X(5BA044, P RANGE LIKE ".alp") \
    X(5BA060, P ON TITLE ".alp") \
    X(5BA080, P OFF TITLE ".alp") \
    X(5BA0A0, P TARGET TITLE ".alp") \
    X(5BA0C4, P CLICK TITLE ".alp") \
    X(5BA0E8, P RANGE TITLE ".alp") \
    X(5BA108, P ON AUTO ".alp") \
    X(5BA124, P OFF AUTO ".alp") \
    X(5BA140, P TARGET AUTO ".alp") \
    X(5BA164, P CLICK AUTO ".alp") \
    X(5BA184, P RANGE AUTO ".alp") \
    X(5BA1A0, P PLAY AUTO ".alp") \
    X(5BA1C0, P CLICK "2" AUTO ".alp") \
    X(5BA1E4, P ON SKIP ".alp") \
    X(5BA204, P OFF SKIP ".alp") \
    X(5BA224, P TARGET SKIP ".alp") \
    X(5BA248, P CLICK SKIP ".alp") \
    X(5BA26C, P RANGE SKIP ".alp") \
    X(5BA28C, P PLAY SKIP ".alp") \
    X(5BA2AC, P PLAY OFF SKIP ".alp") \
    X(5AE8BC, QLOAD) \
    X(5AE8AC, QSAVE) \
    X(5AE224, LOAD SCREEN) \
    X(5AE218, SAVE SCREEN) \
    X(5B4E74, TITLE "\x82\xD6") \
    X(5B4E80, LIKE SCREEN)
#define DECLARE(va, bytes) constexpr char n##va[] = bytes;
NAMES(DECLARE)
#undef DECLARE
#define EVIDENCE(va, bytes) {0x##va, n##va},
const ProgramSystemDialogNativeStringEvidence names[]{NAMES(EVIDENCE)};
#undef EVIDENCE
#undef NAMES
#undef P
#undef ON
#undef OFF
#undef TARGET
#undef CLICK
#undef RANGE
#undef PLAY
#undef QLOAD
#undef QSAVE
#undef LOAD
#undef SAVE
#undef VOICE
#undef OPACITY
#undef LIKE
#undef TITLE
#undef AUTO
#undef SKIP
#undef SCREEN

constexpr std::array<const char*, 7> buttonNames[8]{
    {n5B9C08,n5B9C2C,nullptr,n5B9C50,n5B9C50,n5B9C7C,n5B9CA4},
    {n5B9CC8,n5B9CEC,nullptr,n5B9D10,n5B9D10,n5B9D3C,n5B9D64},
    {n5B9D88,n5B9DA4,nullptr,n5B9DC0,n5B9DC0,n5B9DE4,n5B9E04},
    {n5B9E20,n5B9E3C,nullptr,n5B9E58,n5B9E58,n5B9E7C,n5B9E9C},
    {n5B9EB8,n5B9ED4,nullptr,n5B9EF0,n5B9EF0,n5B9F14,n5B9F34},
    {n5B9F50,n5B9F6C,nullptr,n5B9F88,n5B9F88,n5B9F88,n5B9FAC},
    {n5B9FC8,n5B9FE4,nullptr,n5BA000,n5BA000,n5BA024,n5BA044},
    {n5BA060,n5BA080,nullptr,n5BA0A0,n5BA0A0,n5BA0C4,n5BA0E8}};
constexpr GuiStaticDescriptorNative24h backgrounds[2]{
    {0x30,{n5B9BF0,n5B9BF0,nullptr,n5B9BF0,n5B9BF0,nullptr,nullptr},nullptr},
    {0x17,{n5B9BF0,n5B9BF0,nullptr,n5B9BF0,n5B9BF0,nullptr,nullptr},nullptr}};
constexpr GuiButtonDescriptorNative2Ch buttons00[8]{
    {{0x35,buttonNames[0],nullptr},6,-1}, {{0x34,buttonNames[1],nullptr},6,-1},
    {{0x3A,buttonNames[2],nullptr},6,-1}, {{0x39,buttonNames[3],nullptr},6,-1},
    {{0x31,buttonNames[4],nullptr},6,-1}, {{0x33,buttonNames[5],nullptr},6,-1},
    {{0x38,buttonNames[6],nullptr},6,-1}, {{0x3B,buttonNames[7],nullptr},6,-1}};
constexpr GuiButtonDescriptorNative2Ch buttons01[8]{
    {{0x1C,buttonNames[0],nullptr},6,-1}, {{0x1B,buttonNames[1],nullptr},6,-1},
    {{0x21,buttonNames[2],nullptr},6,-1}, {{0x20,buttonNames[3],nullptr},6,-1},
    {{0x18,buttonNames[4],nullptr},6,-1}, {{0x1A,buttonNames[5],nullptr},6,-1},
    {{0x1F,buttonNames[6],nullptr},6,-1}, {{0x22,buttonNames[7],nullptr},6,-1}};
using Check = SaveLoadMessageBoxCheckBoxDescriptorNative48h;
constexpr std::array<const char*, 7> autoNormal{n5BA108,n5BA124,nullptr,n5BA140,n5BA140,n5BA164,n5BA184};
constexpr std::array<const char*, 7> autoChecked{n5BA1A0,n5BA124,nullptr,n5BA1A0,n5BA1A0,n5BA1C0,n5BA184};
constexpr std::array<const char*, 7> skipNormal{n5BA1E4,n5BA204,nullptr,n5BA224,n5BA224,n5BA248,n5BA26C};
constexpr std::array<const char*, 7> skipChecked{n5BA28C,n5BA2AC,nullptr,n5BA28C,n5BA28C,n5BA248,n5BA26C};
constexpr Check checks00[2]{
    {0x36,autoNormal,autoChecked,nullptr,6,0x1A}, {0x37,skipNormal,skipChecked,nullptr,6,-1}};
constexpr Check checks01[2]{
    {0x1D,autoNormal,autoChecked,nullptr,6,0x1A}, {0x1E,skipNormal,skipChecked,nullptr,6,-1}};
// 608940/608E5C各四组，两个Button组绝不能按事件值重新排序。
const ProgramSystemDialogNativeControlGroup groups00[4]{
    {0,1,backgrounds}, {1,4,buttons00}, {2,2,checks00}, {1,4,buttons00+4}};
const ProgramSystemDialogNativeControlGroup groups01[4]{
    {0,1,backgrounds+1}, {1,4,buttons01}, {2,2,checks01}, {1,4,buttons01+4}};
constexpr ProgramSystemDialogNativeRoute routes00[6]{
    {53,n5AE8BC},{52,n5AE8AC},{58,n5AE224},{57,n5AE218},{59,n5B4E74},{56,n5B4E80}};
constexpr ProgramSystemDialogNativeRoute routes01[6]{
    {28,n5AE8BC},{27,n5AE8AC},{33,n5AE224},{32,n5AE218},{34,n5B4E74},{31,n5B4E80}};

constexpr std::array<std::uint32_t,35> slots00{
    0x509ED0,0x4FC2B0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x5364D0,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50C300,0x52D090,0x508CC0,0x50AB50,
    0x518C30,0x525FB0,0x5231C0,0x5230B0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80};
constexpr std::array<std::uint32_t,35> slots01{
    0x509ED0,0x4FC350,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x5364E0,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50C5F0,0x52D270,0x508CD0,0x50AB50,
    0x518C30,0x525FB0,0x5231C0,0x5230B0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80};
const ProgramSystemDialogNativeTypeEvidence types[2]{
    {0x4DF420,0x5C0280,0x5C10AC,0x5DDAA4,0x53D660,0x4FB300,slots00},
    {0x4DFCA0,0x5C044C,0x5C10F4,0x5DDC6C,0x53D670,0x4FB330,slots01}};

constexpr std::int32_t ctorPrevious[28]{
    -1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,19,19,19,19,2,2,2,1};
constexpr std::uint32_t ctorActions00[28]{
    0x5A1230,0x5A1238,0x5A1243,0x5A125C,0x541E55,0x5A1264,0x541E55,
    0x5A126C,0x541E55,0x5A1277,0x541E55,0x5A1282,0x541E55,0x5A128D,
    0x541E55,0x5A1298,0x541E55,0x5A12A3,0x541E55,0x5A12AE,0x5A12B6,
    0x541E55,0x5A12BE,0x541E55,0x541E55,0x5A12C6,0x541E55,0x541E55};
constexpr std::uint32_t ctorActions01[28]{
    0x5A12F0,0x5A12F8,0x5A1303,0x5A131C,0x541E55,0x5A1324,0x541E55,
    0x5A132C,0x541E55,0x5A1337,0x541E55,0x5A1342,0x541E55,0x5A134D,
    0x541E55,0x5A1358,0x541E55,0x5A1363,0x541E55,0x5A136E,0x5A1376,
    0x541E55,0x5A137E,0x541E55,0x541E55,0x5A1386,0x541E55,0x541E55};
constexpr std::int32_t beginPrevious[8]{-1,-1,-1,-1,-1,-1,-1,-1};
constexpr std::uint32_t beginActions00[8]{0x5A60C0,0x541E55,0x5A60C8,0x541E55,0x5A60D0,0x541E55,0x5A60D8,0x541E55};
constexpr std::uint32_t beginActions01[8]{0x5A6100,0x541E55,0x5A6108,0x541E55,0x5A6110,0x541E55,0x5A6118,0x541E55};
constexpr std::int32_t pollPrevious[3]{-1,-1,-1};
constexpr std::uint32_t pollActions00[3]{0x5A84E0,0x5A84E8,0x541E55};
constexpr std::uint32_t pollActions01[3]{0x5A8510,0x5A8518,0x541E55};
constexpr std::int32_t deletePrevious[1]{-1};
constexpr std::uint32_t deleteActions[1]{0x541E55};
const ProgramSystemDialogNativeEhEvidence eh[8]{
    {0x4DF420,0x5FAF04,0x5FAF28,1,ctorPrevious,ctorActions00},
    {0x4DFCA0,0x5FB8EC,0x5FB910,1,ctorPrevious,ctorActions01},
    {0x50C300,0x5FB034,0x5FB058,1,beginPrevious,beginActions00},
    {0x50C5F0,0x5FBA1C,0x5FBA40,1,beginPrevious,beginActions01},
    {0x52D090,0x5FB0B0,0x5FB098,1,pollPrevious,pollActions00},
    {0x52D270,0x5FBA98,0x5FBA80,1,pollPrevious,pollActions01},
    {0x4FC2B0,0x5FB110,0x5FB108,5,deletePrevious,deleteActions},
    {0x4FC350,0x5FBAF8,0x5FBAF0,5,deletePrevious,deleteActions}};
}
std::span<const ProgramSystemDialogNativeControlGroup,4> ProgramSystem00DialogNativeGroupsSample18() noexcept { return groups00; }
std::span<const ProgramSystemDialogNativeControlGroup,4> ProgramSystem01DialogNativeGroupsSample18() noexcept { return groups01; }
std::span<const ProgramSystemDialogNativeRoute,6> ProgramSystem00DialogNativeRoutesSample18() noexcept { return routes00; }
std::span<const ProgramSystemDialogNativeRoute,6> ProgramSystem01DialogNativeRoutesSample18() noexcept { return routes01; }
std::span<const ProgramSystemDialogNativeTypeEvidence,2> ProgramSystemDialogNativeTypesSample18() noexcept { return types; }
std::span<const ProgramSystemDialogNativeEhEvidence,8> ProgramSystemDialogNativeEhSample18() noexcept { return eh; }
std::span<const ProgramSystemDialogNativeStringEvidence> ProgramSystemDialogNativeStringsSample18() noexcept { return names; }
}
