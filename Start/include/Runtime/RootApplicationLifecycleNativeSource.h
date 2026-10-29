#pragma once
#include <Runtime/MainFrameApplicationNative.h>
#include <Runtime/RootPreWindowInitializationNative.h>
#include <Runtime/FileInfoProgressNative.h>

namespace KaGuYa::Start::Runtime
{
class RootDrawServicesRuntime;
class RootSceneAuxiliaryNativeServicesRuntime;
class RootSceneNativeServicesRuntime;
class NativeGraphicsRuntimeLibrary;
class ICrtRandomSource;
class IWindowModeCommandPlatformApi;
class InputRootOwner;
struct RootApplicationLifecycleNativeDependencies final
{
    RootRuntimeOwner& root;
    RootPreWindowInitializationNativeDependencies pre;
    NativeGraphicsRuntimeLibrary& graphics;
    ICrtRandomSource& random;
    RootDrawServicesRuntime& draws;
    RootSceneAuxiliaryNativeServicesRuntime& auxiliary;
    RootSceneNativeServicesRuntime& scenes;
    IWindowModeCommandPlatformApi& windowCommands;
    FileInfoProgressNativeServices& progress;
    const HINSTANCE& resourceInstance;
    // 仅成功Post诊断借用发布槽；不调用Input getter，也不创建另一套输入根。
    InputRootOwner* diagnosticInput = nullptr;
};
// 正式集合必须与Process FileManager默认工厂同源；全部依赖须覆盖Root/逃逸Ref最终退役。
// 本对象只借服务，不析构Root；模型进度窗可能同步/异步派发，因此scope覆盖整个对象寿命。
class RootApplicationLifecycleNativeSource final : public IMainFrameApplicationNativeSource
{
public:
    explicit RootApplicationLifecycleNativeSource(RootApplicationLifecycleNativeDependencies) noexcept;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    EnvironmentRootStorage* EnvironmentAt4180C0() override;
    bool InitializeBeforeWindowAt4778B0(void*) override;
    bool InitializeAfterWindowAt489C90(void*) override;
    void ReleaseSubsystemsAt477620(void*) override;
private:
    RootApplicationLifecycleNativeDependencies dependencies_;
    FileInfoProgressNativeScope progressScope_;
};
}
