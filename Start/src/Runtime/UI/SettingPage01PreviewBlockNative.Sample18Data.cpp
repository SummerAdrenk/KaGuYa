#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE的CP932字节和共享指针别名；null不替换为空串。
constexpr char s5BCB50[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BCB68[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s5BD324[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x49\x83\x74\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD344[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD36C[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x92\xCA\x8F\xED\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD390[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x49\x83\x74\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD3B0[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD3D4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x4E\x83\x8A\x83\x62\x83\x4E\x48\x83\x56\x81\x5B\x83\x93\x2E\x76\x72\x6D";
constexpr char s5BD3F8[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x54\x83\x93\x83\x76\x83\x8B\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x2E\x76\x72\x6D";
constexpr char s5BD428[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x31\x83\x54\x83\x93\x83\x76\x83\x8B\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x32\x2E\x76\x72\x6D";
constexpr std::array<const char*, 7> thumb{s5BCB50,s5BCB68,nullptr,s5BCB50,s5BCB50,s5BCB50,nullptr};
constexpr std::array<const char*, 7> track{};
const std::array<GuiSliderDescriptorNative6Ch, 3> sliders{{
    {68,thumb,track,nullptr,0,0,1,{994,204,1144,223},{0,0,-12,0}},
    {69,thumb,track,nullptr,0,0,0,{994,238,1144,257},{0,0,-12,0}},
    {70,thumb,track,nullptr,0,0,0,{750,321,1170,340},{0,0,-12,0}}
}};
const std::array<GuiButtonDescriptorNative2Ch, 2> buttons{{
    {{48,{nullptr,s5BD324,nullptr,s5BD344,s5BD344,s5BD36C,nullptr},nullptr},7,-1},
    {{49,{nullptr,s5BD390,nullptr,s5BD3B0,s5BD3B0,s5BD3D4,nullptr},nullptr},7,-1}
}};
const std::array<const char*, 2> names{s5BD3F8,s5BD428};
constexpr std::array<std::uint32_t, 38> table{
    0x509ED0,0x4FCFD0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,
    0x536500,0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,
    0x510B90,0x5106B0,0x511610,0x50BF50,0x52E190,0x5090F0,0x50AB50,
    0x518C30,0x525FB0,0x523B80,0x5239E0,0x50FC80,0x522E90,0x510030,
    0x5100E0,0x50A6A0,0x50B920,0x510220,0x510170,0x5102B0,0x518A80,
    0x50A0B0,0x512BC0,0x5160F0
};
constexpr std::array<std::uint32_t, 54> unwind{
    0xFFFFFFFF,0x5A1B80,0,0x5A1B8B,1,0x5A1B9C,2,0x5A1BAD,3,0x5A1BBE,
    4,0x5A1BC9,5,0x5A1BD1,6,0x5A1BD9,6,0x541E55,6,0x5A1BE4,
    6,0x541E55,6,0x5A1BEF,6,0x541E55,5,0x541E55,4,0x541E55,
    4,0x5A1BF7,4,0x5A1C06,16,0x5A1C11,16,0x541E55,4,0x541E55,
    4,0x5A1C1C,4,0x541E55,4,0x5A1C24,4,0x541E55,4,0x5A1C2C,
    4,0x541E55,3,0x541E55
};
}
std::span<const GuiSliderDescriptorNative6Ch, 3> SettingPage01PreviewSlidersSample18() noexcept { return sliders; }
std::span<const GuiButtonDescriptorNative2Ch, 2> SettingPage01PreviewButtonsSample18() noexcept { return buttons; }
std::span<const char* const, 2> SettingPage01PreviewNamesSample18() noexcept { return names; }
std::span<const std::uint32_t, 38> SettingPage01PreviewOriginalTableSample18() noexcept { return table; }
std::span<const std::uint32_t, 54> SettingPage01PreviewOriginalUnwindSample18() noexcept { return unwind; }
}
