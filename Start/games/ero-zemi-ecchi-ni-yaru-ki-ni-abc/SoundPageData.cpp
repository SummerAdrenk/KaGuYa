#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::EroZemiEcchiNiYaruKiNiAbcSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/05.json；原父构造 VA：0x4E8A00。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55C9E0, 0x55CA04, 0x55CA28
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {126, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {127, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {128, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55CA50, 0x55CABC, 0x55CB28
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {109, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 139, 282, 158}, {0, 0, -12, 0}},
    {110, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 180, 282, 199}, {0, 0, -12, 0}},
    {111, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 220, 282, 239}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x8F\xAC\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x8F\xAC\x2E\x76\x72\x6D"};
constexpr POINT point000{285, 119};
constexpr POINT point001{285, 160};
constexpr POINT point002{285, 201};
// 原描述符 VA：0x55CBD8, 0x55CC20, 0x55CC68
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {78, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {79, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {80, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
};
constexpr char text009[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x32\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text010[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text011[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x49\x83\x93\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
constexpr char text012[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x89\xB9\x90\xBA\x83\x8B\x81\x5B\x83\x76\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55CCB0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group3[]{
    {94, {nullptr, nullptr, nullptr, text009, text009, text010, nullptr}, {text011, nullptr, nullptr, text012, text012, text010, nullptr}, nullptr, 7, -1}
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
// 原描述符 VA：0x55CD28, 0x55CD4C, 0x55CD70, 0x55CD94, 0x55CDB8, 0x55CDDC
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {141, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {129, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {130, {nullptr, text015, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {131, {nullptr, text016, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {132, {nullptr, text017, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {133, {nullptr, text018, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x55CE00, 0x55CE6C, 0x55CED8, 0x55CF44, 0x55CFB0, 0x55D01C
constexpr GuiSliderDescriptorNative6Ch block1Group1[]{
    {123, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 268, 282, 287}, {0, 0, -12, 0}},
    {124, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 309, 282, 328}, {0, 0, -12, 0}},
    {112, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 350, 282, 369}, {0, 0, -12, 0}},
    {113, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 391, 282, 410}, {0, 0, -12, 0}},
    {114, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 432, 282, 451}, {0, 0, -12, 0}},
    {115, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {150, 473, 282, 492}, {0, 0, -12, 0}}
};
constexpr POINT point003{285, 248};
constexpr POINT point004{285, 289};
constexpr POINT point005{285, 330};
constexpr POINT point006{285, 371};
constexpr POINT point007{285, 412};
constexpr POINT point008{285, 453};
// 原描述符 VA：0x55D0B8, 0x55D100, 0x55D148, 0x55D190, 0x55D1D8, 0x55D220
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group2[]{
    {92, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point003, 7, -1},
    {93, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point004, 7, -1},
    {81, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point005, 7, -1},
    {82, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point006, 7, -1},
    {83, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point007, 7, -1},
    {84, {text005, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point008, 7, -1}
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
// 原描述符 VA：0x55D298, 0x55D2C4, 0x55D2F0, 0x55D31C, 0x55D348, 0x55D374
constexpr GuiButtonDescriptorNative2Ch block1Group3[]{
    {{106, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point009}, 7, -1},
    {{107, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point010}, 7, -1},
    {{95, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point011}, 7, -1},
    {{96, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point012}, 7, -1},
    {{97, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point013}, 7, -1},
    {{98, {text019, text020, nullptr, text021, text021, text022, nullptr}, &point014}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 6, block1Group0},
    {SettingPage03ControlKind::Slider, 6, block1Group1},
    {SettingPage03ControlKind::CheckBox, 6, block1Group2},
    {SettingPage03ControlKind::Button, 6, block1Group3}
};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x95\x96\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xD8\x98\x40\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x97\xA2\x94\xFC\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x82\x83\x6A\x83\x4A\x2E\x76\x72\x6D"};
constexpr char text027[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x93\x7E\x8A\x43\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x83\x4C\x83\x83\x83\x89\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55D3D0, 0x55D3F4, 0x55D418, 0x55D43C, 0x55D460, 0x55D484
constexpr GuiStaticDescriptorNative24h block2Group0[]{
    {134, {nullptr, text023, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {135, {nullptr, text024, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {136, {nullptr, text025, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {137, {nullptr, text026, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {138, {nullptr, text027, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {139, {nullptr, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x55D4A8, 0x55D514, 0x55D580, 0x55D5EC, 0x55D658, 0x55D6C4
constexpr GuiSliderDescriptorNative6Ch block2Group1[]{
    {117, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 170, 656, 189}, {0, 0, -12, 0}},
    {118, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 275, 656, 294}, {0, 0, -12, 0}},
    {119, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 379, 656, 398}, {0, 0, -12, 0}},
    {120, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {524, 480, 656, 499}, {0, 0, -12, 0}},
    {121, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 170, 965, 189}, {0, 0, -12, 0}},
    {122, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {833, 275, 965, 294}, {0, 0, -12, 0}}
};
constexpr char text029[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text031[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point015{629, 121};
constexpr POINT point016{629, 226};
constexpr POINT point017{629, 332};
constexpr POINT point018{629, 434};
constexpr POINT point019{935, 121};
constexpr POINT point020{935, 226};
// 原描述符 VA：0x55D760, 0x55D7A8, 0x55D7F0, 0x55D838, 0x55D880, 0x55D8C8
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group2[]{
    {86, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point015, 7, -1},
    {87, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point016, 7, -1},
    {88, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point017, 7, -1},
    {89, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point018, 7, -1},
    {90, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point019, 7, -1},
    {91, {text029, text029, nullptr, text030, text030, text031, nullptr}, {text032, text029, nullptr, text030, text030, text031, nullptr}, &point020, 7, -1}
};
constexpr char text033[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text034[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text035[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text036[]{"\x89\xB9\x97\xCA\x83\x65\x83\x58\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point021{596, 192};
constexpr POINT point022{596, 297};
constexpr POINT point023{596, 402};
constexpr POINT point024{596, 505};
constexpr POINT point025{905, 192};
constexpr POINT point026{905, 297};
// 原描述符 VA：0x55D940, 0x55D96C, 0x55D998, 0x55D9C4, 0x55D9F0, 0x55DA1C
constexpr GuiButtonDescriptorNative2Ch block2Group3[]{
    {{100, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point021}, 7, -1},
    {{101, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point022}, 7, -1},
    {{102, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point023}, 7, -1},
    {{103, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point024}, 7, -1},
    {{104, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point025}, 7, -1},
    {{105, {text033, text034, nullptr, text035, text035, text036, nullptr}, &point026}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Static, 6, block2Group0},
    {SettingPage03ControlKind::Slider, 6, block2Group1},
    {SettingPage03ControlKind::CheckBox, 6, block2Group2},
    {SettingPage03ControlKind::Button, 6, block2Group3}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 109, 78},
    {SettingSoundMaster::Effects, 111, 80},
    {SettingSoundMaster::Voice, 110, 79}
};
constexpr char text037[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text038[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text039[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text040[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text041[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text042[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text037, 123, 92},
    {text038, 124, 93},
    {text039, 112, 81},
    {text040, 113, 82},
    {text041, 114, 83},
    {text042, 115, 84}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {106, 92, 123},
    {107, 93, 124},
    {95, 81, 112},
    {96, 82, 113},
    {97, 83, 114},
    {98, 84, 115}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr char text043[]{"\x95\x96"};
constexpr char text044[]{"\x89\xD8\x98\x40"};
constexpr char text045[]{"\x97\xA2\x94\xFC"};
constexpr char text046[]{"\x83\x82\x83\x6A\x83\x4A"};
constexpr char text047[]{"\x93\x7E\x8A\x43"};
constexpr SettingPage03NamedRow characters[]{
    {text043, 117, 86},
    {text044, 118, 87},
    {text045, 119, 88},
    {text046, 120, 89},
    {text047, 121, 90},
    {text042, 122, 91}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {100, 86, 117},
    {101, 87, 118},
    {102, 88, 119},
    {103, 89, 120},
    {104, 90, 121},
    {105, 91, 122}
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
    108,
    125
};
constexpr std::int32_t mute1Dependents[]{
    109,
    126
};
constexpr std::int32_t mute2Dependents[]{
    110,
    127
};
constexpr std::int32_t mute3Dependents[]{
    111,
    128
};
constexpr std::int32_t mute4Dependents[]{
    123,
    141,
    106
};
constexpr std::int32_t mute5Dependents[]{
    124,
    129,
    107
};
constexpr std::int32_t mute6Dependents[]{
    112,
    130,
    95
};
constexpr std::int32_t mute7Dependents[]{
    113,
    131,
    96
};
constexpr std::int32_t mute8Dependents[]{
    114,
    132,
    97
};
constexpr std::int32_t mute9Dependents[]{
    115,
    133,
    98
};
constexpr std::int32_t mute10Dependents[]{
    117,
    134,
    100
};
constexpr std::int32_t mute11Dependents[]{
    118,
    135,
    101
};
constexpr std::int32_t mute12Dependents[]{
    119,
    136,
    102
};
constexpr std::int32_t mute13Dependents[]{
    120,
    137,
    103
};
constexpr std::int32_t mute14Dependents[]{
    121,
    138,
    104
};
constexpr std::int32_t mute15Dependents[]{
    122,
    139,
    105
};
constexpr SettingPage03MuteRow muteRows[]{
    {77, mute0Dependents},
    {78, mute1Dependents},
    {79, mute2Dependents},
    {80, mute3Dependents},
    {92, mute4Dependents},
    {93, mute5Dependents},
    {81, mute6Dependents},
    {82, mute7Dependents},
    {83, mute8Dependents},
    {84, mute9Dependents},
    {86, mute10Dependents},
    {87, mute11Dependents},
    {88, mute12Dependents},
    {89, mute13Dependents},
    {90, mute14Dependents},
    {91, mute15Dependents}
};
constexpr char text048[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text049[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55C874
constexpr GuiStaticDescriptorNative24h footer[]{
    {24, {text049, nullptr, nullptr, text049, text049, nullptr, nullptr}, nullptr}
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
    std::string_view{text048, 10},
    false,
    {SettingSoundLoopKind::CheckBox, 94, -1, -1, -1, {}, nullptr},
    {false, 9, {-1, -1}, {-1, -1}, {}},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
