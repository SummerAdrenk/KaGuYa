#include "SettingDialogNativeApply.Internal.h"

namespace KaGuYa::Start::Runtime::SettingDialogNativeApplyDetail
{
namespace
{
void MusicAt42BFE0(Sound& body,const char* name,int value,int loop)
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(body.slotsEndAt08));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.slotsBeginAt04));
    if(((end-begin)&0xFFFFFFF8u)==0)return;
    auto* params=Resolve<Params>(body.paramsAt24);if(!params->MusicActiveAt18())return;
    const auto slot=Read(body.musicAt10);
    auto* current=reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(Read(body.slotsBeginAt04))+slot*8u);
    if(!name)throw ATL::CAtlException(E_FAIL);
    if(_mbsicmp(reinterpret_cast<const unsigned char*>(current->nameAt04.GetString()),reinterpret_cast<const unsigned char*>(name)))
        RequireSettingDialogNativeApplyServices().open.OpenAt42BC90(body,Read(body.musicAt10),name,value,loop,0);
    // 42BFE0不写Environment的BGM名，不用4BCCC0的先提交语义代替。
}
}
void ApplySoundPrefix(Body&)
{
    auto& s=RequireSettingDialogNativeApplyServices();s.playback.StopAndClearAt4384D0(*RootSound(0x51CFF2));
    s.music.StopEffectsAt413270(*RootSound(0x51D028),nullptr);
    if(Read(*SavedParams(0x51D05B,false,ArchiveOrder::EndFirst)->MusicMuteAt10()))
    {
        auto* fixed=RootSound(0x51D0D6);s.music.StopAbsoluteAt4383B0(*fixed,Read(fixed->musicAt10));
    }
    else
    {
        if(Field<RuntimeCString>(Registry(2,0x51D11C),0x2C).GetLength())
        {
            auto* params=SavedParams(0x51D137);const char* held=Field<RuntimeCString>(Registry(2,0x51D166),0x2C).GetString();
            auto* fixed=RootSound(0x51D17A);const int value=std::bit_cast<int>(Read(*params->MusicVolumeAt14()));
            MusicAt42BFE0(*fixed,held,value,1);
        }
    }
    auto* params=SavedParams(0x51D1BC,false,ArchiveOrder::EndFirst);auto* fixed=RootSound(0x51D22D);
    const auto volume=std::bit_cast<int>(Read(*params->MusicVolumeAt14()));
    auto* slot=reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(Read(fixed->slotsBeginAt04))+Read(fixed->musicAt10)*8u);
    const int handle=Read(slot->handleAt00);s.sound.SetVolume(handle,100,volume);
    // 原先固定params/manager，volume getter返回后才读handle，Sound模块随后才捕获。
}
void RestoreFirstMappings(Body& b,unsigned& index)
{
    auto& s=RequireSettingDialogNativeApplyServices();index=0;
    for(;;++index)
    {
        auto* system=GameSystem(0x51D29E);const int count=Read(Field<int>(system,0x50));
        if(std::bit_cast<int>(index)>=count)break;
        const char* held=SnapshotAt(b,index)->GetString();
        if(!HeaderLength(held))continue;
        auto* fixed=RootSound(0x51D378);s.effects.RestoreFirstAt42C1B0(*fixed,held,index,0);
        auto* oldSource=SnapshotAt(b,index); // 必须在inline Environment捕获之前固定旧Snapshot槽地址。
        auto* registry=Registry(2,0x51D3CF,true,ArchiveOrder::EndFirst);
        auto* destination=reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(Read(Field<RuntimeCString*>(registry,0x30)))+index*4u);
        AssignMapping(*destination,*oldSource);
    }
}
void RestoreRemainingMappings(Body& b,unsigned first)
{
    auto& s=RequireSettingDialogNativeApplyServices();unsigned index=first;
    while(std::bit_cast<int>(index)<SnapshotCount(b))
    {
        const char* initial=SnapshotAt(b,index)->GetString();
        if(HeaderLength(initial))
        {
            auto* loopProbe=SavedParams(0x51D5CF,true,ArchiveOrder::BeginFirst);
            if(loopProbe->HasLoopEffectAt28())
            {
                if(Read(*SavedParams(0x51D74B,true,ArchiveOrder::BeginFirst)->LoopEffectMuteAt2C()))
                {
                    auto* volumeReceiver=SavedParams(0x51D8C6,true,ArchiveOrder::BeginFirst);
                    auto* system=GameSystem(0x51DA33);
                    const char* name=SnapshotAt(b,index)->GetString();auto* manager=RootSound(0x51DB0B);
                    const auto fallback=Read(*volumeReceiver->LoopEffectVolumeAt30());
                    // groupIndex在volume getter之后读取已固定的旧GameSystem +50。
                    const unsigned group=index-static_cast<unsigned>(Read(Field<int>(system,0x50)));
                    const auto end=reinterpret_cast<std::uintptr_t>(Read(manager->slotsEndAt08));
                    const auto begin=reinterpret_cast<std::uintptr_t>(Read(manager->slotsBeginAt04));
                    if(((end-begin)&0xFFFFFFF8u)!=0)
                    {
                        auto* parameters=Resolve<Params>(manager->paramsAt24);const bool typed=parameters->HasLoopEffectAt28();
                        bool allowed;
                        if(typed)allowed=Read(*Resolve<Params>(manager->paramsAt24)->LoopEffectMuteAt2C())==0;
                        else allowed=Resolve<Params>(manager->paramsAt24)->TypeAllowedAt58(name);
                        if(allowed)
                        {
                            // 51DBEE先拒绝空名，51DBF6才取目标槽；不同于42C070/42BFE0。
                            if(!name)throw ATL::CAtlException(E_FAIL);
                            const unsigned slotIndex=group+Read(manager->secondEffectsBeginAt1C);
                            auto* slot=reinterpret_cast<SoundManagerNativeSlot8h*>(reinterpret_cast<std::uintptr_t>(Read(manager->slotsBeginAt04))+slotIndex*8u);
                            if(_mbsicmp(reinterpret_cast<const unsigned char*>(slot->nameAt04.GetString()),reinterpret_cast<const unsigned char*>(name)))
                            {
                                const bool currentTyped=Resolve<Params>(manager->paramsAt24)->HasLoopEffectAt28();
                                auto* current=ResolveSoundParamsAt418560(*manager);
                                const int configured=currentTyped?std::bit_cast<int>(Read(*current->LoopEffectVolumeAt30())):current->TypeResourceValueAt5C(name);
                                const auto chosen=configured<0?fallback:static_cast<unsigned>(configured);
                                s.open.OpenAt42BC90(*manager,group+Read(manager->secondEffectsBeginAt1C),name,std::bit_cast<int>(chosen),1,0);
                            }
                        }
                    }
                }
                // 原HasLoop=true && mute==0直接到公共赋值，不走NormalEffect分支。
            }
            else
            {
                if(Read(*SavedParams(0x51DC81,false,ArchiveOrder::EndFirst)->NormalEffectMuteAt20()))
                {
                    auto* volumeReceiver=SavedParams(0x51DCFB,false,ArchiveOrder::EndFirst);auto* system=GameSystem(0x51DD7F);
                    const char* name=SnapshotAt(b,index)->GetString();auto* manager=RootSound(0x51DE57);
                    const auto value=std::bit_cast<int>(Read(*volumeReceiver->NormalEffectVolumeAt24()));
                    const unsigned group=index-static_cast<unsigned>(Read(Field<int>(system,0x50)));
                    s.effects.RestoreSecondAt42C070(*manager,name,group,value);
                }
            }
            auto* oldSource=SnapshotAt(b,index);
            auto* registry=Registry(2,0x51DEA9,false,ArchiveOrder::EndFirst);
            auto* destination=reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(Read(Field<RuntimeCString*>(registry,0x30)))+index*4u);
            AssignMapping(*destination,*oldSource);
        }
        ++index;
    }
}
}
