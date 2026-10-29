#include "TitleData.h"

namespace KaGuYa::Start::Runtime::love_x_holic_miwaku_no_otome_to_hakudaku_kankei
{
namespace
{
// Love×Holic ~Miwaku no Otome to Hakudaku Kankei~: original Start.exe table 0x573570, menu 0x47E9F0.
// CP932 bytes from this game; see legacy-title-profiles for VA/raw evidence.
// Original loader receives move=0: retain embedded image coordinates, not table (0,-1).
// Original 0x53C9B8.
constexpr char image0_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C994.
constexpr char image0_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x53C96C.
constexpr char image1_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C948.
constexpr char image1_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x53C924.
constexpr char image2_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C900.
constexpr char image2_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x53C8E0.
constexpr char image2_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x53C8B8.
constexpr char image3_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C894.
constexpr char image3_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x53C874.
constexpr char image3_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x53C850.
constexpr char image4_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C82C.
constexpr char image4_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x53C80C.
constexpr char image4_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x53C7E4.
constexpr char image5_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x53C7BC.
constexpr char image5_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// CString 0x580870, initializer 0x5345D0, literal 0x54030C.
constexpr char background[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
// CString 0x580874, initializer 0x5345F0, literal 0x5389EC.
constexpr char overlay[] = "";
// CString 0x580878, initializer 0x534610, literal 0x53C048.
constexpr char bgm[] = "\x42\x47\x4D\x31\x37\x2E\x62\x67\x6D";
// Menu poll 0x488320, CString 0x580814, initializer 0x534430.
constexpr char click[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x2E\x73\x65\x64";
// Menu poll 0x488320, CString 0x580818, initializer 0x534450.
constexpr char hover[] = "\x91\x49\x91\xF0\x89\xB9\x2E\x73\x65\x64";
const GuiStaticDescriptorNative24h backgroundDescriptor{
    1, {{background, nullptr, nullptr, background, background, nullptr, nullptr}}, nullptr};
// Legacy 0/1/2/3 -> modern 0/(3,4)/5/1. Events 3..8 follow original indices 0..5.
// Fixed hit slot uses the first loaded image. State-dependent alpha equivalence is untested.
const std::array<GuiButtonDescriptorNative2Ch, 6> buttons{{
    {{3, {{nullptr, nullptr, nullptr, image0_1, image0_1, image0_2, nullptr}}, nullptr}, 3, -1},
    {{4, {{nullptr, nullptr, nullptr, image1_1, image1_1, image1_2, nullptr}}, nullptr}, 3, -1},
    {{5, {{nullptr, image2_3, nullptr, image2_1, image2_1, image2_2, nullptr}}, nullptr}, 3, -1},
    {{6, {{nullptr, image3_3, nullptr, image3_1, image3_1, image3_2, nullptr}}, nullptr}, 3, -1},
    {{7, {{nullptr, image4_3, nullptr, image4_1, image4_1, image4_2, nullptr}}, nullptr}, 3, -1},
    {{8, {{nullptr, nullptr, nullptr, image5_1, image5_1, image5_2, nullptr}}, nullptr}, 3, -1},
}};
}
const GuiStaticDescriptorNative24h& SharedBackground() noexcept { return backgroundDescriptor; }
std::span<const GuiButtonDescriptorNative2Ch, 6> SharedButtons() noexcept { return buttons; }
const char* BgmName() noexcept { return bgm; }
const char* ClickCue() noexcept { return click; }
const char* HoverCue() noexcept { return hover; }
// Empty is a verified original default, not a missing-overlay fallback.
const char* OverlayName() noexcept { return overlay; }
// Original CTitle constructor 0x488750 loads this literal.
const char* BlackName() noexcept { return "bg_black.bgd"; }
// Original gate 0x489B00: ((GameSystem+2Ch)&3)!=0 && Save skip byte==0.
TitleNamePromptGate NameGate() noexcept { return TitleNamePromptGate::EditableNameBits; }
// Resource112 EN_CHANGE; byte limits, not character counts.
std::size_t GivenNameMaxBytes() noexcept { return 6; }
std::size_t FamilyNameMaxBytes() noexcept { return 6; }
}
