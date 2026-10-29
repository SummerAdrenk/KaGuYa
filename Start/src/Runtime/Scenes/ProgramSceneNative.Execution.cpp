#include "ProgramSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
void UpdateProgramSceneNativeAt4B9710(ProgramSceneNativeBody20h& b,ProgramSceneNativeServices& services)
{
    using namespace ProgramSceneNativeDetail;
    ProgramSceneNativeScope scope(services);auto& source=services.source;UpdateFrame frame{services.memory};
    const auto phase=Read(b.phaseAt04);
    switch(phase)
    {
    case 0:
    {
        auto* root=Root(source,0x4B97D8,0x4B97DD);
        Field<unsigned>(Resolve<void>(source.RootInformationSlotAt3C(root,0x4B97E2)),0x10)=0;
        auto* gui=Dialog(b);Read(gui->vptrAt00)->beginAt44(gui);b.phaseAt04=1;
        Poll(b,services,frame);return;
    }
    case 1:Poll(b,services,frame);return;
    case 2:
    {
        auto* root=Root(source,0x4B9F4A,0x4B9F4F);
        source.ResetDrawAt47E940(root,Resolve<void>(source.RootDrawSlotAt18(root,0x4B9F55)),0x4B9F76);
        root=Root(source,0x4B9F80,0x4B9F85);source.AdvanceScenesAt4C1760(root,Scenes(source,root,0x4B9F8B),0x4B9FAC);
        Field<unsigned>(Archive(source,2,0x4B9FB3),4)=1;
        Field<unsigned>(Archive(source,2,0x4B9FC8),0x18)|=1;
        Field<std::uint8_t>(Archive(source,2,0x4B9FDA),0x0C)=1;
        const auto enabled=Read(Field<std::uint8_t>(Archive(source,1,0x4B9FEC),0x38))&8;
        auto* target=Archive(source,2,enabled?0x4BA000:0x4BA026);
        Field<unsigned>(target,0x14)=enabled?Read(b.savedAt14):1u;return;
    }
    case 4:case 5:
    {
        const bool save=phase==4;
        auto* root=Root(source,save?0x4BA050:0x4BA0BA,save?0x4BA055:0x4BA0BF);
        source.WriteRootByteAt1AF(root,1,save?0x4BA05F:0x4BA0C9);
        root=Root(source,save?0x4BA066:0x4BA0D0,save?0x4BA06B:0x4BA0D5);
        source.PushSceneAt4A9030(root,Scenes(source,root,save?0x4BA071:0x4BA0DB),ProgramSceneNativeName(save?0x5AE218:0x5AE224),save?0x4BA097:0x4BA101);
        b.phaseAt04=1;return;
    }
    case 6:ApplySelection(b,services,frame,false);return;
    case 8:ApplySelection(b,services,frame,true);return;
    case 10:Modal(b,services,frame);return;
    case 12:
    {
        const auto enabled=Read(Field<std::uint8_t>(Archive(source,1,0x4B9759),0x38))&8;
        if(enabled)
        {
            const auto saved=Read(b.savedAt14)&2;
            Field<unsigned>(Archive(source,2,0x4B9773),0x14)=saved;
        }
        else Field<unsigned>(Archive(source,2,0x4B9784),0x14)=0;
        using Reopen=void(__thiscall*)(void*);auto* gui=Dialog(b);Virtual<Reopen>(gui,0xA4)(gui);
        b.phaseAt04=1;Poll(b,services,frame);return;
    }
    default:return;
    }
}
}
