#include <Runtime/DemoSceneNativeProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
DemoSceneNativeTask::DemoSceneNativeTask(AdoptCompletedTag,DemoSceneNativeServices& services,
    DemoSceneNativeBody18h* body) noexcept:services_(services),body_(body){}
DemoSceneNativeTask::~DemoSceneNativeTask() noexcept
{DemoSceneNativeScope scope(services_);if(body_)body_->vptrAt00->deleteAt04(body_,1);}
void DemoSceneNativeTask::Run(std::int32_t argument)
{DemoSceneNativeScope scope(services_);(void)body_->vptrAt00->runAt00(body_,argument);}
DemoSceneNativeConstructionServices::DemoSceneNativeConstructionServices(ISceneConstructionServices& upstream,
    DemoSceneNativeServices& services) noexcept:SceneConstructionServicesForwarderRuntime(upstream),services_(services){}
std::shared_ptr<ISceneTask> DemoSceneNativeConstructionServices::CreateTitleLogo()
{
    auto* body=NewLogoNativeAt4AAFF1(services_);if(!body)return {};
    // 完成原18h后才建立现代Controller适配控制块；失败不补删原完成正文。
    return std::make_shared<DemoSceneNativeTask>(DemoSceneNativeTask::AdoptCompletedTag{},services_,body);
}
std::shared_ptr<ISceneTask> DemoSceneNativeConstructionServices::CreateEnding()
{
    auto* body=NewEndingNativeAt4AB782(services_);if(!body)return {};
    return std::make_shared<DemoSceneNativeTask>(DemoSceneNativeTask::AdoptCompletedTag{},services_,body);
}
}
