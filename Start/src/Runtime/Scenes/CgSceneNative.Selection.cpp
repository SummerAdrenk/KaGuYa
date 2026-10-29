#include "CgSceneNative.Internal.h"
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <Runtime/SecretCgNativeArchive.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::CgSceneNativeDetail
{
namespace
{
bool Compare(RuntimeCString& value,const char* name)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(value.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);
    const auto result=::_mbscmp(reinterpret_cast<const unsigned char*>(clone+1),reinterpret_cast<const unsigned char*>(name))==0;
    ReleaseRuntimeCStringHeader(clone);return result;
}
void ClearCommand(ICgSceneNativeSource& source,unsigned site)
{SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,site),0x1C),"",0);}
int PublicCount(void* scenes) noexcept
{
    auto* slot=BorrowModeButtonsNativeAt47B7B0(*static_cast<SceneManagerNativeBody48h*>(scenes));
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot->controlAt08));
    return control?std::bit_cast<int>(Read(control->countAt10)):0;
}
class Params final:public ISecretCgParamsRootSource
{
public:
    explicit Params(ICgSceneNativeSource& source):source_(source){}
    const void* CaptureAt418480(unsigned site) override{return source_.CaptureParamsAt418480(site);}
private:ICgSceneNativeSource& source_;
};
int Candidate(SecretCgNativeArchive& secret,int category,unsigned item,int ordinal)
{
    auto* end=Read(secret.mappingsAt18.end);auto* begin=Read(secret.mappingsAt18.begin);
    const auto categoryCount=std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin))>>4;
    if(category>=categoryCount)return -1;
    auto& refs=Advance(begin,category)->referencesAt04;
    auto* first=Read(refs.begin);auto* last=Read(refs.end);
    const auto count=unsigned(std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(last)-reinterpret_cast<std::uintptr_t>(first))/12);
    if(item>=count)return -1;
    auto& original=*Advance(first,std::bit_cast<int>(item));Slot temporary;
    temporary.baseAt00=Read(original.baseAt00);temporary.biasAt04=Read(original.biasAt04);temporary.controlAt08=Read(original.controlAt08);
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(temporary.controlAt08));
    if(control)++control->referenceCountAt04;
    const auto extent=control?std::bit_cast<int>(Read(control->countAt10)):0;
    int resource=-1;
    if(ordinal<extent)
    {
        // 410460把bias与ordinal先按DWORD相加，再乘当前control的stride；不是固定int步长。
        const auto base=Read(temporary.baseAt00);
        const auto* current=static_cast<EnvironmentObjectReferenceControl*>(Read(temporary.controlAt08));
        const auto offset=(Read(temporary.biasAt04)+unsigned(ordinal))*(current?Read(current->strideAt0C):0u);
        const auto* element=base?reinterpret_cast<const int*>(base+offset):nullptr;
        const auto recordId=Read(*element);
        auto& slot=*Advance(Read(secret.recordsAt0C.begin),recordId);
        auto* record=Resolve<SecretCgFlagRecord>(slot);
        if(Read(record->byteFlagAt04))resource=std::bit_cast<int>(Read(Resolve<SecretCgFlagRecord>(slot)->idAt00));
    }
    // 原40CF70与本函数内纯读取之间没有新增EH owner；其正常析构失败终止。
    const auto normal=[](Slot& value) noexcept{ReleaseYtlReference(value);};normal(temporary);return resource;
}
bool Select(Body& body,int item,CgSceneNativeServices& services)
{
    auto& source=services.source;
    ResetCgOperationDialogNativeAt502110(*Resolve<CgOperationDialogNativeBodyA8h>(Operation(body)->dialogAt24));
    ResetCgSelectionNativeAt4B7433(body.selectionAt18);
    Params params(source);
    const auto initialCategory=Read(body.groupAt10);
    auto* secret=static_cast<SecretCgNativeArchive*>(Registry(source,3,0x4B746D));
    if(!ReadCgSelectionNativeAt4AD780(*secret,params,initialCategory,unsigned(item),
        &body.selectionAt18.countAt00,&body.selectionAt18.positionAt08,&body.selectionAt18.valueAt04))return false;
    for(int ordinal=0;ordinal<Read(body.selectionAt18.countAt00);ordinal=Add(ordinal,1))
    {
        // 4B74A0先固定group，然后捕获Secret；不合并为整个Select的一次根查询。
        const auto group=Read(body.groupAt10);secret=static_cast<SecretCgNativeArchive*>(Registry(source,3,0x4B74A5));
        const auto resource=Candidate(*secret,group,unsigned(item),ordinal);
        if(resource!=-1)AppendCgSelectionPairNativeAt4B7584(body.selectionAt18.pairsAt14,{ordinal,resource},services.memory);
        AppendCgOperationAvailabilityNativeAt4FEA00(*Resolve<CgOperationDialogNativeBodyA8h>(Operation(body)->dialogAt24),resource!=-1);
    }
    ResetDrawLayersAt47E940(*static_cast<DrawManagerNativeBody2Ch*>(Draw(source,0x4B7611)));
    DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(Draw(source,0x4B7647)),0)->enabledAt0C=0;
    body.phaseAt04=7;return true;
}
bool TitleRequested(void* scenes,ICgSceneNativeSource& source)
{
    auto& command=Field<RuntimeCString>(scenes,0x1C);
    union Text{RuntimeCString value;Text(){}~Text(){}} temporary;
    auto* clone=CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(command.GetString()))-1);
    auto* chars=reinterpret_cast<char*>(clone+1);std::memcpy(&temporary.value,&chars,4);
    struct Unwind final{RuntimeCString& value;bool active=true;~Unwind() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}} cleanup{temporary.value};
    bool result=::_mbscmp(reinterpret_cast<const unsigned char*>(temporary.value.GetString()),reinterpret_cast<const unsigned char*>(TitleName()))==0;
    if(!result&&CurrentAppreciationSceneProfile().listRightClickToTitle)
    {
        auto* input=source.CaptureInputAt418210(0x4B779B);auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
        result=!Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28);
    }
    cleanup.active=false;DestroyRuntimeCStringAt40CD40(&temporary.value);return result;
}
}
bool PollSelection(Body& body,CgSceneNativeServices& services)
{
    auto& source=services.source;
    if(Compare(Field<RuntimeCString>(Scenes(source,0x4B71B1),0x1C),SettingName()))
    {ClearCommand(source,0x4B7223);source.PushSceneAt4A9030(Scenes(source,0x4B7263),SettingName());return false;}
    auto* group=Group(body);const auto result=GroupMethods(*group)->processAt04(group);
    auto& current=*Advance(Read(body.groupsAt38.begin),Read(body.groupAt10));const auto base=Read(current.baseAt00);
    if(result>=Read(Field<int>(ResolveWithBase<void>(current,base),0x3C)))
    {
        auto route=Sub(result,Read(Field<int>(ResolveWithBase<void>(current,base),0x3C)));
        if(route<PublicCount(Scenes(source,0x4B734D))){body.routeAt14=route;body.phaseAt04=4;}
        else
        {
            const auto items=Read(Field<int>(Group(body),0x3C));
            const auto index=Sub(Sub(result,PublicCount(Scenes(source,0x4B73AD))),items);
            if(Read(body.groupAt10)!=index){body.groupAt10=index;ResetCgButtonNativeAt4C1260(*Group(body),1);}
        }
    }
    else if(result>=0){if(!Select(body,result,services))return false;}
    else
    {
        const bool scene=Compare(Field<RuntimeCString>(Scenes(source,0x4B768F),0x1C),SceneName());
        auto* currentScenes=Scenes(source,0x4B76DE);
        if(scene){SetRuntimeCStringAt437680(Field<RuntimeCString>(currentScenes,0x1C),"",0);body.routeAt14=1;body.phaseAt04=4;}
        else
        {
            const bool music=Compare(Field<RuntimeCString>(currentScenes,0x1C),MusicName());
            currentScenes=Scenes(source,0x4B773C);
            if(music){SetRuntimeCStringAt437680(Field<RuntimeCString>(currentScenes,0x1C),"",0);body.routeAt14=2;body.phaseAt04=4;}
            else if(TitleRequested(currentScenes,source)){ClearCommand(source,0x4B77D0);body.routeAt14=3;body.phaseAt04=4;}
        }
    }
    return true;
}
}
