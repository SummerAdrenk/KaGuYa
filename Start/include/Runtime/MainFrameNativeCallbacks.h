#pragma once
#include <Runtime/IdleLoopNative.h>
#include <Runtime/EditWindowNative.h>

namespace KaGuYa::Start::Runtime
{
class RootFrameRuntime;
class IMainFrameNativeCallbackApi
{
protected:
    ~IMainFrameNativeCallbackApi()=default;
public:
    virtual int TranslateAcceleratorA(HWND,HACCEL,MSG*)=0;
    virtual BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM)=0;
};
IMainFrameNativeCallbackApi& DefaultMainFrameNativeCallbackApi()noexcept;
class IMainFrameNativeIdleSource:public IIdleLoopNativeRootSource
{
public:
    virtual DWORD RunFrameAt4834F0(void* selectedRoot)=0;
};
class RootMainFrameNativeIdleSource final:public IMainFrameNativeIdleSource
{
public:
    RootMainFrameNativeIdleSource(RootRuntimeOwner&,RootFrameRuntime&)noexcept;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    DWORD ExitFlagAt1BC(void*)override;
    DWORD RunFrameAt4834F0(void*)override;
private:
    RootIdleLoopNativeSource root_;
    RootFrameRuntime& frame_;
};
// this分别指向F+34/F+38，不能当F直接读；42D1A0看EAX，424E10仅看AL。
BOOL TranslateMainFrameNativeAt42D1A0(void* filterAt34,MSG*,IMainFrameNativeCallbackApi&);
BOOL IdleMainFrameNativeAt424E10(void* idleAt38,IMainFrameNativeIdleSource&,IMainFrameNativeCallbackApi&);
// 原PE43A080/439F60完整287字节逐字节一致，含ret10；共享24h端点核。
LRESULT DispatchMainFrameNativeMessageAt43A080(EditWindowEndpointNativeBody24h&,UINT,WPARAM,LPARAM,
    IEditWindowNativeMessageApi& api=DefaultEditWindowNativeMessageApi());
}
