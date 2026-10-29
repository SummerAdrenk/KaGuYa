#include <Runtime/TextDisplaySceneProductionRuntime.h>

namespace KaGuYa::Start::Runtime
{
TextDisplayNativeTask::TextDisplayNativeTask(TextDisplayNativeServices& services,IEnvironmentRootMemory& memory)
    :services_(services),body_(NewTextDisplayNativeAt4AB7EE(services,memory)){}
TextDisplayNativeTask::TextDisplayNativeTask(AdoptCompletedTag,TextDisplayNativeServices& services,
    TextDisplayNativeBody90h* completed) noexcept:services_(services),body_(completed){}
TextDisplayNativeTask::~TextDisplayNativeTask() noexcept
{
    TextDisplayNativeScope scope(services_);
    if(body_)body_->vptrAt00->deleteAt04(body_,1);
}
void TextDisplayNativeTask::Run(std::int32_t argument)
{
    TextDisplayNativeScope scope(services_);
    body_->vptrAt00->runAt00(body_,argument);
}
TextDisplaySceneConstructionServicesRuntime::TextDisplaySceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,TextDisplayNativeServices& services,IEnvironmentRootMemory& memory) noexcept
    :SceneConstructionServicesForwarderRuntime(upstream),services_(services),memory_(memory){}
std::shared_ptr<ISceneTask> TextDisplaySceneConstructionServicesRuntime::CreateTextDisplayProcessing()
{
    auto* body=NewTextDisplayNativeAt4AB7EE(services_,memory_);
    if(!body)return {};
    // 正文完成后才交Controller适配控制块；此后分配失败不补回收原完成正文。
    return std::make_shared<TextDisplayNativeTask>(TextDisplayNativeTask::AdoptCompletedTag{},services_,body);
}
}
