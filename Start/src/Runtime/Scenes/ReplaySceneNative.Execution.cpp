#include "ReplaySceneNative.Internal.h"
#include <Runtime/ReplaySceneNativeGlobals.h>
#include <Runtime/ScrVmScriptCommandState.h>

namespace KaGuYa::Start::Runtime::ReplaySceneNativeDetail
{
const char* CgName()noexcept{return "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";}
const char* ScriptName()noexcept{return "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";}
namespace
{
void StartList(Body& body,ReplaySceneNativeServices& services)
{
    auto& source=services.source;ResetOperation(body.operationAt28);body.operationAt28.opcode=57;
    body.operationAt28.source=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt74);body.stepAt58=20;
    auto* parameters=Resolve<void>(Field<Slot>(Registry(source,1,0x4B807B),0x1C));
    const auto* name=ProcessReplaySceneNativeBackgroundMusic().GetString();
    auto* sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B80B5)));
    auto* table=Read(Field<void*>(parameters,0));
    const auto volume=Read(Field<const int*(__thiscall*)(void*)>(table,0x14));
    const auto value=Read(*volume(parameters));
    auto& mapping=Field<RuntimeCString>(Registry(source,2,0x4B80F1),0x2C);
    source.PlayMusicAt4BCCC0(sound,mapping,name,value,1);
    sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B810D)));source.ClearVoicesAt4384D0(sound);
    sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B8143)));
    auto& voices=Field<EnvironmentVectorStorage<RuntimeCString>>(Registry(source,2,0x4B8171),0x30);
    source.StopMappedEffectsAt413270(sound,voices);
    Field<unsigned>(Registry(source,2,0x4B818A),4)=0;++body.phaseAt04;
}
void Animate(Body& body,IReplaySceneNativeSource& source)
{
    auto& operation=body.operationAt28;
    if(source.AccelerateAt4AA970(Registry(source,5,0x4B81A0,true)))
    {operation.current=Add(Read(operation.current),256);if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);}
    if(Read(operation.opcode)==-1||Read(operation.current)>=Read(operation.target))++body.phaseAt04;
    auto* draw=Draw(source,0x4B821F);
    source.RenderAt449DD0(Resolve<void>(Field<Slot>(draw,0x20)),operation);
    operation.current=Add(Read(operation.current),Read(body.stepAt58));
    if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);
    source.InvalidateAfter4182F0(0x4B8284,nullptr);
}
void CaptureBlack(Body& body,IReplaySceneNativeSource& source)
{
    CaptureScreen(body,source,0x4B7E04);Field<unsigned>(Registry(source,2,0x4B7E8C),4)=0;
    ResetOperation(body.operationAt28);body.operationAt28.opcode=0;
    body.operationAt28.source=Resolve<GraphicsSurfaceNativeBody>(body.blackAt5C);
    const auto previous=Read(Resolve<GraphicsSurfaceNativeBody>(body.capturedAt68)->handleAt04);
    ++body.phaseAt04;body.operationAt28.previous=previous;body.stepAt58=16;
}
bool FinishReplay(Body& body,ReplaySceneNativeServices& services)
{
    auto& source=services.source;
    if(!Read(Field<unsigned>(Scenes(source,0x4B7D2C),0x40)))
    {
        body.segmentAt18=Add(Read(body.segmentAt18),1);
        const auto segment=Read(body.segmentAt18),item=Read(body.itemAt14),group=Read(body.groupAt0C);
        (void)Registry(source,4,0x4B7D73);Slot selected;
        FindReplaySelectionNativeAt4ADAE0(selected,source,group,item,segment,services.memory);
        struct Prefix{Slot& slot;bool active=true;~Prefix()noexcept{if(active)ReleaseReplaySelectionNativeAt4A2F90(slot);}} prefix{selected};
        if(Read(selected.baseAt00))
        {body.phaseAt04=7;prefix.active=false;ReleaseReplaySelectionNativeAt4A2F90(selected);return true;}
        Field<unsigned>(Scenes(source,0x4B7F72),0x40)=1;
        prefix.active=false;ReleaseReplaySelectionNativeAt4A2F90(selected);
    }
    ResetReplayButtonNativeAt4C1260(*Group(body),0);
    auto* group=Group(body);auto* target=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt74);
    GroupMethods(*group)->compositeAt38(group,target,nullptr);
    body.phaseAt04=0;source.WriteRootByte(Root(source,0x4B802E),0x218,1);
    // 原4B803F直接fallthrough，不因Root回调修改phase而改走其它状态。
    StartList(body,services);Animate(body,source);return false;
}
}
}
namespace KaGuYa::Start::Runtime
{
void UpdateReplaySceneNativeAt4B7CE0(ReplaySceneNativeBody80h& body,ReplaySceneNativeServices& services)
{
    using namespace ReplaySceneNativeDetail;ReplaySceneNativeScope scope(services);auto& source=services.source;
    for(;;)
    {
        switch(Read(body.phaseAt04))
        {
        case 0:StartList(body,services);Animate(body,source);return;
        case 1:case 5:case 8:case 10:case 13:Animate(body,source);return;
        case 2:
            Field<unsigned>(Registry(source,2,0x4B82A4),4)=3;
            Field<unsigned>(Registry(source,2,0x4B82B9),0x14)=1;body.phaseAt04=3;[[fallthrough]];
        case 3:
            if(PollSelection(body,services)){auto* group=Group(body);GroupMethods(*group)->drawAt34(group);}return;
        case 4:
            ResetOperation(body.operationAt28);body.operationAt28.opcode=58;
            body.operationAt28.source=Resolve<GraphicsSurfaceNativeBody>(body.blackAt5C);
            body.stepAt58=20;body.phaseAt04=5;Animate(body,source);return;
        case 6:
        {
            const auto route=Read(body.routeAt10);
            const auto site=route==0?0x4B8785u:route==2?0x4B8779u:0x4B876Du;
            source.QueueSceneAt47F920(Scenes(source,site,0x4B878F),route==0?CgName():route==2?CgSceneNativeDetail::MusicName():CgSceneNativeDetail::TitleName());return;
        }
        case 7:case 12:CaptureBlack(body,source);continue;
        case 9:PrepareReplay(body,services);return;
        case 11:
        {
            auto* info=Resolve<void>(source.RootInformationSlotAt3C(Root(source,0x4B8B66)));
            Field<unsigned>(info,0x10)=1;
            auto* state=Registry(source,2,0x4B8B9A);Field<unsigned>(state,4)=Read(body.savedModeAt08);
            source.PushSceneAt4A9030(Scenes(source,0x4B8BB3),ScriptName());body.phaseAt04=12;
            const auto script=source.ScriptAt47CD00(Root(source,0x4B8BF5));script.body->commandAt04.reload[0]=1;
            script.publication->commandMask|=ScrVmScriptCommandState::ReloadByteAt08Mask;
            source.WriteRootByte(Root(source,0x4B8C0E),0x218,0);
            source.WriteRootByte(Root(source,0x4B8C24),0x219,1);return;
        }
        case 14:if(FinishReplay(body,services))continue;return;
        default:return; // 原switch域外直接返回，不制造额外业务或恢复状态。
        }
    }
}
}
