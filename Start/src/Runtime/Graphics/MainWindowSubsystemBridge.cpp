#include <Runtime/MainWindowSubsystemBridge.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/SoundRuntimeApi.h>

namespace KaGuYa::Start::Runtime
{
MainWindowSubsystemBridge::MainWindowSubsystemBridge(
    ISoundRuntimeLibrary& sound, IGraphicsRuntimeCalls& graphics) noexcept
    : sound_(sound), graphics_(graphics)
{
}

void MainWindowSubsystemBridge::ActivateAppRestore(WPARAM active)
{
    sound_.ActivateAppRestoreIfLoaded(active);
}

void MainWindowSubsystemBridge::RenderReset()
{
    graphics_.ResetRendererIfLoaded();
}
}
