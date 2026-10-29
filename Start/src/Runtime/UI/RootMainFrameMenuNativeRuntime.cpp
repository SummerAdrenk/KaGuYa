#include <Runtime/RootMainFrameMenuNativeRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
RootMainFrameMenuNativeSource::RootMainFrameMenuNativeSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,
    IScrVmCommandErrorSink& errors,IRuntimeStringManagerSource& strings,ScriptNamedValueNativeTables names)noexcept
    :root_(root),environment_(environment),errors_(errors),strings_(strings),names_(names){}
void RootMainFrameMenuNativeSource::EnsureRootAt413BE0(){root_.EnsureCreatedAt413BE0();}
void* RootMainFrameMenuNativeSource::RootAt6198C4(){return root_.ReadPublishedAt6198C4();}
EnvironmentRootStorage* RootMainFrameMenuNativeSource::EnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
const YtlReferenceSlotRuntime& RootMainFrameMenuNativeSource::ModeAt0C(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x0Cu);
    return static_cast<RootRuntimePayload*>(root)->WindowModeSlotAt0C();
}
const YtlReferenceSlotRuntime& RootMainFrameMenuNativeSource::ScenesAt30(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x30u);
    return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();
}
const YtlReferenceSlotRuntime& RootMainFrameMenuNativeSource::SoundsAt24(void* root)
{
    if(!root)return *reinterpret_cast<const YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<RootRuntimePayload*>(root)->SoundManagerSlotAt24();
}
std::int32_t RootMainFrameMenuNativeSource::PopupIndexAt208(void* root)
{
    if(!root)return *reinterpret_cast<const volatile std::int32_t*>(0x208u);
    return *static_cast<const volatile std::int32_t*>(&static_cast<RootRuntimePayload*>(root)->MenuPositionAt208().indexAt208);
}
std::int32_t RootMainFrameMenuNativeSource::ResolveNamedAt498020(void*,const char* name)
{return ResolveScriptNamedValueNativeAt498020(name,names_,strings_);}
std::int32_t RootMainFrameMenuNativeSource::ReadValueAfter47BDB0(void* root,std::int32_t index)
{return ReadScriptValueAt4C73A0(static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0(),index,errors_);}

RootMainFrameMenuNativeCalls::RootMainFrameMenuNativeCalls(MainFramePopupPositionNativeServices& popup,
    IMainFrameMenuActionsNativeSource& actions,SoundManagerNativeRuntime& sounds,RootScriptRecoveryRuntime& recovery,
    SceneManagerNativeServices& scenes,IMainFrameSettingsCommandNative& settings,AboutDialogNativeServices& about)noexcept
    :popup_(popup),actions_(actions),sounds_(sounds),recovery_(recovery),scenes_(scenes),settings_(settings),about_(about){}
BOOL RootMainFrameMenuNativeCalls::InstallPopupAt4387D0(EditWindowEndpointNativeBody24h& p,HWND w)
{return InstallMainFramePopupPositionNativeAt4387D0(p,w,popup_);}
BYTE RootMainFrameMenuNativeCalls::SetModeAt438AD0(void* root,WindowModeNative2Ch* body,BYTE request,LONG style,LONG ex)
{return WindowModeView(*body,static_cast<RootRuntimePayload*>(root)->WindowModeServices()).SetModeAt438AD0(request,style,ex);}
void RootMainFrameMenuNativeCalls::SettingsAt428E30(void* menu){settings_.RunAt428E30(menu);}
LRESULT RootMainFrameMenuNativeCalls::ToggleSkipAt4230E0(void* m,DWORD n,DWORD id,LPARAM l,BOOL* h)
{return MainFrameMenuActionNativeAt4230E0(m,n,id,l,h,actions_);}
LRESULT RootMainFrameMenuNativeCalls::ToggleAutoAt42AA70(void* m,DWORD n,DWORD id,LPARAM l,BOOL* h)
{return MainFrameMenuActionNativeAt42AA70(m,n,id,l,h,actions_);}
LRESULT RootMainFrameMenuNativeCalls::ActionAt42AF70(void* m,DWORD n,DWORD id,LPARAM l,BOOL* h)
{
    // 旧controller的Factory四字节正文借当前真实SceneManager服务；元数据不进原Ref。
    SceneManagerNativeScope scope(scenes_);return MainFrameMenuActionNativeAt42AF70(m,n,id,l,h,actions_);
}
void RootMainFrameMenuNativeCalls::PlayVoicesAt42C4F0(SoundManagerNativeBody50h* body,const YtlReferenceSlotRuntime* names)
{static_cast<void>(sounds_.Voice(body).PlayBatchRawAt42C4F0(names));}
void RootMainFrameMenuNativeCalls::ResetAndQueueTitleAt47E180(SceneManagerNativeBody48h* body)
{recovery_.ResetAndQueueTitleAt47E180(SceneManagerView(body,scenes_));}
void RootMainFrameMenuNativeCalls::AboutAt42F84A(){RunAboutDialogNativeAt42F84A(about_);}
}
