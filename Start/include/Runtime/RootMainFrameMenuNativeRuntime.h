#pragma once
#include <Runtime/MainFrameMenuNative.h>
#include <Runtime/MainFrameMenuActionsNative.h>
#include <Runtime/MainFramePopupPositionNative.h>
#include <Runtime/AboutDialogNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class IScrVmCommandErrorSink;
class RootScriptRecoveryRuntime;
class SoundManagerNativeRuntime;
// 800D是六页Win32 PropertySheet，尚未由游戏内Setting替代；缺提供者就不能装完整owner。
class IMainFrameSettingsCommandNative
{
public:
    virtual ~IMainFrameSettingsCommandNative()=default;
    virtual void RunAt428E30(void* menu)=0;
};
class RootMainFrameMenuNativeSource final:public IMainFrameMenuNativeSource
{
public:
    RootMainFrameMenuNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,IScrVmCommandErrorSink&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource(),
        ScriptNamedValueNativeTables = GameScriptNamedValueTables())noexcept;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    const YtlReferenceSlotRuntime& ModeAt0C(void*)override;
    const YtlReferenceSlotRuntime& ScenesAt30(void*)override;
    const YtlReferenceSlotRuntime& SoundsAt24(void*)override;
    std::int32_t PopupIndexAt208(void*)override;
    std::int32_t ResolveNamedAt498020(void*,const char*)override;
    std::int32_t ReadValueAfter47BDB0(void*,std::int32_t)override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;ScriptNamedValueNativeTables names_;
};
class RootMainFrameMenuNativeCalls final:public IMainFrameMenuNativeCalls
{
public:
    RootMainFrameMenuNativeCalls(MainFramePopupPositionNativeServices&,IMainFrameMenuActionsNativeSource&,
        SoundManagerNativeRuntime&,RootScriptRecoveryRuntime&,SceneManagerNativeServices&,
        IMainFrameSettingsCommandNative&,AboutDialogNativeServices&)noexcept;
    BOOL InstallPopupAt4387D0(EditWindowEndpointNativeBody24h&,HWND)override;
    BYTE SetModeAt438AD0(void*,WindowModeNative2Ch*,BYTE,LONG,LONG)override;
    void SettingsAt428E30(void*)override;
    LRESULT ToggleSkipAt4230E0(void*,DWORD,DWORD,LPARAM,BOOL*)override;
    LRESULT ToggleAutoAt42AA70(void*,DWORD,DWORD,LPARAM,BOOL*)override;
    LRESULT ActionAt42AF70(void*,DWORD,DWORD,LPARAM,BOOL*)override;
    void PlayVoicesAt42C4F0(SoundManagerNativeBody50h*,const YtlReferenceSlotRuntime*)override;
    void ResetAndQueueTitleAt47E180(SceneManagerNativeBody48h*)override;
    void AboutAt42F84A()override;
private:
    MainFramePopupPositionNativeServices& popup_;IMainFrameMenuActionsNativeSource& actions_;
    SoundManagerNativeRuntime& sounds_;RootScriptRecoveryRuntime& recovery_;SceneManagerNativeServices& scenes_;
    IMainFrameSettingsCommandNative& settings_;AboutDialogNativeServices& about_;
};
// 必须覆盖真实消息泵及所有Popup同步/重入回调；只借服务，不持有原窗口或根Ref。
class MainFrameMenuBindingNativeScope final
{
public:
    MainFrameMenuBindingNativeScope(MainFrameMenuNativeServices& menu,MainFramePopupPositionNativeServices& popup)noexcept
        :menu_(menu),popup_(popup){}
private:
    MainFrameMenuNativeScope menu_;MainFramePopupPositionNativeScope popup_;
};
}
