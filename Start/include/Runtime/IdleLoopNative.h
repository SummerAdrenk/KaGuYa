#pragma once
#include <Runtime/AppModuleMessageLoopsNative.h>
#include <Runtime/ModalDialogServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct IdleLoopNativeBody38h;
struct IdleLoopNativeMethods8h final
{
    using Filter=BOOL(__thiscall*)(IdleLoopNativeBody38h*,MSG*);
    using Idle=BOOL(__thiscall*)(IdleLoopNativeBody38h*,LONG);
    Filter filterAt00;
    Idle idleAt04;
};
struct IdleLoopNativeArray0Ch final
{
    void** valuesAt00;
    DWORD countAt04;
    DWORD capacityAt08;
};
// 4342D1..2EF：六个数组DWORD清零，MSG保持原栈字节；不能用MSG{}代签。
struct IdleLoopNativeBody38h final
{
    const IdleLoopNativeMethods8h* vptrAt00;
    IdleLoopNativeArray0Ch filtersAt04;
    IdleLoopNativeArray0Ch idlesAt10;
    MSG messageAt1C;
    IdleLoopNativeBody38h()noexcept{}
};
static_assert(sizeof(IdleLoopNativeMethods8h)==8&&sizeof(IdleLoopNativeArray0Ch)==0x0C);
static_assert(sizeof(IdleLoopNativeBody38h)==0x38&&offsetof(IdleLoopNativeBody38h,messageAt1C)==0x1C);
static_assert(offsetof(IdleLoopNativeBody38h,idlesAt10)==0x10&&offsetof(MSG,wParam)==8);
class IIdleLoopNativeRootSource
{
public:
    virtual ~IIdleLoopNativeRootSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual DWORD ExitFlagAt1BC(void*)=0;
};
class RootIdleLoopNativeSource final:public IIdleLoopNativeRootSource
{
public:
    explicit RootIdleLoopNativeSource(RootRuntimeOwner& root)noexcept:root_(root){}
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    DWORD ExitFlagAt1BC(void*)override;
private:
    RootRuntimeOwner& root_;
};
const IdleLoopNativeMethods8h& IdleLoopNativeMethods()noexcept;
void InitializeIdleLoopNativeAt4342D1(IdleLoopNativeBody38h&)noexcept;
BOOL FilterIdleLoopNativeAt42D1D0(IdleLoopNativeBody38h&,MSG*,IWindowClassNativeApi&);
BOOL DispatchIdleLoopNativeAt424E60(IdleLoopNativeBody38h&,LONG unused,IWindowClassNativeApi&);
int RunIdleLoopNativeAt43E980(IdleLoopNativeBody38h&,HWND unused,IIdleLoopNativeRootSource&,
    IModalDialogMessageApi&,IWindowClassNativeApi& api=DefaultWindowClassNativeApi());
// 原WM_CREATE的双表登记，先filter后idle，各自失败不回滚其它表，末尾写frame+8C byte0。
void RegisterIdleLoopFrameNativeAt423990(void* frame,AppModuleNativeBody3Ch&,
    AppModuleNativeBody3Ch*& publication,AppModuleMessageLoopsNativeServices& services=DefaultAppModuleMessageLoopsNativeServices());
// caller已执行4826A0并按当前线程417F50选定loop；这里只做WM_DESTROY双表移除。
void RemoveIdleLoopFrameNativeAt42E8D5(void* frame,IdleLoopNativeBody38h&,
    IAtlBaseModuleNativeDiagnostics& diagnostics=DefaultAtlBaseModuleNativeDiagnostics());
// 正常4346DA..71B的显式清理；filter正文不清零，不能擅自填全零或再执行C++析构。
void ReleaseIdleLoopArraysNativeAt4346DA(IdleLoopNativeBody38h&,IWindowClassNativeApi& api=DefaultWindowClassNativeApi());
// 593880选用的共享40E940清理另清filter字段，不能与正常内联清理混同。
void ReleaseIdleLoopArraysNativeAt40E940(IdleLoopNativeBody38h&,IWindowClassNativeApi& api=DefaultWindowClassNativeApi());
}
