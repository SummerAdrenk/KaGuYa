#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::LoveXHolicMiwakuNoOtomeToHakudakuKankeiSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/08.json；原父构造 VA：0x50CAE0。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x577C20, 0x577C44, 0x577C68
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {136, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {137, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {138, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x577CB8, 0x577D24, 0x577D90
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {116, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 139, 282, 158}, {0, 0, -12, 0}},
    {117, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 180, 282, 199}, {0, 0, -12, 0}},
    {118, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 220, 282, 239}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point000{285, 119};
constexpr POINT point001{285, 160};
constexpr POINT point002{285, 201};
// 原描述符 VA：0x577E38, 0x577E80, 0x577EC8
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {79, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {80, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {81, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
};
constexpr char text009[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x32\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text010[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text011[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x49\x83\x93\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text012[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
// 原描述符 VA：0x577F10
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group3[]{
    {95, {nullptr, nullptr, nullptr, text009, text009, text010, nullptr}, {text011, nullptr, nullptr, text012, text012, text010, nullptr}, nullptr, 7, -1}
};
constexpr SettingPage03ControlGroup block0Groups[]{
    {SettingPage03ControlKind::Static, 3, block0Group0},
    {SettingPage03ControlKind::Slider, 3, block0Group1},
    {SettingPage03ControlKind::CheckBox, 3, block0Group2},
    {SettingPage03ControlKind::CheckBox, 1, block0Group3}
};
constexpr char text013[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93\x2E\x76\x72\x6D"};
constexpr char text014[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xE8\x83\x52\x83\x4C\x91\xAB\x83\x52\x83\x4C\x2E\x76\x72\x6D"};
constexpr char text015[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\x77\x83\x7D\x83\x93\x83\x4E\x83\x93\x83\x6A\x2E\x76\x72\x6D"};
constexpr char text016[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x92\xAA\x90\x81\x82\xAB\x2E\x76\x72\x6D"};
constexpr char text017[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xCB\x90\xB8\x89\xB9\x2E\x76\x72\x6D"};
constexpr char text018[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x577F58, 0x577F7C, 0x577FA0, 0x577FC4, 0x577FE8, 0x57800C
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {151, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {139, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {140, {nullptr, text015, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {141, {nullptr, text016, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {142, {nullptr, text017, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {143, {nullptr, text018, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x578030, 0x57809C, 0x578108, 0x578174, 0x5781E0, 0x57824C
constexpr GuiSliderDescriptorNative6Ch block1Group1[]{
    {130, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 268, 282, 287}, {0, 0, -12, 0}},
    {131, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 309, 282, 328}, {0, 0, -12, 0}},
    {119, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 350, 282, 369}, {0, 0, -12, 0}},
    {120, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 391, 282, 410}, {0, 0, -12, 0}},
    {121, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 432, 282, 451}, {0, 0, -12, 0}},
    {122, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 473, 282, 492}, {0, 0, -12, 0}}
};
constexpr POINT point003{285, 248};
constexpr POINT point004{285, 289};
constexpr POINT point005{285, 330};
constexpr POINT point006{285, 371};
constexpr POINT point007{285, 412};
constexpr POINT point008{285, 453};
// 原描述符 VA：0x5782E8, 0x578330, 0x578378, 0x5783C0, 0x578408, 0x578450
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group2[]{
    {93, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point003, 7, -1},
    {94, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point004, 7, -1},
    {82, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point005, 7, -1},
    {83, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point006, 7, -1},
    {84, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point007, 7, -1},
    {85, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point008, 7, -1}
};
constexpr char text019[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text021[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text022[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point009{219, 250};
constexpr POINT point010{219, 291};
constexpr POINT point011{219, 332};
constexpr POINT point012{219, 373};
constexpr POINT point013{219, 414};
constexpr POINT point014{219, 455};
// 原描述符 VA：0x5784C8, 0x5784F4, 0x578520, 0x57854C, 0x578578, 0x5785A4
constexpr GuiButtonDescriptorNative2Ch block1Group3[]{
    {{110, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point009}, 7, -1},
    {{111, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point010}, 7, -1},
    {{99, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point011}, 7, -1},
    {{100, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point012}, 7, -1},
    {{101, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point013}, 7, -1},
    {{102, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point014}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 6, block1Group0},
    {SettingPage03ControlKind::Slider, 6, block1Group1},
    {SettingPage03ControlKind::CheckBox, 6, block1Group2},
    {SettingPage03ControlKind::Button, 6, block1Group3}
};
// 原描述符 VA：0x578760, 0x5787CC, 0x578838, 0x5788A4, 0x578910, 0x57897C, 0x5789E8, 0x578A54, 0x578AC0
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {124, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {522, 156, 654, 175}, {0, 0, -12, 0}},
    {125, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {522, 239, 654, 258}, {0, 0, -12, 0}},
    {126, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {522, 322, 654, 341}, {0, 0, -12, 0}},
    {127, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {522, 405, 654, 424}, {0, 0, -12, 0}},
    {128, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {522, 488, 654, 507}, {0, 0, -12, 0}},
    {129, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {832, 156, 964, 175}, {0, 0, -12, 0}},
    {132, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {832, 239, 964, 258}, {0, 0, -12, 0}},
    {133, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {832, 322, 964, 341}, {0, 0, -12, 0}},
    {134, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {832, 405, 964, 424}, {0, 0, -12, 0}}
};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x5F\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x5F\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x5F\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x5F\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point015{626, 122};
constexpr POINT point016{626, 205};
constexpr POINT point017{626, 288};
constexpr POINT point018{626, 371};
constexpr POINT point019{626, 454};
constexpr POINT point020{936, 122};
constexpr POINT point021{936, 205};
constexpr POINT point022{936, 288};
constexpr POINT point023{936, 371};
// 原描述符 VA：0x578B90, 0x578BD8, 0x578C20, 0x578C68, 0x578CB0, 0x578CF8, 0x578D40, 0x578D88, 0x578DD0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {87, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point015, 7, -1},
    {88, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point016, 7, -1},
    {89, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point017, 7, -1},
    {90, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point018, 7, -1},
    {91, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point019, 7, -1},
    {92, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point020, 7, -1},
    {96, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point021, 7, -1},
    {97, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point022, 7, -1},
    {98, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point023, 7, -1}
};
constexpr char text027[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x5F\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x5F\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text029[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x5F\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point024{588, 124};
constexpr POINT point025{588, 207};
constexpr POINT point026{588, 290};
constexpr POINT point027{588, 373};
constexpr POINT point028{588, 456};
constexpr POINT point029{898, 124};
constexpr POINT point030{898, 207};
constexpr POINT point031{898, 290};
constexpr POINT point032{898, 373};
// 原描述符 VA：0x578E60, 0x578E8C, 0x578EB8, 0x578EE4, 0x578F10, 0x578F3C, 0x578F68, 0x578F94, 0x578FC0
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{104, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point024}, 7, -1},
    {{105, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point025}, 7, -1},
    {{106, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point026}, 7, -1},
    {{107, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point027}, 7, -1},
    {{108, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point028}, 7, -1},
    {{109, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point029}, 7, -1},
    {{112, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point030}, 7, -1},
    {{113, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point031}, 7, -1},
    {{114, {nullptr, text027, nullptr, text028, text028, text029, nullptr}, &point032}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 9, block2Group0},
    {SettingPage03ControlKind::CheckBox, 9, block2Group1},
    {SettingPage03ControlKind::Button, 9, block2Group2}
};
constexpr char text030[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8D\x67\x89\xB9\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5785D0
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {144, {nullptr, text030, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x97\xE9\x8E\xAD\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5785F4
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {145, {nullptr, text031, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x5A\x83\x43\x83\x89\x2E\x76\x72\x6D"};
// 原描述符 VA：0x578618
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {146, {nullptr, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x95\x91\x93\x80\x2E\x76\x72\x6D"};
// 原描述符 VA：0x57863C
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {147, {nullptr, text033, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x41\x83\x94\x83\x8A\x83\x8B\x2E\x76\x72\x6D"};
// 原描述符 VA：0x578660
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {148, {nullptr, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text035[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8B\x6A\x8D\x5B\x2E\x76\x72\x6D"};
// 原描述符 VA：0x578684
constexpr GuiStaticDescriptorNative24h label5Shown[]{
    {149, {nullptr, text035, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text036[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4F\x83\x89\x83\x40\x83\x6C\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5786A8
constexpr GuiStaticDescriptorNative24h label6Shown[]{
    {150, {nullptr, text036, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text037[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4F\x83\x8A\x83\x82\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5786CC
constexpr GuiStaticDescriptorNative24h label7Shown[]{
    {152, {nullptr, text037, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text038[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5786F0
constexpr GuiStaticDescriptorNative24h label8Shown[]{
    {153, {nullptr, text038, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr SettingSoundLabel labels[]{
    {{&label0Shown[0], nullptr}, 0},
    {{&label1Shown[0], nullptr}, 1},
    {{&label2Shown[0], nullptr}, 2},
    {{&label3Shown[0], nullptr}, 3},
    {{&label4Shown[0], nullptr}, 4},
    {{&label5Shown[0], nullptr}, 5},
    {{&label6Shown[0], nullptr}, 6},
    {{&label7Shown[0], nullptr}, 7},
    {{&label8Shown[0], nullptr}, 8}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 116, 79},
    {SettingSoundMaster::Effects, 118, 81},
    {SettingSoundMaster::Voice, 117, 80}
};
constexpr char text039[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text040[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text041[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text042[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text043[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text044[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text039, 130, 93},
    {text040, 131, 94},
    {text041, 119, 82},
    {text042, 120, 83},
    {text043, 121, 84},
    {text044, 122, 85}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {110, 93, 130},
    {111, 94, 131},
    {99, 82, 119},
    {100, 83, 120},
    {101, 84, 121},
    {102, 85, 122}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr char text045[]{"\x8D\x67\x89\xB9"};
constexpr char text046[]{"\x97\xE9\x8E\xAD"};
constexpr char text047[]{"\x83\x5A\x83\x43\x83\x89"};
constexpr char text048[]{"\x95\x91\x93\x80"};
constexpr char text049[]{"\x83\x41\x83\x94\x83\x8A\x83\x8B"};
constexpr char text050[]{"\x8B\x6A\x8D\x5B"};
constexpr char text051[]{"\x83\x4F\x83\x89\x83\x40\x83\x6C"};
constexpr char text052[]{"\x83\x4F\x83\x8A\x83\x82"};
constexpr SettingPage03NamedRow characters[]{
    {text045, 124, 87},
    {text046, 125, 88},
    {text047, 126, 89},
    {text048, 127, 90},
    {text049, 128, 91},
    {text050, 129, 92},
    {text051, 132, 96},
    {text052, 133, 97},
    {text044, 134, 98}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {104, 87, 124},
    {105, 88, 125},
    {106, 89, 126},
    {107, 90, 127},
    {108, 91, 128},
    {109, 92, 129},
    {112, 96, 132},
    {113, 97, 133},
    {114, 98, 134}
};
constexpr int charactersParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8
};
constexpr std::int32_t mute0Dependents[]{
    115,
    135
};
constexpr std::int32_t mute1Dependents[]{
    116,
    136
};
constexpr std::int32_t mute2Dependents[]{
    117,
    137
};
constexpr std::int32_t mute3Dependents[]{
    118,
    138
};
constexpr std::int32_t mute4Dependents[]{
    130,
    151,
    110
};
constexpr std::int32_t mute5Dependents[]{
    131,
    139,
    111
};
constexpr std::int32_t mute6Dependents[]{
    119,
    140,
    99
};
constexpr std::int32_t mute7Dependents[]{
    120,
    141,
    100
};
constexpr std::int32_t mute8Dependents[]{
    121,
    142,
    101
};
constexpr std::int32_t mute9Dependents[]{
    122,
    143,
    102
};
constexpr std::int32_t mute10Dependents[]{
    124,
    144,
    104
};
constexpr std::int32_t mute11Dependents[]{
    125,
    145,
    105
};
constexpr std::int32_t mute12Dependents[]{
    126,
    146,
    106
};
constexpr std::int32_t mute13Dependents[]{
    127,
    147,
    107
};
constexpr std::int32_t mute14Dependents[]{
    128,
    148,
    108
};
constexpr std::int32_t mute15Dependents[]{
    129,
    149,
    109
};
constexpr std::int32_t mute16Dependents[]{
    132,
    150,
    112
};
constexpr std::int32_t mute17Dependents[]{
    133,
    152,
    113
};
constexpr std::int32_t mute18Dependents[]{
    134,
    153,
    114
};
constexpr SettingPage03MuteRow muteRows[]{
    {78, mute0Dependents},
    {79, mute1Dependents},
    {80, mute2Dependents},
    {81, mute3Dependents},
    {93, mute4Dependents},
    {94, mute5Dependents},
    {82, mute6Dependents},
    {83, mute7Dependents},
    {84, mute8Dependents},
    {85, mute9Dependents},
    {87, mute10Dependents},
    {88, mute11Dependents},
    {89, mute12Dependents},
    {90, mute13Dependents},
    {91, mute14Dependents},
    {92, mute15Dependents},
    {96, mute16Dependents},
    {97, mute17Dependents},
    {98, mute18Dependents}
};
constexpr char text053[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text054[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5775EC
constexpr GuiStaticDescriptorNative24h footer[]{
    {25, {text054, nullptr, nullptr, text054, text054, nullptr, nullptr}, nullptr}
};
constexpr unsigned blockOrder[]{
    0,
    1,
    2
};
constexpr SettingPage03ResourceProfile resources{
    blockOrder,
    {block0Groups, block1Groups, block2Groups, {}, {}},
    masters,
    effects,
    characters,
    labels,
    effectsPreviews,
    charactersPreviews,
    effectsParamsIndices,
    charactersParamsIndices,
    muteRows,
    std::string_view{text053, 10},
    false,
    {SettingSoundLoopKind::CheckBox, 95, -1, -1, -1, {}, nullptr},
    {false, 9, {-1, -1}, {-1, -1}, {}},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
