#include "LogDialogNative.Internal.h"
#include <Runtime/TextWindowSoundSource.h>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
int Count(const Text& p)
{
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(p.rowsAt68.begin));
    const auto end=reinterpret_cast<std::uintptr_t>(Read(p.rowsAt68.end));return std::bit_cast<int>(end-begin)/12;
}
Row* At(Text& p,int i){return Resolve<Row>(Read(p.rowsAt68.begin)[i]);}
void Disable(Text& p,int id)
{
    Fresh f;Find(&p.common,f.slot,id);RefLife life{f.slot};
    if(Read(f.slot.baseAt00))
    {
        auto* c=Resolve<Control>(f.slot);Read(c->vptrAt00)->resetAt08(c);
        c=Resolve<Control>(f.slot);Read(c->vptrAt00)->writeEnabledAt4C(c,0);
        c=Resolve<Control>(f.slot);Read(c->vptrAt00)->writeStateAt44(c,0);
    }
    life.TerminateNormal();
}
}
void __fastcall BeginText(Common* p,void*)
{
    auto& b=*reinterpret_cast<Text*>(p);b.positionAt74=-1;ReleaseYtlReference(b.voicesAt78);
    for(auto* at=Read(b.rowsAt68.begin);at!=Read(b.rowsAt68.end);++at)ResetRow(*Resolve<Row>(*at));
}
void UpdateRows(Text& p,int position,int redraw)
{
    if(Read(p.positionAt74)==position)return;
    {
        ScriptDwordVectorStorage keep;
        for(int slot=0;slot<4;++slot)
        {
            const auto desired=Add(position,slot);const auto count=Count(p);
            for(int i=0;i<count;++i)if(Read(At(p,i)->historyAt0C)==desired){keep.Push(static_cast<unsigned>(i));break;}
        }
        for(int i=0;i<Count(p);++i)
        {
            bool found=false;
            for(auto* k=keep.storage.begin;k!=keep.storage.end;++k)if(*k==static_cast<unsigned>(i)){found=true;break;}
            if(!found)ResetRow(*At(p,i));
        }
    } // 原537EFF撤state0并退临时DWORD vector后，才再次413BE0/47BDB0。
    auto* params=Parameters(0x537F3F);
    const auto historyEnd=reinterpret_cast<std::uintptr_t>(Read(params->historyAt3C.storage.end));
    const auto historyBegin=reinterpret_cast<std::uintptr_t>(Read(params->historyAt3C.storage.begin));
    const auto historyCount=std::bit_cast<int>(historyEnd-historyBegin)/4;
    for(int slot=0;slot<4;++slot)
    {
        const int id=10+slot,desired=Add(position,slot);
        if(slot>=historyCount){Disable(p,id);continue;} // 原比较slot，不新增position+slot的范围门控。
        int index=-1;
        for(int i=0;i<Count(p);++i)if(Read(At(p,i)->historyAt0C)==desired){index=i;break;}
        if(index>=0)
        {
            if(Read(At(p,index)->eventAt14)==id)continue;
        }
        else
        {
            for(int i=0;i<Count(p);++i)if(Read(At(p,i)->historyAt0C)<0){index=i;break;}
            if(index<0)continue;
            auto* current=Parameters(0x53823B);
            const auto address=reinterpret_cast<std::uintptr_t>(Read(current->historyAt3C.storage.begin))+static_cast<unsigned>(desired)*4u;
            const auto format=Read(*reinterpret_cast<std::uint32_t*>(address));
            RenderRow(*At(p,index),std::bit_cast<int>(format));
            At(p,index)->historyAt0C=desired;
        }
        const auto point=RowPositions()[slot];At(p,index)->xAt04=point.x;At(p,index)->yAt08=point.y;
        At(p,index)->eventAt14=id;UpdateVoiceButton(p,index);
    }
    if(redraw)Read(p.common.vptrAt00)->invalidateAt64(&p.common,p.boundsAt84);
    p.positionAt74=position;
}
void UpdateVoiceButton(Text& p,int row)
{
    Fresh f;Find(&p.common,f.slot,Read(At(p,row)->eventAt14));RefLife life{f.slot};
    if(Read(f.slot.baseAt00))
    {
        auto* c=Resolve<Control>(f.slot);Read(c->vptrAt00)->resetAt08(c);
        auto* selected=At(p,row);const auto bytes=reinterpret_cast<std::uintptr_t>(Read(selected->voicesAt18.end))-
            reinterpret_cast<std::uintptr_t>(Read(selected->voicesAt18.begin));
        if(bytes<4u){c=Resolve<Control>(f.slot);Read(c->vptrAt00)->writeEnabledAt4C(c,0);}
        else
        {
            c=Resolve<Control>(f.slot);Read(c->vptrAt00)->writeEnabledAt4C(c,1);
            auto* sound=Sound(0x53B29F);auto* parameters=Resolve<EnvironmentSoundParamsArchive>(sound->paramsAt24);bool allowed=false;
            if(parameters->CharacterGroupReadyAtA4())
            {
                for(auto* name=Read(At(p,row)->voicesAt18.begin);name!=Read(At(p,row)->voicesAt18.end);++name)
                {
                    sound=Sound(0x53B357);parameters=Resolve<EnvironmentSoundParamsArchive>(sound->paramsAt24);
                    if(parameters->CharacterResourceAllowedAtA8(name->GetString())){allowed=true;break;}
                }
            }
            c=Resolve<Control>(f.slot);Read(c->vptrAt00)->writeStateAt44(c,allowed?1:0);
        }
    }
    life.TerminateNormal(false);
}
int __fastcall PollText(Common* p,void*)
{
    auto& b=*reinterpret_cast<Text*>(p);auto& s=RequireLogDialogNativeServices().source;
    Read(p->vptrAt00)->processInputAt54(p);const auto clicked=PollEvents(b.eventsAt5C);
    if(clicked>=0)
    {
        auto* end=Read(b.rowsAt68.end);
        for(auto* at=Read(b.rowsAt68.begin);at!=end;++at)
        {
            if(Read(Resolve<Row>(*at)->eventAt14)!=clicked)continue;
            const auto bytes=reinterpret_cast<std::uintptr_t>(Read(Resolve<Row>(*at)->voicesAt18.end))-
                reinterpret_cast<std::uintptr_t>(Read(Resolve<Row>(*at)->voicesAt18.begin));
            if(bytes&0xFFFFFFFCu)
            {
                RebuildVoices(b.voicesAt78,std::bit_cast<int>(bytes)/4);
                for(unsigned i=0;;++i)
                {
                    auto* control=Read(b.voicesAt78.controlAt08);
                    const auto count=control?Read(static_cast<EnvironmentObjectReferenceControl*>(control)->countAt10):0u;
                    if(std::bit_cast<int>(i)>=std::bit_cast<int>(count))break;
                    auto* source=Read(Resolve<Row>(*at)->voicesAt18.begin)+i;
                    auto* destination=VoiceAt(b.voicesAt78,i);AssignRuntimeCStringAt410220(*destination,*source);
                }
                if(s.PlayVoicesAt42C4F0(Sound(0x52D8B1),&b.voicesAt78))s.AdjustEffectsAt4C2410(Sound(0x52D8F3));
            }
            break;
        }
    }
    if(Read(b.voicesAt78.baseAt00)&&!s.AnyVoiceAt4196C0(Sound(0x52D760)))
    {
        if(Read(Field<std::uint8_t>(Registry(1,0x52D964),0x81)))
        {
            auto* save=Registry(1,0x52D9BE,true);
            auto* parameters=Resolve<EnvironmentSoundParamsArchive>(Field<Slot>(save,0x1C));
            const auto gate=TextSoundVoiceGateReceiver::Native(parameters);
            if(gate.Allowed(&b.voicesAt78)&&s.PlayVoicesAt42C4F0(Sound(0x52D9FC),&b.voicesAt78))
                s.AdjustEffectsAt4C2410(Sound(0x52DA37));
        }
    }
    s.RefreshSoundAt433F60(Sound(0x52DA6D));return 1;
}
}
