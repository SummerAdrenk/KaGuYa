#include <Platform/ApplicationStagesRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Platform
{
MainWindowCreationRuntime::MainWindowCreationRuntime(
    WindowHost& host,
    WindowClassRegistrar& registrar,
    const HINSTANCE moduleInstance,
    const HINSTANCE resourceInstance,
    const WindowCreationPlan& plan,
    const HMENU childWindowIdentifier,
    HWND& publishedWindow) noexcept
    : host_(host),
      registrar_(registrar),
      moduleInstance_(moduleInstance),
      resourceInstance_(resourceInstance),
      plan_(plan),
      childWindowIdentifier_(childWindowIdentifier),
      publishedWindow_(publishedWindow)
{
}

HWND MainWindowCreationRuntime::CreateMainWindow()
{
    // 434537..434628：窗口类注册和 CreateWindowExA 失败均由 WindowHost 抛出整数 2。
    const HWND window = host_.Create(
        registrar_,
        moduleInstance_,
        resourceInstance_,
        plan_,
        childWindowIdentifier_);
    // 434628：创建成功后才发布到环境 +2C，后置初始化从同一槽取得 HWND。
    publishedWindow_ = window;
    return window;
}

EnvironmentMainWindowCreationRuntime::EnvironmentMainWindowCreationRuntime(
    Runtime::EnvironmentStartupRuntime& environment, WindowHost& host,
    WindowClassRegistrar& registrar, HINSTANCE moduleInstance, HINSTANCE resourceInstance,
    const IWindowGeometryApi& geometry, HMENU childWindowIdentifier) noexcept
    : environment_(environment), host_(host), registrar_(registrar), moduleInstance_(moduleInstance),
      resourceInstance_(resourceInstance), geometry_(geometry), childWindowIdentifier_(childWindowIdentifier)
{
}

HWND EnvironmentMainWindowCreationRuntime::CreateMainWindow()
{
    const auto plan = BuildWindowCreationPlan(environment_, geometry_);
    const HWND window = host_.Create(registrar_, moduleInstance_, resourceInstance_, plan,
        childWindowIdentifier_);
    // 434618/434628 使用主窗对象的 HWND，不是 CreateWindowExA 的返回值；失败不覆盖根槽。
    static_assert(sizeof(HWND) == sizeof(std::uint32_t));
    environment_.CaptureAt4180C0().windowAt2C = reinterpret_cast<std::uint32_t>(window);
    return window;
}

ApplicationStagesRuntime::ApplicationStagesRuntime(
    Runtime::PreWindowLifecycleState& preWindowState,
    Runtime::IPreWindowServices& preWindowServices,
    IMainWindowCreationRuntime& windowCreation,
    Runtime::IPostWindowRuntimeServices& postWindowServices) noexcept
    : preWindowState_(preWindowState),
      preWindowServices_(preWindowServices),
      windowCreation_(windowCreation),
      postWindowServices_(postWindowServices)
{
}

bool ApplicationStagesRuntime::InitializeBeforeWindow()
{
    return preWindowInitializer_.Run(preWindowState_, preWindowServices_);
}

HWND ApplicationStagesRuntime::CreateMainWindow()
{
    return windowCreation_.CreateMainWindow();
}

bool ApplicationStagesRuntime::InitializeAfterWindow()
{
    try
    {
        // 48AE4A 正常释放、59AB87 展开释放同一 progress；均先于 root 返回。
        // 460730 的 noexcept EH 约束适用于两条路径，不包含子系统 rollback。
        struct InitializationProgressScope final
        {
            Runtime::IPostWindowRuntimeServices& services;
            ~InitializationProgressScope() noexcept
            {
                services.ReleaseInitializationProgress();
            }
        } progressScope{postWindowServices_};

        postWindowLifecycle_.Initialize(postWindowServices_);
        return true;
    }
    catch (...)
    {
        // 5E6850 的 try state 0..23，5E6840 无类型 handler -> 48AE69 返回 false。
        return false;
    }
}

void ApplicationStagesRuntime::ReleaseSubsystems()
{
    postWindowLifecycle_.Release(postWindowServices_);
}
}
