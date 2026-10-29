#include "MusicButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace MusicButtonNativeDetail;
void Invalidate(Body& p,RECT rect){Read(p.common.vptrAt00)->invalidateAt08(&p.common,rect);}
CgUiButtonNativeBody6Ch* PublicWithBase(Body& p,std::uintptr_t base,YtlReferenceControlRuntime* control,unsigned index)
{
    if(!base)return nullptr;
    const auto stride=control?Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C):0u;
    return reinterpret_cast<CgUiButtonNativeBody6Ch*>(base+(Read(p.common.publicGuiAt44.biasAt04)+index)*stride);
}
void EmitExit(Body& p,MusicButtonNativeServices& services)
{
    static_cast<void>(p);auto& source=services.source;
    auto* params=Resolve<void>(Field<Slot>(Registry(source,1,0x4BE85D,true),0x1C));
    const auto* name=source.ExitNameAt61A56C();auto* sound=Sound(source,0x4BE8D6);
    using Getter=const std::int32_t*(__thiscall*)(void*);
    auto* table=Read(*static_cast<const std::byte**>(params));Getter getter;std::memcpy(&getter,table+0x24,4);
    const auto value=Read(*getter(params));source.PlayCueAt4BCD70(sound,name,0,value,1);
}
}
std::int32_t ProcessMusicButtonNativeAt4BE610(MusicButtonNativeBody70h& p)
{
    using namespace MusicButtonNativeDetail;auto& services=RequireMusicButtonNativeServices();
    int result=ReadModeButtonInputNativeAt4AA200(p.common);
    if(result==-1)
    {
        for(int i=0;;++i)
        {
            auto* cached=Read(p.common.publicGuiAt44.controlAt08);
            const int count=cached?std::bit_cast<int>(Read(static_cast<EnvironmentObjectReferenceControl*>(cached)->countAt10)):0;
            if(i>=count)break;if(i==Read(p.common.skippedGuiAt40))continue;
            const auto base=Read(p.common.publicGuiAt44.baseAt00);
            const auto state=Read(PublicWithBase(p,base,cached,static_cast<unsigned>(i))->stateAt04);
            auto* button=PublicWithBase(p,base,cached,static_cast<unsigned>(i));
            if(ProcessCgUiButtonNativeAt4A98E0(*button,0,services.mode.buttons)>=2)result=Add(Read(p.common.itemCountAt3C),i);
            const auto after=Read(p.common.publicGuiAt44.baseAt00);
            auto* current=PublicWithBase(p,after,Read(p.common.publicGuiAt44.controlAt08),static_cast<unsigned>(i));
            if(state==Read(current->stateAt04))continue;
            current=PublicWithBase(p,after,Read(p.common.publicGuiAt44.controlAt08),static_cast<unsigned>(i));
            RECT rect{0,0,Read(current->extentAt38.cx),Read(current->extentAt38.cy)};
            const auto position=Read(current->useNaturalAt2C)?current->naturalAt30:current->positionAt24;
            ::OffsetRect(&rect,position.x,position.y);Invalidate(p,rect);
        }
    }
    if(result==-1||result==-2)
    {
        auto* dialog=Dialog(p);Read(dialog->vptrAt00)->pollAt48(dialog);
        dialog=Dialog(p);result=Read(dialog->vptrAt00)->resultAt50(dialog);
        dialog=Dialog(p);auto* dirty=Read(dialog->vptrAt00)->dirtyAt00(dialog);
        const auto count=std::bit_cast<int>(Read(dirty->vptrAt00)->countAt18(dirty));
        if(count)
        {
            dialog=Dialog(p);dirty=Read(dialog->vptrAt00)->dirtyAt00(dialog);
            auto* rects=Read(dirty->vptrAt00)->rectsAt1C(dirty);
            for(int i=0;i<count;++i){RECT rect;::CopyRect(&rect,rects+i);Invalidate(p,rect);}
        }
    }
    p.movieAt6C=0;bool exit=false;
    if(result>=0)
    {
        if(result>=Read(p.common.itemCountAt3C))exit=true;
        else SelectMusicButtonSoundNativeAt4C1A20(p,result);
    }
    if(exit||result>=Read(p.common.itemCountAt3C))EmitExit(p,services);
    return result;
}
}
