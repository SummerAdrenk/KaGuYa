#include "CgSceneNative.Internal.h"
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <Runtime/LayerWindowNative.h>

namespace KaGuYa::Start::Runtime::CgSceneNativeDetail
{
namespace
{
template<class T>class WindowControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit WindowControl(T* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* p=static_cast<T*>(payloadAt14);
        if(p)Read(p->window.layer.vptrAt00)->deletingDestructorAt08(&p->window.layer,1);
    }
    void DeleteControlAtVslot04(unsigned flags) override{RetireEnvironmentObjectControl(this,flags);}
};
template<class T>void Adopt(Slot& out,T* p,IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(out);if(!p)return;
    if(auto* control=Read(out.controlAt08))control->DeleteControlAtVslot04(1);
    out.biasAt04=0;out.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=memory.AllocateBody(0x18);auto* control=raw?::new(raw)WindowControl<T>(p):nullptr;
    out.controlAt08=control;
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    Read(out.controlAt08)->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->strideAt0C=sizeof(T);
    static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->countAt10=1;
}
struct Raw final
{
    void* value;IEnvironmentRootMemory& memory;unsigned size;bool active=true;
    ~Raw() noexcept{if(active)memory.DeleteBody(value,size);}
};
LayerListNativeBody18h* Registered(void* draw)
{return Resolve<LayerListNativeBody18h>(static_cast<DrawManagerNativeBody2Ch*>(draw)->registeredLayersAt08);}
int SpriteCount(void* draw)
{
    auto* window=reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(draw),0));
    auto* first=Read(window->patternsAt30.begin);auto* last=Read(window->patternsAt30.end);
    const auto count=unsigned(std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(last)-reinterpret_cast<std::uintptr_t>(first))/12);
    // 原4AD2A0只拒绝index>count；index0/count0仍读取begin，不增加安全空表分支。
    (void)count;
    auto* pattern=Resolve<PatternLayerNativeBody80h>(*first);
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(pattern->spritesAt34.controlAt08));
    return control?std::bit_cast<int>(Read(control->countAt10)):0;
}
}
void AdoptDisplay(Slot& out,CgDisplayWindowNativeBody60h* p,IEnvironmentRootMemory& memory){Adopt(out,p,memory);}
void AdoptOperation(Slot& out,CgOperationWindowNativeBody30h* p,IEnvironmentRootMemory& memory){Adopt(out,p,memory);}
}
namespace KaGuYa::Start::Runtime
{
CgSceneNativeBodyB4h* ConstructCgSceneNativeAt49E3B0(CgSceneNativeBodyB4h& body,CgSceneNativeServices& services)
{
    using namespace CgSceneNativeDetail;
    CgSceneNativeScope scope(services);auto& source=services.source;
    body.vptrAt00=&CgSceneNativeMethods();body.selectionAt18.pairsAt14={};body.groupsAt38={};
    ResetOperation(body.operationAt44);
    body.blackAt78={};body.capturedAt84={};body.auxiliaryAt90={};body.displayWindowAt9C={};body.operationWindowAtA8={};
    struct Members final{Body& body;bool active=true;~Members() noexcept{if(active)DestroyCgSceneNativeAt4A4720(body);}} members{body};
    try
    {
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,0x49E4FF),0x1C),"",0);
        if(!source.LoadSurfaceAt480000(Draw(source,0x49E53F),body.blackAt78,"bg_black.bgd"))throw 1;
        {
            auto* raw=services.memory.AllocateBody(0x24);Raw cleanup{raw,services.memory,0x24};
            auto* surface=raw?::new(raw)GraphicsSurfaceNativeBody(services.surfaces,services.buttons.strings):nullptr;
            cleanup.active=false;AdoptGraphicsSurfaceAt407A80(body.auxiliaryAt90,surface);
        }
        const auto height=Read(source.CaptureEnvironmentAt4180C0(0x49E5AA)->heightAt1C);
        const auto width=Read(source.CaptureEnvironmentAt4180C0(0x49E5B5)->widthAt18);
        if(!Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt90)->CreateAt415010(services.surfaces,width,height,0,1))throw 1;
        source.ConfigureScenesAt4703D0(Scenes(source,0x49E5F7),0);
        const auto categoryCount=CurrentAppreciationSceneProfile().categoryCount;
        for(int category=0;category<categoryCount;++category)
        {
            Slot temporary;ConstructCgButtonReferenceNative(temporary,category,services.buttons);
            struct Temporary final{Slot& value;bool active=true;~Temporary() noexcept{if(active)ReleaseYtlReference(value);}} cleanup{temporary};
            AppendGroup(body.groupsAt38,temporary);auto* captured=Read(temporary.controlAt08);
            cleanup.active=false;ReleaseTemporaryCaptured(captured);
            if(captured){temporary.controlAt08=nullptr;temporary.baseAt00=0;}
        }
        body.displayAt0C=0;body.groupAt10=0;
        {
            auto* raw=services.memory.AllocateBody(0x60);Raw cleanup{raw,services.memory,0x60};
            CgDisplayWindowNativeBody60h* display=nullptr;
            if(raw)
            {
                auto* parent=Registered(Draw(source,0x49E759));
                const auto count=SpriteCount(Draw(source,0x49E7AA));
                const auto size=source.ReadScreenSizeAfter4182F0(0x49E7F8);
                display=ConstructCgDisplayWindowNativeAt499A20(*::new(raw)CgDisplayWindowNativeBody60h,size,count,parent,services.buttons.strings);
            }
            cleanup.active=false;AdoptDisplay(body.displayWindowAt9C,display,services.memory);
        }
        {
            auto* raw=services.memory.AllocateBody(0x30);Raw cleanup{raw,services.memory,0x30};
            auto* operation=raw?ConstructCgOperationWindowNativeAt499C10(*::new(raw)CgOperationWindowNativeBody30h,
                Registered(Draw(source,0x49E8EA)),services.operation):nullptr;
            cleanup.active=false;AdoptOperation(body.operationWindowAtA8,operation,services.memory);
        }
        Field<unsigned>(Registry(source,2,0x49E9FE,true),4)=0;
        Field<std::uint8_t>(Registry(source,2,0x49EA50,true),9)=0;
        Field<std::uint8_t>(Registry(source,2,0x49EA9F,true),0x24)=0;
        Field<std::uint8_t>(Registry(source,2,0x49EAEE,true),0x25)=1;
        Field<std::uint8_t>(Registry(source,2,0x49EB3D,true),0x26)=1;
        Field<std::uint8_t>(Registry(source,2,0x49EB8C,true),0x27)=1;
        body.phaseAt04=16;source.WriteRootByte(Root(source,0x49EBE7),0x218,1);
        source.WriteRootByte(Root(source,0x49EBFD),0x219,1);
    }
    catch(...)
    {
        // 49EC22保留已发布成员并返回；仅handler自身失败才展开整个state7拥有链。
        source.QueueTitleAfterConstructorAt47E180(Scenes(source,0x49EC27));
    }
    members.active=false;return &body;
}
}
