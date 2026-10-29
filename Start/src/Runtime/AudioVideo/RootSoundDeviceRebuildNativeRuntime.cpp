#include <Runtime/SoundDeviceRebuildNative.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootSoundDeviceRebuildNativeRuntime::RootSoundDeviceRebuildNativeRuntime(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,ParamsRuntimeRootOwner& params,SoundManagerNativeRuntime& sounds)noexcept
    :root_(root),environment_(environment),params_(params),sounds_(sounds){}
void RootSoundDeviceRebuildNativeRuntime::RebuildAt432C20(SoundManagerNativeBody50h* body,GUID device)
{RebuildSoundDeviceNativeAt432C20(body,device,*this,*this);}
void RootSoundDeviceRebuildNativeRuntime::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootSoundDeviceRebuildNativeRuntime::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
const YtlReferenceSlotRuntime& RootSoundDeviceRebuildNativeRuntime::SoundManagerAt24(void* root)
{
    // 现代payload非21Ch；空原根仍在原+24首读处失败，不从别的root借槽。
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<RootRuntimePayload*>(root)->SoundManagerSlotAt24();
}
EnvironmentRootStorage* RootSoundDeviceRebuildNativeRuntime::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
void* RootSoundDeviceRebuildNativeRuntime::GameSystemAt418480()
{
    auto* root=params_.CaptureAt418480();const volatile auto& ref=root->gameSystemAt04.Native();const auto base=ref.baseAt00;
    if(!base)return nullptr;auto* control=ref.controlAt08;
    const auto stride=control?*reinterpret_cast<const volatile DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu):0u;
    return reinterpret_cast<void*>(base+ref.biasAt04*stride);
}
int RootSoundDeviceRebuildNativeRuntime::MessageBoxA(HWND w,const char* t,const char* c,UINT flags)
{return ::MessageBoxA(w,t,c,flags);}
void RootSoundDeviceRebuildNativeRuntime::StopAbsoluteAt4383B0(SoundManagerNativeBody50h* body,DWORD slot)
{static_cast<void>(sounds_.StopAbsoluteCapturedAt4383B0(body,slot));}
void RootSoundDeviceRebuildNativeRuntime::StopEffectsAt413270(SoundManagerNativeBody50h* body)
{sounds_.StopEffectsCapturedAt413270(body,nullptr);}
BYTE RootSoundDeviceRebuildNativeRuntime::ResetDeviceAt432C99(HWND w)
{return sounds_.Library().Calls().ResetDeviceAt432C99(w);}
void RootSoundDeviceRebuildNativeRuntime::SetGlobalFocusAt432D45(){sounds_.Library().Calls().SetGlobalFocusAt45D1F4(1);}
void RootSoundDeviceRebuildNativeRuntime::InitializeHandlesAt42CA60(SoundManagerNativeBody50h* body)
{sounds_.InitializeHandlesCapturedAt42CA60(body);}
bool RootSoundDeviceRebuildNativeRuntime::PlayBgmAt42BFE0(SoundManagerNativeBody50h* body,const char* name,std::int32_t v,std::int32_t loop)
{return sounds_.PlayBgmCapturedAt42BFE0(body,name,v,loop);}
bool RootSoundDeviceRebuildNativeRuntime::RestoreFirstAt42C1B0(SoundManagerNativeBody50h* body,const char* name,DWORD group,BYTE alternate)
{return sounds_.RestoreFirstCapturedAt42C1B0(body,name,group,alternate);}
bool RootSoundDeviceRebuildNativeRuntime::RestoreSecondAt42C070(SoundManagerNativeBody50h* body,const char* name,DWORD group,std::int32_t v)
{return sounds_.RestoreSecondCapturedAt42C070(body,name,group,v);}
}
