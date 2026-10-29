#include <Runtime/RootPreWindowInitializationNative.h>
#include <Runtime/EnvironmentErrorRuntime.h>
#include <Runtime/EnvironmentPreWindowPersistenceNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
RootPreWindowInitializationNativeServices::RootPreWindowInitializationNativeServices(RootRuntimePayload& selected,
    RootPreWindowInitializationNativeDependencies dependencies) noexcept
    : selected_(selected), dependencies_(dependencies),
      registry_(dependencies.environment, dependencies.strings, dependencies.memory) {}
void RootPreWindowInitializationNativeServices::RegisterEnvironmentArchivesAt47E2B0()
{
    SoundParamsSettingsRootSource sounds(dependencies_.params);
    registry_.RegisterAt47E2B0(dependencies_.params, sounds, dependencies_.paths);
}
bool RootPreWindowInitializationNativeServices::PrepareEnvironmentPersistenceAt480230()
{ return ReadPreWindowEnvironmentAt480230(registry_, dependencies_.files, dependencies_.transaction); }
void RootPreWindowInitializationNativeServices::WriteEnvironmentPersistenceAt488740()
{ WritePreWindowEnvironmentAt488740(registry_, dependencies_.files, dependencies_.transaction); }
void RootPreWindowInitializationNativeServices::InstallWindowModeAt47791D()
{
    selected_.WindowModeServices() = {&dependencies_.graphics, &dependencies_.windows,
        nullptr, nullptr, nullptr, &dependencies_.environment};
    CreateAndReplaceWindowModeAt47791D(selected_.WindowModeSlotAt0C(), dependencies_.strings);
}
const SaveDataEnvironmentArchive& RootPreWindowInitializationNativeServices::SaveDataAtSlot1()
{ return *static_cast<SaveDataEnvironmentArchive*>(registry_.CaptureArchiveAt4180C0(1)); }
ClickInfoArchive& RootPreWindowInitializationNativeServices::ClickInfoAtSlot5()
{ return *static_cast<ClickInfoArchive*>(registry_.CaptureArchiveAt4180C0(5)); }
void RootPreWindowInitializationNativeServices::BuildInitializationErrorTextAt479EA0(RuntimeCString& out)
{
    // 477BE2当场取Env；479EA0直接写caller未构造槽，不先产生现代结果包装。
    Runtime::BuildEnvironmentErrorTextAt479EA0(*dependencies_.environment.CaptureAt4180C0(), out, dependencies_.strings);
}
int RootPreWindowInitializationNativeServices::MessageBoxA(HWND window, const char* text, const char* caption, UINT flags)
{ return dependencies_.messageBox(window, text, caption, flags); }
}
