#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::ShigokareEcchiNaJoshiDaiseiToDokiX2LoveLessonSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/07.json；原父构造 VA：0x4FFB40。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x564BB0, 0x564BD4, 0x564BF8
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {127, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {128, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {129, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x564C48, 0x564CB4, 0x564D20
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {110, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 139, 282, 158}, {0, 0, -12, 0}},
    {111, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 180, 282, 199}, {0, 0, -12, 0}},
    {112, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 220, 282, 239}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point000{285, 119};
constexpr POINT point001{285, 160};
constexpr POINT point002{285, 201};
// 原描述符 VA：0x564DC8, 0x564E10, 0x564E58
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {79, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {80, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {81, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
};
constexpr char text009[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x32\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text010[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text011[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x49\x83\x93\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text012[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
// 原描述符 VA：0x564EA0
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
// 原描述符 VA：0x564EE8, 0x564F0C, 0x564F30, 0x564F54, 0x564F78, 0x564F9C
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {142, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {130, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {131, {nullptr, text015, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {132, {nullptr, text016, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {133, {nullptr, text017, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {134, {nullptr, text018, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x564FC0, 0x56502C, 0x565098, 0x565104, 0x565170, 0x5651DC
constexpr GuiSliderDescriptorNative6Ch block1Group1[]{
    {124, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 268, 282, 287}, {0, 0, -12, 0}},
    {125, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 309, 282, 328}, {0, 0, -12, 0}},
    {113, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 350, 282, 369}, {0, 0, -12, 0}},
    {114, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 391, 282, 410}, {0, 0, -12, 0}},
    {115, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 432, 282, 451}, {0, 0, -12, 0}},
    {116, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 473, 282, 492}, {0, 0, -12, 0}}
};
constexpr POINT point003{285, 248};
constexpr POINT point004{285, 289};
constexpr POINT point005{285, 330};
constexpr POINT point006{285, 371};
constexpr POINT point007{285, 412};
constexpr POINT point008{285, 453};
// 原描述符 VA：0x565278, 0x5652C0, 0x565308, 0x565350, 0x565398, 0x5653E0
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
// 原描述符 VA：0x565458, 0x565484, 0x5654B0, 0x5654DC, 0x565508, 0x565534
constexpr GuiButtonDescriptorNative2Ch block1Group3[]{
    {{107, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point009}, 7, -1},
    {{108, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point010}, 7, -1},
    {{96, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point011}, 7, -1},
    {{97, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point012}, 7, -1},
    {{98, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point013}, 7, -1},
    {{99, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point014}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 6, block1Group0},
    {SettingPage03ControlKind::Slider, 6, block1Group1},
    {SettingPage03ControlKind::CheckBox, 6, block1Group2},
    {SettingPage03ControlKind::Button, 6, block1Group3}
};
// 原描述符 VA：0x565668, 0x5656D4, 0x565740, 0x5657AC, 0x565818, 0x565884
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {118, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 170, 656, 189}, {0, 0, -12, 0}},
    {119, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 275, 656, 294}, {0, 0, -12, 0}},
    {120, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 379, 656, 398}, {0, 0, -12, 0}},
    {121, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 480, 656, 499}, {0, 0, -12, 0}},
    {122, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 170, 965, 189}, {0, 0, -12, 0}},
    {123, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 275, 965, 294}, {0, 0, -12, 0}}
};
constexpr char text023[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point015{629, 121};
constexpr POINT point016{629, 226};
constexpr POINT point017{629, 332};
constexpr POINT point018{629, 434};
constexpr POINT point019{935, 121};
constexpr POINT point020{935, 226};
// 原描述符 VA：0x565920, 0x565968, 0x5659B0, 0x5659F8, 0x565A40, 0x565A88
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {87, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point015, 7, -1},
    {88, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point016, 7, -1},
    {89, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point017, 7, -1},
    {90, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point018, 7, -1},
    {91, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point019, 7, -1},
    {92, {text023, text023, nullptr, text024, text024, text025, nullptr}, {text026, text023, nullptr, text024, text024, text025, nullptr}, &point020, 7, -1}
};
constexpr char text027[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text029[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point021{596, 192};
constexpr POINT point022{596, 297};
constexpr POINT point023{596, 402};
constexpr POINT point024{596, 505};
constexpr POINT point025{905, 192};
constexpr POINT point026{905, 297};
// 原描述符 VA：0x565B00, 0x565B2C, 0x565B58, 0x565B84, 0x565BB0, 0x565BDC
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{101, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point021}, 7, -1},
    {{102, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point022}, 7, -1},
    {{103, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point023}, 7, -1},
    {{104, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point024}, 7, -1},
    {{105, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point025}, 7, -1},
    {{106, {text027, text028, nullptr, text029, text029, text030, nullptr}, &point026}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 6, block2Group0},
    {SettingPage03ControlKind::CheckBox, 6, block2Group1},
    {SettingPage03ControlKind::Button, 6, block2Group2}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xC0\x8F\x83\x2E\x76\x72\x6D"};
// 原描述符 VA：0x565560
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {135, {nullptr, text031, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xAC\x97\x9C\x2E\x76\x72\x6D"};
// 原描述符 VA：0x565584
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {136, {nullptr, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x95\x53\x8D\x81\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5655A8
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {137, {nullptr, text033, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x88\xCB\x97\xA2\x8E\x71\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5655CC
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {138, {nullptr, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text035[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8D\x67\x89\xBE\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5655F0
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {139, {nullptr, text035, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text036[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x83\x4C\x83\x83\x83\x89\x2E\x76\x72\x6D"};
// 原描述符 VA：0x565614
constexpr GuiStaticDescriptorNative24h label5Shown[]{
    {140, {nullptr, text036, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr SettingSoundLabel labels[]{
    {{&label0Shown[0], nullptr}, 0},
    {{&label1Shown[0], nullptr}, 1},
    {{&label2Shown[0], nullptr}, 2},
    {{&label3Shown[0], nullptr}, 3},
    {{&label4Shown[0], nullptr}, 4},
    {{&label5Shown[0], nullptr}, 5}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 110, 79},
    {SettingSoundMaster::Effects, 112, 81},
    {SettingSoundMaster::Voice, 111, 80}
};
constexpr char text037[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text038[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text039[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text040[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text041[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text042[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text037, 124, 93},
    {text038, 125, 94},
    {text039, 113, 82},
    {text040, 114, 83},
    {text041, 115, 84},
    {text042, 116, 85}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {107, 93, 124},
    {108, 94, 125},
    {96, 82, 113},
    {97, 83, 114},
    {98, 84, 115},
    {99, 85, 116}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr char text043[]{"\x89\xC0\x8F\x83"};
constexpr char text044[]{"\x8E\xAC\x97\x9C"};
constexpr char text045[]{"\x95\x53\x8D\x81"};
constexpr char text046[]{"\x88\xCB\x97\xA2\x8E\x71"};
constexpr char text047[]{"\x8D\x67\x89\xBE"};
constexpr SettingPage03NamedRow characters[]{
    {text043, 118, 87},
    {text044, 119, 88},
    {text045, 120, 89},
    {text046, 121, 90},
    {text047, 122, 91},
    {text042, 123, 92}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {101, 87, 118},
    {102, 88, 119},
    {103, 89, 120},
    {104, 90, 121},
    {105, 91, 122},
    {106, 92, 123}
};
constexpr int charactersParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr std::int32_t mute0Dependents[]{
    109,
    126
};
constexpr std::int32_t mute1Dependents[]{
    110,
    127
};
constexpr std::int32_t mute2Dependents[]{
    111,
    128
};
constexpr std::int32_t mute3Dependents[]{
    112,
    129
};
constexpr std::int32_t mute4Dependents[]{
    124,
    142,
    107
};
constexpr std::int32_t mute5Dependents[]{
    125,
    130,
    108
};
constexpr std::int32_t mute6Dependents[]{
    113,
    131,
    96
};
constexpr std::int32_t mute7Dependents[]{
    114,
    132,
    97
};
constexpr std::int32_t mute8Dependents[]{
    115,
    133,
    98
};
constexpr std::int32_t mute9Dependents[]{
    116,
    134,
    99
};
constexpr std::int32_t mute10Dependents[]{
    118,
    135,
    101
};
constexpr std::int32_t mute11Dependents[]{
    119,
    136,
    102
};
constexpr std::int32_t mute12Dependents[]{
    120,
    137,
    103
};
constexpr std::int32_t mute13Dependents[]{
    121,
    138,
    104
};
constexpr std::int32_t mute14Dependents[]{
    122,
    139,
    105
};
constexpr std::int32_t mute15Dependents[]{
    123,
    140,
    106
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
    {92, mute15Dependents}
};
constexpr char text048[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text049[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56457C
constexpr GuiStaticDescriptorNative24h footer[]{
    {25, {text049, nullptr, nullptr, text049, text049, nullptr, nullptr}, nullptr}
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
    std::string_view{text048, 10},
    false,
    {SettingSoundLoopKind::CheckBox, 95, -1, -1, -1, {}, nullptr},
    {false, 9, {-1, -1}, {-1, -1}, {}},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
