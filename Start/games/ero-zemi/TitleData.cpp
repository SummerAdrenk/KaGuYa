#include "TitleData.h"

namespace KaGuYa::Start::Runtime::ero_zemi_ecchi_ni_yaru_ki_ni_abc
{
namespace
{
// Ero Zemi ~Ecchi ni Yaru-ki ni ABC~: original Start.exe table 0x5581A0, menu 0x47C720.
// CP932 bytes from this game; see legacy-title-profiles for VA/raw evidence.
// Original loader receives move=0: retain embedded image coordinates, not table (0,-1).
// Original 0x52624C.
constexpr char image0_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x526228.
constexpr char image0_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xCD\x82\xB6\x82\xDF\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x526200.
constexpr char image1_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x5261DC.
constexpr char image1_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x82\xC2\x82\xC3\x82\xAB\x82\xA9\x82\xE7\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x5261B8.
constexpr char image2_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x526194.
constexpr char image2_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x526174.
constexpr char image2_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x43\x47\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x52614C.
constexpr char image3_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x526128.
constexpr char image3_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x526108.
constexpr char image3_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xF1\x91\x7A\x83\x82\x81\x5B\x83\x68\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x5260E4.
constexpr char image4_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x5260C0.
constexpr char image4_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// Original 0x5260A0.
constexpr char image4_3[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x49\x83\x74\x2E\x76\x72\x6D";
// Original 0x526078.
constexpr char image5_1[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
// Original 0x526050.
constexpr char image5_2[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x83\x51\x81\x5B\x83\x80\x82\xF0\x8F\x49\x97\xB9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
// CString 0x562C38, initializer 0x51DD10, literal 0x529968.
constexpr char background[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x89\xE6\x96\xCA\x2E\x76\x72\x6D";
// CString 0x562C3C, initializer 0x51DD30, literal 0x522918.
constexpr char overlay[] = "";
// CString 0x562C40, initializer 0x51DD50, literal 0x5258DC.
constexpr char bgm[] = "\x42\x47\x4D\x31\x37\x2E\x62\x67\x6D";
// Menu poll 0x485AE0, CString 0x562BE0, initializer 0x51DB70.
constexpr char click[] = "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x2E\x73\x65\x64";
// Menu poll 0x485AE0, CString 0x562BE4, initializer 0x51DB90.
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
// Original CTitle constructor 0x485F10 loads this literal.
const char* BlackName() noexcept { return "bg_black.bgd"; }
// Original gate 0x487260: ((GameSystem+2Ch)&3)!=0 && Save skip byte==0.
TitleNamePromptGate NameGate() noexcept { return TitleNamePromptGate::EditableNameBits; }
// Resource112 EN_CHANGE; byte limits, not character counts.
std::size_t GivenNameMaxBytes() noexcept { return 6; }
std::size_t FamilyNameMaxBytes() noexcept { return 6; }
}
