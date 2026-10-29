#include "ProgramSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime::ProgramSceneNativeDetail
{
void ApplySelection(Body& b,ProgramSceneNativeServices& services,UpdateFrame& frame,bool quickLoad)
{
    auto& source=services.source;auto* gui=Dialog(b);
    using Capture=Slot*(__thiscall*)(void*,Slot*,int);
    Virtual<Capture>(gui,0x9C)(gui,&frame.first,1);frame.state=quickLoad?13:10;
    Copy(frame.argument,frame.first);frame.state=quickLoad?14:11;
    auto* root=Root(source,quickLoad?0x4BA23C:0x4BA17E,quickLoad?0x4BA241:0x4BA183);
    frame.state=quickLoad?13:10;source.SetTransitionAt4C1C60(root,frame.argument,quickLoad?0x4BA24B:0x4BA18D);
    root=Root(source,quickLoad?0x4BA255:0x4BA197,quickLoad?0x4BA25A:0x4BA19C);
    auto* scenes=Scenes(source,root,quickLoad?0x4BA260:0x4BA1A2);
    if(!quickLoad){source.QuickSaveAt485FC0(scenes,0,0x4BA1C5);b.phaseAt04=1;}
    else if(!source.QuickLoadAt485CE0(scenes,0x4BA281))b.phaseAt04=1;
    else
    {
        root=Root(source,0x4BA293,0x4BA298);source.ClearScenesAt4C1460(root,Scenes(source,root,0x4BA29E),0x4BA2BF);
        root=Root(source,0x4BA2C9,0x4BA2CE);source.QueueSceneAt47F920(root,Scenes(source,root,0x4BA2D4),ProgramSceneNativeName(0x5B4DB0),0x4BA2FA);
        Field<unsigned>(Archive(source,2,0x4BA301),4)=0;
    }
    frame.state=quickLoad?15:12;ReleaseRetainedNormal(frame.first);frame.state=-1;
}
void Modal(Body& b,ProgramSceneNativeServices& services,UpdateFrame& frame)
{
    auto& source=services.source;
    if(Read(Field<std::uint8_t>(Archive(source,1,0x4BA32B),0x7D)))
    {
        auto* raw=services.memory.AllocateBody(0x84);frame.rawConfirmation=static_cast<GuiDialogNativeBody5Ch*>(raw);frame.state=16;
        auto* confirm=raw?source.ConstructConfirmAt4E3330(raw,1):nullptr;
        frame.state=-1;ConstructGeneralConfirmDialogNativeReferenceAt4515E0(frame.confirmation,confirm,1);frame.state=17;
        using Capture=Slot*(__thiscall*)(void*,Slot*,int);auto* gui=Dialog(b);
        Virtual<Capture>(gui,0x9C)(gui,&frame.second,1);frame.state=18;
        Copy(frame.argument,frame.second);
        // 48CA30的按值参数在callee内部取得独立展开资格并消费；caller不再减持。
        source.AssignConfirmAt48CA30(Resolve<GuiDialogNativeBody5Ch>(frame.confirmation),frame.argument,0x4BA3F1);
        auto* root=Root(source,0x4BA3FB,0x4BA400);
        auto* information=Resolve<void>(source.RootInformationSlotAt3C(root,0x4BA406));
        const auto accepted=source.RunModalAt5071D0(information,&frame.confirmation,0x4BA42B)==6;
        frame.state=19;ReleaseNormal(frame.second);
        frame.state=20;ReleaseNormal(frame.confirmation);frame.state=-1;
        if(!accepted){b.phaseAt04=1;return;}
    }
    auto* environment=source.CaptureEnvironmentAt4180C0(0x4BA4B8);
    static_cast<void>(source.PostMessage(reinterpret_cast<HWND>(std::uintptr_t(Read(environment->windowAt2C))),WM_COMMAND,0x8015,0,0x4BA4CC));
    Field<unsigned>(Archive(source,2,0x4BA4D4),4)=0;
}
}
