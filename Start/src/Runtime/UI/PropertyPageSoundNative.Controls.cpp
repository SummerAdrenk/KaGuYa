#include "PropertyPageSoundNative.Internal.h"
#include <Runtime/RuntimeCStringInputNative.h>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
int ParsePropertyPageSoundNumberNativeAt40BEB0(const char* text,int length) noexcept
{
    int first=0,last=length;DWORD radix=10,value=0,sign=1;
    if(length>=2&&(text[length-1]=='h'||text[length-1]=='H')) {radix=16;--last;}
    else if(length>=3&&text[0]=='0'&&(text[1]=='x'||text[1]=='X')) {radix=16;first=2;}
    for(int i=first;i<last;++i)
    {
        const auto c=static_cast<unsigned char>(text[i]);DWORD digit;
        if(c>='0'&&c<='9')digit=c-'0';
        else if(radix==16&&c>='a'&&c<='f')digit=c-'a'+10;
        else if(radix==16&&c>='A'&&c<='F')digit=c-'A'+10;
        else if(radix==10&&i==first&&c=='-') {sign=0xFFFFFFFFu;continue;}
        else return 0;
        value=value*radix+digit;
    }
    return std::bit_cast<int>(value*sign);
}
}

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
// 原CP932字节，不以当前编译器源字符集转换这些配置key。
const char EffectKey[]="\x8c\xf8\x89\xca\x89\xb9";
const char VoiceKeyText[]="\x89\xb9\x90\xba";
const char AlternateKey[]="\x83\x74\x83\x46\x83\x89\x83\x60\x83\x49";
const char NameKey[]="\x96\xbc\x91\x4f";
const char SurnameKey[]="\x96\xbc\x8e\x9a";
HWND Item(Page& p,int id,Services& s) {return s.api.GetDlgItem(Read(p.pageAt00.endpointAt00.windowAt04),id);}
LRESULT Send(HWND h,UINT m,WPARAM w,LPARAM l,Services& s) {return s.api.SendMessageA(h,m,w,l);}
void Enable(Page& p,int id,BOOL b,Services& s) {s.api.EnableWindow(Item(p,id,s),b);}
void Style(HWND h,LONG mask,LONG bits,Services& s)
{const auto old=s.api.GetWindowLongA(h,GWL_STYLE),next=(old&~mask)|bits;if(next!=old)s.api.SetWindowLongA(h,GWL_STYLE,next);}
void Checkboxes(Page& p,bool read,Services& s)
{
    auto* flags=reinterpret_cast<BYTE*>(&p)+0x70;
    for(int i=0;i<6;++i)
    {
        if(read) {const auto h=Item(p,1200+i,s);flags[i]=Send(h,0xF0,0,0,s)==1;}
        else {const BOOL old=flags[i]!=0;const auto h=Item(p,1200+i,s);Send(h,0xF1,old,0,s);}
    }
}
namespace
{
void Length(RuntimeCString& text,int length)
{
    auto* chars=const_cast<char*>(text.GetString());auto* head=reinterpret_cast<ATL::CStringData*>(chars)-1;
    if(length<0||length>head->nAllocLength)throw ATL::CAtlException(E_INVALIDARG);
    head->nDataLength=length;chars[length]=0;
}
void Buffer(RuntimeCString& text,int length)
{
    auto* head=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()))-1;
    if(length<0)throw ATL::CAtlException(E_INVALIDARG);
    if(((1-head->nRefs)|(head->nAllocLength-length))<0)PrepareRuntimeCStringWriteAt42D590(text,length);
    Length(text,length);
}
int Product(int a,int b) noexcept {return std::bit_cast<int>(static_cast<DWORD>(a)*static_cast<DWORD>(b))/100;}
void SetAbsolute(SoundManagerNativeBody50h* body,DWORD slot,int value,Services& s)
{
    const auto handle=At<int>(Read(body->slotsBeginAt04),slot*8);s.source.SetVolumeAt438170(handle,100,value);
}
}
int ParseText(const RuntimeCString& text,Services& s)
{
    // 40BEB0自己还会40B530复制输入，不能只解析借用的控件buffer。
    RuntimeCStringResult held([&](RuntimeCString* out){ConstructRuntimeCStringInputNativeAt40B530(out,text.GetString(),s.strings);});
    return ParsePropertyPageSoundNumberNativeAt40BEB0(held.Value().GetString(),held.Value().GetLength());
}
int TypeNumber(Page& p,int index,Services& s)
{
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    ListItem3Ch item{};item.subItem=1;
    for(DWORD n=256;;n+=n)
    {
        if(n>0x7FFFFFFFu)throw ATL::CAtlException(E_INVALIDARG);Buffer(text.Value(),static_cast<int>(n));
        item.text=const_cast<char*>(text.Value().GetString());item.textSize=static_cast<int>(n);
        const auto length=Send(Read(p.listAt78.windowAt04),0x102D,index,reinterpret_cast<LPARAM>(&item),s);
        if(length<static_cast<int>(n)-1)break;
    }
    const auto* chars=text.Value().GetString();Length(text.Value(),static_cast<int>(strnlen_s(chars,text.Value().GetAllocLength())));
    return ParseText(text.Value(),s);
}
DWORD TreeData(Page& p,HTREEITEM treeItem,Services& s)
{
    TVITEMA item{};item.mask=4;item.hItem=treeItem;
    return Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s)?static_cast<DWORD>(item.lParam):0;
}
PropertyPageSoundVoiceParam10h* Voice(Page& p,DWORD index)
{return static_cast<PropertyPageSoundVoiceParam10h*>(Resolve(Read(p.voicesAt58.begin)[index]));}
int TreeNumber(Page& p,HTREEITEM treeItem,Services& s)
{
    TVITEMA item{};item.mask=0x40;item.hItem=treeItem;Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s);
    if(item.cChildren)return -1;const auto data=TreeData(p,treeItem,s);
    if(!data||data>=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))return -1;
    return std::bit_cast<int>(Read(Voice(p,data)->volumeAt0C));
}
int Number(Page& p,int id,Services& s)
{
    if(id==1404)return TypeNumber(p,static_cast<int>(Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s)),s);
    if(id==1405)return TreeNumber(p,Next(p,9,nullptr,s),s);
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    const auto h=Item(p,id,s);const auto length=s.api.GetWindowTextLengthA(h);Buffer(text.Value(),length);
    s.api.GetWindowTextA(h,const_cast<char*>(text.Value().GetString()),length+1);return ParseText(text.Value(),s);
}
void SetNumber(Page& p,int value,int id,Services& s)
{
    if(id==1405)
    {
        const auto selection=Next(p,9,nullptr,s);const auto data=TreeData(p,selection,s);if(!data)return;
        RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
        FormatRuntimeCStringAt417130(text.Value(),"%s",Voice(p,data)->keyAt08.GetString(),value);
        TVITEMA item{};item.mask=1;item.hItem=selection;item.pszText=const_cast<char*>(text.Value().GetString());
        Send(Read(p.treeAt9C),0x110D,0,reinterpret_cast<LPARAM>(&item),s);return;
    }
    const auto h=id==1404?nullptr:Item(p,id,s);
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    FormatRuntimeCStringAt417130(text.Value(),"%d",value);
    if(id==1404)
    {
        const auto index=Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s);
        ListItem3Ch item{};item.mask=1;item.item=static_cast<int>(index);item.subItem=1;item.text=const_cast<char*>(text.Value().GetString());
        Send(Read(p.listAt78.windowAt04),0x1006,0,reinterpret_cast<LPARAM>(&item),s);
    }
    else s.api.SetWindowTextA(h,text.Value().GetString());
}
void Slider(Page& p,int id,int position,Services& s)
{const auto h=Item(p,id,s);Send(h,0x407,0,0,s);Send(h,0x408,0,100,s);Send(h,0x414,20,0,s);Send(h,0x405,1,position,s);}
void TreeState(Page& p,HTREEITEM h,UINT state,Services& s)
{TVITEMA item{};item.mask=8;item.hItem=h;item.state=state;item.stateMask=0xF000;Send(Read(p.treeAt9C),0x110D,0,reinterpret_cast<LPARAM>(&item),s);}
UINT TreeState(Page& p,HTREEITEM h,Services& s)
{return static_cast<UINT>(Send(Read(p.treeAt9C),0x1127,reinterpret_cast<WPARAM>(h),0xF000,s));}
HTREEITEM Next(Page& p,UINT code,HTREEITEM h,Services& s)
{return reinterpret_cast<HTREEITEM>(Send(Read(p.treeAt9C),0x110A,code,reinterpret_cast<LPARAM>(h),s));}
bool VoiceKey(const PropertyPageSoundVoiceParam10h& voice,const char* key)
{return _mbscmp(reinterpret_cast<const unsigned char*>(voice.masterAt04.GetString()),reinterpret_cast<const unsigned char*>(key))==0;}
void RefreshMusic(Page& p,Services& s) {const auto enabled=Read(p.musicAt70)!=0;Enable(p,1500,enabled,s);Enable(p,1400,enabled,s);}
void RefreshVoice(Page& p,Services& s)
{const auto enabled=Read(p.voiceAt72)!=0;Enable(p,1502,enabled,s);Enable(p,1402,enabled,s);Enable(p,1204,enabled,s);}
void RefreshLower(Page& p,Services& s)
{
    const auto enabled=Read(p.lowerEnabledAt75)!=0;Enable(p,1203,enabled,s);
    const auto lower=Read(p.lowerAt73)!=0;Enable(p,1503,enabled&&lower,s);Enable(p,1403,enabled&&lower,s);
}
void RefreshEffects(Page& p,Services& s)
{
    const auto enabled=Read(p.effectsAt71)!=0;Enable(p,1501,enabled,s);Enable(p,1401,enabled,s);
    s.api.EnableWindow(Read(p.listAt78.windowAt04),enabled);Style(Read(p.listAt78.windowAt04),8,enabled?8:0,s);
    if(enabled&&Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s)>0)
    {
        const auto i=Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s);ListItem3Ch item{};item.state=item.stateMask=2;
        Send(Read(p.listAt78.windowAt04),0x102B,i,reinterpret_cast<LPARAM>(&item),s);
    }
    const auto i=Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s);
    const auto state=static_cast<DWORD>(Send(Read(p.listAt78.windowAt04),0x102C,i,0xF000,s))>>12;
    Enable(p,1100,enabled&&state!=1,s);Enable(p,1504,enabled&&state!=1,s);
}
namespace
{
UINT EnabledTreeState(Page& p,HTREEITEM item,Services& s)
{
    auto state=(TreeState(p,item,s)>>12)&15;
    if(state<1||state>4)throw std::logic_error("Categorized sound tree has no valid checkbox state");
    return state>2?state-2:state;
}
void RefreshNode(Page& p,HTREEITEM h,UINT inherited,Services& s)
{
    auto state=(static_cast<WORD>(TreeState(p,h,s))>>12);if(state>2)state-=2;
    TVITEMA item{};item.mask=0x40;item.hItem=h;Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s);
    if(item.cChildren)
    {
        TreeState(p,h,(inherited==1?state+2:state)<<12,s);
        for(auto child=Next(p,4,h,s);child;child=Next(p,1,child,s))RefreshNode(p,child,inherited==1?inherited:state,s);
        return;
    }
    const auto index=TreeData(p,h,s);if(!index||index>=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))return;
    if((VoiceKey(*Voice(p,index),VoiceKeyText)&&!Read(p.voiceAt72))||
        (VoiceKey(*Voice(p,index),EffectKey)&&!Read(p.effectsAt71))||inherited==1)state+=2;
    TreeState(p,h,state<<12,s);
}
}
HTREEITEM CategorizedVoiceLeaf(Page& p,HTREEITEM group,Services& s)
{
    const auto leaf=Next(p,4,group,s);
    if(!leaf||Next(p,1,leaf,s))throw std::logic_error("Categorized sound character requires one voice leaf");
    const auto data=TreeData(p,leaf,s);
    if(!data||data>=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))
        throw std::logic_error("Categorized sound tree voice index is invalid");
    const auto* voice=Voice(p,data);
    if(!VoiceKey(*voice,VoiceKeyText)||::_mbscmp(reinterpret_cast<const unsigned char*>(voice->keyAt08.GetString()),
        reinterpret_cast<const unsigned char*>(VoiceKeyText)))
        throw std::logic_error("Categorized sound tree voice key differs from its original binding");
    return leaf;
}
void SynchronizeCategorizedTreeClick(Page& p,HTREEITEM changed,Services& s)
{
    const auto root=Next(p,0,nullptr,s);
    if(changed==root)
    {
        // 树根与1202是同一个旧voice master mute的两种视图，不创建额外Using持久化位。
        p.voiceAt72=EnabledTreeState(p,root,s)==2;
        Send(Item(p,1202,s),BM_SETCHECK,Read(p.voiceAt72)?BST_CHECKED:BST_UNCHECKED,0,s);
        return;
    }
    for(auto group=Next(p,4,root,s);group;group=Next(p,1,group,s))
    {
        const auto leaf=CategorizedVoiceLeaf(p,group,s);
        if(changed!=group&&changed!=leaf)continue;
        const auto state=EnabledTreeState(p,changed,s)<<12;
        TreeState(p,changed==group?leaf:group,state,s);
        return;
    }
    throw std::logic_error("Categorized sound checkbox is outside the registered tree");
}
void RefreshTree(Page& p,Services& s)
{
    const auto root=Next(p,0,nullptr,s);if(root)
    {
        if(CategorizedVoiceUi())
        {
            const bool enabled=Read(p.voiceAt72)!=0;
            TreeState(p,root,enabled?0x2000:0x1000,s);
            for(auto group=Next(p,4,root,s);group;group=Next(p,1,group,s))
            {
                const auto leaf=CategorizedVoiceLeaf(p,group,s);
                const auto state=EnabledTreeState(p,leaf,s)+(enabled?0u:2u);
                // 父与唯一叶是同一mute；不能因父未勾选就禁用它自己的别名叶。
                TreeState(p,group,state<<12,s);TreeState(p,leaf,state<<12,s);
            }
        }
        else
        {
            const auto state=(TreeState(p,root,s)>>12)&15;
            for(auto child=Next(p,4,root,s);child;child=Next(p,1,child,s))RefreshNode(p,child,state,s);
        }
    }
    const auto enabled=Read(p.voiceAt72)!=0;const auto selection=Next(p,9,nullptr,s);bool playable=false;
    if(selection)
    {
        TVITEMA item{};item.mask=0x40;item.hItem=selection;Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s);
        if(!item.cChildren)playable=(TreeState(p,selection,s)&0xF000)==0x2000;
    }
    Enable(p,1101,enabled&&playable,s);Enable(p,1505,enabled&&playable,s);
    if(!enabled||!playable)s.source.ClearAt4384D0(Sound(s));
}
void RefreshVolume(Page& p,int id,Services& s)
{
    if(id==1400||id==1500)
    {
        const auto slot=Read(SoundGetter(s)->musicAt10);auto* root=Root(s);const auto value=Number(p,1400,s);
        SetAbsolute(SoundGetter(root,s),slot,value,s);return;
    }
    if(id==1401||id==1501)
    {
        const auto slot=Read(SoundGetter(s)->voicesBeginAt14);if(s.source.PlayingAt419370(SoundGetter(s),slot))
        {
            const auto data=TreeData(p,Next(p,9,nullptr,s),s);
            if(data&&data<Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin))&&VoiceKey(*Voice(p,data),EffectKey))
            {
                const auto factor=TreeNumber(p,Next(p,9,nullptr,s),s);const auto selectedSlot=Read(SoundGetter(s)->voicesBeginAt14);
                auto* root=Root(s);const auto value=Product(factor,Number(p,1401,s));SetAbsolute(SoundGetter(root,s),selectedSlot,value,s);
            }
        }
    }
    if(id==1401||id==1501||id==1404||id==1504)
    {
        const auto slot=Read(Sound(s)->remainingEffectsBeginAt20);if(!s.source.PlayingAt419370(Sound(s),slot))return;
        const auto factor=TypeNumber(p,static_cast<int>(Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s)),s);
        const auto selectedSlot=Read(SoundGetter(s)->remainingEffectsBeginAt20);auto* root=Root(s);
        const auto value=Product(factor,Number(p,1401,s));SetAbsolute(SoundGetter(root,s),selectedSlot,value,s);return;
    }
    if(id!=1402&&id!=1502&&id!=1405&&id!=1505)return;
    const auto slot=Read(Sound(s)->voicesBeginAt14);if(!s.source.PlayingAt419370(Sound(s),slot))return;
    const auto data=TreeData(p,Next(p,9,nullptr,s),s);
    if(!data||data>=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))return;
    int control;
    if(VoiceKey(*Voice(p,data),VoiceKeyText))control=1402;
    else if(VoiceKey(*Voice(p,data),EffectKey))control=1401;else return;
    const auto factor=TreeNumber(p,Next(p,9,nullptr,s),s);const auto selectedSlot=Read(SoundGetter(s)->voicesBeginAt14);
    auto* root=Root(s);const auto value=Product(factor,Number(p,control,s));SetAbsolute(SoundGetter(root,s),selectedSlot,value,s);
}
void SheetButtons(HWND parent,Services& s)
{
    if(Read(s.sheetButtonsAt6197EC))return;
    s.api.SetWindowTextA(s.api.GetDlgItem(parent,2),"\x95\xc2\x82\xb6\x82\xe9");
    s.api.SetWindowTextA(s.api.GetDlgItem(parent,1),"\x8d\x58\x90\x56");
    const auto old=s.api.GetWindowLongA(parent,GWL_EXSTYLE),next=old&~0x400;
    if(next!=old)s.api.SetWindowLongA(parent,GWL_EXSTYLE,next);s.sheetButtonsAt6197EC=1;
}
}
