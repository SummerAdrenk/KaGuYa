#include <Runtime/MainFrameMenuActionsNative.h>
#include <Runtime/LayerItemNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Source=IMainFrameMenuActionsNativeSource;
using Slot=YtlReferenceSlotRuntime;
static_assert(sizeof(void*)==4&&sizeof(DWORD)==4&&sizeof(LRESULT)==4&&sizeof(BOOL)==4);
static_assert(sizeof(Slot)==0x0C&&offsetof(SceneManagerNativeBody48h,unwrittenAt40)==0x40);
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T* Resolve(const Slot& slot)noexcept
{
    // 原内联Ref：空base才短路；空control仍读bias，stride为0，算术按DWORD回绕。
    const auto base=Read(slot.baseAt00);
    if(!base)return nullptr;
    const auto control=Read(slot.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    const auto bias=Read(slot.biasAt04);
    return reinterpret_cast<T*>(base+bias*stride);
}
DrawManagerNativeBody2Ch* CaptureDraw(Source& source)
{
    source.EnsureRootAt413BE0();
    auto* const root=source.RootAt6198C4();
    return Resolve<DrawManagerNativeBody2Ch>(source.DrawAt18(root));
}
SceneManagerNativeBody48h* CaptureScenes(Source& source)
{
    source.EnsureRootAt413BE0();
    auto* const root=source.RootAt6198C4();
    return Resolve<SceneManagerNativeBody48h>(source.ScenesAt30(root));
}
LRESULT Toggle(Source& source,DWORD offset)
{
    auto* const environment=source.EnvironmentAt4180C0();
    // 4230E6/42AA76先end后begin；signed差/12后用unsigned jbe，不能换成size_t计数。
    const auto end=reinterpret_cast<DWORD>(Read(environment->archivesAt30.end));
    auto* const begin=Read(environment->archivesAt30.begin);
    const auto count=DWORD(std::bit_cast<std::int32_t>(end-reinterpret_cast<DWORD>(begin))/12);
    auto* const body=count<=2u?nullptr:Resolve<std::byte>(begin[2]);
    auto* const flag=reinterpret_cast<volatile DWORD*>(reinterpret_cast<DWORD>(body)+offset);
    *flag=*flag^2u; // 42312C/42AABC：完整DWORD异或；解析为空仍访问，不伪造成功早退。

    // 423161..4231A8 / 42AAF1..42AB38与418C10同核，含count==2的物理槽读取。
    auto* const probe=DrawLayerAt418C10(*CaptureDraw(source),2);
    if(Read(probe->enabledAt0C)!=0)
    {
        // 门控成功后重新Ensure/读根/解Draw/取slot2，不复用probe；mode是栈上完整0。
        auto* const selected=DrawLayerAt418C10(*CaptureDraw(source),2);
        source.SetToolModeAt434FB0(reinterpret_cast<ToolWindowNativeBody40h*>(selected),0);
    }
    return 0;
}
}

RootMainFrameMenuActionsNativeSource::RootMainFrameMenuActionsNativeSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,ToolWindowNativeInteractionServices& tools,SceneControllerNativeServices& scenes)noexcept
    :root_(root),environment_(environment),tools_(tools),scenes_(scenes){}
EnvironmentRootStorage* RootMainFrameMenuActionsNativeSource::EnvironmentAt4180C0()
{return environment_.CaptureAt4180C0();}
void RootMainFrameMenuActionsNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFrameMenuActionsNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
const Slot& RootMainFrameMenuActionsNativeSource::DrawAt18(void* selected)
{
    if(!selected)return *reinterpret_cast<const Slot*>(0x18u);
    return static_cast<RootRuntimePayload*>(selected)->DrawManagerSlotAt18();
}
const Slot& RootMainFrameMenuActionsNativeSource::ScenesAt30(void* selected)
{
    if(!selected)return *reinterpret_cast<const Slot*>(0x30u);
    return static_cast<RootRuntimePayload*>(selected)->SceneManagerSlotAt30();
}
void RootMainFrameMenuActionsNativeSource::SetToolModeAt434FB0(ToolWindowNativeBody40h* selected,std::int32_t mode)
{SetToolModeNativeAt434FB0(*selected,mode,tools_);}
void RootMainFrameMenuActionsNativeSource::PushSceneTaskAt40B050(EnvironmentVectorStorage<Slot>& vector,TaskNativeBody4h* task)
{PushSceneTaskNativeAt40B050(vector,task,scenes_);}

LRESULT MainFrameMenuActionNativeAt4230E0(void*,DWORD,DWORD,LPARAM,BOOL*,Source& source)
{return Toggle(source,0x18u);}
LRESULT MainFrameMenuActionNativeAt42AA70(void*,DWORD,DWORD,LPARAM,BOOL*,Source& source)
{return Toggle(source,0x14u);}
LRESULT MainFrameMenuActionNativeAt42AF70(void*,DWORD,DWORD,LPARAM,BOOL*,Source& source)
{
    auto* const manager=CaptureScenes(source);
    auto* const controller=Resolve<SceneControllerNativeBody28h>(manager->controllerAt10);
    // 42AFE8..42B040与4BCF80同序且同state0->541E55；只该释放区有终止型EH资格。
    // 不以现代guard恢复end；factory/Push异常在state=-1，保留已弹栈前缀向外传播。
    PopSceneTaskNativeAt4BCF80(*controller);

    const auto end=reinterpret_cast<DWORD>(Read(controller->pendingAt04.end));
    const auto begin=reinterpret_cast<DWORD>(Read(controller->pendingAt04.begin));
    const auto count=std::bit_cast<std::int32_t>(end-begin)>>2;
    if(count!=0)
    {
        const auto* const name=Read(*reinterpret_cast<const char* const*>(begin+DWORD(count)*4u-4u));
        controller->pendingAt04.end=reinterpret_cast<const char**>(
            reinterpret_cast<DWORD>(Read(controller->pendingAt04.end))-4u);
        auto* const factory=Resolve<SceneFactoryNativeBody4h>(controller->factoryAt1C);
        auto* const task=factory->vptrAt00->createAt00(factory,name);
        // factory回调可改active向量；此处把旧controller的当前向量交共同叶，不捕获其end。
        source.PushSceneTaskAt40B050(controller->activeAt10,task);
    }
    auto* const current=CaptureScenes(source); // 42B092以后只新取manager，不再取其controller。
    *reinterpret_cast<volatile DWORD*>(reinterpret_cast<DWORD>(current)+0x40u)=1;
    return 0;
}
}
