#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原18 PE只读数据；宏仅合并完全相同CP932字节前缀，所有描述符编译常量。
#define SYSTEM "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xE6\x96\xCA"
#define PAGE SYSTEM "3"
#define OFF "\x83\x49\x83\x74"
#define TARGET "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67"
#define CLICK "\x83\x4E\x83\x8A\x83\x62\x83\x4E"
#define ON "\x83\x49\x83\x93"
#define MUTE "\x83\x7E\x83\x85\x81\x5B\x83\x67\x8F\xF3\x91\xD4"
constexpr GuiStaticDescriptorNative24h Static(int id,const char* n)
{return {id,{nullptr,n,nullptr,nullptr,nullptr,nullptr,nullptr},nullptr};}
constexpr GuiStaticDescriptorNative24h Image(int id,const char* n)
{return {id,{n,nullptr,nullptr,n,n,nullptr,nullptr},nullptr};}
#define RADIOPREFIX SYSTEM "\x8B\xA4\x92\xCA\x83\x7B\x83\x5E\x83\x93"
constexpr char yesOff[]=RADIOPREFIX "\x82\xB7\x82\xE9" OFF ".vrm";
constexpr char yesTarget[]=RADIOPREFIX "\x82\xB7\x82\xE9" TARGET ".vrm";
constexpr char yesClick[]=RADIOPREFIX "\x82\xB7\x82\xE9" CLICK ".vrm";
constexpr char noOff[]=RADIOPREFIX "\x82\xB5\x82\xC8\x82\xA2" OFF ".vrm";
constexpr char noTarget[]=RADIOPREFIX "\x82\xB5\x82\xC8\x82\xA2" TARGET ".vrm";
constexpr char noClick[]=RADIOPREFIX "\x82\xB5\x82\xC8\x82\xA2" CLICK ".vrm";
constexpr GuiButtonDescriptorNative2Ch Yes(int id,const POINT* point)
{return {{id,{nullptr,yesOff,nullptr,yesTarget,yesTarget,yesClick,nullptr},point},7,-1};}
constexpr GuiButtonDescriptorNative2Ch No(int id,const POINT* point)
{return {{id,{nullptr,noOff,nullptr,noTarget,noTarget,noClick,nullptr},point},7,-1};}
constexpr POINT yesNoPoints[]{{406,226},{497,226},{406,395},{497,395},{406,565},{497,565}};
constexpr GuiButtonDescriptorNative2Ch r00[]{Yes(232,&yesNoPoints[0]),No(233,&yesNoPoints[1])};
constexpr GuiButtonDescriptorNative2Ch r01[]{Yes(237,&yesNoPoints[2]),No(238,&yesNoPoints[3])};
constexpr GuiButtonDescriptorNative2Ch r02[]{Yes(242,&yesNoPoints[4]),No(243,&yesNoPoints[5])};
const SettingPage04RadioGroup groups00[]{{231,r00}},groups01[]{{234,r01}},groups02[]{{241,r02}};
constexpr GuiStaticDescriptorNative24h s00[]{
    Image(228,PAGE "API\x95\x60\x89\xE6.vrm"),Image(229,PAGE "DirectX\x95\x60\x89\xE6.vrm"),
    Static(230,PAGE MUTE "\x8B\x4E\x93\xAE\x90\xDD\x92\xE8\x83\x5F\x83\x43\x83\x41\x83\x8D\x83\x4F\x82\xF0\x95\x5C\x8E\xA6.vrm")};
constexpr GuiStaticDescriptorNative24h s01[]{
    Static(235,PAGE MUTE "\x96\xBC\x91\x4F\x95\xCF\x8D\x58\x83\x5F\x83\x43\x83\x41\x83\x8D\x83\x4F\x82\xF0\x95\x5C\x8E\xA6.vrm"),
    Static(236,PAGE MUTE "\x96\xBC\x91\x4F\x82\xF0\x95\xCF\x8D\x58.vrm")};
