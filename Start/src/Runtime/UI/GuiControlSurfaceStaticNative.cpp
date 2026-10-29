#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;
struct ReferenceLifetime
{
    Slot& slot;bool armed=true;
    ~ReferenceLifetime() noexcept{if(armed)ReleaseYtlReference(slot);}
    void Finish() noexcept
    {
        armed=false;
        auto* control=slot.controlAt08;
        if(control&&--control->referenceCountAt04==0)
        {
            if(control->owningAt08)control->DestroyPayloadAtVslot00();
            if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
        }
    }
    void FinishCaptured(YtlReferenceControlRuntime* control) noexcept
    {
        armed=false;
        if(control&&--control->referenceCountAt04==0)
        {
            if(control->owningAt08)control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
Slot Copy(const Slot& source) noexcept{Slot out;CopyGuiClickedEventReference(out,source);return out;}
LONG Add(LONG a,LONG b) noexcept{return std::bit_cast<LONG>(std::uint32_t(a)+std::uint32_t(b));}
struct RawLifetime
{
    void* body;std::size_t size;bool armed=true;
    ~RawLifetime() noexcept{if(armed)::operator delete(body,size);}
};
}
std::int32_t AppendGuiControlSurfaceArgumentAt538350(GuiControlResourceNativeBody38h& body,
    std::int32_t slot,Slot owned)
{
    ReferenceLifetime argument{owned};
    std::int32_t index=-1;
    if(slot<7)
    {
        index=GuiControlSurfaceReferenceCount(body.surfacesAt1C);
        AppendGuiControlSurfaceReference(body.surfacesAt1C,owned);
        // 原5383DF没有负下标检查；保持已确认的原写入，不发明slot归一化。
        const auto address=reinterpret_cast<std::uintptr_t>(&body)+std::uint32_t(slot)*4u;
        *reinterpret_cast<std::int32_t*>(address)=index;
    }
    argument.Finish();
    return index;
}
YtlReferenceSlotRuntime* CreateGuiStaticFromSurfaceAt505570(Slot& output,
    void* parent,std::int32_t event,Slot ownedSurface,IGraphicsSurfaceApi& surfaces)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    ReferenceLifetime argument{ownedSurface};
    if(!ownedSurface.baseAt00)throw 1;
    auto* resource=NewGuiControlResourceAt5047DD();
    Slot group;ConstructGuiControlResourceReferenceAt4D76A0(group,resource,1);
    ReferenceLifetime groupLifetime{group};
    // 505601先缓存base/control；后续解析只重取bias，不能把callee修改后当成另一receiver。
    const auto base=group.baseAt00;auto* control=group.controlAt08;
    AppendGuiControlSurfaceArgumentAt538350(RequireGuiControlResource(group),0,Copy(ownedSurface));
    // 505629以旧EDI判空，但50563C加当前group.base；不能把判空快照也当成地址。
    Slot captured{base?group.baseAt00:0,group.biasAt04,control};
    if(GuiControlSurfaceReferenceCount(RequireGuiControlResource(captured).surfacesAt1C)>0)
        RequireGuiControlResource(captured).slotIndicesAt00[3]=0;
    captured={group.baseAt00,group.biasAt04,control};
    if(GuiControlSurfaceReferenceCount(RequireGuiControlResource(captured).surfacesAt1C)>0)
        RequireGuiControlResource(captured).slotIndicesAt00[4]=0;
    auto* source=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(ownedSurface);
    const auto width=source->widthAt08,height=source->heightAt0C;
    source=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(ownedSurface);
    const RECT bounds{source->originXAt10,source->originYAt14,
        Add(source->originXAt10,width),Add(source->originYAt14,height)};
    // 5056E5/5056F4重捕base/control，跨CopyRect和new50保持，只有bias在505743晚读。
    const auto staticBase=group.baseAt00;control=group.controlAt08;
    captured={staticBase,group.biasAt04,control};
    ::CopyRect(&RequireGuiControlResource(captured).combinedRectAt28,&bounds);
    auto* raw=::operator new(0x50);RawLifetime rawLifetime{raw,0x50};
    GuiControlNativeBody50h* child=nullptr;
    if(raw)
    {
        captured={staticBase,group.biasAt04,control};
        if(control)++control->referenceCountAt04;
        child=ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(raw),parent,event,captured);
    }
    rawLifetime.armed=false;
    // 505773撤raw资格，4D7940失败不补删已完成50h；返回槽此前始终fresh未写。
    ConstructGuiStaticReferenceAt4D7940(output,child,1);
    groupLifetime.FinishCaptured(control);
    argument.Finish();
    return &output;
}
}
