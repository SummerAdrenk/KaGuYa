#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScrParamsResetRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootScriptRecoveryRuntime::RootScriptRecoveryRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, decltype(&::MessageBoxA) messageBox,
    IEnvironmentRootMemory& paramsMemory, IRuntimeStringManagerSource& paramsStrings,
    TextWindowLayerNativeServices* nativeText) noexcept
    : environment_(environment), root_(root), messageBox_(messageBox), paramsMemory_(paramsMemory),
      paramsStrings_(paramsStrings),nativeText_(nativeText) {}

void RootScriptRecoveryRuntime::ShowError(const char* text)
{
    // 4BA5CF/4BA5F3各只捕获一次Environment，MessageBox期间不保活或复制原CString。
    const auto& environment = environment_.CaptureAt4180C0();
    static_cast<void>(messageBox_(reinterpret_cast<HWND>(environment.windowAt2C),
        text, environment.titleAt10.GetString(), MB_ICONERROR));
}

void RootScriptRecoveryRuntime::ShowOriginalError(const char* text) { ShowError(text); }
void RootScriptRecoveryRuntime::ShowSyntaxError() { ShowError("ReadScript syntax error!"); }

void RootScriptRecoveryRuntime::CleanupCurrentEnvironment()
{
    root_.EnsureCreatedAt413BE0();
    const auto receiver = root_.RequirePublished().PostWindow().BorrowSceneManagerAt30();
    ResetAndQueueTitleAt47E180(receiver);
}

void RootScriptRecoveryRuntime::ResetAndQueueTitleAt47E180(SceneManagerRuntime* receiver)
{ResetAndQueueTitleAt47E180(SceneManagerView(receiver));}

void RootScriptRecoveryRuntime::ResetAndQueueTitleAt47E180(SceneManagerView receiver)
{
    // 47E1AC固定CParams，47E1C8重新捕获DrawManager；最后仍使用入口的SceneManager。
    root_.EnsureCreatedAt413BE0();
    auto& params = root_.RequirePublished().ParamsAt47BDB0();
    ScrParamsResetRuntime::ResetCapturedAt46CE30(params, paramsMemory_, paramsStrings_);
    root_.EnsureCreatedAt413BE0();
    const auto drawing = root_.RequirePublished().PostWindow().DrawLayersAt18();
    // PostWindow返回原生Draw view；47E1F4的空receiver须到原字段访问才失败，不能变成业务catch可吞的异常。
    drawing.TextAtSlotThree().ResetTextAt46D170(nativeText_);
    if (!receiver.IsNative()&&!receiver)
        throw std::logic_error("47E23B null SceneManager (modern safety boundary)");
    receiver.ClearScenes();
    // 47E285的5AE938：标题；Queue不立即调用场景工厂。
    receiver.QueueScene("\x83\x5E\x83\x43\x83\x67\x83\x8B");
}
}
