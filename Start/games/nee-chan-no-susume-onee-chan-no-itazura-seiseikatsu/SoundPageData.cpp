#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::NeeChanNoSusumeOneeChanNoItazuraSeiseikatsuSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/09.json；原父构造 VA：0x4FB310。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55CEF0, 0x55CF14, 0x55CF38
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {122, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {123, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {124, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55CF98, 0x55D004, 0x55D070
constexpr GuiSliderDescriptorNative6Ch block0Group1[]{
    {119, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 209, 401, 228}, {0, 0, -12, 0}},
    {120, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 252, 401, 271}, {0, 0, -12, 0}},
    {121, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 297, 401, 316}, {0, 0, -12, 0}}
};
constexpr char text005[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text006[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text007[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr char text008[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr POINT point000{489, 202};
constexpr POINT point001{489, 246};
constexpr POINT point002{489, 290};
// 原描述符 VA：0x55D130, 0x55D178, 0x55D1C0
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block0Group2[]{
    {115, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point000, 7, -1},
    {116, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point001, 7, -1},
    {117, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point002, 7, -1}
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
// 原描述符 VA：0x55D250, 0x55D274, 0x55D298, 0x55D2BC, 0x55D2E0, 0x55D304
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {143, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {144, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {145, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {146, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {147, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {148, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x55D328, 0x55D394, 0x55D400, 0x55D46C, 0x55D4D8, 0x55D544
constexpr GuiSliderDescriptorNative6Ch block1Group1[]{
    {137, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 354, 401, 373}, {0, 0, -12, 0}},
    {138, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 398, 401, 417}, {0, 0, -12, 0}},
    {139, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 442, 401, 461}, {0, 0, -12, 0}},
    {140, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 486, 401, 505}, {0, 0, -12, 0}},
    {141, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 530, 401, 549}, {0, 0, -12, 0}},
    {142, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {279, 574, 401, 593}, {0, 0, -12, 0}}
};
constexpr POINT point003{489, 347};
constexpr POINT point004{489, 391};
constexpr POINT point005{489, 435};
constexpr POINT point006{489, 479};
constexpr POINT point007{489, 523};
constexpr POINT point008{489, 567};
// 原描述符 VA：0x55D5E0, 0x55D628, 0x55D670, 0x55D6B8, 0x55D700, 0x55D748
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block1Group2[]{
    {125, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point003, 7, -1},
    {127, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point004, 7, -1},
    {129, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point005, 7, -1},
    {131, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point006, 7, -1},
    {133, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point007, 7, -1},
    {135, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point008, 7, -1}
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
// 原描述符 VA：0x55D7C0, 0x55D7EC, 0x55D818, 0x55D844, 0x55D870, 0x55D89C
constexpr GuiButtonDescriptorNative2Ch block1Group3[]{
    {{126, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point009}, 7, -1},
    {{128, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point010}, 7, -1},
    {{130, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point011}, 7, -1},
    {{132, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point012}, 7, -1},
    {{134, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point013}, 7, -1},
    {{136, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point014}, 7, -1}
};
constexpr SettingPage03ControlGroup block1Groups[]{
    {SettingPage03ControlKind::Static, 6, block1Group0},
    {SettingPage03ControlKind::Slider, 6, block1Group1},
    {SettingPage03ControlKind::CheckBox, 6, block1Group2},
    {SettingPage03ControlKind::Button, 6, block1Group3}
};
// 原描述符 VA：0x55D9F0, 0x55DA5C, 0x55DAC8, 0x55DB34, 0x55DBA0
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {167, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {168, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {169, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {170, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}},
    {171, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 607, 845, 626}, {0, 0, -12, 0}}
};
constexpr POINT point015{844, 197};
constexpr POINT point016{844, 290};
constexpr POINT point017{844, 383};
constexpr POINT point018{844, 476};
constexpr POINT point019{844, 569};
// 原描述符 VA：0x55DC58, 0x55DCA0, 0x55DCE8, 0x55DD30, 0x55DD78
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {149, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point015, 7, -1},
    {151, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point016, 7, -1},
    {153, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point017, 7, -1},
    {155, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1},
    {157, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point019, 7, -1}
};
constexpr POINT point020{795, 197};
constexpr POINT point021{795, 290};
constexpr POINT point022{795, 383};
constexpr POINT point023{795, 476};
constexpr POINT point024{795, 569};
// 原描述符 VA：0x55DDE8, 0x55DE14, 0x55DE40, 0x55DE6C, 0x55DE98
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{150, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point020}, 7, -1},
    {{152, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point021}, 7, -1},
    {{154, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point022}, 7, -1},
    {{156, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point023}, 7, -1},
    {{158, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point024}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 5, block2Group0},
    {SettingPage03ControlKind::CheckBox, 5, block2Group1},
    {SettingPage03ControlKind::Button, 5, block2Group2}
};
constexpr char text019[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x31\x2E\x76\x72\x6D"};
constexpr POINT point025{565, 191};
// 原描述符 VA：0x55D8F0
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {176, {text019, text020, nullptr, nullptr, nullptr, nullptr, nullptr}, &point025}
};
constexpr char text021[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr char text022[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr POINT point026{565, 284};
// 原描述符 VA：0x55D914
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {177, {text021, text022, nullptr, nullptr, nullptr, nullptr, nullptr}, &point026}
};
constexpr char text023[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr char text024[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr POINT point027{565, 377};
// 原描述符 VA：0x55D938
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {178, {text023, text024, nullptr, nullptr, nullptr, nullptr, nullptr}, &point027}
};
constexpr char text025[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr char text026[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr POINT point028{565, 470};
// 原描述符 VA：0x55D95C
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {179, {text025, text026, nullptr, nullptr, nullptr, nullptr, nullptr}, &point028}
};
constexpr char text027[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr char text028[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr POINT point029{565, 563};
// 原描述符 VA：0x55D980
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {180, {text027, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, &point029}
};
constexpr SettingSoundLabel labels[]{
    {{&label0Shown[0], nullptr}, 0},
    {{&label1Shown[0], nullptr}, 1},
    {{&label2Shown[0], nullptr}, 2},
    {{&label3Shown[0], nullptr}, 3},
    {{&label4Shown[0], nullptr}, 4}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 119, 115},
    {SettingSoundMaster::Effects, 121, 117},
    {SettingSoundMaster::Voice, 120, 116}
};
constexpr char text029[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text030[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text031[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text032[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text033[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text034[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text029, 137, 125},
    {text030, 138, 127},
    {text031, 139, 129},
    {text032, 140, 131},
    {text033, 141, 133},
    {text034, 142, 135}
};
constexpr SettingPage03PreviewRow effectsPreviews[]{
    {126, 125, 137},
    {128, 127, 138},
    {130, 129, 139},
    {132, 131, 140},
    {134, 133, 141},
    {136, 135, 142}
};
constexpr int effectsParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5
};
constexpr char text035[]{"\x97\x95\x88\xDF"};
constexpr char text036[]{"\x90\xC2\x97\x74"};
constexpr char text037[]{"\x8E\x8D\x8E\x71"};
constexpr char text038[]{"\x8E\x9E\x89\x4A"};
constexpr SettingPage03NamedRow characters[]{
    {text035, 167, 149},
    {text036, 168, 151},
    {text037, 169, 153},
    {text038, 170, 155},
    {text034, 171, 157}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {150, 149, 167},
    {152, 151, 168},
    {154, 153, 169},
    {156, 155, 170},
    {158, 157, 171}
};
constexpr int charactersParamsIndices[]{
    0,
    1,
    2,
    3,
    4
};
constexpr std::int32_t mute0Dependents[]{
    119,
    122
};
constexpr std::int32_t mute1Dependents[]{
    120,
    123
};
constexpr std::int32_t mute2Dependents[]{
    121,
    124
};
constexpr std::int32_t mute3Dependents[]{
    137,
    143,
    126
};
constexpr std::int32_t mute4Dependents[]{
    138,
    144,
    128
};
constexpr std::int32_t mute5Dependents[]{
    139,
    145,
    130
};
constexpr std::int32_t mute6Dependents[]{
    140,
    146,
    132
};
constexpr std::int32_t mute7Dependents[]{
    141,
    147,
    134
};
constexpr std::int32_t mute8Dependents[]{
    142,
    148,
    136
};
constexpr std::int32_t mute9Dependents[]{
    167,
    176,
    150
};
constexpr std::int32_t mute10Dependents[]{
    168,
    177,
    152
};
constexpr std::int32_t mute11Dependents[]{
    169,
    178,
    154
};
constexpr std::int32_t mute12Dependents[]{
    170,
    179,
    156
};
constexpr std::int32_t mute13Dependents[]{
    171,
    180,
    158
};
constexpr SettingPage03MuteRow muteRows[]{
    {115, mute0Dependents},
    {116, mute1Dependents},
    {117, mute2Dependents},
    {125, mute3Dependents},
    {127, mute4Dependents},
    {129, mute5Dependents},
    {131, mute6Dependents},
    {133, mute7Dependents},
    {135, mute8Dependents},
    {149, mute9Dependents},
    {151, mute10Dependents},
    {153, mute11Dependents},
    {155, mute12Dependents},
    {157, mute13Dependents}
};
constexpr char text039[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text040[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text041[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text042[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point030{333, 627};
constexpr char text043[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text044[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text045[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point031{424, 627};
// 原描述符 VA：0x55E070, 0x55E09C
constexpr GuiButtonDescriptorNative2Ch loopButtons[]{
    {{186, {nullptr, text040, nullptr, text041, text041, text042, nullptr}, &point030}, 7, -1},
    {{187, {nullptr, text043, nullptr, text044, text044, text045, nullptr}, &point031}, 7, -1}
};
constexpr char text046[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55D9CC
constexpr GuiStaticDescriptorNative24h loopLabel[]{
    {188, {nullptr, text046, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text047[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x55CD2C
constexpr GuiStaticDescriptorNative24h footer[]{
    {27, {text047, nullptr, nullptr, text047, text047, nullptr, nullptr}, nullptr}
};
constexpr unsigned blockOrder[]{
    0,
    1,
    2,
    3
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
    std::string_view{text039, 10},
    false,
    {SettingSoundLoopKind::Radio, -1, 220, 186, 187, loopButtons, &loopLabel[0]},
    {false, 9, {-1, -1}, {-1, -1}, {}},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
