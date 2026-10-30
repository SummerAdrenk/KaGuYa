#include "SettingsOtherData.h"

namespace KaGuYa::Start::Runtime::MamagotoMamaToNaishoNoEcchiShimashoSettingsOtherData
{
namespace
{
// Generated from settings-other-audit typed data and selected-evidence.json.
constexpr char s0[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s1[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char s2[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr POINT p0{406, 226};
constexpr char s3[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char s4[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char s5[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr POINT p1{497, 226};
constexpr GuiButtonDescriptorNative2Ch b0r0buttons[] = {
    {{192, {nullptr, s0, nullptr, s1, s1, s2, nullptr}, &p0}, 7, -1},
    {{193, {nullptr, s3, nullptr, s4, s4, s5, nullptr}, &p1}, 7, -1}
};
constexpr char s6[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x41\x50\x49\x95\x60\x89\xE6\x2E\x76\x72\x6D";
constexpr char s7[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x44\x69\x72\x65\x63\x74\x58\x95\x60\x89\xE6\x2E\x76\x72\x6D";
constexpr GuiStaticDescriptorNative24h b0static[] = {
    {189, {s6, nullptr, nullptr, s6, s6, nullptr, nullptr}, nullptr},
    {190, {s7, nullptr, nullptr, s7, s7, nullptr, nullptr}, nullptr}
};
constexpr SettingOtherRadio b0radios[] = {
    {191, b0r0buttons}
};
constexpr SettingOtherChild b0order[] = {
    {SettingOtherChildKind::Radio, 0, true},
    {SettingOtherChildKind::Static, 0, true},
    {SettingOtherChildKind::Static, 1, true}
};
constexpr SettingOtherBoolBinding b0bools[] = {
    {SettingOtherBool::SkipStartup, 191, true, 193, 192, SettingOtherCommit::OnBlockEnd}
};
constexpr SettingOtherBackendIndicator b0backend[] = {
    {189, 0},
    {190, 1}
};
constexpr POINT p2{406, 395};
constexpr POINT p3{497, 395};
constexpr GuiButtonDescriptorNative2Ch b1r0buttons[] = {
    {{195, {nullptr, s0, nullptr, s1, s1, s2, nullptr}, &p2}, 7, -1},
    {{196, {nullptr, s3, nullptr, s4, s4, s5, nullptr}, &p3}, 7, -1}
};
constexpr char s8[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x93\x96\xBC\x91\x4F\x82\xF0\x95\xCF\x8D\x58\x82\xB7\x82\xE9\x2E\x76\x72\x6D";
constexpr char s9[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x96\xBC\x91\x4F\x82\xF0\x95\xCF\x8D\x58\x82\xB7\x82\xE9\x2E\x76\x72\x6D";
constexpr char s10[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x96\xBC\x91\x4F\x82\xF0\x95\xCF\x8D\x58\x82\xB7\x82\xE9\x2E\x76\x72\x6D";
constexpr GuiButtonDescriptorNative2Ch b1button[] = {
    {{197, {s8, nullptr, nullptr, s9, s9, s10, nullptr}, nullptr}, 3, -1}
};
constexpr SettingOtherRadio b1radios[] = {
    {194, b1r0buttons}
};
constexpr SettingOtherChild b1order[] = {
    {SettingOtherChildKind::Radio, 0, true},
    {SettingOtherChildKind::Button, 0, true}
};
constexpr SettingOtherBoolBinding b1bools[] = {
    {SettingOtherBool::SkipNewGameName, 194, true, 196, 195, SettingOtherCommit::OnBlockEnd}
};
constexpr POINT p4{406, 565};
constexpr POINT p5{497, 565};
constexpr GuiButtonDescriptorNative2Ch b2r0buttons[] = {
    {{200, {nullptr, s0, nullptr, s1, s1, s2, nullptr}, &p4}, 7, -1},
    {{201, {nullptr, s3, nullptr, s4, s4, s5, nullptr}, &p5}, 7, -1}
};
constexpr char s11[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x91\xAC\x93\x78\x2E\x76\x72\x6D";
constexpr GuiStaticDescriptorNative24h b2static[] = {
    {198, {nullptr, s11, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char s12[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D";
constexpr char s13[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr GuiSliderDescriptorNative6Ch b2slider[] = {
    {202, {s12, s13, nullptr, s12, s12, s12, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {309, 619, 459, 638}, {0, 0, -12, 0}}
};
constexpr SettingOtherRadio b2radios[] = {
    {199, b2r0buttons}
};
constexpr SettingOtherChild b2order[] = {
    {SettingOtherChildKind::Radio, 0, true},
    {SettingOtherChildKind::Static, 0, true},
    {SettingOtherChildKind::Slider, 0, true}
};
constexpr SettingOtherBoolBinding b2bools[] = {
    {SettingOtherBool::EffectsEnabled, 199, false, 200, 201, SettingOtherCommit::OnBlockEnd}
};
constexpr int b2gateControls[] = {
    202,
    198
};
constexpr char s14[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x74\x91\x49\x91\xF0\x8E\x88\x82\xCC\x95\x5C\x8E\xA6\x2E\x76\x72\x6D";
constexpr char s15[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x91\x49\x91\xF0\x8E\x88\x82\xCC\x95\x5C\x8E\xA6\x2E\x76\x72\x6D";
constexpr char s16[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x91\x49\x91\xF0\x8E\x88\x82\xCC\x95\x5C\x8E\xA6\x2E\x76\x72\x6D";
constexpr POINT p6{852, 227};
constexpr char s17[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x74\xE4\x53\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s18[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\xE4\x53\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s19[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\xE4\x53\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s20[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x74\xE4\x53\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s21[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\xE4\x53\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s22[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\xE4\x53\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr GuiButtonDescriptorNative2Ch b4r0buttons[] = {
    {{204, {nullptr, s14, nullptr, s15, s15, s16, nullptr}, &p6}, 7, -1},
    {{205, {nullptr, s17, nullptr, s18, s18, s19, nullptr}, nullptr}, 7, -1},
    {{206, {nullptr, s20, nullptr, s21, s21, s22, nullptr}, nullptr}, 7, -1}
};
constexpr POINT p7{852, 276};
constexpr char s23[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x74\x8C\xFB\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s24[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8C\xFB\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s25[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8C\xFB\x93\xE0\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s26[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x49\x83\x74\x8C\xFB\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s27[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8C\xFB\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr char s28[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8C\xFB\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7\x2E\x76\x72\x6D";
constexpr GuiButtonDescriptorNative2Ch b4r1buttons[] = {
    {{208, {nullptr, s14, nullptr, s15, s15, s16, nullptr}, &p7}, 7, -1},
    {{209, {nullptr, s23, nullptr, s24, s24, s25, nullptr}, nullptr}, 7, -1},
    {{210, {nullptr, s26, nullptr, s27, s27, s28, nullptr}, nullptr}, 7, -1}
};
constexpr SettingOtherRadio b4radios[] = {
    {203, b4r0buttons},
    {207, b4r1buttons}
};
constexpr SettingOtherChild b4order[] = {
    {SettingOtherChildKind::Radio, 0, true},
    {SettingOtherChildKind::Radio, 1, true}
};
constexpr SettingOtherChoice b4InsertionChoicevalues[] = {
    {204, 65535},
    {205, 0},
    {206, 1}
};
constexpr SettingOtherChoice b4OralChoicevalues[] = {
    {208, 65535},
    {209, 0},
    {210, 1}
};
constexpr SettingOtherWordBinding b4words[] = {
    {SettingOtherWord::InsertionChoice, 203, b4InsertionChoicevalues, 204, true},
    {SettingOtherWord::OralChoice, 207, b4OralChoicevalues, 208, true}
};
constexpr SettingOtherBlockProfile blocks[] = {
    {SettingOtherBlock::Startup, 19, b0order, b0static, {}, b0radios, {}, {}, b0bools, {}, {}, {}, b0backend, {0, 0}},
    {SettingOtherBlock::Name, 20, b1order, {}, {}, b1radios, b1button, {}, b1bools, {}, {}, {}, {}, {0, 0}},
    {SettingOtherBlock::Effects, 21, b2order, b2static, {}, b2radios, {}, b2slider, b2bools, {}, {}, b2gateControls, {}, {0, 3}},
    {SettingOtherBlock::VersionInfo, 22, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {0, 0}},
    {SettingOtherBlock::SceneChoices, 23, b4order, {}, {}, b4radios, {}, {}, {}, b4words, {}, {}, {}, {0, 0}}
};
constexpr char s29[] = "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x33\x2E\x76\x72\x6D";
constexpr GuiStaticDescriptorNative24h background = {28, {s29, nullptr, nullptr, s29, s29, nullptr, nullptr}, nullptr};
constexpr char s30[] = "\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E";
const SettingOtherResourceProfile profile = {blocks, &background, {SettingOtherNameEntry::LocalResource112, 197, -1, SettingOtherNameAccept::ClearSkipThenDeferredRadioCommit, SettingOtherCapability::AlwaysAvailable, {6, 6}, {s30, -24, 700, 2, 0, 0, false, 0, 0, 0xFFFFFFFFu, {0, 0}, true}, {427, 460}, -1, {}, {}, SettingOtherNameRefresh::RebuildName}, {true, {s30, -24, 700, 2, 0, 0, false, 0, 0, 0xFFFFFFFFu, {0, 0}, true}, SettingOtherPlacement::RightAligned, {{{1176, 411}, {1176, 460}}}}};
}
const SettingOtherResourceProfile& Profile() noexcept { return profile; }
}
