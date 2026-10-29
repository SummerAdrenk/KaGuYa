#pragma once
#include <Runtime/MainFrameNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/WindowModeNative.h>
#include <Runtime/SoundManagerNativeLifecycle.h>
#include <Runtime/SceneManagerNative.h>

namespace KaGuYa::Start::Runtime
{
class IMainFrameMenuNativeSource
{
public:
    virtual ~IMainFrameMenuNativeSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual const YtlReferenceSlotRuntime& ModeAt0C(void*)=0;
    virtual const YtlReferenceSlotRuntime& ScenesAt30(void*)=0;
    virtual const YtlReferenceSlotRuntime& SoundsAt24(void*)=0;
    virtual std::int32_t PopupIndexAt208(void*)=0;
    virtual std::int32_t ResolveNamedAt498020(void* archive0,const char*)=0;
    virtual std::int32_t ReadValueAfter47BDB0(void* capturedRoot,std::int32_t)=0;
};
class IMainFrameMenuNativeCalls
{
public:
    virtual ~IMainFrameMenuNativeCalls()=default;
    virtual BOOL InstallPopupAt4387D0(EditWindowEndpointNativeBody24h&,HWND)=0;
    virtual BYTE SetModeAt438AD0(void* capturedRoot,WindowModeNative2Ch*,BYTE,LONG,LONG)=0;
    virtual void SettingsAt428E30(void* menu)=0;
    virtual LRESULT ToggleSkipAt4230E0(void* menu,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual LRESULT ToggleAutoAt42AA70(void* menu,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual LRESULT ActionAt42AF70(void* menu,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual void PlayVoicesAt42C4F0(SoundManagerNativeBody50h*,const YtlReferenceSlotRuntime*)=0;
    virtual void ResetAndQueueTitleAt47E180(SceneManagerNativeBody48h*)=0;
    virtual void AboutAt42F84A()=0;
};
class IMainFrameMenuNativeApi
{
public:
    virtual ~IMainFrameMenuNativeApi()=default;
    virtual HWND FindWindowA(LPCSTR,LPCSTR)=0;
    virtual int MessageBoxA(HWND,LPCSTR,LPCSTR,UINT)=0;
    virtual BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM)=0;
};
IMainFrameMenuNativeApi& DefaultMainFrameMenuNativeApi()noexcept;
struct MainFrameMenuNativeServices final
{
    IMainFrameMenuNativeSource& source;
    IMainFrameMenuNativeCalls& calls;
    IMainFrameMenuNativeApi& api;
};
// 原ECX=M=F+54；单槽表按ret18的真实thiscall入口供父分派读取。
struct MainFrameMenuNativeVtable1 final
{
    BOOL(__thiscall* processAt00)(void*,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD);
};
static_assert(sizeof(MainFrameMenuNativeVtable1)==4);
class MainFrameMenuNativeScope final
{
public:
    explicit MainFrameMenuNativeScope(MainFrameMenuNativeServices&)noexcept;
    ~MainFrameMenuNativeScope()noexcept;
    MainFrameMenuNativeScope(const MainFrameMenuNativeScope&)=delete;
    MainFrameMenuNativeScope& operator=(const MainFrameMenuNativeScope&)=delete;
private:
    MainFrameMenuNativeServices* previous_;
};
const MainFrameMenuNativeVtable1& MainFrameMenuNativeVtable()noexcept;
BOOL ProcessMainFrameMenuNativeAt42F040(MainFrameNativeBody90h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    MainFrameMenuNativeServices&);
// 两个薄包装只匹配各自低WORD，其他参数保持原未使用，不增添来源/通知过滤。
LRESULT ExecuteMainFrameSceneCommandNativeAt428B30(DWORD,DWORD,LPARAM,BOOL*,IMainFrameMenuNativeSource&);
LRESULT ExecuteMainFrameNavigationCommandNativeAt423440(DWORD,DWORD,LPARAM,BOOL*,IMainFrameMenuNativeSource&);
}
