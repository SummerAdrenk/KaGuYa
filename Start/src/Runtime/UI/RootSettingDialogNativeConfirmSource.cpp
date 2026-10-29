#include <Runtime/RootSettingDialogNativeConfirmSource.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
RootSettingDialogNativeConfirmSource::RootSettingDialogNativeConfirmSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,ParamsRuntimeRootOwner& params,NativeSerialize18Services& serialization,
    decltype(&::PostMessageA) post) noexcept
    :root_(root),environment_(environment),params_(params),serialization_(serialization),post_(post){}
EnvironmentRootStorage* RootSettingDialogNativeConfirmSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return environment_.CaptureAt4180C0();}
ParamsRuntimeRoot* RootSettingDialogNativeConfirmSource::CaptureParamsAt418480(std::uint32_t)
{return params_.CaptureAt418480();}
void RootSettingDialogNativeConfirmSource::EnsureRootAt413BE0(std::uint32_t)
{root_.EnsureCreatedAt413BE0();}
void* RootSettingDialogNativeConfirmSource::RootAt6198C4(std::uint32_t)
{return root_.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootSettingDialogNativeConfirmSource::RootSystemSlotAt3C(void* fixedRoot)
{return static_cast<RootRuntimePayload*>(fixedRoot)->InformationDisplaySlotAt3C();}
BOOL RootSettingDialogNativeConfirmSource::PostMessageA(HWND window,UINT message,WPARAM w,LPARAM l)
{return post_(window,message,w,l);}
const void* RootSettingDialogNativeConfirmSource::SerializeMethodsAt5B22F4()
{return &NativeSerialize18Methods();}
const void* RootSettingDialogNativeConfirmSource::ISerializeMethods()
{return &NativeISerializeMethods();}
void RootSettingDialogNativeConfirmSource::SerializeArchiveAtVslot00(void* fixedArchive,
    SettingDialogNativeConfirmSerialize18h& serializer,std::uint32_t site)
{
    // 539275/5392AC的receiver均由Save+28/+1C原SoundParams工厂确定为44h。
    // 保留传入18h正文与当前mode/cursor，不能调用接受SerializeBuffer的旧虚槽。
    NativeSerialize18Scope scope(serialization_);
    SerializeSoundParamsForSettingConfirmNative18(fixedArchive,serializer,site);
}
}
