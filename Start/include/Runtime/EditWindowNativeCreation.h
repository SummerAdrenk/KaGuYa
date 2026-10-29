#pragma once
#include <Runtime/EditWindowNativeCollection.h>
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
class IEditWindowNativeCreationApi
{
public:
    virtual ~IEditWindowNativeCreationApi()=default;
    virtual DWORD GetCurrentThreadId()=0;
    virtual void SetLastError(DWORD)=0;
    virtual BOOL EnableWindow(HWND,BOOL)=0;
    virtual HWND CreateWindowExA(DWORD,LPCSTR,LPCSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,void*)=0;
};
IEditWindowNativeCreationApi& DefaultEditWindowNativeCreationApi() noexcept;
class IEditWindowNativeParentSource
{
public:
    virtual ~IEditWindowNativeParentSource()=default;
    virtual HWND CaptureWindowAt4180C0()=0;
};
struct EditWindowNativeCreationDependencies final
{
    WindowClassModuleNativeBody38h& module;
    WindowClassWinModuleNativeBody2Ch& winModule;
    WindowClassNativeInfo68h& windowClass;
    WindowClassNativeInfo68h& controlClass;
    const RECT& creationRectAt601054;
    const HINSTANCE& instanceAt61A69C;
    IWindowClassNativeApi& classes;
    IEditWindowNativeCreationApi& windows;
    IEditWindowNativeParentSource& environment;
};
class EditWindowNativeCreationRuntime final:public IEditControlNativeCreationSource
{
public:
    explicit EditWindowNativeCreationRuntime(EditWindowNativeCreationDependencies d) noexcept:dependencies_(d){}
    void CreateControlAt485628(EditControlNativeBody28h&,HWND,DWORD) override;
    EditWindowNativeBody98h* CreateAt46FA30(EditWindowNativeReferences&,DWORD disable,DWORD alignment,std::uintptr_t borderArgument);
    LRESULT InitialAt48E090(HWND,UINT,WPARAM,LPARAM);
    // 原416B00不清匹配节点，弹出对象借用，不增加引用。
    EditWindowEndpointNativeBody24h* PopForCurrentThreadAt416B00();
private:
    void PushCreationAt46FC82(EditWindowEndpointNativeBody24h&);
    void CreateEndpoint(EditWindowEndpointNativeBody24h&,WindowClassNativeInfo68h&,DWORD,HWND);
    EditWindowNativeCreationDependencies dependencies_;
};
// 原两个Initial入口逐指令同义；scope只提供正文外模块借用，不存在第二创建链。
LRESULT CALLBACK InitialEditWindowNativeAt48E090(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK InitialEditControlNativeAt48E020(HWND,UINT,WPARAM,LPARAM);
}
