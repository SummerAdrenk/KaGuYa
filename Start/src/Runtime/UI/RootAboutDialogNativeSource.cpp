#include <Runtime/AboutDialogNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>

namespace KaGuYa::Start::Runtime
{
RootAboutDialogNativeSource::RootAboutDialogNativeSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
    ParamsRuntimeRootOwner& params)noexcept:root_(root),environment_(environment),params_(params){}
void RootAboutDialogNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootAboutDialogNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
void RootAboutDialogNativeSource::WriteMinimizeAt1B2(void* root,BYTE value)
{
    if(!root){*reinterpret_cast<volatile BYTE*>(0x1B2u)=value;return;}
    static_cast<RootRuntimePayload*>(root)->Frame().Lifecycle().Native().minimizeAt1B2=value;
}
EnvironmentRootStorage* RootAboutDialogNativeSource::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
void* RootAboutDialogNativeSource::GameSystemAt418480()
{
    auto* params=params_.CaptureAt418480();const volatile auto& ref=params->gameSystemAt04.Native();
    const auto base=ref.baseAt00;if(!base)return nullptr;auto* control=ref.controlAt08;
    const auto stride=control?*reinterpret_cast<const volatile DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu):0u;
    return reinterpret_cast<void*>(base+ref.biasAt04*stride);
}
}
