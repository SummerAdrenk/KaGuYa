#pragma once

#include <Runtime/EndingSceneRuntime.h>
#include <Runtime/LogoSceneRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
class RootSceneDemoNameRuntime;
// 0x49B5C0 每次构造 CDemo 时都会建立新的 0xA4 CDemoSeq。
// 两种 factory 分开保留其静态类型，避免把尚未证明相同的派生行为合并。
class ILogoDemoSequenceFactory
{
public:
    virtual ~ILogoDemoSequenceFactory() = default;
    [[nodiscard]] virtual std::unique_ptr<ILogoDemoSequenceRuntime>
        CreateLogoDemoSequence() = 0;
};

class IEndingDemoSequenceFactory
{
public:
    virtual ~IEndingDemoSequenceFactory() = default;
    [[nodiscard]] virtual std::unique_ptr<IEndingDemoSequenceRuntime>
        CreateEndingDemoSequence() = 0;
};

class LogoSceneProductionObjectGraphRuntime final
{
public:
    LogoSceneProductionObjectGraphRuntime(
        ILogoDemoSequenceFactory& sequenceFactory,
        ILogoSceneRuntimeServices& sceneServices, IDemoParamsSource& params);

    [[nodiscard]] const std::shared_ptr<LogoSceneRuntime>& Scene() const noexcept;

private:
    std::unique_ptr<ILogoDemoSequenceRuntime> sequence_;
    std::shared_ptr<LogoSceneRuntime> scene_;
};

class EndingSceneProductionObjectGraphRuntime final
{
public:
    EndingSceneProductionObjectGraphRuntime(
        IEndingDemoSequenceFactory& sequenceFactory,
        IEndingSceneRuntimeServices& sceneServices, IDemoParamsSource& params);

    [[nodiscard]] const std::shared_ptr<EndingSceneRuntime>& Scene() const noexcept;

private:
    std::unique_ptr<IEndingDemoSequenceRuntime> sequence_;
    std::shared_ptr<EndingSceneRuntime> scene_;
};

// 只接管 CSceneFactory::Create 的“タイトルロゴ”和“エンディング”分支。
// 返回的别名引用让每个场景持有自己独立的 CDemoSeq 对象图。
class DemoSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    DemoSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        ILogoDemoSequenceFactory& logoSequenceFactory,
        ILogoSceneRuntimeServices& logoSceneServices,
        IEndingDemoSequenceFactory& endingSequenceFactory,
        IEndingSceneRuntimeServices& endingSceneServices, IDemoParamsSource& params) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask> CreateTitleLogo() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateEnding() override;

private:
    ILogoDemoSequenceFactory* logoSequenceFactory_ = nullptr;
    ILogoSceneRuntimeServices* logoSceneServices_ = nullptr;
    IEndingDemoSequenceFactory* endingSequenceFactory_ = nullptr;
    IEndingSceneRuntimeServices* endingSceneServices_ = nullptr;
    IDemoParamsSource& params_;
};

// Ending 名字只接真实 SceneManager+3C owner；其它场景服务沿显式上游转发。
class RootEndingSceneRuntimeServices final : public IEndingSceneRuntimeServices
{
public:
    RootEndingSceneRuntimeServices(IEndingSceneRuntimeServices& upstream,
        RootSceneDemoNameRuntime& names) noexcept;
    const RuntimeCString* CaptureDemoNameSlotAt49DA53() override;
    void StopRootSlot24Sounds() override;
    void StopRootSlot24SoundsUsingDescriptor2ListAt30() override;
    void WriteDescriptor2DwordAt04(std::uint32_t value) override;
    void WriteDescriptor2DwordAt14(std::uint32_t value) override;
    bool ShouldRequestCompletion() override;
    void QueueScene(const char* name) override;
    void ClearScreen(std::int32_t value) override;
    void InvalidateScreenRect(const void* rect) override;
    void AdvanceActiveSceneFromPending() override;
private:
    IEndingSceneRuntimeServices& upstream_;
    RootSceneDemoNameRuntime& names_;
};
}
