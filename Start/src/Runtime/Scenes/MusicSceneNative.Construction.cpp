#include "MusicSceneNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime::MusicSceneNativeDetail
{
namespace
{
class ButtonControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit ButtonControl(void* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* p=payloadAt14;if(!p)return;
        using Delete=void*(__thiscall*)(void*,std::uint32_t);
        auto* table=Read(*static_cast<const std::byte**>(p));Delete fn;std::memcpy(&fn,table,4);fn(p,1);
    }
    void DeleteControlAtVslot04(std::uint32_t f) override{RetireEnvironmentObjectControl(this,f);}
};
static_assert(sizeof(ButtonControl)==0x18);
}
void ResetOperation(TransitOperation& operation) noexcept
{
    auto& p=static_cast<volatile TransitOperation&>(operation);
    p.destination=0;p.previous=0;p.source=nullptr;p.mask=0;p.y=0;p.x=0;
    p.clip=nullptr;p.opcode=-1;p.targetKind=0;p.current=0;p.target=255;p.range=0;
}
void AdoptButton(Slot& out,void* p)
{
    ReleaseYtlReference(out);if(!p)return;
    if(auto* current=Read(out.controlAt08))current->DeleteControlAtVslot04(1);
    out.biasAt04=0;out.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=RequireMusicSceneNativeServices().memory.AllocateBody(0x18);
    auto* control=raw?::new(raw) ButtonControl(p):nullptr;out.controlAt08=control;
    // 4A0914保留原null+4写入及正文先发布前缀，不以空Ref或回滚隐藏失败。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    out.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C=0x70;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10=1;
}
void CaptureScreen(Body& body,IMusicSceneNativeSource& source,std::uint32_t site)
{
    auto* draw=Draw(source,site);Slot temporary;
    auto* returned=source.CaptureScreenAt4ACDD0(draw,&temporary);RefLife cleanup{temporary};
    CopyAssignGraphicsSurfaceReferenceAt465870(body.backupAt54,*returned);cleanup.NormalTerminate();
}
}
namespace KaGuYa::Start::Runtime
{
MusicSceneNativeBody64h* ConstructMusicSceneNativeAt4A0730(MusicSceneNativeBody64h& body,MusicSceneNativeServices& services)
{
    using namespace MusicSceneNativeDetail;MusicSceneNativeScope scope(services);auto& source=services.source;
    body.vptrAt00=&MusicSceneNativeMethods();body.buttonAt08={};ResetOperation(body.operationAt14);
    body.blackAt48={};body.backupAt54={};
    struct Unwind final
    {
        Body& body;bool active=true;
        ~Unwind() noexcept
        {if(active){ReleaseYtlReference(body.backupAt54);ReleaseYtlReference(body.blackAt48);ReleaseYtlReference(body.buttonAt08);Retire(body);}}
    }unwind{body};
    if(source.LoadSurfaceAt480000(Draw(source,0x4A0808),body.blackAt48,BlackName()))
    {
        source.ConfigureScenesAt4703D0(Scenes(source,0x4A084C),2);
        {
            auto* raw=services.memory.AllocateBody(0x70);
            struct Raw final{void* p;IEnvironmentRootMemory& memory;bool active=true;~Raw() noexcept{if(active)memory.DeleteBody(p,0x70);}} allocation{raw,services.memory};
            auto* button=raw?services.buttons.ConstructAt4A1C70(raw):nullptr;
            allocation.active=false;AdoptButton(body.buttonAt08,button);
        }
        CaptureScreen(body,source,0x4A093B);
        source.ClearScreenAfter4182F0(0x4A09AC,nullptr);source.ClearDirtyAfter4182F0(0x4A09B8);
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Scenes(source,0x4A09C9),0x1C),"",0);
        Field<std::uint32_t>(Registry(source,2,0x4A0A06),4)=0;
        Field<std::uint8_t>(Registry(source,2,0x4A0A1B),9)=0;
        Field<std::uint8_t>(Registry(source,2,0x4A0A2D),0x24)=1;
        Field<std::uint8_t>(Registry(source,2,0x4A0A3F),0x25)=1;
        Field<std::uint8_t>(Registry(source,2,0x4A0A51),0x26)=0;
        auto* root=Root(source,0x4A0A66);auto* sound=Resolve<void>(source.RootSoundSlotAt24(root));
        SetRuntimeCStringAt437680(Field<RuntimeCString>(Registry(source,2,0x4A0A94),0x2C),"",0);
        source.StopSoundAt4383B0(sound,Read(Field<std::uint32_t>(sound,0x10)));
        body.phaseAt04=0;root=Root(source,0x4A0AC5);source.WriteRootByte(root,0x218,1);
        root=Root(source,0x4A0ADB);source.WriteRootByte(root,0x219,1);
    }
    // 480000=false原样返回；04/44/60继续保持分配字节。
    unwind.active=false;return &body;
}
}
