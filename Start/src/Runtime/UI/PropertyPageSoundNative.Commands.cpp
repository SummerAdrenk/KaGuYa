#include "PropertyPageSoundNative.Internal.h"

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
void AdvancePreviewCursor(std::uint32_t* state,const EnvironmentVectorStorage<RuntimeCString>& files)
{
    *state=Read(*state)+1u;
    // 42AF45/42AF47、42B816/42B818均signed JL；DWORD递增后按有符号位型比较。
    const auto count=std::bit_cast<int>(Delta(Read(files.end),Read(files.begin)))>>2;
    if(std::bit_cast<std::int32_t>(Read(*state))>=count)*state=0;
}
namespace
{
int Product(int a,int b) noexcept {return std::bit_cast<int>(static_cast<DWORD>(a)*static_cast<DWORD>(b))/100;}
void StopVoice(Services& s,bool getter=false) {s.source.ClearAt4384D0(getter?SoundGetter(s):Sound(s));}
void StopEffect(Services& s)
{auto* body=Sound(s);const auto slot=Read(body->remainingEffectsBeginAt20);s.source.StopAt4383B0(body,slot);}
bool SelectedKey(Page& p,const char* key,Services& s)
{
    const auto data=TreeData(p,Next(p,9,nullptr,s),s);
    return data&&data<Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin))&&VoiceKey(*Voice(p,data),key);
}
void PreviewMusic(Page& p,Services& s)
{
    if(!Read(p.musicAt70))
    {auto* body=Sound(s);const auto slot=Read(body->musicAt10);s.source.StopAt4383B0(body,slot);return;}
    if(!UnSave(s)->StringAt2C().GetLength())return;
    const auto* name=UnSave(s)->StringAt2C().GetString();const auto slot=Read(Sound(s)->musicAt10);
    auto* root=Root(s);const auto value=Number(p,1400,s);s.source.OpenAt42BC90(Sound(root,s),slot,name,value,1,0);
}
void PreviewType(Page& p,Services& s)
{
    const auto index=Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s);if(index<0)return;
    auto* files=Params(s,true)->TypeFilesAt54(static_cast<int>(index));if(!files)return;
    auto* state=Params(s,true)->TypeStateAt50(static_cast<int>(index));if(!state)return;
    const auto selected=Send(Read(p.listAt78.windowAt04),0x100C,0xFFFFFFFFu,2,s);
    const auto factor=TypeNumber(p,static_cast<int>(selected),s);const auto value=Product(factor,Number(p,1401,s));
    const auto* name=Read(files->begin)[Read(*state)].GetString();const auto slot=Read(Sound(s)->remainingEffectsBeginAt20);
    auto* body=Sound(s);s.source.OpenAt42BC90(body,slot,name,value,0,0);
    AdvancePreviewCursor(state,*files);
}
LRESULT PreviewCharacter(Page& p,Services& s)
{
    const auto data=TreeData(p,Next(p,9,nullptr,s),s);if(!data)return 0;
    auto* params=Params(s,true);auto* voice=Voice(p,data);const char* key=voice->keyAt08.GetString();
    auto* files=params->RegistrationFilesAt98(Read(Voice(p,data)->characterAt00),key);if(!files)return 0;
    params=Params(s,true);voice=Voice(p,data);key=voice->keyAt08.GetString();
    auto* state=params->RegistrationStateAt94(Read(Voice(p,data)->characterAt00),key);if(!state)return 0;
    int control;
    if(VoiceKey(*Voice(p,data),VoiceKeyText))control=1402;
    else if(VoiceKey(*Voice(p,data),EffectKey))control=1401;else return 1;
    const auto factor=Number(p,control,s);const auto value=Product(factor,TreeNumber(p,Next(p,9,nullptr,s),s));
    RuntimeCStringResult mapped([&](RuntimeCString* out)
    {Params(s,true)->MapResourceAtA0(out,Read(files->begin)[Read(*state)].GetString());});
    const auto slot=Read(SoundGetter(s)->voicesBeginAt14);auto* body=SoundGetter(s);
    s.source.OpenAt42BC90(body,slot,mapped.Value().GetString(),value,0,0);
    AdvancePreviewCursor(state,*files);
    return 0;
}
LRESULT EditChanged(Page& p,HWND control,Services& s)
{
    DWORD i=0;
    while(i<Count8(Read(p.volumesAt64.end),Read(p.volumesAt64.begin)))
    {
        if(control==Item(p,Read(Read(p.volumesAt64.begin)[i].textAt00),s))
        {
            const auto text=Read(Read(p.volumesAt64.begin)[i].textAt00);const auto value=Number(p,text,s);
            if(value<0)SetNumber(p,0,Read(Read(p.volumesAt64.begin)[i].textAt00),s);
            else if(value>100)SetNumber(p,100,Read(Read(p.volumesAt64.begin)[i].textAt00),s);
            // 42476B使用原v12，不使用上面修正后的文本值。
            const auto slider=Item(p,Read(Read(p.volumesAt64.begin)[i].sliderAt04),s);
            Send(slider,0x405,1,std::bit_cast<int>(100u-static_cast<DWORD>(value)),s);
            RefreshVolume(p,Read(Read(p.volumesAt64.begin)[i].textAt00),s);
        }
        ++i;
    }
    return 0;
}
}
LRESULT Command(Page& p,WORD command,HWND control,Services& s)
{
    if(command==0x300)return EditChanged(p,control,s);
    if(command==1100) {PreviewType(p,s);return 0;}
    if(command==1101)return PreviewCharacter(p,s);
    Checkboxes(p,true,s);
    switch(command)
    {
    case 1200:RefreshMusic(p,s);PreviewMusic(p,s);break;
    case 1201:RefreshEffects(p,s);RefreshTree(p,s);
        if(!Read(p.effectsAt71)) {StopEffect(s);if(SelectedKey(p,EffectKey,s))StopVoice(s,true);}break;
    case 1202:RefreshVoice(p,s);RefreshTree(p,s);
        if(!Read(p.voiceAt72)&&SelectedKey(p,VoiceKeyText,s))StopVoice(s,true);break;
    case 1203:case 1205:RefreshLower(p,s);break;
    }
    return 0;
}
LRESULT Scroll(Page& p,HWND control,Services& s)
{
    DWORD i=0;if(!Count8(Read(p.volumesAt64.end),Read(p.volumesAt64.begin)))return 0;
    while(control!=Item(p,Read(Read(p.volumesAt64.begin)[i].sliderAt04),s))
    {if(++i>=Count8(Read(p.volumesAt64.end),Read(p.volumesAt64.begin)))return 0;}
    const auto slider=Item(p,Read(Read(p.volumesAt64.begin)[i].sliderAt04),s);
    const auto position=Send(slider,0x400,0,0,s);
    const auto value=std::bit_cast<int>(100u-static_cast<DWORD>(position));
    SetNumber(p,value,Read(Read(p.volumesAt64.begin)[i].textAt00),s);
    RefreshVolume(p,Read(Read(p.volumesAt64.begin)[i].sliderAt04),s);
    if(Read(Read(p.volumesAt64.begin)[i].sliderAt04)==1505)
    {
        const auto data=TreeData(p,Next(p,9,nullptr,s),s);
        if(data&&data<Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))Voice(p,data)->volumeAt0C=static_cast<DWORD>(value);
    }
    return 0;
}
LRESULT NotifyTree(Page& p,NMTREEVIEWA* n,Services& s)
{
    TVITEMA item{};item.mask=0x40;item.hItem=n->itemNew.hItem;Send(Read(p.treeAt9C),0x110C,0,reinterpret_cast<LPARAM>(&item),s);
    const auto state=n->itemNew.state;
    if(item.cChildren)
    {if(state&2) {StopVoice(s);Style(Item(p,1505,s),0x80,0x80,s);}}
    else if(state&2)
    {
        StopVoice(s);Style(Item(p,1505,s),0x80,0,s);
        const auto value=TreeNumber(p,n->itemNew.hItem,s);Send(Item(p,1505,s),0x405,1,std::bit_cast<int>(100u-static_cast<DWORD>(value)),s);
    }
    else if((state&0xF000)==0x1000&&n->itemNew.hItem==Next(p,9,nullptr,s))StopVoice(s,true);
    RefreshVoice(p,s);RefreshTree(p,s);return 0;
}
}
