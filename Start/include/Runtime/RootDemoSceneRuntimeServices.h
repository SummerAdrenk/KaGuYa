#pragma once
#include <Runtime/SceneManagerView.h>
#include <Runtime/LogoSceneRuntime.h>
#include <Runtime/EndingSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;
class InputRootOwner;
class RootRuntimeOwner;
class IGraphicsScreenCalls;
class SceneManagerRuntime;
class UnSaveDataRuntime;

// Logo/Ending 共享4B5FC0，但各次根捕获和各自终结副作用仍独立执行。
// 仅借用长期服务；不保存本次SceneManager、声音正文或环境归档快照。
class RootDemoSceneRuntimeServices final : public ILogoSceneRuntimeServices,
    public IEndingSceneRuntimeServices
{
public:
    RootDemoSceneRuntimeServices(EnvironmentRootOwner&, InputRootOwner&,
        RootRuntimeOwner&, IGraphicsScreenCalls&) noexcept;
    bool ShouldRequestCompletion() override;
    void ClearGlobalDescriptorTarget(std::uint32_t index) override;
    void ClearRootObjectAt24Binding() override;
    void QueueScene(const char* name) override;
    const RuntimeCString* CaptureDemoNameSlotAt49DA53() override;
    void StopRootSlot24Sounds() override;
    void StopRootSlot24SoundsUsingDescriptor2ListAt30() override;
    void WriteDescriptor2DwordAt04(std::uint32_t value) override;
    void WriteDescriptor2DwordAt14(std::uint32_t value) override;
    void ClearScreen(std::int32_t rect) override;
    void InvalidateScreenRect(const void* rect) override;
    void AdvanceActiveSceneFromPending() override;
private:
    SceneManagerView CaptureSceneManager();
    UnSaveDataRuntime& CaptureUnSave();
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    RootRuntimeOwner& root_;
    IGraphicsScreenCalls& graphics_;
};
}
