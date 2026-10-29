#include <Runtime/RootSettingDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootSettingDialogNativeSource::RootSettingDialogNativeSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
    InputRootOwner& input,NativeGraphicsRuntimeLibrary& graphics,SoundManagerNativeRuntime& sounds,
    decltype(&::PostMessageA) postMessage) noexcept
    :root_(root),environment_(environment),input_(input),graphics_(graphics),sounds_(sounds),postMessage_(postMessage){}
void RootSettingDialogNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootSettingDialogNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
std::uint8_t RootSettingDialogNativeSource::RootByteAt218(void* p)
{return static_cast<RootRuntimePayload*>(p)->ReadByteAt218();}
YtlReferenceSlotRuntime& RootSettingDialogNativeSource::RootDrawSlotAt18(void* p)
{return static_cast<RootRuntimePayload*>(p)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootSettingDialogNativeSource::RootSoundSlotAt24(void* p)
{return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();}
std::int32_t& RootSettingDialogNativeSource::RootPageAt214(void* p)
{return reinterpret_cast<std::int32_t&>(static_cast<RootRuntimePayload*>(p)->Frame().BorrowValueAt214());}
EnvironmentRootStorage* RootSettingDialogNativeSource::SystemAt4180C0(){return environment_.CaptureAt4180C0();}
InputRootNativeBody24h* RootSettingDialogNativeSource::InputAt418210(){return input_.CaptureAt418210();}
YtlReferenceSlotRuntime* RootSettingDialogNativeSource::CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{
    // 4ACDD0不消费调用者的Draw ECX，仍用同一Graphics原screen capture核。
    CaptureGraphicsScreenAt4ACDD0(*out,graphics_,graphics_,graphics_);return out;
}
void RootSettingDialogNativeSource::StopSoundAt4384D0(void* p)
{(void)sounds_.ClearVoicesCapturedAt4384D0(static_cast<SoundManagerNativeBody50h*>(p));}
void RootSettingDialogNativeSource::StopSoundMappingsAt413270(void* p,EnvironmentVectorStorage<RuntimeCString>* fixed)
{sounds_.StopEffectsCapturedAt413270(static_cast<SoundManagerNativeBody50h*>(p),fixed);}
void RootSettingDialogNativeSource::PrepareSoundAt433F60(void* p)
{sounds_.Text(static_cast<SoundManagerNativeBody50h*>(p)).volume.Refresh();}
void RootSettingDialogNativeSource::DrawBackgroundAt5272BF(std::int32_t handle,const RECT* clip)
{(void)graphics_.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,clip,0);}
BOOL RootSettingDialogNativeSource::PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)
{return postMessage_(window,message,w,l);}
}
