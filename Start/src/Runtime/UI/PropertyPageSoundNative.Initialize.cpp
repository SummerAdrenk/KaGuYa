#include "PropertyPageSoundNative.Internal.h"

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
namespace
{
class RefCleanup final
{
public:
    explicit RefCleanup(YtlReferenceSlotRuntime& r) noexcept :ref_(&r) {}
    ~RefCleanup() noexcept {if(ref_)ReleaseLocal(*ref_);}
    void Finish() noexcept {auto* r=ref_;ref_=nullptr;ReleaseLocal(*r);}
private:
    YtlReferenceSlotRuntime* ref_;
};
void AppendWithCopy(Page& p,const YtlReferenceSlotRuntime& r,Services& s)
{
    YtlReferenceSlotRuntime temp;
    temp.baseAt00=Read(r.baseAt00);temp.biasAt04=Read(r.biasAt04);temp.controlAt08=Read(r.controlAt08);
    if(temp.controlAt08)++temp.controlAt08->referenceCountAt04;
    RefCleanup cleanup(temp);AppendVoice(p,temp,s);cleanup.Finish();
}
HTREEITEM InsertTree(Page& p,HTREEITEM parent,const char* text,Services& s)
{
    TVINSERTSTRUCTA insert{};insert.hParent=parent;insert.hInsertAfter=reinterpret_cast<HTREEITEM>(0xFFFF0002u);
    insert.item.mask=1;insert.item.pszText=const_cast<char*>(text);
    return reinterpret_cast<HTREEITEM>(Send(Read(p.treeAt9C),0x1100,0,reinterpret_cast<LPARAM>(&insert),s));
}
void ChildBody(Page& p,int character,HTREEITEM parent,const char* key,int length,Services& s)
{
    YtlReferenceSlotRuntime ref;MakeVoice(ref,s);RefCleanup cleanup(ref);
    auto* voice=static_cast<PropertyPageSoundVoiceParam10h*>(Resolve(ref));voice->characterAt00=character;
    SetRuntimeCStringAt437680(voice->keyAt08,key,length);
    voice->volumeAt0C=*RegistrationParams(s,length!=4,length==4)->RegistrationVolumeAt8C(character,voice->keyAt08.GetString());
    {
        RuntimeCStringResult master([&](RuntimeCString* out){RegistrationParams(s,length!=4,length==4)->RegistrationMasterAt9C(out,character,voice->keyAt08.GetString());});
        AssignRuntimeCStringAt410220(voice->masterAt04,master.Value());
    }
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    FormatRuntimeCStringAt417130(text.Value(),"%s",voice->keyAt08.GetString(),voice->volumeAt0C);
    const auto h=InsertTree(p,parent,text.Value().GetString(),s);
    const auto mute=*RegistrationParams(s,false,true)->RegistrationMuteAt90(character,length==4?voice->keyAt08.GetString():AlternateKey);
    TreeState(p,h,mute?0x1000:0x2000,s);AppendWithCopy(p,ref,s);
    TVITEMA item{};item.mask=4;item.hItem=h;item.lParam=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin))-1;
    Send(Read(p.treeAt9C),0x110D,0,reinterpret_cast<LPARAM>(&item),s);
    // 原先释放显示串，再切terminate state22/29释放临时Ref，见外层显式作用域。
}
void BuildTypes(Page& p,Services& s)
{
    if(!Params(s,true)->HasTypeGroupAt34())return;
    Slider(p,1504,0,s);RECT rect{};s.api.GetWindowRect(Read(p.listAt78.windowAt04),&rect);
    const auto scroll=s.api.GetSystemMetrics(2),edge=s.api.GetSystemMetrics(45);
    LVCOLUMNA column{};column.mask=5;column.fmt=0;column.pszText=const_cast<char*>(NameKey);
    const int width=rect.right-rect.left-scroll-(2*edge+30);if(width!=-1){column.mask=7;column.cx=width;}
    Send(Read(p.listAt78.windowAt04),0x101B,0,reinterpret_cast<LPARAM>(&column),s);
    column.mask=7;column.fmt=1;column.cx=30;column.pszText=const_cast<char*>("\x89\xb9\x97\xca");
    Send(Read(p.listAt78.windowAt04),0x101B,1,reinterpret_cast<LPARAM>(&column),s);
    RuntimeCStringResult text([&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,s.strings);});
    for(int i=0;i<Params(s)->TypeCountAt3C();++i)
    {
        const auto index=Send(Read(p.listAt78.windowAt04),0x1004,0,0,s);
        {
            RuntimeCStringResult name([&](RuntimeCString* out){Params(s)->TypeNameAt44(out,i);});
            ListItem3Ch item{};item.mask=1;item.item=static_cast<int>(index);item.text=const_cast<char*>(name.Value().GetString());
            Send(Read(p.listAt78.windowAt04),0x1007,0,reinterpret_cast<LPARAM>(&item),s);
        }
        ListItem3Ch item{};const auto mute=*Params(s)->TypeMuteAt48(i);item.state=mute?0x1000:0x2000;item.stateMask=0xF000;
        Send(Read(p.listAt78.windowAt04),0x102B,i,reinterpret_cast<LPARAM>(&item),s);
        const auto volume=*Params(s)->TypeVolumeAt4C(i);FormatRuntimeCStringAt417130(text.Value(),"%d",volume);
        item={};item.mask=1;item.item=static_cast<int>(index);item.subItem=1;item.text=const_cast<char*>(text.Value().GetString());
        Send(Read(p.listAt78.windowAt04),0x1006,0,reinterpret_cast<LPARAM>(&item),s);
    }
    if(Send(Read(p.listAt78.windowAt04),0x1004,0,0,s))
    {
        ListItem3Ch item{};item.state=item.stateMask=2;Send(Read(p.listAt78.windowAt04),0x102B,0,reinterpret_cast<LPARAM>(&item),s);
        const auto position=100u-*Params(s,true)->TypeVolumeAt4C(0);Send(Item(p,1504,s),0x405,1,position,s);
    }
}
void BuildCharacters(Page& p,Services& s)
{
    if(!Params(s,true)->CharacterGroupEnabledAt70())return;
    Slider(p,1505,0,s);
    // 各原PE均缺BITMAP205；显式安装完整标准状态图，不再把空列表传给树控件。
    s.api.InstallSoundTreeStateImages(Read(p.treeAt9C));
    const auto root=InsertTree(p,reinterpret_cast<HTREEITEM>(0xFFFF0000u),"\x83\x4c\x83\x83\x83\x89\x83\x4e\x83\x5e\x81\x5b\x96\xbc",s);
    const auto usingGroup=CategorizedVoiceUi()?*Params(s,true)->VoiceMuteAt68()==0:*Params(s,true)->CharacterGroupUsingAt74()!=0;
    TreeState(p,root,usingGroup?0x2000:0x1000,s);
    ReleaseVoiceRange(Read(p.voicesAt58.begin),Read(p.voicesAt58.end));p.voicesAt58.end=Read(p.voicesAt58.begin);
    {
        YtlReferenceSlotRuntime sentinel;MakeVoice(sentinel,s);RefCleanup cleanup(sentinel);AppendWithCopy(p,sentinel,s);cleanup.Finish();
    }
    for(int i=0;i<Params(s)->CharacterCountAt78();++i)
    {
        RuntimeCStringResult name([&](RuntimeCString* out){Params(s)->CharacterNameAt80(out,i);});
        if(!*Params(s)->CharacterDisplayAt84(i))SetRuntimeCStringAt437680(name.Value(),"\x81\x48\x81\x48\x81\x48",6);
        RuntimeCStringResult expanded([&](RuntimeCString* out){ExpandName(p,out,name.Value(),s);});
        const auto group=InsertTree(p,root,expanded.Value().GetString(),s);
        auto* selected=RegistrationParams(s,true,false);
        const auto usingCharacter=CategorizedVoiceUi()?*selected->RegistrationMuteAt90(i,VoiceKeyText)==0:*selected->CharacterUsingAt7C(i)!=0;
        TreeState(p,group,usingCharacter?0x2000:0x1000,s);
        // 5E2AEC两处catch(...)只覆盖各子项state17..22、24..29；父节点与姓名错误仍外传。
        if(CategorizedVoiceUi())
        {
            ChildBody(p,i,group,VoiceKeyText,4,s);
            static_cast<void>(CategorizedVoiceLeaf(p,group,s));
        }
        else
        {
            try {ChildBody(p,i,group,VoiceKeyText,4,s);} catch(...) {}
            try {ChildBody(p,i,group,AlternateKey,10,s);} catch(...) {}
        }
    }
    Send(Read(p.treeAt9C),0x1102,2,reinterpret_cast<LPARAM>(root),s);
    Send(Read(p.treeAt9C),0x110B,9,reinterpret_cast<LPARAM>(root),s);
}
}
RuntimeCString* ExpandName(Page& p,RuntimeCString* out,const RuntimeCString& input,Services& s)
{
    auto* clone=CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(input.GetString()))-1);
    const char* chars=reinterpret_cast<const char*>(clone+1);std::memcpy(out,&chars,4);
    try
    {
        for(;;)
        {
            if(out->GetLength()<=0)return out;
            const auto* first=_mbschr(reinterpret_cast<const unsigned char*>(out->GetString()),'%');if(!first)return out;
            const auto offset=static_cast<int>(reinterpret_cast<const char*>(first)-out->GetString());if(offset<0)return out;
            RuntimeCStringResult left([&](RuntimeCString* v){ConstructRuntimeCStringLeftAt419D00(v,*out,offset,s.strings);});
            RuntimeCStringResult rest([&](RuntimeCString* v){ConstructRuntimeCStringMidAt420C80(v,*out,offset+1,out->GetLength()-offset-1,s.strings);});
            const auto* second=_mbschr(reinterpret_cast<const unsigned char*>(rest.Value().GetString()),'%');
            if(rest.Value().GetLength()<=0||!second)throw const_cast<char*>("\x83\x47\x83\x89\x81\x5b");
            const auto end=static_cast<int>(reinterpret_cast<const char*>(second)-rest.Value().GetString());
            if(end<0)throw const_cast<char*>("\x83\x47\x83\x89\x81\x5b");
            RuntimeCStringResult key([&](RuntimeCString* v){ConstructRuntimeCStringLeftAt419D00(v,rest.Value(),end,s.strings);});
            AssignRuntimeCStringAt410220(*out,left.Value());const auto* k=reinterpret_cast<const unsigned char*>(key.Value().GetString());
            const RuntimeCString* replacement=nullptr;
            if(!_mbscmp(k,reinterpret_cast<const unsigned char*>(SurnameKey)))replacement=Read(p.surnameAtA4)?Read(p.surnameAtA4):&Save(s,true)->stringAt5C;
            else if(!_mbscmp(k,reinterpret_cast<const unsigned char*>(NameKey)))replacement=Read(p.nameAtA0)?Read(p.nameAtA0):&Save(s,true)->stringAt58;
            if(replacement)AppendRuntimeCStringAt413550(*out,replacement->GetString(),replacement->GetLength());
            RuntimeCStringResult tail([&](RuntimeCString* v){ConstructRuntimeCStringMidAt420C80(v,rest.Value(),end+1,rest.Value().GetLength()-end-1,s.strings);});
            AppendRuntimeCStringAt413550(*out,tail.Value().GetString(),tail.Value().GetLength());
        }
    }
    catch(...)
    {
        try {DestroyRuntimeCStringAt40CD40(out);} catch(...) {std::terminate();}
        throw;
    }
}
}

