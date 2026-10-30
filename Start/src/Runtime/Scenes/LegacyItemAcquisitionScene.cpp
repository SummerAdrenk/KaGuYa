#include <Runtime/LegacyItemAcquisitionNative.h>
#include <new>
#include <stdexcept>
namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint8_t __fastcall Run(LegacyItemAcquisitionScene* b,void*,int)
{b->vptr->update(b);b->vptr->draw(b);return 1;}
LegacyItemAcquisitionScene* __fastcall Delete(LegacyItemAcquisitionScene* b,void*,unsigned flags) noexcept
{DestroyLegacyItemAcquisitionScene(*b);if(flags&1u)::operator delete(b,sizeof(*b));return b;}
void __fastcall Update(LegacyItemAcquisitionScene* b,void*){UpdateLegacyItemAcquisitionScene(*b);}
void __fastcall Draw(LegacyItemAcquisitionScene*,void*){} // 原0x5069F0为空；页面由GUI poll/转场提交。
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const LegacyItemAcquisitionSceneVtable Methods{
    reinterpret_cast<decltype(Methods.run)>(&Run),reinterpret_cast<decltype(Methods.destroy)>(&Delete),
    reinterpret_cast<decltype(Methods.update)>(&Update),reinterpret_cast<decltype(Methods.draw)>(&Draw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
bool PreserveSkip(SaveDataEnvironmentArchive& s){return (s.bytesAt34[4]&8u)!=0;}
}
LegacyItemAcquisitionScene* ConstructLegacyItemAcquisitionScene(void* raw,LegacyItemAcquisitionServices& s)
{
    RequireLegacyItemAcquisitionProfile(s.profile);
    auto* b=::new(raw) LegacyItemAcquisitionScene{&Methods,&s,nullptr,0,0};
    auto* gui=::operator new(LegacyItemAcquisitionDialogStorageSize());
    try{b->dialog=ConstructLegacyItemAcquisitionDialog(gui,s);}catch(...){::operator delete(gui);throw;}
    try
    {
        // 原0x495F90：只有GUI构造完成后才改变环境；保存完整skip，不重造旧1Ch布局。
        s.source.UnSave().FlagsAt18()&=2u;
        s.source.UnSave().DwordAt0C()&=0xFFFFFF00u;
        b->savedSkip=s.source.UnSave().FlagsAt14();s.source.UnSave().FlagsAt14()&=2u;
        s.source.ResetScriptState();return b;
    }
    catch(...){DestroyLegacyItemAcquisitionScene(*b);throw;}
}
void UpdateLegacyItemAcquisitionScene(LegacyItemAcquisitionScene& b)
{
    auto& source=b.services->source;auto& gui=*b.dialog;
    if(b.phase==0){source.ClearModalResult();gui.vptrAt00->beginAt44(&gui);b.phase=1;}
    if(b.phase==12)
    {
        source.UnSave().FlagsAt14()=PreserveSkip(source.SaveData())?b.savedSkip&2u:0;
        ReopenLegacyItemAcquisitionSystem(gui);b.phase=1;
    }
    if(b.phase==2)
    {
        // 原0x4961D0 case2先reset draw/pop，再恢复等待/auto/skip。没有写menu_result/prog_result。
        source.Finish();source.UnSave().ModeAt04()=1;source.UnSave().FlagsAt18()|=1u;
        source.UnSave().DwordAt0C()=(source.UnSave().DwordAt0C()&0xFFFFFF00u)|1u;
        source.UnSave().FlagsAt14()=PreserveSkip(source.SaveData())?b.savedSkip:1u;b.phase=3;return;
    }
    if(b.phase!=1)return;
    for(auto action:{LegacyItemAcquisitionAction::Save,LegacyItemAcquisitionAction::Load,LegacyItemAcquisitionAction::QuickSave,
        LegacyItemAcquisitionAction::QuickLoad,LegacyItemAcquisitionAction::Settings,LegacyItemAcquisitionAction::Title,LegacyItemAcquisitionAction::Likability})
    {
        if(!source.Requested(action))continue;
        if(action==LegacyItemAcquisitionAction::Save||action==LegacyItemAcquisitionAction::Load||action==LegacyItemAcquisitionAction::QuickSave)
            source.CapturePreview();
        source.Command("");
        switch(action)
        {
        case LegacyItemAcquisitionAction::Save:case LegacyItemAcquisitionAction::Load:source.Push(action);return;
        case LegacyItemAcquisitionAction::QuickSave:source.QuickSave();return;
        case LegacyItemAcquisitionAction::QuickLoad:
            if(source.QuickLoad()){source.QueueLoadAfter();source.UnSave().ModeAt04()=0;b.phase=3;}return;
        case LegacyItemAcquisitionAction::Settings:source.Push(action);b.phase=12;return;
        case LegacyItemAcquisitionAction::Title:
            if(source.ConfirmTitle()){source.PostTitle();source.UnSave().ModeAt04()=0;b.phase=3;}return;
        case LegacyItemAcquisitionAction::Likability:source.Push(action);return;
        }
    }
    if(!gui.vptrAt00->pollAt48(&gui)){gui.vptrAt00->endAt4C(&gui);b.phase=2;}
}
void DestroyLegacyItemAcquisitionScene(LegacyItemAcquisitionScene& b) noexcept
{if(b.dialog){b.dialog->vptrAt00->deletingDestructorAt04(b.dialog,1);b.dialog=nullptr;}}
const char* LegacyItemAcquisitionSceneName() noexcept
{return "\x83\x41\x83\x43\x83\x65\x83\x80\x8E\xE6\x93\xBE\x8F\x88\x97\x9D";}
const char* LegacyItemAcquisitionActionName(LegacyItemAcquisitionAction a)
{
    switch(a)
    {
    case LegacyItemAcquisitionAction::Save:return "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
    case LegacyItemAcquisitionAction::Load:return "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
    case LegacyItemAcquisitionAction::QuickSave:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
    case LegacyItemAcquisitionAction::QuickLoad:return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
    case LegacyItemAcquisitionAction::Settings:return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
    case LegacyItemAcquisitionAction::Title:return "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6";
    case LegacyItemAcquisitionAction::Likability:return "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
    }
    throw std::invalid_argument("Unknown item acquisition action");
}
}