constexpr GuiStaticDescriptorNative24h s02[]{Static(240,PAGE MUTE "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x91\xAC\x93\x78.vrm")};
constexpr GuiStaticDescriptorNative24h s05[]{Static(253,PAGE MUTE "\x83\x4A\x83\x45\x83\x93\x83\x67\x83\x5F\x83\x45\x83\x93\x90\xDD\x92\xE8.vrm")};
#define CHANGE "\x96\xBC\x91\x4F\x82\xF0\x95\xCF\x8D\x58\x82\xB7\x82\xE9"
constexpr char nameOn[]=PAGE ON CHANGE ".vrm",nameTarget[]=PAGE TARGET CHANGE ".vrm",nameClick[]=PAGE CLICK CHANGE ".vrm";
constexpr GuiButtonDescriptorNative2Ch nameButton{{239,{nameOn,nullptr,nullptr,nameTarget,nameTarget,nameClick,nullptr},nullptr},3,-1};
constexpr char thumbOn[]="\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x93.vrm";
constexpr char thumbOff[]="\x8B\xA4\x92\xCA\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x83\x49\x83\x74.vrm";
constexpr GuiSliderDescriptorNative6Ch speed{244,{thumbOn,thumbOff,nullptr,thumbOn,thumbOn,thumbOn,nullptr},{},nullptr,0,0,0,{309,619,459,638},{0,0,-12,0}};
// 608080六Button；只有246/250有外部POINT，其余位置来自真实资源。
#define CHOICE "\x91\x49\x91\xF0\x8E\x88\x82\xCC\x95\x5C\x8E\xA6"
#define TEXTIN "\xE4\x53\x93\xE0\x82\xC9\x8F\x6F\x82\xB7"
#define TEXTOUT "\xE4\x53\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7"
#define MOUTHIN "\x8C\xFB\x93\xE0\x82\xC9\x8F\x6F\x82\xB7"
#define MOUTHOUT "\x8C\xFB\x8A\x4F\x82\xC9\x8F\x6F\x82\xB7"
#define NAMES(tag,suffix) constexpr char tag##Off[]=PAGE OFF suffix ".vrm"; \
    constexpr char tag##Target[]=PAGE TARGET suffix ".vrm";constexpr char tag##Click[]=PAGE CLICK suffix ".vrm"
NAMES(choice,CHOICE);NAMES(textIn,TEXTIN);NAMES(textOut,TEXTOUT);NAMES(mouthIn,MOUTHIN);NAMES(mouthOut,MOUTHOUT);
constexpr GuiButtonDescriptorNative2Ch Option(int id,const char* off,const char* target,const char* click,const POINT* point=nullptr)
{return {{id,{nullptr,off,nullptr,target,target,click,nullptr},point},7,-1};}
constexpr POINT choicePoints[]{{852,227},{852,276}};
constexpr GuiButtonDescriptorNative2Ch r04first[]{
    Option(246,choiceOff,choiceTarget,choiceClick,&choicePoints[0]),Option(247,textInOff,textInTarget,textInClick),Option(248,textOutOff,textOutTarget,textOutClick)};
constexpr GuiButtonDescriptorNative2Ch r04second[]{
    Option(250,choiceOff,choiceTarget,choiceClick,&choicePoints[1]),Option(251,mouthInOff,mouthInTarget,mouthInClick),Option(252,mouthOutOff,mouthOutTarget,mouthOutClick)};
