#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::PureXHolicJunketsuOtomeToKoninKankeiSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/15.json；原父构造 VA：0x4F8790。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561150, 0x561174, 0x561198
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {125, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {126, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {127, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5611E0, 0x56124C, 0x5612B8
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {122, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 209, 401, 228}, {0, 0, -12, 0}},
    {123, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 252, 401, 271}, {0, 0, -12, 0}},
    {124, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 297, 401, 316}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point000{489, 202};
constexpr POINT point001{489, 246};
constexpr POINT point002{489, 290};
// 原描述符 VA：0x561360, 0x5613A8, 0x5613F0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {118, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {119, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {120, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
};
constexpr SettingPage03ControlGroup block0Groups[]{
    {SettingPage03ControlKind::Static, 3, block0Group0},
    {SettingPage03ControlKind::Slider, 3, block0Group1},
    {SettingPage03ControlKind::CheckBox, 3, block0Group2}
};
constexpr char text009[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93\x2E\x76\x72\x6D"};
constexpr char text010[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xE8\x83\x52\x83\x4C\x91\xAB\x83\x52\x83\x4C\x2E\x76\x72\x6D"};
constexpr char text011[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\x77\x83\x7D\x83\x93\x83\x4E\x83\x93\x83\x6A\x2E\x76\x72\x6D"};
constexpr char text012[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x92\xAA\x90\x81\x82\xAB\x2E\x76\x72\x6D"};
constexpr char text013[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x8E\xCB\x90\xB8\x89\xB9\x2E\x76\x72\x6D"};
constexpr char text014[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561480, 0x5614A4, 0x5614C8, 0x5614EC, 0x561510, 0x561534
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {146, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {147, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {148, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {149, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {150, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {151, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x561558, 0x5615C4, 0x561630, 0x56169C, 0x561708, 0x561774
constexpr GuiSliderDescriptorNative6Ch block1Group1[]{
    {140, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 354, 401, 373}, {0, 0, -12, 0}},
    {141, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 398, 401, 417}, {0, 0, -12, 0}},
    {142, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 442, 401, 461}, {0, 0, -12, 0}},
    {143, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 486, 401, 505}, {0, 0, -12, 0}},
    {144, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 530, 401, 549}, {0, 0, -12, 0}},
    {145, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 574, 401, 593}, {0, 0, -12, 0}}
};
constexpr POINT point003{489, 347};
constexpr POINT point004{489, 391};
constexpr POINT point005{489, 435};
constexpr POINT point006{489, 479};
constexpr POINT point007{489, 523};
constexpr POINT point008{489, 567};
// 原描述符 VA：0x561810, 0x561858, 0x5618A0, 0x5618E8, 0x561930, 0x561978
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group2[]{
    {128, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point003, 7, -1},
    {130, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point004, 7, -1},
    {132, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point005, 7, -1},
    {134, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point006, 7, -1},
    {136, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point007, 7, -1},
    {138, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point008, 7, -1}
};
constexpr char text015[]{"\x89\xB9\x90\xBA\x8D\xC4\x90\xB6\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text016[]{"\x89\xB9\x90\xBA\x8D\xC4\x90\xB6\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text017[]{"\x89\xB9\x90\xBA\x8D\xC4\x90\xB6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text018[]{"\x89\xB9\x90\xBA\x8D\xC4\x90\xB6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point009{440, 347};
constexpr POINT point010{440, 391};
constexpr POINT point011{440, 435};
constexpr POINT point012{440, 479};
constexpr POINT point013{440, 523};
constexpr POINT point014{440, 567};
// 原描述符 VA：0x5619F0, 0x561A1C, 0x561A48, 0x561A74, 0x561AA0, 0x561ACC
constexpr GuiButtonDescriptorNative2Ch block1Group3[]{
    {{129, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point009}, 7, -1},
    {{131, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point010}, 7, -1},
    {{133, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point011}, 7, -1},
    {{135, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point012}, 7, -1},
    {{137, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point013}, 7, -1},
    {{139, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point014}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 6, block1Group0},
    {SettingPage03ControlKind::Slider, 6, block1Group1},
    {SettingPage03ControlKind::CheckBox, 6, block1Group2},
    {SettingPage03ControlKind::Button, 6, block1Group3}
};
// 原描述符 VA：0x561E38, 0x561EA4, 0x561F10, 0x561F7C, 0x561FE8, 0x562054, 0x5620C0, 0x56212C, 0x562198, 0x562204, 0x562270, 0x5622DC, 0x562348, 0x5623B4, 0x562420, 0x56248C, 0x5624F8
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {186, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {187, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {188, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {189, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}},
    {190, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 607, 845, 626}, {0, 0, -12, 0}},
    {191, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 235, 1186, 254}, {0, 0, -12, 0}},
    {192, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 328, 1186, 347}, {0, 0, -12, 0}},
    {193, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 421, 1186, 440}, {0, 0, -12, 0}},
    {194, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 514, 1186, 533}, {0, 0, -12, 0}},
    {195, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {196, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {197, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {198, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}},
    {199, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 607, 845, 626}, {0, 0, -12, 0}},
    {200, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 235, 1186, 254}, {0, 0, -12, 0}},
    {201, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 328, 1186, 347}, {0, 0, -12, 0}},
    {202, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 421, 1186, 440}, {0, 0, -12, 0}}
};
constexpr POINT point015{844, 197};
constexpr POINT point016{844, 290};
constexpr POINT point017{844, 383};
constexpr POINT point018{844, 476};
constexpr POINT point019{844, 569};
constexpr POINT point020{1185, 197};
constexpr POINT point021{1185, 290};
constexpr POINT point022{1185, 383};
constexpr POINT point023{1185, 476};
// 原描述符 VA：0x5625D8, 0x562620, 0x562668, 0x5626B0, 0x5626F8, 0x562740, 0x562788, 0x5627D0, 0x562818, 0x562860, 0x5628A8, 0x5628F0, 0x562938, 0x562980, 0x5629C8, 0x562A10, 0x562A58
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {152, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point015, 7, -1},
    {154, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point016, 7, -1},
    {156, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point017, 7, -1},
    {158, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1},
    {160, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point019, 7, -1},
    {162, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point020, 7, -1},
    {164, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point021, 7, -1},
    {166, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point022, 7, -1},
    {168, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point023, 7, -1},
    {170, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point015, 7, -1},
    {172, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point016, 7, -1},
    {174, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point017, 7, -1},
    {176, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1},
    {178, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point019, 7, -1},
    {180, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point020, 7, -1},
    {182, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point021, 7, -1},
    {184, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point022, 7, -1}
};
constexpr POINT point024{795, 197};
constexpr POINT point025{795, 290};
constexpr POINT point026{795, 383};
constexpr POINT point027{795, 476};
constexpr POINT point028{795, 569};
constexpr POINT point029{1136, 197};
constexpr POINT point030{1136, 290};
constexpr POINT point031{1136, 383};
constexpr POINT point032{1136, 476};
// 原描述符 VA：0x562AF0, 0x562B1C, 0x562B48, 0x562B74, 0x562BA0, 0x562BCC, 0x562BF8, 0x562C24, 0x562C50, 0x562C7C, 0x562CA8, 0x562CD4, 0x562D00, 0x562D2C, 0x562D58, 0x562D84, 0x562DB0
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{153, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point024}, 7, -1},
    {{155, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point025}, 7, -1},
    {{157, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point026}, 7, -1},
    {{159, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point027}, 7, -1},
    {{161, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point028}, 7, -1},
    {{163, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point029}, 7, -1},
    {{165, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point030}, 7, -1},
    {{167, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point031}, 7, -1},
    {{169, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point032}, 7, -1},
    {{171, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point024}, 7, -1},
    {{173, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point025}, 7, -1},
    {{175, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point026}, 7, -1},
    {{177, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point027}, 7, -1},
    {{179, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point028}, 7, -1},
    {{181, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point029}, 7, -1},
    {{183, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point030}, 7, -1},
    {{185, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point031}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 17, block2Group0},
    {SettingPage03ControlKind::CheckBox, 17, block2Group1},
    {SettingPage03ControlKind::Button, 17, block2Group2}
};
constexpr char text019[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x31\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x563380, 0x5633A4
constexpr GuiStaticDescriptorNative24h block4Group0[]{
    {224, {text019, nullptr, nullptr, text019, text019, nullptr, nullptr}, nullptr},
    {225, {text020, nullptr, nullptr, text020, text020, nullptr, nullptr}, nullptr}
};
constexpr char text021[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x6F\x66\x66\x2E\x76\x72\x6D"};
constexpr char text022[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D"};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x6F\x66\x66\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5633C8, 0x5633F4
constexpr GuiButtonDescriptorNative2Ch block4Group1[]{
    {{226, {text021, nullptr, nullptr, text022, text022, text023, nullptr}, nullptr}, 3, -1},
    {{227, {text024, nullptr, nullptr, text025, text025, text026, nullptr}, nullptr}, 3, -1}
};
constexpr SettingPage03ControlGroup block4Groups[]{
    {SettingPage03ControlKind::Static, 2, block4Group0},
    {SettingPage03ControlKind::Button, 2, block4Group1}
};
constexpr char text027[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr POINT point033{565, 191};
// 原描述符 VA：0x561B48
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {203, {text027, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, &point033}
};
constexpr char text029[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr POINT point034{565, 284};
// 原描述符 VA：0x561B6C
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {204, {text029, text030, nullptr, nullptr, nullptr, nullptr, nullptr}, &point034}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr POINT point035{565, 377};
// 原描述符 VA：0x561B90
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {205, {text031, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, &point035}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr POINT point036{565, 470};
// 原描述符 VA：0x561BB4
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {206, {text033, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
};
constexpr char text035[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr char text036[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr POINT point037{565, 563};
// 原描述符 VA：0x561BD8
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {207, {text035, text036, nullptr, nullptr, nullptr, nullptr, nullptr}, &point037}
};
constexpr char text037[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr char text038[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr POINT point038{906, 191};
// 原描述符 VA：0x561BFC
constexpr GuiStaticDescriptorNative24h label5Shown[]{
    {208, {text037, text038, nullptr, nullptr, nullptr, nullptr, nullptr}, &point038}
};
constexpr char text039[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr char text040[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr POINT point039{906, 284};
// 原描述符 VA：0x561C20
constexpr GuiStaticDescriptorNative24h label6Shown[]{
    {209, {text039, text040, nullptr, nullptr, nullptr, nullptr, nullptr}, &point039}
};
constexpr char text041[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x38\x2E\x76\x72\x6D"};
constexpr char text042[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x38\x2E\x76\x72\x6D"};
constexpr POINT point040{906, 377};
// 原描述符 VA：0x561C44
constexpr GuiStaticDescriptorNative24h label7Shown[]{
    {210, {text041, text042, nullptr, nullptr, nullptr, nullptr, nullptr}, &point040}
};
constexpr char text043[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x39\x2E\x76\x72\x6D"};
constexpr char text044[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x39\x2E\x76\x72\x6D"};
constexpr POINT point041{906, 470};
// 原描述符 VA：0x561C68
constexpr GuiStaticDescriptorNative24h label8Shown[]{
    {211, {text043, text044, nullptr, nullptr, nullptr, nullptr, nullptr}, &point041}
};
constexpr char text045[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x30\x2E\x76\x72\x6D"};
constexpr char text046[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x30\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561C8C
constexpr GuiStaticDescriptorNative24h label9Shown[]{
    {212, {text045, text046, nullptr, nullptr, nullptr, nullptr, nullptr}, &point033}
};
constexpr char text047[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x31\x2E\x76\x72\x6D"};
constexpr char text048[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x31\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561CB0
constexpr GuiStaticDescriptorNative24h label10Shown[]{
    {213, {text047, text048, nullptr, nullptr, nullptr, nullptr, nullptr}, &point034}
};
constexpr char text049[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x32\x2E\x76\x72\x6D"};
constexpr char text050[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561CD4
constexpr GuiStaticDescriptorNative24h label11Shown[]{
    {214, {text049, text050, nullptr, nullptr, nullptr, nullptr, nullptr}, &point035}
};
constexpr char text051[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x33\x2E\x76\x72\x6D"};
constexpr char text052[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x33\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561CF8
constexpr GuiStaticDescriptorNative24h label12Shown[]{
    {215, {text051, text052, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
};
constexpr char text053[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x34\x2E\x76\x72\x6D"};
constexpr char text054[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x34\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561D1C
constexpr GuiStaticDescriptorNative24h label13Shown[]{
    {216, {text053, text054, nullptr, nullptr, nullptr, nullptr, nullptr}, &point037}
};
constexpr char text055[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x35\x2E\x76\x72\x6D"};
constexpr char text056[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x35\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561D40
constexpr GuiStaticDescriptorNative24h label14Shown[]{
    {217, {text055, text056, nullptr, nullptr, nullptr, nullptr, nullptr}, &point038}
};
constexpr char text057[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x36\x2E\x76\x72\x6D"};
constexpr char text058[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x36\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561D64
constexpr GuiStaticDescriptorNative24h label15Shown[]{
    {218, {text057, text058, nullptr, nullptr, nullptr, nullptr, nullptr}, &point039}
};
constexpr char text059[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr char text060[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561D88
constexpr GuiStaticDescriptorNative24h label16Shown[]{
    {219, {text059, text060, nullptr, nullptr, nullptr, nullptr, nullptr}, &point040}
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
    {{&label8Shown[0], nullptr}, 8},
    {{&label9Shown[0], nullptr}, 9},
    {{&label10Shown[0], nullptr}, 10},
    {{&label11Shown[0], nullptr}, 11},
    {{&label12Shown[0], nullptr}, 12},
    {{&label13Shown[0], nullptr}, 13},
    {{&label14Shown[0], nullptr}, 14},
    {{&label15Shown[0], nullptr}, 15},
    {{&label16Shown[0], nullptr}, 16}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 122, 118},
    {SettingSoundMaster::Effects, 124, 120},
    {SettingSoundMaster::Voice, 123, 119}
};
constexpr char text061[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text062[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text063[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text064[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text065[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text066[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text061, 140, 128},
    {text062, 141, 130},
    {text063, 142, 132},
    {text064, 143, 134},
    {text065, 144, 136},
    {text066, 145, 138}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {129, 128, 140},
    {131, 130, 141},
    {133, 132, 142},
    {135, 134, 143},
    {137, 136, 144},
    {139, 138, 145}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr char text067[]{"\x83\x6E\x83\x67\x83\x7A\x83\x8B"};
constexpr char text068[]{"\x83\x41\x83\x58\x83\x67\x83\x89\x83\x43\x83\x41"};
constexpr char text069[]{"\x83\x41\x83\x8B\x83\x8B"};
constexpr char text070[]{"\x83\x82\x83\x8B\x83\x4B\x83\x93"};
constexpr char text071[]{"\x95\x91\x93\x80"};
constexpr char text072[]{"\x8D\x67\x89\xB9"};
constexpr char text073[]{"\x96\xE9\x8D\xB3"};
constexpr char text074[]{"\x97\xE9\x8E\xAD"};
constexpr char text075[]{"\x83\x54\x83\x8A\x81\x5B"};
constexpr char text076[]{"\x83\x5A\x83\x43\x83\x89"};
constexpr char text077[]{"\x83\x41\x83\x94\x83\x8A\x83\x8B"};
constexpr char text078[]{"\x8B\x6A\x8D\x5B"};
constexpr char text079[]{"\x88\xA9"};
constexpr char text080[]{"\x83\x7E\x81\x5B\x83\x81"};
constexpr char text081[]{"\x83\x4F\x83\x8A\x83\x82"};
constexpr char text082[]{"\x83\x4F\x83\x89\x83\x40\x83\x6C"};
constexpr SettingPage03NamedRow characters[]{
    {text067, 186, 152},
    {text068, 187, 154},
    {text069, 188, 156},
    {text070, 189, 158},
    {text071, 190, 160},
    {text072, 191, 162},
    {text073, 192, 164},
    {text074, 193, 166},
    {text075, 194, 168},
    {text076, 195, 170},
    {text077, 196, 172},
    {text078, 197, 174},
    {text079, 198, 176},
    {text080, 199, 178},
    {text081, 200, 180},
    {text082, 201, 182},
    {text066, 202, 184}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {153, 152, 186},
    {155, 154, 187},
    {157, 156, 188},
    {159, 158, 189},
    {161, 160, 190},
    {163, 162, 191},
    {165, 164, 192},
    {167, 166, 193},
    {169, 168, 194},
    {171, 170, 195},
    {173, 172, 196},
    {175, 174, 197},
    {177, 176, 198},
    {179, 178, 199},
    {181, 180, 200},
    {183, 182, 201},
    {185, 184, 202}
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
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16
};
constexpr std::int32_t mute0Dependents[]{
    122,
    125
};
constexpr std::int32_t mute1Dependents[]{
    123,
    126
};
constexpr std::int32_t mute2Dependents[]{
    124,
    127
};
constexpr std::int32_t mute3Dependents[]{
    140,
    146,
    129
};
constexpr std::int32_t mute4Dependents[]{
    141,
    147,
    131
};
constexpr std::int32_t mute5Dependents[]{
    142,
    148,
    133
};
constexpr std::int32_t mute6Dependents[]{
    143,
    149,
    135
};
constexpr std::int32_t mute7Dependents[]{
    144,
    150,
    137
};
constexpr std::int32_t mute8Dependents[]{
    145,
    151,
    139
};
constexpr std::int32_t mute9Dependents[]{
    186,
    203,
    153
};
constexpr std::int32_t mute10Dependents[]{
    187,
    204,
    155
};
constexpr std::int32_t mute11Dependents[]{
    188,
    205,
    157
};
constexpr std::int32_t mute12Dependents[]{
    189,
    206,
    159
};
constexpr std::int32_t mute13Dependents[]{
    190,
    207,
    161
};
constexpr std::int32_t mute14Dependents[]{
    191,
    208,
    163
};
constexpr std::int32_t mute15Dependents[]{
    192,
    209,
    165
};
constexpr std::int32_t mute16Dependents[]{
    193,
    210,
    167
};
constexpr std::int32_t mute17Dependents[]{
    194,
    211,
    169
};
constexpr std::int32_t mute18Dependents[]{
    195,
    212,
    171
};
constexpr std::int32_t mute19Dependents[]{
    196,
    213,
    173
};
constexpr std::int32_t mute20Dependents[]{
    197,
    214,
    175
};
constexpr std::int32_t mute21Dependents[]{
    198,
    215,
    177
};
constexpr std::int32_t mute22Dependents[]{
    199,
    216,
    179
};
constexpr std::int32_t mute23Dependents[]{
    200,
    217,
    181
};
constexpr std::int32_t mute24Dependents[]{
    201,
    218,
    183
};
constexpr std::int32_t mute25Dependents[]{
    202,
    219,
    185
};
constexpr SettingPage03MuteRow muteRows[]{
    {118, mute0Dependents},
    {119, mute1Dependents},
    {120, mute2Dependents},
    {128, mute3Dependents},
    {130, mute4Dependents},
    {132, mute5Dependents},
    {134, mute6Dependents},
    {136, mute7Dependents},
    {138, mute8Dependents},
    {152, mute9Dependents},
    {154, mute10Dependents},
    {156, mute11Dependents},
    {158, mute12Dependents},
    {160, mute13Dependents},
    {162, mute14Dependents},
    {164, mute15Dependents},
    {166, mute16Dependents},
    {168, mute17Dependents},
    {170, mute18Dependents},
    {172, mute19Dependents},
    {174, mute20Dependents},
    {176, mute21Dependents},
    {178, mute22Dependents},
    {180, mute23Dependents},
    {182, mute24Dependents},
    {184, mute25Dependents}
};
constexpr char text083[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text084[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text085[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text086[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point042{333, 627};
constexpr char text087[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text088[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text089[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point043{424, 627};
// 原描述符 VA：0x563328, 0x563354
constexpr GuiButtonDescriptorNative2Ch loopButtons[]{
    {{221, {nullptr, text084, nullptr, text085, text085, text086, nullptr}, &point042}, 7, -1},
    {{222, {nullptr, text087, nullptr, text088, text088, text089, nullptr}, &point043}, 7, -1}
};
constexpr char text090[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5611BC
constexpr GuiStaticDescriptorNative24h loopLabel[]{
    {223, {nullptr, text090, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr std::array<int, 4> pagerRows[]{
    {186, 203, 153, 152},
    {187, 204, 155, 154},
    {188, 205, 157, 156},
    {189, 206, 159, 158},
    {190, 207, 161, 160},
    {191, 208, 163, 162},
    {192, 209, 165, 164},
    {193, 210, 167, 166},
    {194, 211, 169, 168},
    {195, 212, 171, 170},
    {196, 213, 173, 172},
    {197, 214, 175, 174},
    {198, 215, 177, 176},
    {199, 216, 179, 178},
    {200, 217, 181, 180},
    {201, 218, 183, 182},
    {202, 219, 185, 184}
};
constexpr char text091[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561128
constexpr GuiStaticDescriptorNative24h footer[]{
    {29, {text091, nullptr, nullptr, text091, text091, nullptr, nullptr}, nullptr}
};
constexpr unsigned blockOrder[]{
    0,
    1,
    4,
    2,
    3
};
constexpr SettingPage03ResourceProfile resources{
    blockOrder,
    {block0Groups, block1Groups, block2Groups, {}, block4Groups},
    masters,
    effects,
    characters,
    labels,
    effectsPreviews,
    charactersPreviews,
    effectsParamsIndices,
    charactersParamsIndices,
    muteRows,
    std::string_view{text083, 10},
    false,
    {SettingSoundLoopKind::Radio, -1, 220, 221, 222, loopButtons, &loopLabel[0]},
    {true, 9, {224, 225}, {226, 227}, pagerRows},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
