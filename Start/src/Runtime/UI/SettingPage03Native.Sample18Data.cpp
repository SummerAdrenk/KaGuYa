#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 指定SHA原PE只读数据。拼接仅用于相同CP932字节前缀，运行期不读PE/VA。
#define PAGE "\x83\x54\x83\x45\x83\x93\x83\x68\x89\xE6\x96\xCA"
#define MUTE "\x83\x7E\x83\x85\x81\x5B\x83\x67"
#define STATE "\x8F\xF3\x91\xD4"
#define OFF "\x83\x49\x83\x74"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define CHARVOICE "\x83\x4C\x83\x83\x83\x89\x89\xB9\x90\xBA"
constexpr char muteOff[] = PAGE MUTE OFF ".vrm"; // 5B6394
constexpr char muteTarget[] = PAGE MUTE TARGET ".vrm";
constexpr char muteClick[] = PAGE MUTE CLICK ".vrm";
constexpr char muteOn[] = PAGE MUTE "\x83\x49\x83\x93.vrm";
constexpr char playOn[] = "\x89\xB9\x90\xBA\x8D\xC4\x90\xB6\x83\x49\x83\x93.vrm";
constexpr char playOff[] = "\x89\xB9\x90\xBA\x8D\xC4\x90\xB6" OFF ".vrm";
constexpr char playTarget[] = "\x89\xB9\x90\xBA\x8D\xC4\x90\xB6" TARGET ".vrm";
constexpr char playClick[] = "\x89\xB9\x90\xBA\x8D\xC4\x90\xB6" CLICK ".vrm";
constexpr char thumbOn[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93.vrm"; // 5BCB50
constexpr char thumbOff[] = "\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B" OFF ".vrm";
constexpr std::array<const char*, 7> thumb{thumbOn, thumbOff, nullptr, thumbOn, thumbOn, thumbOn, nullptr};
constexpr GuiSliderDescriptorNative6Ch Slider(int event, RECT r)
{ return {event, thumb, {}, nullptr, 0, 0, 0, r, {0, 0, -12, 0}}; }
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h Check(int event, const POINT* point)
{ return {event, {nullptr,muteOff,nullptr,muteTarget,muteTarget,muteClick,nullptr},
    {muteOn,muteOff,nullptr,muteTarget,muteTarget,muteClick,nullptr}, point, 7, -1}; }
constexpr GuiButtonDescriptorNative2Ch Play(int event, const POINT* point)
{ return {{event, {playOn,playOff,nullptr,playTarget,playTarget,playClick,nullptr}, point}, 7, -1}; }
constexpr GuiStaticDescriptorNative24h Static(int event, const char* name)
{ return {event, {nullptr,name,nullptr,nullptr,nullptr,nullptr,nullptr}, nullptr}; }
// 607A80: static 6026B8/3, slider 602748/3, checkbox 6028C8/3。
constexpr GuiStaticDescriptorNative24h labels00[]{
    Static(125, PAGE MUTE STATE "bgm.vrm"), Static(126, PAGE MUTE STATE "\x89\xB9\x90\xBA.vrm"),
    Static(127, PAGE MUTE STATE "se.vrm")};
constexpr GuiSliderDescriptorNative6Ch sliders00[]{
    Slider(122,{279,209,401,228}), Slider(123,{279,252,401,271}), Slider(124,{279,297,401,316})};
constexpr POINT points00[]{{489,202},{489,246},{489,290}};
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h checks00[]{
    Check(118,&points00[0]),Check(119,&points00[1]),Check(120,&points00[2])};
constexpr SettingPage03ControlGroup groups00[]{
    {SettingPage03ControlKind::Static,3,labels00},{SettingPage03ControlKind::Slider,3,sliders00},
    {SettingPage03ControlKind::CheckBox,3,checks00}};
// 607AA4: 四组，6/6/6/6。normal/checked七槽、POINT、hit、-1均原位。
constexpr GuiStaticDescriptorNative24h labels01[]{
    Static(146,PAGE MUTE STATE "\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93.vrm"),
    Static(147,PAGE MUTE STATE "\x8E\xE8\x83\x52\x83\x4C\x91\xAB\x83\x52\x83\x4C.vrm"),
    Static(148,PAGE MUTE STATE "\x8E\x77\x83\x7D\x83\x93\x83\x4E\x83\x93\x83\x6A.vrm"),
    Static(149,PAGE MUTE STATE "\x92\xAA\x90\x81\x82\xAB.vrm"),
    Static(150,PAGE MUTE STATE "\x8E\xCB\x90\xB8\x89\xB9.vrm"),
    Static(151,PAGE MUTE STATE "\x82\xBB\x82\xCC\x91\xBCse.vrm")};
constexpr GuiSliderDescriptorNative6Ch sliders01[]{
    Slider(140,{279,354,401,373}),Slider(141,{279,398,401,417}),Slider(142,{279,442,401,461}),
    Slider(143,{279,486,401,505}),Slider(144,{279,530,401,549}),Slider(145,{279,574,401,593})};
constexpr POINT checks01Points[]{{489,347},{489,391},{489,435},{489,479},{489,523},{489,567}};
constexpr POINT play01Points[]{{440,347},{440,391},{440,435},{440,479},{440,523},{440,567}};
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h checks01[]{
    Check(128,&checks01Points[0]),Check(130,&checks01Points[1]),Check(132,&checks01Points[2]),
    Check(134,&checks01Points[3]),Check(136,&checks01Points[4]),Check(138,&checks01Points[5])};
constexpr GuiButtonDescriptorNative2Ch buttons01[]{
    Play(129,&play01Points[0]),Play(131,&play01Points[1]),Play(133,&play01Points[2]),
    Play(135,&play01Points[3]),Play(137,&play01Points[4]),Play(139,&play01Points[5])};
constexpr SettingPage03ControlGroup groups01[]{
    {SettingPage03ControlKind::Static,6,labels01},{SettingPage03ControlKind::Slider,6,sliders01},
    {SettingPage03ControlKind::CheckBox,6,checks01},{SettingPage03ControlKind::Button,6,buttons01}};
// 603068..6031CF: 9主descriptor + index3唯一备用6031AC；其余备用是null。
#define LABEL(number) PAGE CHARVOICE number ".vrm"
#define MUTELABEL(number) PAGE MUTE STATE CHARVOICE number ".vrm"
constexpr POINT labelPoints[]{{565,191},{565,284},{565,377},{565,470},{565,563},
    {906,191},{906,284},{906,377},{906,470}};
constexpr GuiStaticDescriptorNative24h labels02[]{
    {203,{LABEL("1"),MUTELABEL("1"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[0]},
    {204,{LABEL("2"),MUTELABEL("2"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[1]},
    {205,{LABEL("3"),MUTELABEL("3"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[2]},
    {206,{LABEL("4"),MUTELABEL("4"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[3]},
    {207,{LABEL("5"),MUTELABEL("5"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[4]},
    {208,{LABEL("6"),MUTELABEL("6"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[5]},
    {209,{LABEL("7"),MUTELABEL("7"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[6]},
    {210,{LABEL("8"),MUTELABEL("8"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[7]},
    {211,{PAGE "\x82\xBB\x82\xCC\x91\xBC.vrm",PAGE MUTE STATE "\x82\xBB\x82\xCC\x91\xBC.vrm",
        nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[8]}};
constexpr GuiStaticDescriptorNative24h hidden02{206,{LABEL("4\x89\xF0\x95\xFA\x91\x4F"),
    MUTELABEL("4\x89\xF0\x95\xFA\x91\x4F"),nullptr,nullptr,nullptr,nullptr,nullptr},&labelPoints[3]};
constexpr std::array<SettingPage03LabelPair,9> labelPairs{{
    {&labels02[0],nullptr},{&labels02[1],nullptr},{&labels02[2],nullptr},{&labels02[3],&hidden02},
    {&labels02[4],nullptr},{&labels02[5],nullptr},{&labels02[6],nullptr},{&labels02[7],nullptr},{&labels02[8],nullptr}}};
constexpr GuiSliderDescriptorNative6Ch sliders02[]{
    Slider(186,{717,235,845,254}),Slider(187,{717,328,845,347}),Slider(188,{717,421,845,440}),
    Slider(189,{717,514,845,533}),Slider(190,{717,607,845,626}),Slider(191,{1058,235,1186,254}),
    Slider(192,{1058,328,1186,347}),Slider(193,{1058,421,1186,440}),Slider(194,{1058,514,1186,533})};
constexpr POINT checks02Points[]{{844,197},{844,290},{844,383},{844,476},{844,569},
    {1185,197},{1185,290},{1185,383},{1185,476}};
constexpr POINT play02Points[]{{795,197},{795,290},{795,383},{795,476},{795,569},
    {1136,197},{1136,290},{1136,383},{1136,476}};
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h checks02[]{
    Check(152,&checks02Points[0]),Check(154,&checks02Points[1]),Check(156,&checks02Points[2]),
    Check(158,&checks02Points[3]),Check(160,&checks02Points[4]),Check(162,&checks02Points[5]),
    Check(164,&checks02Points[6]),Check(166,&checks02Points[7]),Check(168,&checks02Points[8])};
constexpr GuiButtonDescriptorNative2Ch buttons02[]{
    Play(153,&play02Points[0]),Play(155,&play02Points[1]),Play(157,&play02Points[2]),
    Play(159,&play02Points[3]),Play(161,&play02Points[4]),Play(163,&play02Points[5]),
    Play(165,&play02Points[6]),Play(167,&play02Points[7]),Play(169,&play02Points[8])};
constexpr SettingPage03ControlGroup groups02[]{
    {SettingPage03ControlKind::Slider,9,sliders02},{SettingPage03ControlKind::CheckBox,9,checks02},
    {SettingPage03ControlKind::Button,9,buttons02}};
#define SEND PAGE CHARVOICE "\x5F\x83\x79\x81\x5B\x83\x57\x91\x97\x82\xE8"
constexpr char label04a[] = SEND "1.vrm";
constexpr char label04b[] = SEND "2.vrm";
constexpr char backOff[] = SEND "\x8C\xE3\x82\xD6_off.vrm";
constexpr char backTarget[] = SEND "\x8C\xE3\x82\xD6_target.vrm";
constexpr char backClick[] = SEND "\x8C\xE3\x82\xD6_click.vrm";
constexpr char forwardOff[] = SEND "\x91\x4F\x82\xD6_off.vrm";
constexpr char forwardTarget[] = SEND "\x91\x4F\x82\xD6_target.vrm";
constexpr char forwardClick[] = SEND "\x91\x4F\x82\xD6_click.vrm";
constexpr GuiStaticDescriptorNative24h labels04[]{
    {224,{label04a,nullptr,nullptr,label04a,label04a,nullptr,nullptr},nullptr},
    {225,{label04b,nullptr,nullptr,label04b,label04b,nullptr,nullptr},nullptr}};
constexpr GuiButtonDescriptorNative2Ch buttons04[]{
    {{226,{backOff,nullptr,nullptr,backTarget,backTarget,backClick,nullptr},nullptr},3,-1},
    {{227,{forwardOff,nullptr,nullptr,forwardTarget,forwardTarget,forwardClick,nullptr},nullptr},3,-1}};
constexpr SettingPage03ControlGroup groups04[]{
    {SettingPage03ControlKind::Static,2,labels04},{SettingPage03ControlKind::Button,2,buttons04}};
#define SYSTEM "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93"
constexpr char radioYesOff[] = SYSTEM "\x82\xB7\x82\xE9" OFF ".vrm";
constexpr char radioYesTarget[] = SYSTEM "\x82\xB7\x82\xE9" TARGET ".vrm";
constexpr char radioYesClick[] = SYSTEM "\x82\xB7\x82\xE9" CLICK ".vrm";
constexpr char radioNoOff[] = SYSTEM "\x82\xB5\x82\xC8\x82\xA2" OFF ".vrm";
constexpr char radioNoTarget[] = SYSTEM "\x82\xB5\x82\xC8\x82\xA2" TARGET ".vrm";
constexpr char radioNoClick[] = SYSTEM "\x82\xB5\x82\xC8\x82\xA2" CLICK ".vrm";
constexpr POINT radioPoints[]{{333,627},{424,627}};
constexpr std::array<GuiButtonDescriptorNative2Ch,2> radioButtons{{
    {{221,{nullptr,radioYesOff,nullptr,radioYesTarget,radioYesTarget,radioYesClick,nullptr},&radioPoints[0]},7,-1},
    {{222,{nullptr,radioNoOff,nullptr,radioNoTarget,radioNoTarget,radioNoClick,nullptr},&radioPoints[1]},7,-1}}};
constexpr auto loopLabel = Static(223,PAGE MUTE STATE "\x89\xB9\x90\xBA\x82\xF0\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x8D\xC4\x90\xB6.vrm");
constexpr char pageImage[] = PAGE ".vrm";
constexpr GuiStaticDescriptorNative24h footer{29,{pageImage,nullptr,nullptr,pageImage,pageImage,nullptr,nullptr},nullptr};
constexpr char registration[] = "\x83\x7D\x83\x8B\x83\x60\x90\xDD\x92\xE8"; // 5C0CCC
constexpr std::array<SettingPage03NamedRow,6> types{{
    {"\x91\x7D\x93\xFC\x83\x73\x83\x58\x83\x67\x83\x93",140,128},
    {"\x8E\xE8\x83\x52\x83\x4C/\x91\xAB\x83\x52\x83\x4C",141,130},
    {"\x8E\x77\x83\x7D\x83\x93/\x83\x4E\x83\x93\x83\x6A",142,132},
    {"\x92\xAA\x90\x81\x82\xAB",143,134},{"\x8E\xCB\x90\xB8\x89\xB9",144,136},{"\x82\xBB\x82\xCC\x91\xBC",145,138}}};
constexpr std::array<SettingPage03NamedRow,9> characters{{
    {"\x90\x53\x8D\xD8",186,152},{"\x90\xE1\x88\xA4",187,154},{"\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67",188,156},
    {"\x89\xC4\x91\xE3\x8E\x71",189,158},{"\x89\x6C\x96\xA4",190,160},{"\x8E\x6A\x94\x54\x88\x9F",191,162},
    {"\xE4\xBB\x88\xA4",192,164},{"\x97\x52\x88\xCB\x8E\x71",193,166},{"\x82\xBB\x82\xCC\x91\xBC",194,168}}};
constexpr SettingPage03PreviewRow typePreview[]{{129,128,140},{131,130,141},{133,132,142},{135,134,143},{137,136,144},{139,138,145}};
constexpr SettingPage03PreviewRow characterPreview[]{{153,152,186},{155,154,187},{157,156,188},{159,158,189},{161,160,190},
    {163,162,191},{165,164,192},{167,166,193},{169,168,194}};
constexpr int d00[]{122,125},d01[]{123,126},d02[]{124,127};
constexpr int d10[]{140,146,129},d11[]{141,147,131},d12[]{142,148,133},d13[]{143,149,135},d14[]{144,150,137},d15[]{145,151,139};
constexpr int d20[]{186,203,153},d21[]{187,204,155},d22[]{188,205,157},d23[]{189,206,159},d24[]{190,207,161},
    d25[]{191,208,163},d26[]{192,209,165},d27[]{193,210,167},d28[]{194,211,169};
const std::array<SettingPage03MuteRow,18> muteRows{{
    {118,d00},{119,d01},{120,d02},{128,d10},{130,d11},{132,d12},{134,d13},{136,d14},{138,d15},
    {152,d20},{154,d21},{156,d22},{158,d23},{160,d24},{162,d25},{164,d26},{166,d27},{168,d28}}};
#undef PAGE
#undef MUTE
#undef STATE
#undef OFF
#undef TARGET
#undef CLICK
#undef CHARVOICE
#undef LABEL
#undef MUTELABEL
#undef SEND
#undef SYSTEM
}
std::span<const SettingPage03ControlGroup> SettingPage03GroupsSample18(std::uint32_t block)
{
    switch (block) { case 0:return groups00; case 1:return groups01; case 2:return groups02; case 4:return groups04; }
    throw std::invalid_argument("Page03 descriptor group");
}
std::span<const SettingPage03LabelPair,9> SettingPage03LabelsSample18() noexcept { return labelPairs; }
std::span<const SettingPage03NamedRow,6> SettingPage03TypesSample18() noexcept { return types; }
std::span<const SettingPage03NamedRow,9> SettingPage03CharactersSample18() noexcept { return characters; }
std::span<const SettingPage03PreviewRow> SettingPage03PreviewRowsSample18(bool chars) noexcept
{ return chars ? std::span<const SettingPage03PreviewRow>(characterPreview) : std::span<const SettingPage03PreviewRow>(typePreview); }
std::span<const SettingPage03MuteRow,18> SettingPage03MuteRowsSample18() noexcept { return muteRows; }
std::span<const GuiButtonDescriptorNative2Ch,2> SettingPage03RadioButtonsSample18() noexcept { return radioButtons; }
const GuiStaticDescriptorNative24h& SettingPage03LoopLabelSample18() noexcept { return loopLabel; }
const GuiStaticDescriptorNative24h& SettingPage03FooterSample18() noexcept { return footer; }
const char* SettingPage03RegistrationNameSample18() noexcept { return registration; }
}