namespace KaGuYa::Start::Runtime
{
LRESULT InitializePropertyPageSoundControlsNativeAt425690(PropertyPageSoundNativeBodyA8h& p,PropertyPageSoundNativeServices& s)
{
    using namespace PropertySoundDetail;PropertyPageSoundNativeScope scope(s);
    SheetButtons(s.api.GetParent(Read(p.pageAt00.endpointAt00.windowAt04)),s);
    p.volumesAt64.end=Read(p.volumesAt64.begin);
    for(int i=0;i<6;++i)AppendPair(p,{1400+i,1500+i},s);
    p.musicAt70=*Params(s)->MusicMuteAt10()==0;
    Slider(p,1500,100u-*Params(s)->MusicVolumeAt14(),s);SetNumber(p,*Params(s)->MusicVolumeAt14(),1400,s);
    p.effectsAt71=*Params(s)->NormalEffectMuteAt20()==0;
    Slider(p,1501,100u-*Params(s)->NormalEffectVolumeAt24(),s);SetNumber(p,*Params(s)->NormalEffectVolumeAt24(),1401,s);
    p.voiceAt72=*Params(s)->VoiceMuteAt68()==0;
    Slider(p,1502,100u-*Params(s)->VoiceVolumeAt6C(),s);SetNumber(p,*Params(s)->VoiceVolumeAt6C(),1402,s);
    p.waitAt74=Read(Save(s)->valueAt81)!=0;
    p.lowerAt73=At<BYTE>(Save(s),0x85)==0;
    Slider(p,1503,100u-At<DWORD>(Save(s),0x88),s);SetNumber(p,At<int>(Save(s),0x88),1403,s);
    p.lowerEnabledAt75=At<BYTE>(Save(s,true),0x84)==0;
    const auto list=Item(p,1800,s);const auto procedure=Read(p.listAt78.vptrAt00)->procedureAt08(&p.listAt78);
    auto* thunk=Read(p.listAt78.thunkAt14);
    if(!thunk) {thunk=AllocateWindowThunkNativeAt53F0E4(s.endpoints.thunks);p.listAt78.thunkAt14=thunk;}
    if(thunk)
    {
        InitializeWindowThunkNativeAt53F1E6(thunk,reinterpret_cast<DWORD>(procedure),&p.listAt78,s.endpoints.thunks);
        const auto code=WindowThunkNativeCodeAt53F149(Read(p.listAt78.thunkAt14),s.endpoints.thunks);
        const auto previous=s.api.SetWindowLongA(list,GWL_WNDPROC,std::bit_cast<LONG>(reinterpret_cast<DWORD>(code)));
        if(previous)
        {p.listAt78.previousAt20=reinterpret_cast<WNDPROC>(previous);p.listAt78.windowAt04=list;Send(list,0x1036,0,36,s);}
    }
    p.treeAt9C=Item(p,2000,s);BuildTypes(p,s);BuildCharacters(p,s);
    Checkboxes(p,false,s);RefreshMusic(p,s);RefreshEffects(p,s);RefreshVoice(p,s);RefreshLower(p,s);RefreshTree(p,s);return 1;
}
}
