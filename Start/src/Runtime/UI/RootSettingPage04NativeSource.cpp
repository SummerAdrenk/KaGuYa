#include <Runtime/RootSettingPage04NativeSource.h>
#include <Runtime/SettingOtherResourceProfile.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ConstructFace(RuntimeCString& fresh, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(fresh, strings);
    struct Unwind final
    { RuntimeCString* value; ~Unwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); } } unwind{&fresh};
    SetRuntimeCStringAt437680(fresh, SettingPage04FontFaceInitialSample18(), 13);
    unwind.value = nullptr;
}
}
EnvironmentRootStorage* RootSettingPage04NativeSource::CaptureEnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
const char* RootSettingPage04NativeSource::FontFaceAt61A64C() { return nameFace_.GetString(); }
const char* RootSettingPage04NativeSource::FontFaceAt61A650() { return systemFace_.GetString(); }
GameResourceNamePromptResult RootSettingPage04NativeSource::RunLocalNamePrompt()
{
    const auto& name = CurrentSettingOtherProfile().name;
    if (!root_ || !params_ || name.entry != SettingOtherNameEntry::LocalResource112)
        throw std::logic_error("Local settings name prompt is not bound to the active game");
    return ShowGameResourceNamePrompt({*root_,environment_,*params_,
        ProcessAtlBaseModuleNative().resourceInstanceAt08,name.limits,GameResourceNamePromptContext::LegacySettings});
}
void ConstructSettingNameFontFaceAt406BC0(RuntimeCString& fresh, IRuntimeStringManagerSource& strings)
{ ConstructFace(fresh, strings); }
void ConstructSettingSystemFontFaceAt407430(RuntimeCString& fresh, IRuntimeStringManagerSource& strings)
{ ConstructFace(fresh, strings); }
}
