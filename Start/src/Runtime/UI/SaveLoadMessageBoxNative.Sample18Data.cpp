#include <Runtime/SaveLoadMessageBoxNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 以下是实读PE的CP932字节，不依赖编译器执行字符集或现代名称转换。
constexpr char frame[] = "\x8A\x6D\x94\x46\x91\xE5\x98\x67.vrm";
constexpr char slotPage[] = "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x82\xCC"
    "\x83\x5A\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E\x8D\xED\x8F\x9C.vrm";
constexpr char wholePage[] = "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x82\xCC"
    "\x83\x5A\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E\x91\x53\x8D\xED\x8F\x9C.vrm";
constexpr char quick[] = "\x8A\x6D\x94\x46\x83\x4E\x83\x43\x83\x62\x83\x4E"
    "\x83\x5A\x81\x5B\x83\x75\x91\x53\x8D\xED\x8F\x9C.vrm";
constexpr char automatic[] = "\x8A\x6D\x94\x46\x83\x49\x81\x5B\x83\x67\x83\x5A"
    "\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E\x91\x53\x8D\xED\x8F\x9C.vrm";
constexpr char yes[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2.vrm";
constexpr char yesTarget[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2"
    "\x83\x4A\x81\x5B\x83\x5C\x83\x8B.vrm";
constexpr char yesClick[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2"
    "\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char no[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6.vrm";
constexpr char noTarget[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6"
    "\x83\x4A\x81\x5B\x83\x5C\x83\x8B.vrm";
constexpr char noClick[] = "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2\x82\xA6"
    "\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr GuiStaticDescriptorNative24h statics[]{
    {17, {frame, nullptr, nullptr, frame, frame, nullptr, nullptr}, nullptr},
    {18, {slotPage, nullptr, nullptr, slotPage, slotPage, nullptr, nullptr}, nullptr},
    {19, {wholePage, nullptr, nullptr, wholePage, wholePage, nullptr, nullptr}, nullptr},
    {20, {quick, nullptr, nullptr, quick, quick, nullptr, nullptr}, nullptr},
    {21, {automatic, nullptr, nullptr, automatic, automatic, nullptr, nullptr}, nullptr}
};
constexpr GuiButtonDescriptorNative2Ch buttons[]{
    {{22, {yes, nullptr, nullptr, yesTarget, yesTarget, yesClick, nullptr}, nullptr}, 7, -1},
    {{23, {no, nullptr, nullptr, noTarget, noTarget, noClick, nullptr}, nullptr}, 7, -1}
};
const SaveLoadMessageBoxNativeControlGroup groups[]{ {0, 5, statics}, {1, 2, buttons} };
constexpr GuiDialogNavigationMapping pairs[]{ {22, 23}, {23, 22}, {-1, 0} };
constexpr SaveLoadMessageBoxNativeNavigationGroup navigation[]{ {3, pairs}, {4, pairs} };
#define MESSAGEBOX_DIGIT(n) "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x90\x94\x8E\x9A" n ".vrm"
constexpr const char* digits[]{
    MESSAGEBOX_DIGIT("0"), MESSAGEBOX_DIGIT("1"), MESSAGEBOX_DIGIT("2"), MESSAGEBOX_DIGIT("3"), MESSAGEBOX_DIGIT("4"),
    MESSAGEBOX_DIGIT("5"), MESSAGEBOX_DIGIT("6"), MESSAGEBOX_DIGIT("7"), MESSAGEBOX_DIGIT("8"), MESSAGEBOX_DIGIT("9")
};
#undef MESSAGEBOX_DIGIT
constexpr POINT positions[]{ {455, 284}, {471, 284}, {487, 284}, {707, 284}, {724, 284} };
constexpr std::uint32_t original[]{
    0x509ED0, 0x4FDDA0, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310, 0x536500,
    0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740, 0x510BA0, 0x5106D0,
    0x511610, 0x50EAA0, 0x52DBC0, 0x508E80, 0x50AB70, 0x518C30, 0x5267E0, 0x5231C0,
    0x5230B0, 0x50FC80, 0x522E90, 0x510030, 0x5100E0, 0x50A6A0, 0x50B920, 0x510220,
    0x510170, 0x5102B0, 0x518A80, 0x5086B0, 0x5253D0, 0x51C100
};
constexpr std::array<std::uint32_t, 9> rawStatics[]{
    {17, 0x5B8D44, 0, 0, 0x5B8D44, 0x5B8D44, 0, 0, 0},
    {18, 0x5B8D54, 0, 0, 0x5B8D54, 0x5B8D54, 0, 0, 0},
    {19, 0x5B8D78, 0, 0, 0x5B8D78, 0x5B8D78, 0, 0, 0},
    {20, 0x5B8D9C, 0, 0, 0x5B8D9C, 0x5B8D9C, 0, 0, 0},
    {21, 0x5B8DBC, 0, 0, 0x5B8DBC, 0x5B8DBC, 0, 0, 0}
};
constexpr std::array<std::uint32_t, 11> rawButtons[]{
    {22, 0x5B6C90, 0, 0, 0x5B6CA4, 0x5B6CA4, 0x5B6CC0, 0, 0, 7, 0xFFFFFFFF},
    {23, 0x5B6CDC, 0, 0, 0x5B6CF0, 0x5B6CF0, 0x5B6D0C, 0, 0, 7, 0xFFFFFFFF}
};
}
std::span<const SaveLoadMessageBoxNativeControlGroup, 2> SaveLoadMessageBoxNativeGroupsSample18() noexcept { return groups; }
std::span<const SaveLoadMessageBoxNativeNavigationGroup, 2> SaveLoadMessageBoxNativeNavigationSample18() noexcept { return navigation; }
std::span<const GuiStaticDescriptorNative24h, 5> SaveLoadMessageBoxNativeStaticsSample18() noexcept { return statics; }
std::span<const GuiButtonDescriptorNative2Ch, 2> SaveLoadMessageBoxNativeButtonsSample18() noexcept { return buttons; }
std::span<const char* const, 10> SaveLoadMessageBoxNativeDigitsSample18() noexcept { return digits; }
std::span<const POINT, 5> SaveLoadMessageBoxNativeDigitPositionsSample18() noexcept { return positions; }
std::span<const std::uint32_t, 38> SaveLoadMessageBoxNativeOriginalVtableSample18() noexcept { return original; }
std::span<const std::array<std::uint32_t, 9>, 5> SaveLoadMessageBoxNativeRawStaticsSample18() noexcept { return rawStatics; }
std::span<const std::array<std::uint32_t, 11>, 2> SaveLoadMessageBoxNativeRawButtonsSample18() noexcept { return rawButtons; }
}
