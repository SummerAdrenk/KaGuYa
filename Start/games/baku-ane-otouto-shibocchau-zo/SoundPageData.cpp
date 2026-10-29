#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::BakuAneOtoutoShibocchauZoSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/01.json；原父构造 VA：0x4EBFD0。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56ECEC, 0x56ED10, 0x56ED34
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {119, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {120, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {121, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x570330, 0x57039C, 0x570408
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {102, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 139, 282, 158}, {0, 0, -12, 0}},
    {103, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 180, 282, 199}, {0, 0, -12, 0}},
    {104, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 221, 282, 240}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point000{285, 119};
constexpr POINT point001{285, 160};
constexpr POINT point002{285, 201};
// 原描述符 VA：0x56FCB8, 0x56FD00, 0x56FD48
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {72, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {73, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {74, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
};
constexpr SettingPage03ControlGroup block0Groups[]{
    {SettingPage03ControlKind::Static, 3, block0Group0},
    {SettingPage03ControlKind::Slider, 3, block0Group1},
    {SettingPage03ControlKind::CheckBox, 3, block0Group2}
};
constexpr char text009[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xE8\x83\x52\x83\x4C\x91\xAB\x83\x52\x83\x4C\x2E\x76\x72\x6D"};
constexpr char text010[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\x77\x83\x7D\x83\x93\x83\x4E\x83\x93\x83\x6A\x2E\x76\x72\x6D"};
constexpr char text011[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x92\xAA\x90\x81\x82\xAB\x2E\x76\x72\x6D"};
constexpr char text012[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xCB\x90\xB8\x89\xB9\x2E\x76\x72\x6D"};
constexpr char text013[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56ED58, 0x56ED7C, 0x56EDA0, 0x56EDC4, 0x56EDE8
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {122, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {123, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {124, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {125, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {126, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text014[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x74\x83\x46\x83\x89\x83\x60\x83\x49\x2E\x76\x72\x6D"};
constexpr char text015[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56EEE4, 0x56EF08
constexpr GuiStaticDescriptorNative24h block1Group1[]{
    {133, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {134, {nullptr, text015, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x570474, 0x5704E0, 0x57054C, 0x5705B8, 0x570624
constexpr GuiSliderDescriptorNative6Ch block1Group2[]{
    {105, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 350, 282, 369}, {0, 0, -12, 0}},
    {106, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 391, 282, 410}, {0, 0, -12, 0}},
    {107, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 432, 282, 451}, {0, 0, -12, 0}},
    {108, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 473, 282, 492}, {0, 0, -12, 0}},
    {109, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 514, 282, 533}, {0, 0, -12, 0}}
};
// 原描述符 VA：0x570918, 0x570984
constexpr GuiSliderDescriptorNative6Ch block1Group3[]{
    {116, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 268, 282, 287}, {0, 0, -12, 0}},
    {117, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 309, 282, 328}, {0, 0, -12, 0}}
};
constexpr POINT point003{285, 330};
constexpr POINT point004{285, 371};
constexpr POINT point005{285, 412};
constexpr POINT point006{285, 453};
constexpr POINT point007{285, 494};
// 原描述符 VA：0x56FD90, 0x56FDD8, 0x56FE20, 0x56FE68, 0x56FEB0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group4[]{
    {75, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point003, 7, -1},
    {76, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point004, 7, -1},
    {77, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point005, 7, -1},
    {78, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point006, 7, -1},
    {79, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point007, 7, -1}
};
constexpr POINT point008{285, 248};
constexpr POINT point009{285, 289};
// 原描述符 VA：0x5700A8, 0x5700F0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group5[]{
    {86, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point008, 7, -1},
    {87, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point009, 7, -1}
};
constexpr char text016[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text017[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text018[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text019[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point010{219, 332};
constexpr POINT point011{219, 373};
constexpr POINT point012{219, 414};
constexpr POINT point013{219, 455};
constexpr POINT point014{219, 496};
// 原描述符 VA：0x56F5A0, 0x56F5CC, 0x56F5F8, 0x56F624, 0x56F650
constexpr GuiButtonDescriptorNative2Ch block1Group6[]{
    {{88, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point010}, 7, -1},
    {{89, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point011}, 7, -1},
    {{90, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point012}, 7, -1},
    {{91, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point013}, 7, -1},
    {{92, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point014}, 7, -1}
};
constexpr POINT point015{219, 250};
constexpr POINT point016{219, 291};
// 原描述符 VA：0x56F548, 0x56F574
constexpr GuiButtonDescriptorNative2Ch block1Group7[]{
    {{99, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point015}, 7, -1},
    {{100, {text016, text017, nullptr, text018, text018, text019, nullptr}, &point016}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 5, block1Group0},
    {SettingPage03ControlKind::Static, 2, block1Group1},
    {SettingPage03ControlKind::Slider, 5, block1Group2},
    {SettingPage03ControlKind::Slider, 2, block1Group3},
    {SettingPage03ControlKind::CheckBox, 5, block1Group4},
    {SettingPage03ControlKind::CheckBox, 2, block1Group5},
    {SettingPage03ControlKind::Button, 5, block1Group6},
    {SettingPage03ControlKind::Button, 2, block1Group7}
};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x94\x54\x8A\x47\x8E\xC0\x2E\x76\x72\x6D"};
constexpr char text021[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x97\x46\x97\x9C\x89\xB9\x2E\x76\x72\x6D"};
constexpr char text022[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xC0\x8D\xF7\x2E\x76\x72\x6D"};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8D\xD8\x97\xAF\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x41\x83\x8A\x83\x58\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x83\x4C\x83\x83\x83\x89\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56EE0C, 0x56EE30, 0x56EE54, 0x56EE78, 0x56EE9C, 0x56EEC0
constexpr GuiStaticDescriptorNative24h block2Group0[]{
    {127, {nullptr, text020, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {128, {nullptr, text021, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {129, {nullptr, text022, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {130, {nullptr, text023, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {131, {nullptr, text024, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {132, {nullptr, text025, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x570690, 0x5706FC, 0x570768, 0x5707D4, 0x570840, 0x5708AC
constexpr GuiSliderDescriptorNative6Ch block2Group1[]{
    {110, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 170, 656, 189}, {0, 0, -12, 0}},
    {111, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 275, 656, 294}, {0, 0, -12, 0}},
    {112, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 379, 656, 398}, {0, 0, -12, 0}},
    {113, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 480, 656, 499}, {0, 0, -12, 0}},
    {114, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 170, 965, 189}, {0, 0, -12, 0}},
    {115, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 275, 965, 294}, {0, 0, -12, 0}}
};
constexpr char text026[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text027[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text029[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point017{629, 121};
constexpr POINT point018{629, 226};
constexpr POINT point019{629, 332};
constexpr POINT point020{629, 434};
constexpr POINT point021{935, 121};
constexpr POINT point022{935, 226};
// 原描述符 VA：0x56FEF8, 0x56FF40, 0x56FF88, 0x56FFD0, 0x570018, 0x570060
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group2[]{
    {80, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point017, 7, -1},
    {81, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point018, 7, -1},
    {82, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point019, 7, -1},
    {83, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point020, 7, -1},
    {84, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point021, 7, -1},
    {85, {text026, text026, nullptr, text027, text027, text028, nullptr}, {text029, text026, nullptr, text027, text027, text028, nullptr}, &point022, 7, -1}
};
constexpr char text030[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text031[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text033[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point023{596, 192};
constexpr POINT point024{596, 297};
constexpr POINT point025{596, 402};
constexpr POINT point026{596, 505};
constexpr POINT point027{905, 192};
constexpr POINT point028{905, 297};
// 原描述符 VA：0x56F440, 0x56F46C, 0x56F498, 0x56F4C4, 0x56F4F0, 0x56F51C
constexpr GuiButtonDescriptorNative2Ch block2Group3[]{
    {{93, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point023}, 7, -1},
    {{94, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point024}, 7, -1},
    {{95, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point025}, 7, -1},
    {{96, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point026}, 7, -1},
    {{97, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point027}, 7, -1},
    {{98, {text030, text031, nullptr, text032, text032, text033, nullptr}, &point028}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Static, 6, block2Group0},
    {SettingPage03ControlKind::Slider, 6, block2Group1},
    {SettingPage03ControlKind::CheckBox, 6, block2Group2},
    {SettingPage03ControlKind::Button, 6, block2Group3}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 102, 72},
    {SettingSoundMaster::Effects, 104, 74},
    {SettingSoundMaster::Voice, 103, 73}
};
constexpr char text034[]{"\x83\x74\x83\x46\x83\x89\x83\x60\x83\x49"};
constexpr char text035[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text036[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text037[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text038[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text039[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text040[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text034, 116, 86},
    {text035, 117, 87},
    {text036, 105, 75},
    {text037, 106, 76},
    {text038, 107, 77},
    {text039, 108, 78},
    {text040, 109, 79}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {99, 86, 116},
    {100, 87, 117},
    {88, 75, 105},
    {89, 76, 106},
    {90, 77, 107},
    {91, 78, 108},
    {92, 79, 109}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5,
    6
};
constexpr char text041[]{"\x94\x54\x8A\x47\x8E\xC0"};
constexpr char text042[]{"\x97\x46\x97\x9C\x89\xB9"};
constexpr char text043[]{"\x8E\xC0\x8D\xF7"};
constexpr char text044[]{"\x8D\xD8\x97\xAF"};
constexpr char text045[]{"\x83\x41\x83\x8A\x83\x58"};
constexpr SettingPage03NamedRow characters[]{
    {text041, 110, 80},
    {text042, 111, 81},
    {text043, 112, 82},
    {text044, 113, 83},
    {text045, 114, 84},
    {text040, 115, 85}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {93, 80, 110},
    {94, 81, 111},
    {95, 82, 112},
    {96, 83, 113},
    {97, 84, 114},
    {98, 85, 115}
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
    101,
    118
};
constexpr std::int32_t mute1Dependents[]{
    102,
    119
};
constexpr std::int32_t mute2Dependents[]{
    103,
    120
};
constexpr std::int32_t mute3Dependents[]{
    104,
    121
};
constexpr std::int32_t mute4Dependents[]{
    116,
    133,
    99
};
constexpr std::int32_t mute5Dependents[]{
    117,
    134,
    100
};
constexpr std::int32_t mute6Dependents[]{
    105,
    122,
    88
};
constexpr std::int32_t mute7Dependents[]{
    106,
    123,
    89
};
constexpr std::int32_t mute8Dependents[]{
    107,
    124,
    90
};
constexpr std::int32_t mute9Dependents[]{
    108,
    125,
    91
};
constexpr std::int32_t mute10Dependents[]{
    109,
    126,
    92
};
constexpr std::int32_t mute11Dependents[]{
    110,
    127,
    93
};
constexpr std::int32_t mute12Dependents[]{
    111,
    128,
    94
};
constexpr std::int32_t mute13Dependents[]{
    112,
    129,
    95
};
constexpr std::int32_t mute14Dependents[]{
    113,
    130,
    96
};
constexpr std::int32_t mute15Dependents[]{
    114,
    131,
    97
};
constexpr std::int32_t mute16Dependents[]{
    115,
    132,
    98
};
constexpr SettingPage03MuteRow muteRows[]{
    {71, mute0Dependents},
    {72, mute1Dependents},
    {73, mute2Dependents},
    {74, mute3Dependents},
    {86, mute4Dependents},
    {87, mute5Dependents},
    {75, mute6Dependents},
    {76, mute7Dependents},
    {77, mute8Dependents},
    {78, mute9Dependents},
    {79, mute10Dependents},
    {80, mute11Dependents},
    {81, mute12Dependents},
    {82, mute13Dependents},
    {83, mute14Dependents},
    {84, mute15Dependents},
    {85, mute16Dependents}
};
constexpr char text046[]{"\x89\xB9\x90\xBA"};
constexpr char text047[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x56ECA4
constexpr GuiStaticDescriptorNative24h footer[]{
    {20, {text047, nullptr, nullptr, text047, text047, nullptr, nullptr}, nullptr}
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
    {},
    effectsPreviews,
    charactersPreviews,
    effectsParamsIndices,
    charactersParamsIndices,
    muteRows,
    std::string_view{text046, 4},
    true,
    {SettingSoundLoopKind::None, -1, -1, -1, -1, {}, nullptr},
    {false, 9, {-1, -1}, {-1, -1}, {}},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
