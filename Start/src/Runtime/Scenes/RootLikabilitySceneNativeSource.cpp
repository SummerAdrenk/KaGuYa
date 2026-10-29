#include <Runtime/RootLikabilitySceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/SceneManagerView.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootLikabilitySceneNativeSource::CaptureEnvironmentAt4180C0(unsigned)
{return dependencies_.environment.CaptureAt4180C0();}
EnvironmentRootStorage*& RootLikabilitySceneNativeSource::EnvironmentPublicationAt6198C8(unsigned)
{return dependencies_.environment.PublicationSlot();}
InputRootNativeBody24h* RootLikabilitySceneNativeSource::CaptureInputAt418210(unsigned)
{return dependencies_.input.CaptureAt418210();}
void RootLikabilitySceneNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootLikabilitySceneNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootLikabilitySceneNativeSource::RootDrawSlotAt18(void* root)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootLikabilitySceneNativeSource::RootScenesSlotAt30(void* root)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
ScriptParamsNativeBody* RootLikabilitySceneNativeSource::ParamsAt47BDB0(void* root,unsigned)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
YtlReferenceSlotRuntime* RootLikabilitySceneNativeSource::CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime* out,unsigned)
{
    // 原callee不消费Draw ECX；caller已选定receiver，不再Ensure或重读当前Root。
    CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;
}
void RootLikabilitySceneNativeSource::DrawBackgroundAt52624D(int handle,const RECT* clip)
{static_cast<void>(dependencies_.graphics.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,clip,0));}
void RootLikabilitySceneNativeSource::AdvanceScenesAt4C1760(void* manager)
{SceneManagerView(static_cast<SceneManagerNativeBody48h*>(manager),RequireSceneManagerNativeServices()).PopThenActivatePendingAt4C1760();}
GuiDialogNativeBody5Ch* RootLikabilitySceneNativeSource::ConstructDialogAt4E0E30(void* raw)
{return ConstructLikabilityDialogNativeAt4E0E30(raw,RequireLikabilitySceneNativeServices().gui);}
std::int32_t RootLikabilitySceneNativeSource::ReadLikabilityPage()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* manager=ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(
        dependencies_.root.RequirePublished().SceneManagerSlotAt30());
    if(!manager)throw std::logic_error("Likability requires the current SceneManager");
    // 统一现代Manager的已初始化+44页状态；不能读取18基线未初始化的+40。
    // 原05/10–15的+40与16–17的+44都映射到该同一业务字段，不增加第二份页码缓存。
    return std::bit_cast<std::int32_t>(manager->fieldAt44);
}
void RootLikabilitySceneNativeSource::StoreLikabilityPage(std::int32_t page)
{
    dependencies_.root.EnsureCreatedAt413BE0();
    auto* manager=ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(
        dependencies_.root.RequirePublished().SceneManagerSlotAt30());
    if(!manager)throw std::logic_error("Likability close requires the current SceneManager");
    manager->fieldAt44=std::bit_cast<std::uint32_t>(page);
}
IGraphicsSurfaceRawCopyApi& RootLikabilitySceneNativeSource::LikabilityCopyApi(){return dependencies_.graphics;}
LikabilityAnimationServices& RootLikabilitySceneNativeSource::LikabilityAnimation()
{
    if(!dependencies_.animation)throw std::logic_error("Likability02 animation services are not bound");
    return *dependencies_.animation;
}
RootLikabilitySceneServicesRuntime::RootLikabilitySceneServicesRuntime(RootLikabilitySceneNativeDependencies d)noexcept
    :source_(d),input_(d.input),screen_(d.graphics),
     dialog_{source_,d.loader,d.graphics,d.graphics.GuiCalls(),input_,screen_,d.environment.LockSource(),d.memory,d.errors,d.strings,d.names,&source_},
     scene_{source_,dialog_,d.memory}{}
}
