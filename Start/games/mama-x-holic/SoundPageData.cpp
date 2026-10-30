#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::MamaXHolicMiwakuNoMamaToAmaAmaKankeiSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/11.json；原父构造 VA：0x4FF820。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566350, 0x566374, 0x566398
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {125, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {126, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {127, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5663E0, 0x56644C, 0x5664B8
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
// 原描述符 VA：0x566560, 0x5665A8, 0x5665F0
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
// 原描述符 VA：0x566680, 0x5666A4, 0x5666C8, 0x5666EC, 0x566710, 0x566734
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {146, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {147, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {148, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {149, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {150, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {151, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x566758, 0x5667C4, 0x566830, 0x56689C, 0x566908, 0x566974
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
// 原描述符 VA：0x566A10, 0x566A58, 0x566AA0, 0x566AE8, 0x566B30, 0x566B78
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
// 原描述符 VA：0x566BF0, 0x566C1C, 0x566C48, 0x566C74, 0x566CA0, 0x566CCC
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
// 原描述符 VA：0x566F80, 0x566FEC, 0x567058, 0x5670C4, 0x567130, 0x56719C, 0x567208, 0x567274, 0x5672E0, 0x56734C, 0x5673B8, 0x567424, 0x567490
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {178, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {179, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {180, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {181, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}},
    {182, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 607, 845, 626}, {0, 0, -12, 0}},
    {183, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 235, 1186, 254}, {0, 0, -12, 0}},
    {184, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 328, 1186, 347}, {0, 0, -12, 0}},
    {185, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 421, 1186, 440}, {0, 0, -12, 0}},
    {186, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 514, 1186, 533}, {0, 0, -12, 0}},
    {187, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {188, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {189, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {190, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}}
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
// 原描述符 VA：0x567568, 0x5675B0, 0x5675F8, 0x567640, 0x567688, 0x5676D0, 0x567718, 0x567760, 0x5677A8, 0x5677F0, 0x567838, 0x567880, 0x5678C8
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
    {176, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1}
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
// 原描述符 VA：0x567958, 0x567984, 0x5679B0, 0x5679DC, 0x567A08, 0x567A34, 0x567A60, 0x567A8C, 0x567AB8, 0x567AE4, 0x567B10, 0x567B3C, 0x567B68
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
    {{177, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point027}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 13, block2Group0},
    {SettingPage03ControlKind::CheckBox, 13, block2Group1},
    {SettingPage03ControlKind::Button, 13, block2Group2}
};
constexpr char text019[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x31\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x568050, 0x568074
constexpr GuiStaticDescriptorNative24h block4Group0[]{
    {208, {text019, nullptr, nullptr, text019, text019, nullptr, nullptr}, nullptr},
    {209, {text020, nullptr, nullptr, text020, text020, nullptr, nullptr}, nullptr}
};
constexpr char text021[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x6F\x66\x66\x2E\x76\x72\x6D"};
constexpr char text022[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D"};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x8C\xE3\x82\xD6\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x6F\x66\x66\x2E\x76\x72\x6D"};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x91\x4F\x82\xD6\x5F\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D"};
// 原描述符 VA：0x568098, 0x5680C4
constexpr GuiButtonDescriptorNative2Ch block4Group1[]{
    {{210, {text021, nullptr, nullptr, text022, text022, text023, nullptr}, nullptr}, 3, -1},
    {{211, {text024, nullptr, nullptr, text025, text025, text026, nullptr}, nullptr}, 3, -1}
};
constexpr SettingPage03ControlGroup block4Groups[]{
    {SettingPage03ControlKind::Static, 2, block4Group0},
    {SettingPage03ControlKind::Button, 2, block4Group1}
};
constexpr char text027[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr POINT point033{565, 191};
// 原描述符 VA：0x566D40
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {191, {text027, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, &point033}
};
constexpr char text029[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr POINT point034{565, 284};
// 原描述符 VA：0x566D64
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {192, {text029, text030, nullptr, nullptr, nullptr, nullptr, nullptr}, &point034}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr POINT point035{565, 377};
// 原描述符 VA：0x566D88
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {193, {text031, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, &point035}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr POINT point036{565, 470};
// 原描述符 VA：0x566DAC
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {194, {text033, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
};
constexpr char text035[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr char text036[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr POINT point037{565, 563};
// 原描述符 VA：0x566DD0
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {195, {text035, text036, nullptr, nullptr, nullptr, nullptr, nullptr}, &point037}
};
constexpr char text037[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr char text038[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr POINT point038{906, 191};
// 原描述符 VA：0x566DF4
constexpr GuiStaticDescriptorNative24h label5Shown[]{
    {196, {text037, text038, nullptr, nullptr, nullptr, nullptr, nullptr}, &point038}
};
constexpr char text039[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr char text040[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr POINT point039{906, 284};
// 原描述符 VA：0x566E18
constexpr GuiStaticDescriptorNative24h label6Shown[]{
    {197, {text039, text040, nullptr, nullptr, nullptr, nullptr, nullptr}, &point039}
};
constexpr char text041[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x38\x2E\x76\x72\x6D"};
constexpr char text042[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x38\x2E\x76\x72\x6D"};
constexpr POINT point040{906, 377};
// 原描述符 VA：0x566E3C
constexpr GuiStaticDescriptorNative24h label7Shown[]{
    {198, {text041, text042, nullptr, nullptr, nullptr, nullptr, nullptr}, &point040}
};
constexpr char text043[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x39\x2E\x76\x72\x6D"};
constexpr char text044[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x39\x2E\x76\x72\x6D"};
constexpr POINT point041{906, 470};
// 原描述符 VA：0x566E60
constexpr GuiStaticDescriptorNative24h label8Shown[]{
    {199, {text043, text044, nullptr, nullptr, nullptr, nullptr, nullptr}, &point041}
};
constexpr char text045[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x30\x2E\x76\x72\x6D"};
constexpr char text046[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x30\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566E84
constexpr GuiStaticDescriptorNative24h label9Shown[]{
    {200, {text045, text046, nullptr, nullptr, nullptr, nullptr, nullptr}, &point033}
};
constexpr char text047[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x31\x2E\x76\x72\x6D"};
constexpr char text048[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x31\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566EA8
constexpr GuiStaticDescriptorNative24h label10Shown[]{
    {201, {text047, text048, nullptr, nullptr, nullptr, nullptr, nullptr}, &point034}
};
constexpr char text049[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x32\x2E\x76\x72\x6D"};
constexpr char text050[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566ECC
constexpr GuiStaticDescriptorNative24h label11Shown[]{
    {202, {text049, text050, nullptr, nullptr, nullptr, nullptr, nullptr}, &point035}
};
constexpr char text051[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr char text052[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566EF0
constexpr GuiStaticDescriptorNative24h label12Shown[]{
    {203, {text051, text052, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
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
    {{&label12Shown[0], nullptr}, 12}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 122, 118},
    {SettingSoundMaster::Effects, 124, 120},
    {SettingSoundMaster::Voice, 123, 119}
};
constexpr char text053[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text054[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text055[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text056[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text057[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text058[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text053, 140, 128},
    {text054, 141, 130},
    {text055, 142, 132},
    {text056, 143, 134},
    {text057, 144, 136},
    {text058, 145, 138}
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
constexpr char text059[]{"\x95\x91\x93\x80"};
constexpr char text060[]{"\x96\xE9\x8D\xB3"};
constexpr char text061[]{"\x83\x54\x83\x8A\x81\x5B"};
constexpr char text062[]{"\x88\xA9"};
constexpr char text063[]{"\x83\x7E\x81\x5B\x83\x81"};
constexpr char text064[]{"\x8D\x67\x89\xB9"};
constexpr char text065[]{"\x97\xE9\x8E\xAD"};
constexpr char text066[]{"\x83\x5A\x83\x43\x83\x89"};
constexpr char text067[]{"\x83\x41\x83\x94\x83\x8A\x83\x8B"};
constexpr char text068[]{"\x8B\x6A\x8D\x5B"};
constexpr char text069[]{"\x83\x4F\x83\x89\x83\x40\x83\x6C"};
constexpr char text070[]{"\x83\x4F\x83\x8A\x83\x82"};
constexpr SettingPage03NamedRow characters[]{
    {text059, 178, 152},
    {text060, 179, 154},
    {text061, 180, 156},
    {text062, 181, 158},
    {text063, 182, 160},
    {text064, 183, 162},
    {text065, 184, 164},
    {text066, 185, 166},
    {text067, 186, 168},
    {text068, 187, 170},
    {text069, 188, 172},
    {text070, 189, 174},
    {text058, 190, 176}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {153, 152, 178},
    {155, 154, 179},
    {157, 156, 180},
    {159, 158, 181},
    {161, 160, 182},
    {163, 162, 183},
    {165, 164, 184},
    {167, 166, 185},
    {169, 168, 186},
    {171, 170, 187},
    {173, 172, 188},
    {175, 174, 189},
    {177, 176, 190}
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
    12
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
    178,
    191,
    153
};
constexpr std::int32_t mute10Dependents[]{
    179,
    192,
    155
};
constexpr std::int32_t mute11Dependents[]{
    180,
    193,
    157
};
constexpr std::int32_t mute12Dependents[]{
    181,
    194,
    159
};
constexpr std::int32_t mute13Dependents[]{
    182,
    195,
    161
};
constexpr std::int32_t mute14Dependents[]{
    183,
    196,
    163
};
constexpr std::int32_t mute15Dependents[]{
    184,
    197,
    165
};
constexpr std::int32_t mute16Dependents[]{
    185,
    198,
    167
};
constexpr std::int32_t mute17Dependents[]{
    186,
    199,
    169
};
constexpr std::int32_t mute18Dependents[]{
    187,
    200,
    171
};
constexpr std::int32_t mute19Dependents[]{
    188,
    201,
    173
};
constexpr std::int32_t mute20Dependents[]{
    189,
    202,
    175
};
constexpr std::int32_t mute21Dependents[]{
    190,
    203,
    177
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
    {176, mute21Dependents}
};
constexpr char text071[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text072[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text073[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text074[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point042{333, 627};
constexpr char text075[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text076[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text077[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point043{424, 627};
// 原描述符 VA：0x567FF8, 0x568024
constexpr GuiButtonDescriptorNative2Ch loopButtons[]{
    {{205, {nullptr, text072, nullptr, text073, text073, text074, nullptr}, &point042}, 7, -1},
    {{206, {nullptr, text075, nullptr, text076, text076, text077, nullptr}, &point043}, 7, -1}
};
constexpr char text078[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5663BC
constexpr GuiStaticDescriptorNative24h loopLabel[]{
    {207, {nullptr, text078, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr std::array<int, 4> pagerRows[]{
    {178, 191, 153, 152},
    {179, 192, 155, 154},
    {180, 193, 157, 156},
    {181, 194, 159, 158},
    {182, 195, 161, 160},
    {183, 196, 163, 162},
    {184, 197, 165, 164},
    {185, 198, 167, 166},
    {186, 199, 169, 168},
    {187, 200, 171, 170},
    {188, 201, 173, 172},
    {189, 202, 175, 174},
    {190, 203, 177, 176}
};
constexpr char text079[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x566328
constexpr GuiStaticDescriptorNative24h footer[]{
    {29, {text079, nullptr, nullptr, text079, text079, nullptr, nullptr}, nullptr}
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
    std::string_view{text071, 10},
    false,
    {SettingSoundLoopKind::Radio, -1, 220, 205, 206, loopButtons, &loopLabel[0]},
    {true, 9, {208, 209}, {210, 211}, pagerRows},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
