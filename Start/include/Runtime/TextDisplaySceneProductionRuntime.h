#pragma once
#include <Runtime/SceneFactoryRuntime.h>
#include <Runtime/TextDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
// 现有Controller的宿主适配层；业务状态与四Ref均只存在于原90h正文。
// shared_ptr控制块的分配域仍属于现有Controller边界，不称原YTL工厂返回ABI。
class TextDisplayNativeTask final : public ISceneTask
{
public:
    TextDisplayNativeTask(TextDisplayNativeServices&,IEnvironmentRootMemory&);
    struct AdoptCompletedTag final {};
    TextDisplayNativeTask(AdoptCompletedTag,TextDisplayNativeServices&,TextDisplayNativeBody90h*) noexcept;
    ~TextDisplayNativeTask() noexcept override;
    void Run(std::int32_t) override;
    TextDisplayNativeBody90h* NativeBody() const noexcept{return body_;}
private:
    TextDisplayNativeServices& services_;
    TextDisplayNativeBody90h* body_;
};
class TextDisplaySceneConstructionServicesRuntime final : public SceneConstructionServicesForwarderRuntime
{
public:
    TextDisplaySceneConstructionServicesRuntime(ISceneConstructionServices&,TextDisplayNativeServices&,
        IEnvironmentRootMemory& =DefaultEnvironmentRootMemory()) noexcept;
    std::shared_ptr<ISceneTask> CreateTextDisplayProcessing() override;
private:
    TextDisplayNativeServices& services_;
    IEnvironmentRootMemory& memory_;
};
}
