#include <Runtime/LegacyMapSelectionNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Scene=LegacyMapSelectionScene;
void __fastcall Update(Scene* b,void*){UpdateLegacyMapSelectionScene(*b);}
void __fastcall Draw(Scene*,void*){} // 十款原Draw均null；绘图属于GUI Poll，不能覆盖Transit。
std::uint8_t __fastcall Run(Scene* b,void*,int){UpdateLegacyMapSelectionScene(*b);return 1;}
Scene* __fastcall Delete(Scene* b,void*,unsigned flags) noexcept
{DestroyLegacyMapSelectionScene(*b);if(flags&1)::operator delete(b,sizeof(Scene));return b;}
const LegacyMapSelectionSceneVtable methods{
    reinterpret_cast<decltype(LegacyMapSelectionSceneVtable::run)>(&Run),
    reinterpret_cast<decltype(LegacyMapSelectionSceneVtable::destroy)>(&Delete),
    reinterpret_cast<decltype(LegacyMapSelectionSceneVtable::update)>(&Update),
    reinterpret_cast<decltype(LegacyMapSelectionSceneVtable::draw)>(&Draw)};
void WaitMark(UnSaveDataRuntime& u,std::uint8_t value)
{u.DwordAt0C()=(u.DwordAt0C()&0xFFFFFF00u)|value;}
bool SystemRequest(Scene& b)
{
    // 04:4964A0、05:482620及其余八款先处理同序系统请求，再轮询地图。
    auto& source=b.services->source;using A=LegacyMapSelectionAction;
    for(auto action:{A::Save,A::Load,A::QuickSave,A::QuickLoad,A::Settings,A::Title,A::Likability})
    {
        if(!source.Requested(action))continue;
        // 04:49684A/05:482B70至15:47C2D5均先capture->SetPreview，再Clear；失败保留请求与输入。
        if(action==A::Save||action==A::Load||action==A::QuickSave)source.CapturePreview();
        source.ClearCommand();auto& gui=*reinterpret_cast<LegacyMapSelectionDialog*>(b.dialog);
        for(auto& panel:gui.panels)panel->common.vptrAt00->resetInputAt3C(&panel->common);
        if(action==A::Save||action==A::Load)
        {source.PushSystemScene(action);}
        else if(action==A::Settings||action==A::Likability)
        {source.PushSystemScene(action);if(action==A::Settings)b.phase=12;}
        else if(action==A::Title)
        {
            if(!source.SaveData().bytesAt77[6]||source.Confirm(1,0x21)==6)
            {source.PostTitle();source.UnSave().ModeAt04()=0;}
        }
        else
        {
            const bool load=action==A::QuickLoad;
            // 原04-12分阶段确认由现代共享Quick Entries承载；此处不能重复模式3/4确认。
            if(!load)source.QuickSave();
            else if(source.QuickLoad()){source.QueueLoadAfter();source.UnSave().ModeAt04()=0;}
        }
        return true;
    }
    return false;
}
}
LegacyMapSelectionScene* ConstructLegacyMapSelectionScene(void* storage,LegacyMapSelectionServices& services)
{
    auto* b=::new(storage) LegacyMapSelectionScene{&methods,&services,nullptr,0,0};
    b->dialog=NewLegacyMapSelectionDialog(services);
    try
    {
        auto& u=services.source.UnSave();u.FlagsAt18()&=2u;WaitMark(u,0);
        b->savedFlags=u.FlagsAt14();u.FlagsAt14()&=2u;services.source.Params().messageFormatIndexAt60=-1;
        auto& script=services.source.Script();script.commandAt04.flags=0;script.commandAt04.reload[0]=0;
    }
    catch(...){b->dialog->vptrAt00->deletingDestructorAt04(b->dialog,1);b->dialog=nullptr;throw;}
    return b;
}
void UpdateLegacyMapSelectionScene(LegacyMapSelectionScene& b)
{
    auto& s=*b.services;LegacyMapSelectionScope scope(s);
    if(b.phase==12)
    {
        s.source.UnSave().FlagsAt14()=(s.source.SaveData().bytesAt34[4]&8)?b.savedFlags&2u:0u;
        ResumeLegacyMapSelectionSettings(*reinterpret_cast<LegacyMapSelectionDialog*>(b.dialog));b.phase=1;
    }
    if(b.phase==0){s.source.PrepareLayers();b.dialog->vptrAt00->beginAt44(b.dialog);b.phase=1;}
    if(b.phase==1)
    {
        if(SystemRequest(b))return;
        if(!b.dialog->vptrAt00->pollAt48(b.dialog)){b.dialog->vptrAt00->endAt4C(b.dialog);b.phase=2;}
        return;
    }
    if(b.phase==2)
    {
        // Finish可从SceneManager退役本对象；其后只用先捕获的服务与savedFlags。
        const auto saved=b.savedFlags;s.source.FinishScene();auto& u=s.source.UnSave();
        u.ModeAt04()=1;u.FlagsAt18()|=1;WaitMark(u,1);u.FlagsAt14()=(s.source.SaveData().bytesAt34[4]&8)?saved:1;
    }
}
void DestroyLegacyMapSelectionScene(LegacyMapSelectionScene& b) noexcept
{if(b.dialog){b.dialog->vptrAt00->deletingDestructorAt04(b.dialog,1);b.dialog=nullptr;}}
}
