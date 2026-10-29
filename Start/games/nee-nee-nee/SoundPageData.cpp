#include "SoundPageData.h"
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>

namespace KaGuYa::Start::Runtime::NeeNeeNeeSoundPageData
{
namespace
{
// 由 sound-page-profile-integration/tools/generate.py 生成；数据维护请修改生成器。
// 审计：sound-page-profile-audit/profiles/12.json；原父构造 VA：0x4FAB30。
// 保留 CP932 原字节、图片空槽、显式/空 POINT 和全部描述符尾字段。
constexpr char text000[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x62\x67\x6D\x2E\x76\x72\x6D"};
constexpr char text001[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x2E\x76\x72\x6D"};
constexpr char text002[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x73\x65\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561BD8, 0x561BFC, 0x561C20
constexpr GuiStaticDescriptorNative24h block0Group0[]{
    {125, {nullptr, text000, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {126, {nullptr, text001, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {127, {nullptr, text002, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr char text003[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93\x2E\x76\x72\x6D"};
constexpr char text004[]{"\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561C68, 0x561CD4, 0x561D40
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
// 原描述符 VA：0x561DE8, 0x561E30, 0x561E78
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
// 原描述符 VA：0x561F08, 0x561F2C, 0x561F50, 0x561F74, 0x561F98, 0x561FBC
constexpr GuiStaticDescriptorNative24h block1Group0[]{
    {146, {nullptr, text009, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {147, {nullptr, text010, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {148, {nullptr, text011, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {149, {nullptr, text012, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {150, {nullptr, text013, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr},
    {151, {nullptr, text014, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
// 原描述符 VA：0x561FE0, 0x56204C, 0x5620B8, 0x562124, 0x562190, 0x5621FC
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
// 原描述符 VA：0x562298, 0x5622E0, 0x562328, 0x562370, 0x5623B8, 0x562400
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
// 原描述符 VA：0x562478, 0x5624A4, 0x5624D0, 0x5624FC, 0x562528, 0x562554
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
// 原描述符 VA：0x562650, 0x5626BC, 0x562728, 0x562794
constexpr GuiSliderDescriptorNative6Ch block2Group0[]{
    {178, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 235, 845, 254}, {0, 0, -12, 0}},
    {179, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 328, 845, 347}, {0, 0, -12, 0}},
    {180, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 421, 845, 440}, {0, 0, -12, 0}},
    {181, {text003, text004, nullptr, text003, text003, text003, nullptr}, {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr, 0, 0, 0, {717, 514, 845, 533}, {0, 0, -12, 0}}
};
constexpr POINT point015{844, 197};
constexpr POINT point016{844, 290};
constexpr POINT point017{844, 383};
constexpr POINT point018{844, 476};
// 原描述符 VA：0x562820, 0x562868, 0x5628B0, 0x5628F8
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h block2Group1[]{
    {152, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point015, 7, -1},
    {154, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point016, 7, -1},
    {156, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point017, 7, -1},
    {158, {nullptr, text005, nullptr, text006, text006, text007, nullptr}, {text008, text005, nullptr, text006, text006, text007, nullptr}, &point018, 7, -1}
};
constexpr POINT point019{795, 197};
constexpr POINT point020{795, 290};
constexpr POINT point021{795, 383};
constexpr POINT point022{795, 476};
// 原描述符 VA：0x562960, 0x56298C, 0x5629B8, 0x5629E4
constexpr GuiButtonDescriptorNative2Ch block2Group2[]{
    {{153, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point019}, 7, -1},
    {{155, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point020}, 7, -1},
    {{157, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point021}, 7, -1},
    {{159, {text015, text016, nullptr, text017, text017, text018, nullptr}, &point022}, 7, -1}
};
constexpr SettingPage03ControlGroup block2Groups[]{
    {SettingPage03ControlKind::Slider, 4, block2Group0},
    {SettingPage03ControlKind::CheckBox, 4, block2Group1},
    {SettingPage03ControlKind::Button, 4, block2Group2}
};
constexpr char text019[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x31\x2E\x76\x72\x6D"};
constexpr char text020[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8\x32\x2E\x76\x72\x6D"};
// 原描述符 VA：0x562CA8, 0x562CCC
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
// 原描述符 VA：0x562CF0, 0x562D1C
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
constexpr POINT point023{565, 191};
// 原描述符 VA：0x5625A0
constexpr GuiStaticDescriptorNative24h label0Shown[]{
    {191, {text027, text028, nullptr, nullptr, nullptr, nullptr, nullptr}, &point023}
};
constexpr char text029[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr char text030[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x32\x2E\x76\x72\x6D"};
constexpr POINT point024{565, 284};
// 原描述符 VA：0x5625C4
constexpr GuiStaticDescriptorNative24h label1Shown[]{
    {192, {text029, text030, nullptr, nullptr, nullptr, nullptr, nullptr}, &point024}
};
constexpr char text031[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr char text032[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA\x33\x2E\x76\x72\x6D"};
constexpr POINT point025{565, 377};
// 原描述符 VA：0x5625E8
constexpr GuiStaticDescriptorNative24h label2Shown[]{
    {193, {text031, text032, nullptr, nullptr, nullptr, nullptr, nullptr}, &point025}
};
constexpr char text033[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr char text034[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x82\xBB\x82\xCC\x91\xBC\x2E\x76\x72\x6D"};
constexpr POINT point026{565, 470};
// 原描述符 VA：0x56260C
constexpr GuiStaticDescriptorNative24h label3Shown[]{
    {194, {text033, text034, nullptr, nullptr, nullptr, nullptr, nullptr}, &point026}
};
constexpr SettingSoundLabel labels[]{
    {{&label0Shown[0], nullptr}, 0},
    {{&label1Shown[0], nullptr}, 1},
    {{&label2Shown[0], nullptr}, 2},
    {{&label3Shown[0], nullptr}, 3}
};
constexpr SettingSoundMasterRow masters[]{
    {SettingSoundMaster::Music, 122, 118},
    {SettingSoundMaster::Effects, 124, 120},
    {SettingSoundMaster::Voice, 123, 119}
};
constexpr char text035[]{"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93"};
constexpr char text036[]{"\x8E\xE8\x83\x52\x83\x4C\x2F\x91\xAB\x83\x52\x83\x4C"};
constexpr char text037[]{"\x8E\x77\x83\x7D\x83\x93\x2F\x83\x4E\x83\x93\x83\x6A"};
constexpr char text038[]{"\x92\xAA\x90\x81\x82\xAB"};
constexpr char text039[]{"\x8E\xCB\x90\xB8\x89\xB9"};
constexpr char text040[]{"\x82\xBB\x82\xCC\x91\xBC"};
constexpr SettingPage03NamedRow effects[]{
    {text035, 140, 128},
    {text036, 141, 130},
    {text037, 142, 132},
    {text038, 143, 134},
    {text039, 144, 136},
    {text040, 145, 138}
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
constexpr char text041[]{"\x89\xC0\x89\xBE"};
constexpr char text042[]{"\x88\xC9\x8B\x66"};
constexpr char text043[]{"\x97\x9E\x93\x95"};
constexpr SettingPage03NamedRow characters[]{
    {text041, 178, 152},
    {text042, 179, 154},
    {text043, 180, 156},
    {text040, 181, 158}
};
constexpr SettingPage03PreviewRow charactersPreviews[]{
    {153, 152, 178},
    {155, 154, 179},
    {157, 156, 180},
    {159, 158, 181}
};
constexpr int charactersParamsIndices[]{
    0,
    1,
    2,
    3
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
    {158, mute12Dependents}
};
constexpr char text044[]{"\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"};
constexpr char text045[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text046[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text047[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB7\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point027{333, 627};
constexpr char text048[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x49\x83\x74\x2E\x76\x72\x6D"};
constexpr char text049[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D"};
constexpr char text050[]{"\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93\x82\xB5\x82\xC8\x82\xA2\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
constexpr POINT point028{424, 627};
// 原描述符 VA：0x562C50, 0x562C7C
constexpr GuiButtonDescriptorNative2Ch loopButtons[]{
    {{205, {nullptr, text045, nullptr, text046, text046, text047, nullptr}, &point027}, 7, -1},
    {{206, {nullptr, text048, nullptr, text049, text049, text050, nullptr}, &point028}, 7, -1}
};
constexpr char text051[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561C44
constexpr GuiStaticDescriptorNative24h loopLabel[]{
    {207, {nullptr, text051, nullptr, nullptr, nullptr, nullptr, nullptr}, nullptr}
};
constexpr std::array<int, 4> pagerRows[]{
    {178, 191, 153, 152},
    {179, 192, 155, 154},
    {180, 193, 157, 156},
    {181, 194, 159, 158}
};
constexpr char text052[]{"\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA\x2E\x76\x72\x6D"};
// 原描述符 VA：0x561BB0
constexpr GuiStaticDescriptorNative24h footer[]{
    {29, {text052, nullptr, nullptr, text052, text052, nullptr, nullptr}, nullptr}
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
    std::string_view{text044, 10},
    false,
    {SettingSoundLoopKind::Radio, -1, 220, 205, 206, loopButtons, &loopLabel[0]},
    {false, 9, {208, 209}, {210, 211}, pagerRows},
    &footer[0]
};
}
const SettingPage03ResourceProfile& Profile() noexcept { return resources; }
}
