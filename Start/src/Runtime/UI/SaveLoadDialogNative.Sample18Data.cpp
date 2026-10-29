#include <Runtime/SaveLoadDialogNativeData.h>

#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 权威PE SHA256 FBD6288599ABD4BF3CF3FC9717602DD81E056EE4F795955569DBC77FD7EAF277。
// 包括ASCII部分也逐字节转义，字符串末尾由字面量提供一个NUL。
constexpr std::array<const char*, 10> pageDigits{{ // 605FA0..605FC4。
    "\x70\x61\x67\x65\x30\x2E\x76\x72\x6D", // 5B74F4
    "\x70\x61\x67\x65\x31\x2E\x76\x72\x6D", // 5B7500
    "\x70\x61\x67\x65\x32\x2E\x76\x72\x6D", // 5B750C
    "\x70\x61\x67\x65\x33\x2E\x76\x72\x6D", // 5B7518
    "\x70\x61\x67\x65\x34\x2E\x76\x72\x6D", // 5B7524
    "\x70\x61\x67\x65\x35\x2E\x76\x72\x6D", // 5B7530
    "\x70\x61\x67\x65\x36\x2E\x76\x72\x6D", // 5B753C
    "\x70\x61\x67\x65\x37\x2E\x76\x72\x6D", // 5B7548
    "\x70\x61\x67\x65\x38\x2E\x76\x72\x6D", // 5B7554
    "\x70\x61\x67\x65\x39\x2E\x76\x72\x6D", // 5B7560
}};
constexpr std::array<const char*, 16> slotDigits{{ // 6060D0..60610C；cmp esi,40h。
    "\x66\x31\x2E\x76\x72\x6D", // 5B7D54
    "\x66\x32\x2E\x76\x72\x6D", // 5B7D80
    "\x66\x33\x2E\x76\x72\x6D", // 5B7DAC
    "\x66\x34\x2E\x76\x72\x6D", // 5B7DD4
    "\x66\x35\x2E\x76\x72\x6D", // 5B7E00
    "\x66\x36\x2E\x76\x72\x6D", // 5B7E2C
    "\x66\x37\x2E\x76\x72\x6D", // 5B7E5C
    "\x66\x38\x2E\x76\x72\x6D", // 5B7E94
    "\x4C\x66\x31\x2E\x76\x72\x6D", // 5B7EC8
    "\x4C\x66\x32\x2E\x76\x72\x6D", // 5B7EF8
    "\x4C\x66\x33\x2E\x76\x72\x6D", // 5B7F2C
    "\x4C\x66\x34\x2E\x76\x72\x6D", // 5B7F60
    "\x4C\x66\x35\x2E\x76\x72\x6D", // 5B7F68
    "\x4C\x66\x36\x2E\x76\x72\x6D", // 5B7F9C
    "\x4C\x66\x37\x2E\x76\x72\x6D", // 5B7FD0
    "\x4C\x66\x38\x2E\x76\x72\x6D", // 5B8000
}};
constexpr std::array<const char*, 24> dateDigits{{ // 606110..60616C；cmp esi,60h。
    "\x79\x30\x2E\x76\x72\x6D", // 5B8068
    "\x79\x31\x2E\x76\x72\x6D", // 5B8070
    "\x79\x32\x2E\x76\x72\x6D", // 5B80A8
    "\x79\x33\x2E\x76\x72\x6D", // 5B80B0
    "\x79\x34\x2E\x76\x72\x6D", // 5B80B8
    "\x79\x35\x2E\x76\x72\x6D", // 5B80C0
    "\x79\x36\x2E\x76\x72\x6D", // 5B80C8
    "\x79\x37\x2E\x76\x72\x6D", // 5B80D0
    "\x79\x38\x2E\x76\x72\x6D", // 5B80D8
    "\x79\x39\x2E\x76\x72\x6D", // 5B80E0
    "\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D", // 5B80E8：コロン
    "\x8E\xCE\x90\xFC\x2E\x76\x72\x6D", // 5B80F4：斜線
    "\x4C\x79\x30\x2E\x76\x72\x6D", // 5B8100
    "\x4C\x79\x31\x2E\x76\x72\x6D", // 5B8108
    "\x4C\x79\x32\x2E\x76\x72\x6D", // 5B8110
    "\x4C\x79\x33\x2E\x76\x72\x6D", // 5B8118
    "\x4C\x79\x34\x2E\x76\x72\x6D", // 5B8120
    "\x4C\x79\x35\x2E\x76\x72\x6D", // 5B8128
    "\x4C\x79\x36\x2E\x76\x72\x6D", // 5B8130
    "\x4C\x79\x37\x2E\x76\x72\x6D", // 5B8138
    "\x4C\x79\x38\x2E\x76\x72\x6D", // 5B8140
    "\x4C\x79\x39\x2E\x76\x72\x6D", // 5B8148
    "\x4C\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D", // 5B8150：Lコロン
    "\x4C\x8E\xCE\x90\xFC\x2E\x76\x72\x6D", // 5B815C：L斜線
}};
constexpr std::array<const char*, 2> noData{{ // 606050..606054。
    "\x4E\x4F\x44\x41\x54\x41\x2E\x76\x72\x6D", // 5B7944
    "\x4C\x4E\x4F\x44\x41\x54\x41\x2E\x76\x72\x6D", // 5B7950
}};
constexpr std::array<const char*, 2> noPreview{{ // 606058..60605C。
    "\x96\xB3\x92\x6E\x2E\x76\x72\x6D", // 5B7980：無地
    "\x4C\x96\xB3\x92\x6E\x2E\x76\x72\x6D", // 5B79B4：L無地
}};
constexpr std::array<const char*, 2> newMarker{{ // 606060..606064。
    "\x4E\x45\x57\x2E\x76\x72\x6D", // 5B7A74
    "\x4C\x4E\x45\x57\x2E\x76\x72\x6D", // 5B7AA4
}};
// 4EBDF6与4EBE66各遍历六组；保留原顺序，运行时相对坐标另由构造计算。
constexpr std::array<POINT, 6> pagePositions{{
    {604, 132}, {582, 132}, {560, 132}, {710, 132}, {688, 132}, {666, 132},
}};
constexpr std::array<SaveLoadResourceTableNativeAudit, 6> resourceTables{{
    {0x605FA0u, 0x4EBCBCu, 0x4EBDEAu, 0x605FC8u, 10},
    {0x6060D0u, 0x4EBF5Au, 0x4EC03Eu, 0x606110u, 16},
    {0x606110u, 0x4EC0BAu, 0x4EC19Eu, 0x606170u, 24},
    {0x606050u, 0x4EC21Au, 0x4EC2F1u, 0x606058u, 2},
    {0x606058u, 0x4EC36Au, 0x4EC441u, 0x606060u, 2},
    {0x606060u, 0x4EC4BAu, 0x4EC595u, 0x606068u, 2},
}};
// 5BE3C4为本类RTTI COL；5BE45C是下一类COL，不是第38槽。
constexpr std::array<std::uint32_t, 37> originalVtable{{
    0x50A020u, 0x4FDD70u, 0x5025A0u, 0x50A5D0u, // +00
    0x4FF020u, 0x509D20u, 0x5363C0u, 0x536500u, // +10
    0x50FF40u, 0x538660u, 0x510630u, 0x500800u, // +20
    0x532170u, 0x51E740u, 0x510B90u, 0x5106B0u, // +30
    0x511610u, 0x50E8B0u, 0x52F7A0u, 0x508CF0u, // +40
    0x50ABB0u, 0x518C30u, 0x525FB0u, 0x524860u, // +50
    0x524800u, 0x50FC80u, 0x522E90u, 0x510030u, // +60
    0x5100E0u, 0x50A6A0u, 0x50B920u, 0x510220u, // +70
    0x510170u, 0x5102B0u, 0x518A80u, 0x4FF130u, // +80
    0x509DE0u, // +90
}};
// 5F5368的九个原DWORD；maxState=41、unwindMap=5F538C、EHFlags=1。
constexpr std::array<std::uint32_t, 9> ctorFuncInfo{{
    0x19930522u, 41, 0x5F538Cu, 0, 0, 0, 0, 0, 1,
}};
// 完整原unwind map；terminate态也保留，不转译成生产清理策略。
constexpr std::array<SaveLoadCtorUnwindNativeAudit, 41> ctorUnwind{{
    {-1, 0x5A2520u}, {0, 0x5A2528u}, {1, 0x5A2536u}, {2, 0x5A2544u},
    {3, 0x5A2552u}, {4, 0x5A2560u}, {5, 0x5A256Eu}, {6, 0x5A257Cu},
    {7, 0x5A258Au}, {8, 0x5A2598u}, {9, 0x5A25A6u}, {10, 0x5A25B4u},
    {11, 0x5A25C2u}, {12, 0x5A25D0u},
    {13, 0x541E55u}, {13, 0x5A25DEu}, {15, 0x5A25E6u}, {13, 0x541E55u},
    {13, 0x541E55u}, {13, 0x5A25F5u}, {19, 0x5A25FDu}, {13, 0x541E55u},
    {13, 0x541E55u}, {13, 0x5A260Cu}, {23, 0x5A2614u}, {13, 0x541E55u},
    {13, 0x541E55u}, {13, 0x5A2623u}, {27, 0x5A262Bu}, {13, 0x541E55u},
    {13, 0x541E55u}, {13, 0x5A263Au}, {31, 0x5A2642u}, {13, 0x541E55u},
    {13, 0x541E55u}, {13, 0x5A2651u}, {35, 0x5A2659u}, {13, 0x541E55u},
    {13, 0x5A2668u}, {13, 0x5A2677u}, {13, 0x5A2689u},
}};
constexpr SaveLoadCtorCallerNativeAudit ctorCaller{
    0x4A17B0u, 0x4A1805u, 0x4A1837u, 0x4A1840u, 0x4A1844u,
    0x4A1845u, 0x4EB950u, 0x4EBC20u, 0x4EC872u,
};
}

