#include <Runtime/LegacyCharacterSelectionNative.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Scene=LegacyCharacterSelectionScene;
void __fastcall Update(Scene* b,void*){UpdateLegacyCharacterSelectionScene(*b);}
void __fastcall Draw(Scene* b,void*)
{
    const auto phase=reinterpret_cast<LegacyCharacterSelectionDialog*>(b->dialog)->phase;
    if(b->services->source.UnSave().ModeAt04()!=0&&phase!=LegacyCharacterSelectionPhase::Opening&&phase!=LegacyCharacterSelectionPhase::Closing)
        b->services->source.DrawBackgroundAndDialog(*b->dialog);
}
std::uint8_t __fastcall Run(Scene* b,void*,int){UpdateLegacyCharacterSelectionScene(*b);Draw(b,nullptr);return 1;}
Scene* __fastcall Delete(Scene* b,void*,unsigned flags) noexcept
{DestroyLegacyCharacterSelectionScene(*b);if(flags&1)::operator delete(b,sizeof(Scene));return b;}
const LegacyCharacterSelectionSceneVtable methods{
    reinterpret_cast<decltype(LegacyCharacterSelectionSceneVtable::run)>(&Run),
    reinterpret_cast<decltype(LegacyCharacterSelectionSceneVtable::destroy)>(&Delete),
    reinterpret_cast<decltype(LegacyCharacterSelectionSceneVtable::update)>(&Update),
    reinterpret_cast<decltype(LegacyCharacterSelectionSceneVtable::draw)>(&Draw)};
void WaitMark(UnSaveDataRuntime& u,std::uint8_t value)
{u.DwordAt0C()=(u.DwordAt0C()&0xFFFFFF00u)|value;}
bool SystemRequest(Scene& b)
{
    auto& source=b.services->source;auto& gui=*b.dialog;
    using A=LegacyCharacterSelectionAction;
    // 六款父流程都先处理这些请求，再轮询选择结果。
    for(auto action:{A::Save,A::Load,A::QuickSave,A::QuickLoad,A::Settings,A::Title,A::Likability})
    {
        if(!source.Requested(action))continue;
        // 六款原入口先发布存档预览；取消仍保留，捕获失败前不得清掉请求。
        if(action==A::Save||action==A::Load||action==A::QuickSave)source.CapturePreview();
        source.ClearCommand();gui.vptrAt00->resetInputAt3C(&gui);
        if(action==A::Save||action==A::Load)
        {source.PushSystemScene(action);}
        else if(action==A::Settings||action==A::Likability)
        {source.PushSystemScene(action);if(action==A::Settings)b.phase=12;}
        else if(action==A::Title)
        {
            if(!source.SaveData().bytesAt77[6]||source.Confirm(1,nullptr,0x21)==6)
            {source.PostTitle();source.UnSave().ModeAt04()=0;}
        }
        else
        {
            const bool load=action==A::QuickLoad;
            // 实际共享Quick入口持有唯一确认；这里再确认会重复询问。
            if(!load)source.QuickSave();
            else if(source.QuickLoad()){source.QueueLoadAfter();source.UnSave().ModeAt04()=0;}
        }
        return true;
    }
    return false;
}
}
LegacyCharacterSelectionScene* ConstructLegacyCharacterSelectionScene(void* storage,LegacyCharacterSelectionServices& services)
{
    auto* b=::new(storage) LegacyCharacterSelectionScene{&methods,&services,nullptr,0,0};
    b->dialog=NewLegacyCharacterSelectionDialog(services);
    try
    {
        auto& u=services.source.UnSave();u.FlagsAt18()&=2u;WaitMark(u,0);
        b->savedFlags=u.FlagsAt14();u.FlagsAt14()&=2u;
        services.source.Params().messageFormatIndexAt60=-1;
        auto& script=services.source.Script();script.commandAt04.flags=0;script.commandAt04.reload[0]=0;
    }
    catch(...){b->dialog->vptrAt00->deletingDestructorAt04(b->dialog,1);b->dialog=nullptr;throw;}
    return b;
}
void UpdateLegacyCharacterSelectionScene(LegacyCharacterSelectionScene& b)
{
    auto& s=*b.services;LegacyCharacterSelectionScope scope(s);
    if(b.phase==12)
    {
        auto& u=s.source.UnSave();u.FlagsAt14()=(s.source.SaveData().bytesAt34[4]&8)?b.savedFlags&2u:0u;
        auto& dialog=*reinterpret_cast<LegacyCharacterSelectionDialog*>(b.dialog);
        for(auto& c:dialog.controls)if(c.body->eventAt04==s.profile.skipEvent)
            reinterpret_cast<GuiCheckBoxNativeBody60h*>(c.body)->checkedAt5C=(u.FlagsAt14()&2)!=0;
        b.dialog->vptrAt00->resetInputAt3C(b.dialog);const auto size=s.source.ScreenSize();RECT r{0,0,size.cx,size.cy};
        auto* dirty=b.dialog->vptrAt00->dirtyAt00(b.dialog);dirty->vptrAt00->addRectAt08(dirty,&r);b.phase=1;
    }
    if(b.phase==0)
    {s.source.PrepareLayers();b.dialog->vptrAt00->beginAt44(b.dialog);b.phase=1;}
    if(b.phase==1)
    {
        if(SystemRequest(b))return;
        auto& dialog=*reinterpret_cast<LegacyCharacterSelectionDialog*>(b.dialog);
        if(dialog.phase==LegacyCharacterSelectionPhase::Confirming)
        {
            const auto result=s.source.Confirm(s.profile.confirmationMode,s.profile.confirmationFallback,0x21);
            ResolveLegacyCharacterSelectionConfirmation(dialog,result);return;
        }
        if(!b.dialog->vptrAt00->pollAt48(b.dialog))
        {b.dialog->vptrAt00->endAt4C(b.dialog);b.phase=2;}
        return;
    }
    if(b.phase==2)
    {
        const auto saved=b.savedFlags;s.source.FinishScene();
        auto& u=s.source.UnSave();u.ModeAt04()=1;u.FlagsAt18()|=1;WaitMark(u,1);
        u.FlagsAt14()=(s.source.SaveData().bytesAt34[4]&8)?saved:1;
    }
}
void DestroyLegacyCharacterSelectionScene(LegacyCharacterSelectionScene& b) noexcept
{if(b.dialog){b.dialog->vptrAt00->deletingDestructorAt04(b.dialog,1);b.dialog=nullptr;}}
}
