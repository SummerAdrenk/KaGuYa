#include "ReplaySceneNative.Internal.h"
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::ReplaySceneNativeDetail
{
namespace
{
bool Compare(RuntimeCString& value,const char* name)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(value.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);
    const bool result=::_mbscmp(reinterpret_cast<const unsigned char*>(clone+1),reinterpret_cast<const unsigned char*>(name))==0;
    ReleaseRuntimeCStringHeader(clone);return result;
}
int PublicCount(void* scenes)noexcept
{
    auto* slot=BorrowModeButtonsNativeAt47B7B0(*static_cast<SceneManagerNativeBody48h*>(scenes));
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot->controlAt08));
    return control?std::bit_cast<int>(Read(control->countAt10)):0;
}
void ClearCommand(IReplaySceneNativeSource& source,unsigned site)
{SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,site),0x1C),"",0);}
bool TitleRequested(void* scenes,IReplaySceneNativeSource& source)
{
    auto& command=Field<RuntimeCString>(scenes,0x1C);
    auto* clone=CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(command.GetString()))-1);
    union Text{RuntimeCString value;Text(){}~Text(){}} text;
    // Clone引用以原生指针进入空槽，仍由prefix按原时点释放。
    auto* chars=reinterpret_cast<char*>(clone+1);std::memcpy(static_cast<void*>(&text.value),&chars,4);
    struct Prefix{RuntimeCString& value;bool active=true;~Prefix()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}} prefix{text.value};
    bool result=::_mbscmp(reinterpret_cast<const unsigned char*>(text.value.GetString()),reinterpret_cast<const unsigned char*>(CgSceneNativeDetail::TitleName()))==0;
    if(!result&&CurrentAppreciationSceneProfile().listRightClickToTitle)
    {
        auto* input=source.CaptureInputAt418210(0x4B8681);auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
        result=!Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28);
    }
    prefix.active=false;DestroyRuntimeCStringAt40CD40(&text.value);return result;
}
}
bool PollSelection(Body& body,ReplaySceneNativeServices& services)
{
    auto& source=services.source;
    if(Compare(Field<RuntimeCString>(Scenes(source,0x4B82D8),0x1C),CgSceneNativeDetail::SettingName()))
    {ClearCommand(source,0x4B834A);source.PushSceneAt4A9030(Scenes(source,0x4B838A),CgSceneNativeDetail::SettingName());return false;}
    auto* group=Group(body);const auto result=GroupMethods(*group)->processAt04(group);body.itemAt14=result;
    auto& slot=*Advance(Read(body.groupsAt1C.begin),Read(body.groupAt0C));const auto base=Read(slot.baseAt00);
    if(result>=Read(Field<int>(ResolveWithBase<void>(slot,base),0x3C)))
    {
        const auto route=Sub(result,Read(Field<int>(ResolveWithBase<void>(slot,base),0x3C)));
        if(route<PublicCount(Scenes(source,0x4B8492))){body.routeAt10=route;body.phaseAt04=4;}
        else
        {
            const auto items=Read(Group(body)->common.itemCountAt3C);
            const auto count=PublicCount(Scenes(source,0x4B84F2));
            const auto next=Sub(Sub(Read(body.itemAt14),count),items);
            if(Read(body.groupAt0C)!=next){body.groupAt0C=next;ResetReplayButtonNativeAt4C1260(*Group(body),1);}
        }
    }
    else if(result>=0)
    {body.phaseAt04=7;body.segmentAt18=0;Field<unsigned>(Scenes(source,0x4B8559),0x40)=0;}
    else
    {
        const bool cg=Compare(Field<RuntimeCString>(Scenes(source,0x4B8575),0x1C),CgName());
        auto* scenes=Scenes(source,0x4B85C4);
        if(cg){SetRuntimeCStringAt437680(Field<RuntimeCString>(scenes,0x1C),"",0);body.routeAt10=0;body.phaseAt04=4;}
        else
        {
            const bool music=Compare(Field<RuntimeCString>(scenes,0x1C),CgSceneNativeDetail::MusicName());
            scenes=Scenes(source,0x4B8622);
            if(music){SetRuntimeCStringAt437680(Field<RuntimeCString>(scenes,0x1C),"",0);body.routeAt10=2;body.phaseAt04=4;}
            else if(TitleRequested(scenes,source)){ClearCommand(source,0x4B86B6);body.routeAt10=3;body.phaseAt04=4;}
        }
    }
    return true;
}
}
