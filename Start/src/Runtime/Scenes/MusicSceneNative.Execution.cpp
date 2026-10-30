#include "MusicSceneNative.Internal.h"
#include <Runtime/InputNativeObjects.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::MusicSceneNativeDetail
{
namespace
{
// 三个内联比较在原FuncInfo的state=-1；正常Free使用缓存header，不清源槽。
bool CommandEquals(IMusicSceneNativeSource& source,unsigned site,const char* expected)
{
    auto& value=Field<RuntimeCString>(Scenes(source,site),0x1C);
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(value.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);
    const auto* chars=reinterpret_cast<const unsigned char*>(clone+1);
    const bool result=::_mbscmp(chars,reinterpret_cast<const unsigned char*>(expected))==0;
    ReleaseRuntimeCStringHeader(clone);return result;
}
void* Button(Body& p) noexcept{return Resolve<void>(p.buttonAt08);}
void Begin(Body& p,IMusicSceneNativeSource& source,Slot& selected,int opcode,unsigned site)
{
    ResetOperation(p.operationAt14);p.operationAt14.opcode=opcode;
    auto* surface=Resolve<GraphicsSurfaceNativeBody>(selected);
    ++p.phaseAt04;p.operationAt14.source=surface;p.stepAt44=20;
    Field<std::uint32_t>(Registry(source,2,site),4)=0;
}
void Animate(Body& p,IMusicSceneNativeSource& source)
{
    auto& operation=p.operationAt14;
    if(source.AccelerateAt4AA970(Registry(source,5,0x4B8F5D,true)))
    {
        operation.current=Add(Read(operation.current),256);
        if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);
    }
    if(Read(operation.opcode)==-1||Read(operation.current)>=Read(operation.target))++p.phaseAt04;
    auto* draw=Draw(source,0x4B8FD9);
    auto* transit=Resolve<void>(Field<Slot>(draw,0x20));source.RenderAt449DD0(transit,operation);
    operation.current=Add(Read(operation.current),Read(p.stepAt44));
    if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);
    source.InvalidateAfter4182F0(0x4B9041,nullptr);
}
bool ReturnToTitle(IMusicSceneNativeSource& source)
{
    // 4ADAC0构造独立TEXT；只有该第三个比较跨Input/gate，state2的EH清当前槽。
    auto& command=Field<RuntimeCString>(Scenes(source,0x4B931D),0x1C);
    union Text{RuntimeCString value;Text(){}~Text(){}} temporary;
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(command.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);auto* chars=reinterpret_cast<char*>(clone+1);
    // Clone引用以原生指针进入空槽，仍由cleanup按原时点释放。
    std::memcpy(static_cast<void*>(&temporary.value),&chars,4);
    struct Unwind final{RuntimeCString& value;bool active=true;~Unwind() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&value);}} cleanup{temporary.value};
    bool result=::_mbscmp(reinterpret_cast<const unsigned char*>(temporary.value.GetString()),
        reinterpret_cast<const unsigned char*>(TitleName()))==0;
    if(!result)
    {
        // 4B9359原E8目标实为418210，不是导出码误标的threadpool函数。
        auto* input=source.CaptureInputAt418210(0x4B9359);
        auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
        result=!Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28);
    }
    cleanup.active=false;DestroyRuntimeCStringAt40CD40(&temporary.value);return result;
}
void Poll(Body& p,IMusicSceneNativeSource& source)
{
    if(CommandEquals(source,0x4B907B,SettingName()))
    {
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,0x4B90F0),0x1C),"",0);
        source.PushSceneAt4A9030(Scenes(source,0x4B9130),SettingName());return;
    }
    auto* button=Button(p);
    using Poll=std::int32_t(__thiscall*)(void*);
    auto* table=Read(*static_cast<const std::byte**>(button));Poll poll;std::memcpy(&poll,table+4,4);
    const auto result=poll(button);
    const auto cachedBase=Read(p.buttonAt08.baseAt00); // 4B91B8的EDX跨比较/route/movie分支保留。
    const auto count=Read(Field<std::int32_t>(ResolveWithBase<void>(p.buttonAt08,cachedBase),0x3C));
    if(result>=count)
    {
        // 第二次读取itemCount仍重解button，不能借比较时的快照。
        p.routeAt60=Sub(result,Read(Field<std::int32_t>(ResolveWithBase<void>(p.buttonAt08,cachedBase),0x3C)));++p.phaseAt04;
    }
    else if(Read(Field<std::uint8_t>(ResolveWithBase<void>(p.buttonAt08,cachedBase),0x6C)))p.phaseAt04=7;
    else if(CommandEquals(source,0x4B9259,CgName())){p.routeAt60=0;++p.phaseAt04;}
    else if(CommandEquals(source,0x4B92BB,SceneName())){p.routeAt60=1;++p.phaseAt04;}
    else if(ReturnToTitle(source)){p.routeAt60=3;++p.phaseAt04;}
    source.SynchronizeButtonAt4ABB40(Button(p));
}
void ResetAfterMovie(Body& p,IMusicSceneNativeSource& source)
{
    auto* button=Button(p);const auto selected=Read(Field<std::int32_t>(button,0x68));
    if(selected!=-1)
    {
        if(auto* item=source.ButtonItemAt4ACF20(button,selected))
        {Field<std::uint32_t>(item,4)=0;Field<std::uint8_t>(item,0x20)=0;}
    }
    Field<std::int32_t>(button,0x68)=-1;
    const auto height=Read(source.CaptureEnvironmentAt4180C0(0x4B8DDC)->heightAt1C);
    const auto width=Read(source.CaptureEnvironmentAt4180C0(0x4B8DE4)->widthAt18);
    const RECT rect{0,0,std::bit_cast<int>(width),std::bit_cast<int>(height)};
    using Invalidate=void(__thiscall*)(void*,RECT);
    auto* table=Read(*static_cast<const std::byte**>(button));Invalidate invalidate;std::memcpy(&invalidate,table+8,4);
    invalidate(button,rect);source.SynchronizeButtonAt4ABB40(button);Field<std::uint8_t>(button,0x6C)=0;
    CaptureScreen(p,source,0x4B8E26);source.ClearScreenAfter4182F0(0x4B8EA0,nullptr);
    source.ClearDirtyAfter4182F0(0x4B8EAC);p.phaseAt04=0;
}
}
}
namespace KaGuYa::Start::Runtime
{
void UpdateMusicSceneNativeAt4B8C90(MusicSceneNativeBody64h& p,MusicSceneNativeServices& services)
{
    using namespace MusicSceneNativeDetail;MusicSceneNativeScope scope(services);auto& source=services.source;
    for(;;)
    {
        switch(Read(p.phaseAt04))
        {
        case 0:Begin(p,source,p.backupAt54,57,0x4B8F4A);continue;
        case 1:case 5:case 8:Animate(p,source);return;
        case 2:Field<std::uint32_t>(Registry(source,2,0x4B9060),4)=3;++p.phaseAt04;[[fallthrough]];
        case 3:Poll(p,source);return;
        case 4:case 7:Begin(p,source,p.blackAt48,58,0x4B8D62);continue;
        case 6:
        {
            const auto route=Read(p.routeAt60);const auto site=route==0?0x4B94A4u:route==1?0x4B945Fu:0x4B9414u;
            source.QueueSceneAt47F920(Scenes(source,site),route==0?CgName():route==1?SceneName():TitleName());return;
        }
        case 10:ResetAfterMovie(p,source);continue;
        default:return; // 原9及所有>10均无业务，不伪造影片完成推进。
        }
    }
}
}
