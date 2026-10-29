#pragma once
#include <Runtime/DialogEndpointNative.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class IAboutDialogNativeSource
{
public:
    virtual ~IAboutDialogNativeSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual void WriteMinimizeAt1B2(void*,BYTE)=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual void* GameSystemAt418480()=0;
};
class RootAboutDialogNativeSource final:public IAboutDialogNativeSource
{
public:
    RootAboutDialogNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,ParamsRuntimeRootOwner&)noexcept;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    void WriteMinimizeAt1B2(void*,BYTE)override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    void* GameSystemAt418480()override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;ParamsRuntimeRootOwner& params_;
};
class IAboutDialogNativeApi
{
public:
    virtual ~IAboutDialogNativeApi()=default;
    virtual HWND GetActiveWindow()=0;
    virtual void SetLastError(DWORD)=0;
    virtual INT_PTR DialogBoxParamA(HINSTANCE,LPCSTR,HWND,DLGPROC,LPARAM)=0;
    virtual BOOL SetDlgItemTextA(HWND,int,LPCSTR)=0;
    virtual BOOL EndDialog(HWND,INT_PTR)=0;
};
IAboutDialogNativeApi& DefaultAboutDialogNativeApi()noexcept;
struct AboutDialogNativeServices final
{
    IAboutDialogNativeSource& source;
    IAboutDialogNativeApi& api;
    DialogEndpointNativeServices& endpoint;
    WindowClassModuleNativeBody38h& nativeBase;
};
class AboutDialogNativeScope final
{
public:
    explicit AboutDialogNativeScope(AboutDialogNativeServices&)noexcept;
    ~AboutDialogNativeScope()noexcept;
    AboutDialogNativeScope(const AboutDialogNativeScope&)=delete;
    AboutDialogNativeScope& operator=(const AboutDialogNativeScope&)=delete;
private:
    AboutDialogNativeServices* previous_;
};
const DialogEndpointNativeVtable4& AboutDialogNativeVtable()noexcept;
void InitializeAboutDialogNativeAt42F84A(DialogEndpointNativeBody20h&)noexcept;
void RunAboutDialogNativeAt42F84A(AboutDialogNativeServices&);
LRESULT InitializeAboutDialogNativeAt424EB0(DialogEndpointNativeBody20h&,AboutDialogNativeServices&);
BOOL ProcessAboutDialogNativeAt42E520(DialogEndpointNativeBody20h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,AboutDialogNativeServices&);
}
