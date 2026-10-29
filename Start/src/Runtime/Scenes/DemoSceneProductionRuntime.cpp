#include <Runtime/DemoSceneProductionRuntime.h>
#include <Runtime/RootSceneDemoNameRuntime.h>

#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template <typename T>
void RequireSequence(const std::unique_ptr<T>& sequence)
{
    // 原 0x49B5C0 后续立即解引用 CDemoSeq；空对象不是可恢复状态。
    if (!sequence)
        throw std::bad_alloc();
}
}

LogoSceneProductionObjectGraphRuntime::LogoSceneProductionObjectGraphRuntime(
    ILogoDemoSequenceFactory& sequenceFactory,
    ILogoSceneRuntimeServices& sceneServices, IDemoParamsSource& params)
    : sequence_(sequenceFactory.CreateLogoDemoSequence())
{
    RequireSequence(sequence_);
    scene_ = std::make_shared<LogoSceneRuntime>(*sequence_, sceneServices, params);
}

const std::shared_ptr<LogoSceneRuntime>&
LogoSceneProductionObjectGraphRuntime::Scene() const noexcept
{
    return scene_;
}

EndingSceneProductionObjectGraphRuntime::EndingSceneProductionObjectGraphRuntime(
    IEndingDemoSequenceFactory& sequenceFactory,
    IEndingSceneRuntimeServices& sceneServices, IDemoParamsSource& params)
    : sequence_(sequenceFactory.CreateEndingDemoSequence())
{
    RequireSequence(sequence_);
    scene_ = std::make_shared<EndingSceneRuntime>(*sequence_, sceneServices, params);
}

const std::shared_ptr<EndingSceneRuntime>&
EndingSceneProductionObjectGraphRuntime::Scene() const noexcept
{
    return scene_;
}

DemoSceneConstructionServicesRuntime::DemoSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    ILogoDemoSequenceFactory& logoSequenceFactory,
    ILogoSceneRuntimeServices& logoSceneServices,
    IEndingDemoSequenceFactory& endingSequenceFactory,
    IEndingSceneRuntimeServices& endingSceneServices, IDemoParamsSource& params) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      logoSequenceFactory_(&logoSequenceFactory),
      logoSceneServices_(&logoSceneServices),
      endingSequenceFactory_(&endingSequenceFactory),
      endingSceneServices_(&endingSceneServices), params_(params)
{
}

std::shared_ptr<ISceneTask>
DemoSceneConstructionServicesRuntime::CreateTitleLogo()
{
    auto owner = std::make_shared<LogoSceneProductionObjectGraphRuntime>(
        *logoSequenceFactory_, *logoSceneServices_, params_);
    return std::shared_ptr<ISceneTask>(owner, owner->Scene().get());
}

std::shared_ptr<ISceneTask>
DemoSceneConstructionServicesRuntime::CreateEnding()
{
    auto owner = std::make_shared<EndingSceneProductionObjectGraphRuntime>(
        *endingSequenceFactory_, *endingSceneServices_, params_);
    return std::shared_ptr<ISceneTask>(owner, owner->Scene().get());
}

RootEndingSceneRuntimeServices::RootEndingSceneRuntimeServices(
    IEndingSceneRuntimeServices& upstream, RootSceneDemoNameRuntime& names) noexcept
    : upstream_(upstream), names_(names) {}

const RuntimeCString* RootEndingSceneRuntimeServices::CaptureDemoNameSlotAt49DA53()
{ return names_.CaptureNameSlotAt49DA53(); }
void RootEndingSceneRuntimeServices::StopRootSlot24Sounds()
{ upstream_.StopRootSlot24Sounds(); }
void RootEndingSceneRuntimeServices::StopRootSlot24SoundsUsingDescriptor2ListAt30()
{ upstream_.StopRootSlot24SoundsUsingDescriptor2ListAt30(); }
void RootEndingSceneRuntimeServices::WriteDescriptor2DwordAt04(std::uint32_t value)
{ upstream_.WriteDescriptor2DwordAt04(value); }
void RootEndingSceneRuntimeServices::WriteDescriptor2DwordAt14(std::uint32_t value)
{ upstream_.WriteDescriptor2DwordAt14(value); }
bool RootEndingSceneRuntimeServices::ShouldRequestCompletion()
{ return upstream_.ShouldRequestCompletion(); }
void RootEndingSceneRuntimeServices::QueueScene(const char* name)
{ upstream_.QueueScene(name); }
void RootEndingSceneRuntimeServices::ClearScreen(std::int32_t value)
{ upstream_.ClearScreen(value); }
void RootEndingSceneRuntimeServices::InvalidateScreenRect(const void* rect)
{ upstream_.InvalidateScreenRect(rect); }
void RootEndingSceneRuntimeServices::AdvanceActiveSceneFromPending()
{ upstream_.AdvanceActiveSceneFromPending(); }
}