const SettingPage04RadioGroup groups04[]{{245,r04first},{249,r04second}};
NAMES(hidden,"\x94\xF1\x95\x5C\x8E\xA6");NAMES(right,"\x89\x45\x95\x5C\x8E\xA6");NAMES(left,"\x8D\xB6\x95\x5C\x8E\xA6");
NAMES(small,"\x8F\xAC\x83\x54\x83\x43\x83\x59");NAMES(medium,"\x92\x86\x83\x54\x83\x43\x83\x59");NAMES(large,"\x91\xE5\x83\x54\x83\x43\x83\x59");
#define COLOR "\x95\x5C\x8E\xA6\x83\x4A\x83\x89\x81\x5B"
NAMES(white,COLOR "\x83\x7A\x83\x8F\x83\x43\x83\x67");NAMES(gray,COLOR "\x83\x4F\x83\x8C\x81\x5B");
constexpr GuiButtonDescriptorNative2Ch r05first[]{Option(255,hiddenOff,hiddenTarget,hiddenClick),Option(256,rightOff,rightTarget,rightClick),Option(257,leftOff,leftTarget,leftClick)};
constexpr GuiButtonDescriptorNative2Ch r05second[]{Option(259,smallOff,smallTarget,smallClick),Option(260,mediumOff,mediumTarget,mediumClick),Option(261,largeOff,largeTarget,largeClick)};
constexpr GuiButtonDescriptorNative2Ch r05third[]{Option(263,whiteOff,whiteTarget,whiteClick),Option(264,grayOff,grayTarget,grayClick)};
const SettingPage04RadioGroup groups05[]{{254,r05first},{258,r05second},{262,r05third}};
#define HALF COLOR "\x94\xBC\x93\xA7\x96\xBE"
constexpr char halfTarget[]=PAGE TARGET HALF ".vrm",halfClick[]=PAGE CLICK HALF ".vrm";
constexpr char halfOn[]=PAGE ON HALF ".vrm",halfOnTarget[]=PAGE ON TARGET HALF ".vrm",halfOnClick[]=PAGE ON CLICK HALF ".vrm";
constexpr SaveLoadMessageBoxCheckBoxDescriptorNative48h halfCheck{265,{nullptr,nullptr,nullptr,halfTarget,halfTarget,halfClick,nullptr},
    {halfOn,nullptr,nullptr,halfOnTarget,halfOnTarget,halfOnClick,nullptr},nullptr,7,-1};
constexpr auto footer=Image(30,PAGE ".vrm");
// 406BC0/407430共用5ADC0C原字节，但宿主应建立两份独立CString全局。
constexpr char fontInitial[]="\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E";
constexpr POINT anchors[]{{427,460},{1176,566},{1176,615}};
#undef SYSTEM
#undef PAGE
#undef OFF
#undef TARGET
#undef CLICK
#undef ON
#undef MUTE
#undef RADIOPREFIX
#undef CHANGE
#undef CHOICE
#undef TEXTIN
#undef TEXTOUT
#undef MOUTHIN
#undef MOUTHOUT
#undef NAMES
#undef COLOR
#undef HALF
}
std::span<const SettingPage04RadioGroup> SettingPage04RadioGroupsSample18(unsigned kind)
{switch(kind){case 0:return groups00;case 1:return groups01;case 2:return groups02;case 4:return groups04;case 5:return groups05;}throw std::invalid_argument("Page04 radio group");}
std::span<const GuiStaticDescriptorNative24h> SettingPage04StaticsSample18(unsigned kind)
{switch(kind){case 0:return s00;case 1:return s01;case 2:return s02;case 5:return s05;}throw std::invalid_argument("Page04 static group");}
const GuiButtonDescriptorNative2Ch& SettingPage04NameButtonSample18() noexcept{return nameButton;}
const GuiSliderDescriptorNative6Ch& SettingPage04SpeedSliderSample18() noexcept{return speed;}
const SaveLoadMessageBoxCheckBoxDescriptorNative48h& SettingPage04CountCheckSample18() noexcept{return halfCheck;}
const GuiStaticDescriptorNative24h& SettingPage04FooterSample18() noexcept{return footer;}
const char* SettingPage04FontFaceInitialSample18() noexcept{return fontInitial;}
POINT SettingPage04TextAnchorSample18(unsigned index){if(index>=3)throw std::invalid_argument("Page04 text anchor");return anchors[index];}
}
