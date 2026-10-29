#include "ReplaySceneNative.Internal.h"
#include <Runtime/AppreciationSceneResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
ReplaySceneNativeBody80h* ConstructReplaySceneNativeAt49EC90(ReplaySceneNativeBody80h& body,ReplaySceneNativeServices& services)
{
    using namespace ReplaySceneNativeDetail;
    ReplaySceneNativeScope scope(services);auto& source=services.source;
    body.vptrAt00=&ReplaySceneNativeMethods();body.groupsAt1C={};ResetOperation(body.operationAt28);
    body.blackAt5C={};body.capturedAt68={};body.auxiliaryAt74={};
    struct Members{Body& body;bool active=true;~Members()noexcept{if(active)DestroyReplaySceneNativeAt4A6C60(body);}} members{body};
    try
    {
        if(!source.LoadSurfaceAt480000(Draw(source,0x49ED80),body.blackAt5C,"bg_black.bgd"))throw 1;
        {
            auto* raw=services.memory.AllocateBody(0x24);
            struct Raw{void* p;IEnvironmentRootMemory& memory;bool active=true;~Raw()noexcept{if(active)memory.DeleteBody(p,0x24);}} cleanup{raw,services.memory};
            auto* surface=raw?::new(raw)GraphicsSurfaceNativeBody(services.surfaces,services.buttons.strings):nullptr;
            cleanup.active=false;AdoptGraphicsSurfaceAt407A80(body.auxiliaryAt74,surface);
        }
        const auto height=Read(source.CaptureEnvironmentAt4180C0(0x49EDEC)->heightAt1C);
        const auto width=Read(source.CaptureEnvironmentAt4180C0(0x49EDF4)->widthAt18);
        if(!Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt74)->CreateAt415010(services.surfaces,width,height,0,1))throw 1;
        source.ConfigureScenesAt4703D0(Scenes(source,0x49EE35),1);
        const auto categoryCount=CurrentAppreciationSceneProfile().categoryCount;
        for(int category=0;category<categoryCount;++category)
        {
            Slot temporary;ConstructReplayButtonReferenceNative(temporary,category,services.buttons);
            struct Temporary{Slot& value;bool active=true;~Temporary()noexcept{if(active)ReleaseYtlReference(value);}} cleanup{temporary};
            AppendSceneGroupReferenceNativeAt499030(body.groupsAt1C,temporary);
            auto* captured=Read(temporary.controlAt08);cleanup.active=false;
            CgSceneNativeDetail::ReleaseTemporaryCaptured(captured);
            if(captured){temporary.controlAt08=nullptr;temporary.baseAt00=0;}
        }
        body.groupAt0C=0;
        Field<unsigned>(Registry(source,2,0x49EF76,true),4)=0;
        Field<std::uint8_t>(Registry(source,2,0x49EFC8,true),9)=0;
        Field<std::uint8_t>(Registry(source,2,0x49F017,true),0x24)=1;
        Field<std::uint8_t>(Registry(source,2,0x49F066,true),0x25)=0;
        Field<std::uint8_t>(Registry(source,2,0x49F0B5,true),0x26)=1;
        Field<std::uint8_t>(Registry(source,2,0x49F104,true),0x27)=1;
        body.phaseAt04=14;source.WriteRootByte(Root(source,0x49F15F),0x218,1);
        source.WriteRootByte(Root(source,0x49F175),0x219,1);
        Field<unsigned>(Scenes(source,0x49F18B),0x40)=1;
    }
    catch(...)
    {
        // 原49F20B/FuncInfo5F0450：handler正常返回仍发布部分对象；handler抛出才展开成员。
        source.QueueTitleAfterConstructorAt47E180(Scenes(source,0x49F210));
    }
    members.active=false;return &body;
}
}
