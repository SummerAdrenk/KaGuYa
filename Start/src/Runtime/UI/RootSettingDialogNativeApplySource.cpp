#include <Runtime/RootSettingDialogNativeApplySource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootSettingDialogNativeApplySource::RootSettingDialogNativeApplySource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
    ParamsRuntimeRootOwner& params,NativeGraphicsRuntimeLibrary& graphics,decltype(&::MessageBoxA) messageBox) noexcept
    :root_(root),environment_(environment),params_(params),graphics_(graphics),messageBox_(messageBox){}
EnvironmentRootStorage* RootSettingDialogNativeApplySource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return environment_.CaptureAt4180C0();}
EnvironmentRootStorage* RootSettingDialogNativeApplySource::CaptureEnvironmentInlineAt6198C8(std::uint32_t)
{
    // 51D3CF等内联展开同601160锁/60h懒根核，不能直接借Published绕过原捕获。
    return environment_.CaptureAt4180C0();
}
ParamsRuntimeRoot* RootSettingDialogNativeApplySource::CaptureParamsInlineAt6198C0(std::uint32_t)
{
    // 51D29E/51DA33等保留独立601118锁/6198C0核；用原空返回契约，不改GetOrCreate安全边界。
    return params_.CaptureAt418480();
}
ParamsRuntimeRoot* RootSettingDialogNativeApplySource::CaptureParamsAt418480(std::uint32_t)
{return params_.CaptureAt418480();}
void RootSettingDialogNativeApplySource::EnsureRootAt413BE0(std::uint32_t){root_.EnsureCreatedAt413BE0();}
void* RootSettingDialogNativeApplySource::RootAt6198C4(std::uint32_t){return root_.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootSettingDialogNativeApplySource::RootSoundSlotAt24(void* p)
{
    if (!p) return *reinterpret_cast<YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();
}
YtlReferenceSlotRuntime& RootSettingDialogNativeApplySource::RootDrawSlotAt18(void* p)
{
    if (!p) return *reinterpret_cast<YtlReferenceSlotRuntime*>(0x18u);
    return static_cast<RootRuntimePayload*>(p)->DrawManagerSlotAt18();
}
YtlReferenceSlotRuntime& RootSettingDialogNativeApplySource::RootFileSlotAt00(void* p)
{return static_cast<RootRuntimePayload*>(p)->FileManagerSlotAt00();}
GraphicsRootNativeBody1Ch* RootSettingDialogNativeApplySource::CaptureGraphicsAt4182F0(std::uint32_t)
{return graphics_.CaptureAt4182F0();}
int RootSettingDialogNativeApplySource::MessageBoxA(HWND window,const char* text,const char* caption,UINT flags)
{return messageBox_(window,text,caption,flags);}
}