std::span<const char* const> SaveLoadPageDigitsNativeSample18() noexcept { return pageDigits; }
std::span<const char* const> SaveLoadSlotDigitsNativeSample18() noexcept { return slotDigits; }
std::span<const char* const> SaveLoadDateDigitsNativeSample18() noexcept { return dateDigits; }
std::span<const char* const> SaveLoadNoDataNativeSample18() noexcept { return noData; }
std::span<const char* const> SaveLoadNoPreviewNativeSample18() noexcept { return noPreview; }
std::span<const char* const> SaveLoadNewMarkerNativeSample18() noexcept { return newMarker; }
std::span<const POINT, 6> SaveLoadPageDigitPositionsNativeSample18() noexcept { return pagePositions; }
std::span<const std::uint32_t, 37> SaveLoadDialogOriginalVtableNativeSample18() noexcept { return originalVtable; }
std::span<const SaveLoadResourceTableNativeAudit, 6> SaveLoadResourceTablesNativeSample18Audit() noexcept
{ return resourceTables; }
std::span<const std::uint32_t, 9> SaveLoadDialogCtorFuncInfoNativeSample18Audit() noexcept { return ctorFuncInfo; }
std::span<const SaveLoadCtorUnwindNativeAudit, 41> SaveLoadDialogCtorUnwindNativeSample18Audit() noexcept
{ return ctorUnwind; }
const SaveLoadCtorCallerNativeAudit& SaveLoadDialogCtorCallerNativeSample18Audit() noexcept { return ctorCaller; }
}
