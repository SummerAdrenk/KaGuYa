#pragma once
#include <Runtime/DemoSceneNative.h>
#include <Runtime/SceneFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 业务状态和序列唯一存在于18h/A4h；这里只适配尚未迁移的Controller接口。
class DemoSceneNativeTask final : public ISceneTask
{
public:
    struct AdoptCompletedTag final{};
    DemoSceneNativeTask(AdoptCompletedTag,DemoSceneNativeServices&,DemoSceneNativeBody18h*) noexcept;
    ~DemoSceneNativeTask() noexcept override;
    void Run(std::int32_t) override;
    DemoSceneNativeBody18h* NativeBody() const noexcept{return body_;}
private:
    DemoSceneNativeServices& services_;
    DemoSceneNativeBody18h* body_;
};
class DemoSceneNativeConstructionServices final : public SceneConstructionServicesForwarderRuntime
{
public:
    DemoSceneNativeConstructionServices(ISceneConstructionServices&,DemoSceneNativeServices&) noexcept;
    std::shared_ptr<ISceneTask> CreateTitleLogo() override;
    std::shared_ptr<ISceneTask> CreateEnding() override;
private:
    DemoSceneNativeServices& services_;
};
}
