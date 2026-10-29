#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::HakoniwaSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/17.json；原父构造 VA：0x4D7490。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5A82F8, 0x5A831C, 0x5A8340
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {125, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {126, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {127, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5A8368, 0x5A83D4, 0x5A8440
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
// 原描述符 VA：0x5A84C8, 0x5A8510, 0x5A8558
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
// 原描述符 VA：0x5A85A0, 0x5A85C4, 0x5A85E8, 0x5A860C, 0x5A8630, 0x5A8654
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {146, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {147, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {148, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {149, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {150, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {151, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x5A8678, 0x5A86E4, 0x5A8750, 0x5A87BC, 0x5A8828, 0x5A8894
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
// 原描述符 VA：0x5A8930, 0x5A8978, 0x5A89C0, 0x5A8A08, 0x5A8A50, 0x5A8A98
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
// 原描述符 VA：0x5A8B10, 0x5A8B3C, 0x5A8B68, 0x5A8B94, 0x5A8BC0, 0x5A8BEC
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
// 原描述符 VA：0x5A8DA0, 0x5A8E0C, 0x5A8E78, 0x5A8EE4, 0x5A8F50, 0x5A8FBC, 0x5A9028, 0x5A9094
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {186, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {187, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {188, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {189, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}},
    {190, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 607, 845, 626}, {0, 0, -12, 0}},
    {191, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 235, 1186, 254}, {0, 0, -12, 0}},
    {192, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 328, 1186, 347}, {0, 0, -12, 0}},
    {193, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {1058, 421, 1186, 440}, {0, 0, -12, 0}}
};
constexpr POINT point015{844, 197};
constexpr POINT point016{844, 290};
constexpr POINT point017{844, 383};
constexpr POINT point018{844, 476};
constexpr POINT point019{844, 569};
constexpr POINT point020{1185, 197};
constexpr POINT point021{1185, 290};
constexpr POINT point022{1185, 383};
// 原描述符 VA：0x5A9140, 0x5A9188, 0x5A91D0, 0x5A9218, 0x5A9260, 0x5A92A8, 0x5A92F0, 0x5A9338
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {152, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point015, 7, -1},
    {154, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point016, 7, -1},
    {156, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point017, 7, -1},
    {158, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1},
    {160, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point019, 7, -1},
    {162, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point020, 7, -1},
    {164, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point021, 7, -1},
    {166, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point022, 7, -1}
};
constexpr POINT point023{795, 197};
constexpr POINT point024{795, 290};
constexpr POINT point025{795, 383};
constexpr POINT point026{795, 476};
constexpr POINT point027{795, 569};
constexpr POINT point028{1136, 197};
constexpr POINT point029{1136, 290};
constexpr POINT point030{1136, 383};
// 原描述符 VA：0x5A93C0, 0x5A93EC, 0x5A9418, 0x5A9444, 0x5A9470, 0x5A949C, 0x5A94C8, 0x5A94F4
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{153, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point023}, 7, -1},
    {{155, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point024}, 7, -1},
    {{157, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point025}, 7, -1},
    {{159, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point026}, 7, -1},
    {{161, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point027}, 7, -1},
    {{163, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point028}, 7, -1},
    {{165, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point029}, 7, -1},
    {{167, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point030}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 8, block2Group0},
    {SettingPage03ControlKind::CheckBox, 8, block2Group1},
    {SettingPage03ControlKind::Button, 8, block2Group2}
};
constexpr char text019[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x31\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5A6668, 0x5A668C
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
// 原描述符 VA：0x5A66B0, 0x5A66DC
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
constexpr POINT point031{565, 191};
// 原描述符 VA：0x5A8C58
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {203, {text027, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, &point031}
};
constexpr char text029[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr POINT point032{565, 284};
// 原描述符 VA：0x5A8C7C
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {204, {text029, text030, nullptr, nullptr, nullptr, nullptr, nullptr}, &point032}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr POINT point033{565, 377};
// 原描述符 VA：0x5A8CA0
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {205, {text031, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, &point033}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x34\x2E\x76\x72\x6D"};
constexpr POINT point034{565, 470};
// 原描述符 VA：0x5A8CC4
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {206, {text033, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, &point034}
};
constexpr char text035[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr char text036[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x35\x2E\x76\x72\x6D"};
constexpr POINT point035{565, 563};
// 原描述符 VA：0x5A8CE8
constexpr GuiStaticDescriptorNative24h label4Shown[]{
    {207, {text035, text036, nullptr, nullptr, nullptr, nullptr, nullptr}, &point035}
};
constexpr char text037[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr char text038[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x2E\x76\x72\x6D"};
constexpr POINT point036{906, 191};
// 原描述符 VA：0x5A8D0C
constexpr GuiStaticDescriptorNative24h label5Shown[]{
    {208, {text037, text038, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
};
constexpr char text039[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x89\xF0\x95\xFA\x91\x4F\x2E\x76\x72\x6D"};
constexpr char text040[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x36\x89\xF0\x95\xFA\x91\x4F\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5A8D78
constexpr GuiStaticDescriptorNative24h label5Hidden[]{
    {208, {text039, text040, nullptr, nullptr, nullptr, nullptr, nullptr}, &point036}
};
constexpr char text041[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr char text042[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x37\x2E\x76\x72\x6D"};
constexpr POINT point037{906, 284};
// 原描述符 VA：0x5A8D30
constexpr GuiStaticDescriptorNative24h label6Shown[]{
    {209, {text041, text042, nullptr, nullptr, nullptr, nullptr, nullptr}, &point037}
};
constexpr char text043[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr char text044[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr POINT point038{906, 377};
// 原描述符 VA：0x5A8D54
constexpr GuiStaticDescriptorNative24h label7Shown[]{
    {210, {text043, text044, nullptr, nullptr, nullptr, nullptr, nullptr}, &point038}
};
constexpr SettingSoundLabel labels[]{
    {{&label0Shown[0], nullptr}, 0},
    {{&label1Shown[0], nullptr}, 1},
    {{&label2Shown[0], nullptr}, 2},
    {{&label3Shown[0], nullptr}, 3},
    {{&label4Shown[0], nullptr}, 4},
    {{&label5Shown[0], &label5Hidden[0]}, 5},
    {{&label6Shown[0], nullptr}, 6},
    {{&label7Shown[0], nullptr}, 7}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 122, 118},
    {SettingSoundMaster::Effects, 124, 120},
    {SettingSoundMaster::Voice, 123, 119}
};
constexpr char text045[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text046[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text047[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text048[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text049[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text050[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text045, 140, 128},
    {text046, 141, 130},
    {text047, 142, 132},
    {text048, 143, 134},
    {text049, 144, 136},
    {text050, 145, 138}
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
constexpr char text051[]{"\x8F\xAC\x89\x48\x8B\xE8"};
constexpr char text052[]{"\x89\x69\x8B\x76"};
constexpr char text053[]{"\x95\xC9\x8D\xF7"};
constexpr char text054[]{"\x8E\xE9\x89\xC4"};
constexpr char text055[]{"\x8E\xB5\x88\xE4"};
constexpr char text056[]{"\x8F\xBB\x8A\x47"};
constexpr char text057[]{"\x8C\x59\x93\xF1"};
constexpr SettingPage03NamedRow characters[]{
    {text051, 186, 152},
    {text052, 187, 154},
    {text053, 188, 156},
    {text054, 189, 158},
    {text055, 190, 160},
    {text056, 191, 162},
    {text057, 192, 164},
    {text050, 193, 166}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {153, 152, 186},
    {155, 154, 187},
    {157, 156, 188},
    {159, 158, 189},
    {161, 160, 190},
    {163, 162, 191},
    {165, 164, 192},
    {167, 166, 193}
};
constexpr int charactersParamsIndices[]{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7
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
    {166, mute16Dependents}
};
constexpr char text058[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text059[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text060[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text061[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point039{333, 627};
constexpr char text062[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text063[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text064[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point040{424, 627};
// 原描述符 VA：0x5AB7F8, 0x5AB824
constexpr GuiButtonDescriptorNative2Ch loopButtons[]{
    {{221, {nullptr, text059, nullptr, text060, text060, text061, nullptr}, &point039}, 7, -1},
    {{222, {nullptr, text062, nullptr, text063, text063, text064, nullptr}, &point040}, 7, -1}
};
constexpr char text065[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5AB684
constexpr GuiStaticDescriptorNative24h loopLabel[]{
    {223, {nullptr, text065, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr std::array<int, 4> pagerRows[]{
    {186, 203, 153, 152},
    {187, 204, 155, 154},
    {188, 205, 157, 156},
    {189, 206, 159, 158},
    {190, 207, 161, 160},
    {191, 208, 163, 162},
    {192, 209, 165, 164},
    {193, 210, 167, 166}
};
constexpr char text066[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x5A7DBC
constexpr GuiStaticDescriptorNative24h footer[]{
    {29, {text066, nullptr, nullptr, text066, text066, nullptr, nullptr}, nullptr}
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
    std::string_view{text058, 10},
    false,
    {SettingSoundLoopKind::Radio, -1, 220, 221, 222, loopButtons, &loopLabel[0]},
    {false, 9, {224, 225}, {226, 227}, pagerRows},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
