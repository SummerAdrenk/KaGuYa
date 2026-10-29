#include "PropertyPageSoundNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
BOOL ApplyPropertyPageSoundNativeAt4216F0(PropertyPageSoundNativeBodyA8h& p,PropertyPageSoundNativeServices& s)
{
    using namespace PropertySoundDetail;PropertyPageSoundNativeScope scope(s);Checkboxes(p,true,s);
    auto* selected=Params(s,false,false,true);const auto music=Read(p.musicAt70);
    auto* mute=selected->MusicMuteAt10();*mute=music==0;
    auto* volume=Params(s)->MusicVolumeAt14();*volume=static_cast<DWORD>(Number(p,1400,s));
    selected=Params(s);const auto effects=Read(p.effectsAt71);mute=selected->NormalEffectMuteAt20();*mute=effects==0;
    volume=Params(s)->NormalEffectVolumeAt24();*volume=static_cast<DWORD>(Number(p,1401,s));
    selected=Params(s);const auto voice=Read(p.voiceAt72);mute=selected->VoiceMuteAt68();*mute=voice==0;
    volume=Params(s)->VoiceVolumeAt6C();*volume=static_cast<DWORD>(Number(p,1402,s));
    auto* save=Save(s,false,false,false,true);save->valueAt81=Read(p.waitAt74)!=0;
    save=Save(s,false,false,false,true);save->bytesAt84[1]=Read(p.lowerAt73)==0;
    save=Save(s,false,false,false,true);const auto lower=Number(p,1403,s);std::memcpy(reinterpret_cast<BYTE*>(save)+0x88,&lower,4);
    save=Save(s,false,false,false,true);save->bytesAt84[0]=Read(p.lowerEnabledAt75)==0;
    if(Params(s)->HasTypeGroupAt34())
    {
        for(int i=0;;++i)
        {
            if(i>=Params(s,false,true)->TypeCountAt3C())break;
            auto* params=Params(s,false,true);
            const auto state=Send(Read(p.listAt78.windowAt04),0x102C,i,0xF000,s);
            auto* typeMute=params->TypeMuteAt48(i);*typeMute=(static_cast<DWORD>(state)&0xFFFFF000u)==0x1000;
            params=Params(s,false,true);const auto value=TypeNumber(p,i,s);*params->TypeVolumeAt4C(i)=static_cast<DWORD>(value);
        }
    }
    if(Params(s,true)->CharacterGroupEnabledAt70())
    {
        const auto root=Next(p,0,nullptr,s);auto* params=Params(s,true);
        if(!CategorizedVoiceUi())
        {
            const auto rootState=TreeState(p,root,s)&0xF000;*params->CharacterGroupUsingAt74()=rootState==0x2000;
        }
        auto group=Next(p,4,root,s);
        for(int i=0;;++i)
        {
            const auto count=Params(s)->CharacterCountAt78();if(i>=count||!group)break;
            if(CategorizedVoiceUi())static_cast<void>(CategorizedVoiceLeaf(p,group,s));
            else
            {
                auto state=static_cast<WORD>(TreeState(p,group,s))>>12;if(state>2)state-=2;
                *Params(s)->CharacterUsingAt7C(i)=state==2;
            }
            for(auto child=Next(p,4,group,s);child;child=Next(p,1,child,s))
            {
                const auto data=TreeData(p,child,s);
                if(!data||data>=Count12(Read(p.voicesAt58.end),Read(p.voicesAt58.begin)))continue;
                auto childState=(TreeState(p,child,s)>>12)&15;if(childState>2)childState-=2;
                params=Params(s,false,true);const auto* key=Voice(p,data)->keyAt08.GetString();
                *params->RegistrationMuteAt90(i,key)=childState==1;
                params=Params(s,false,true);key=Voice(p,data)->keyAt08.GetString();auto* fixed=Voice(p,data);
                auto* destination=params->RegistrationVolumeAt8C(i,key);*destination=Read(fixed->volumeAt0C);
            }
            group=Next(p,1,group,s);
        }
    }
    return TRUE;
}
}
