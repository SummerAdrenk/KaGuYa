#include <Runtime/RootPreSceneEngineUpdateRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootPreSceneEngineUpdateRuntime::RootPreSceneEngineUpdateRuntime(EnvironmentStartupRuntime& environment,
    ParamsRuntimeRootOwner& params, RootRuntimeOwner& root, Platform::IInputRuntime& input,
    ScriptNamedValueNativeTables tables, decltype(&::MessageBoxA) messageBox,
    ToolWindowNativeInteractionServices* nativeTool, IRuntimeStringManagerSource& strings) noexcept
    : state_(environment.Archives(), input), tools_(environment, root, input, tables, messageBox, nativeTool,strings),
      soundSource_(environment.Archives(), params, root), soundRestore_(soundSource_),
      services_(tools_, soundRestore_), update_(state_, services_) {}

void RootPreSceneEngineUpdateRuntime::Update() { update_.Update(); }
}
