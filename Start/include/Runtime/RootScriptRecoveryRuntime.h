#pragma once
#include <Runtime/SceneManagerView.h>

#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/ScrVmYieldRuntimeServices.h>
#include <Runtime/ScrParamsResetRuntime.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class SceneManagerRuntime;
struct TextWindowLayerNativeServices;

// 4BA530的错误出口及4AC6E0/47E180恢复链；只借根，不复制场景、参数或窗口状态。
class RootScriptRecoveryRuntime final : public IReadScriptErrorBoundary,
    public IScrVmYieldEnvironmentCleanup
{
public:
    RootScriptRecoveryRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root,
        decltype(&::MessageBoxA) messageBox = ::MessageBoxA,
        IEnvironmentRootMemory& paramsMemory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& paramsStrings = DefaultRuntimeStringManagerSource(),
        TextWindowLayerNativeServices* nativeText=nullptr) noexcept;

    void ShowOriginalError(const char* messageCurrentCodePage) override;
    void ShowSyntaxError() override;
    void CleanupCurrentEnvironment() override;
    // 47E180的调用者已捕获receiver；其可空性检查要等到原第一次解引用的位置。
    void ResetAndQueueTitleAt47E180(SceneManagerRuntime* receiver);
    void ResetAndQueueTitleAt47E180(SceneManagerView receiver);

private:
    void ShowError(const char* text);
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
    decltype(&::MessageBoxA) messageBox_;
    // 46CE30只借用分配/字符串服务，不把服务地址附到原生正文或control。
    IEnvironmentRootMemory& paramsMemory_;
    IRuntimeStringManagerSource& paramsStrings_;
    TextWindowLayerNativeServices* nativeText_;
};
}
