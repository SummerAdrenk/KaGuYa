#include "ProgramSceneNative.Internal.h"
#include <mbstring.h>
#include <Runtime/ProgramProfileDialogNative.h>

namespace KaGuYa::Start::Runtime::ProgramSceneNativeDetail
{
namespace
{
bool NameMatches(UpdateFrame& frame,void* scenes,unsigned nameAddress,int state)
{
    auto& text=Field<RuntimeCString>(scenes,0x1C);
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);const auto* chars=reinterpret_cast<const char*>(clone+1);
    // Clone引用只写原生指针；缓存header及state仍在发布后取得清理资格。
    std::memcpy(static_cast<void*>(&frame.text.value),&chars,4);frame.cachedText=clone;frame.state=state;
    return ::_mbscmp(reinterpret_cast<const unsigned char*>(chars),reinterpret_cast<const unsigned char*>(ProgramSceneNativeName(nameAddress)))==0;
}
void ReleaseName(UpdateFrame& frame)
{
    auto* header=frame.cachedText;
    frame.state=-1;ReleaseRuntimeCStringHeader(header);
}
struct Route final
{
    unsigned name,predicate,predicateEnsure,predicateRead,predicateCall;
    unsigned afterEnsure,yesRead,noRead,drawSlot,capture,submitEnsure,submitRead,submit;
    unsigned clearEnsure,clearRead,clearSlot,clearCall,phase;
    int stringState,refState,normalState;
};
constexpr Route routes[]{
    {0x5AE218,0x47F830,0x4B989E,0x4B98A3,0x4B98A9,0x4B98DC,0x4B98E9,0x4B99CA,0x4B98EF,0x4B9914,0x4B9925,0x4B992A,0x4B9934,0x4B993E,0x4B9943,0x4B9949,0x4B9974,4,0,1,2},
    {0x5AE224,0x47F650,0x4B9A27,0x4B9A2C,0x4B9A32,0x4B9A65,0x4B9A72,0x4B9B15,0x4B9A78,0x4B9A9D,0x4B9AAE,0x4B9AB3,0x4B9ABD,0x4B9AC7,0x4B9ACC,0x4B9AD2,0x4B9AFD,5,3,4,5},
    {0x5AE8AC,0x47F780,0x4B9B72,0x4B9B77,0x4B9B7D,0x4B9BB0,0x4B9BBD,0x4B9C6E,0x4B9BC3,0x4B9BE8,0x4B9BF9,0x4B9BFE,0x4B9C08,0x4B9C12,0x4B9C17,0x4B9C1D,0x4B9C48,6,6,7,7}};
void CaptureTransition(Body& body,ProgramSceneNativeServices& services,UpdateFrame& frame,void* captured,const Route& route)
{
    auto& source=services.source;
    auto* draw=Resolve<void>(source.RootDrawSlotAt18(captured,route.drawSlot));
    source.CaptureScreenAt4ACDD0(draw,&frame.first,route.capture);frame.state=route.refState;
    auto* root=Root(source,route.submitEnsure,route.submitRead);source.SubmitPreviewAt48CFF0(root,&frame.first,route.submit);
    root=Root(source,route.clearEnsure,route.clearRead);
    SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,root,route.clearSlot),0x1C),"",0);
    body.phaseAt04=route.phase;
    frame.state=route.normalState;
    if(route.phase==6)ReleaseNormal(frame.first);else ReleaseRetainedNormal(frame.first);
    frame.state=-1;
}
}
bool Poll(Body& b,ProgramSceneNativeServices& services,UpdateFrame& frame)
{
    if (ProgramProfileDialogHasModal(Dialog(b)))
    {
        auto* gui=Dialog(b);
        static_cast<void>(gui->vptrAt00->pollAt48(gui));
        return false;
    }
    auto& source=services.source;
    auto* root=Root(source,0x4B983C,0x4B9841);auto* scenes=Scenes(source,root,0x4B9846);
    for(const auto& route:routes)
    {
        bool matched=NameMatches(frame,scenes,route.name,route.stringState);
        if(!matched)
        {
            auto* inputRoot=Root(source,route.predicateEnsure,route.predicateRead);
            matched=source.InputPredicate(inputRoot,route.predicate,0,route.predicateCall)!=0;
        }
        ReleaseName(frame);
        // 四个输入/名称判定的正常尾都有无条件Ensure，假分支也不能省去。
        source.EnsureRootAt413BE0(route.afterEnsure);
        root=source.RootAt6198C4(matched?route.yesRead:route.noRead);
        if(matched){CaptureTransition(b,services,frame,root,route);return true;}
        scenes=Scenes(source,root,route.noRead+5);
    }
    bool matched=NameMatches(frame,scenes,0x5AE8BC,8);
    if(!matched)
    {
        auto* inputRoot=Root(source,0x4B9CCB,0x4B9CD0);
        matched=source.InputPredicate(inputRoot,0x47F6D0,0,0x4B9CD6)!=0;
    }
    ReleaseName(frame);root=Root(source,0x4B9D09,0x4B9D0E);
    scenes=Scenes(source,root,0x4B9D14);
    if(matched){SetRuntimeCStringAt437680(Field<RuntimeCString>(scenes,0x1C),"",0);b.phaseAt04=8;return true;}
    // 设置/标题的临时串没有caller EH资格，不能擅加自动释放来“修复”异常。
    matched=NameMatches(frame,scenes,0x5B4E68,-1);ReleaseName(frame);
    root=Root(source,0x4B9D84,0x4B9D89);
    if(matched)
    {
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,root,0x4B9D93),0x1C),"",0);
        root=Root(source,0x4B9DAC,0x4B9DB1);source.PushSceneAt4A9030(root,Scenes(source,root,0x4B9DBC),ProgramSceneNativeName(0x5B4E68),0x4B9DC3);
        b.phaseAt04=12;return true;
    }
    scenes=Scenes(source,root,0x4B9DE5);
    matched=NameMatches(frame,scenes,0x5B4E74,-1);ReleaseName(frame);
    root=Root(source,0x4B9E0F,0x4B9E14);
    if(matched){SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,root,0x4B9E1E),0x1C),"",0);b.phaseAt04=10;return true;}
    scenes=Scenes(source,root,0x4B9E4E);
    matched=NameMatches(frame,scenes,0x5B4E80,9);
    if(!matched)
    {
        auto* inputRoot=Root(source,0x4B9E80,0x4B9E85);
        matched=source.InputPredicate(inputRoot,0x47F530,1,0x4B9E8D)!=0;
    }
    ReleaseName(frame);
    if(matched)
    {
        if (IsProgramProfileDialog(Dialog(b)))
            throw std::logic_error("This game's Likability workflow is not profiled; refusing Mamatomo fallback");
        root=Root(source,0x4B9EB4,0x4B9EB9);SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,root,0x4B9EBF),0x1C),"",0);
        root=Root(source,0x4B9ED8,0x4B9EDD);source.PushSceneAt4A9030(root,Scenes(source,root,0x4B9EE8),ProgramSceneNativeName(0x5B4E80),0x4B9EEF);return true;
    }
    auto* gui=Dialog(b);if(Read(gui->vptrAt00)->pollAt48(gui)!=0)return false;
    gui=Dialog(b);Read(gui->vptrAt00)->endAt4C(gui);b.phaseAt04=2;return true;
}
}
