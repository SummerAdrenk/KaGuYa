#pragma once
#include <Runtime/AppModuleNative.h>

namespace KaGuYa::Start::Runtime
{
class IWindowClassNativeApi;
struct AppModuleRegistryMapNative8h final
{
    const wchar_t* keyAt00;
    const wchar_t* valueAt04;
};
static_assert(sizeof(AppModuleRegistryMapNative8h)==8);
class IAppModuleRegistryObjectNative
{
protected:
    ~IAppModuleRegistryObjectNative()=default;
public:
    virtual void Construct()=0;
    virtual HRESULT InitializeLock()=0;
    virtual void* RegistrarInterface()noexcept=0;
    virtual HRESULT AddReplacement(const wchar_t*,const wchar_t*)=0;
    virtual HRESULT ApplyResource(const wchar_t* file,const char* name,const char* type,BOOL registering)=0;
    virtual void Destroy()=0;
};
class IAppModuleRegistryPlatformNative
{
protected:
    ~IAppModuleRegistryPlatformNative()=default;
public:
    virtual HINSTANCE ModuleInstance()=0;
    virtual DWORD GetModuleFileNameA(HMODULE,char*,DWORD)=0;
    virtual HMODULE GetModuleHandleA(const char*)=0;
    virtual DWORD GetLastError()=0;
    virtual bool StackAvailable(std::uint32_t)=0;
    virtual void* Allocate(std::uint32_t)=0;
    virtual void Free(void*)=0;
    virtual wchar_t* AnsiToWide(wchar_t*,const char*,std::uint32_t)=0;
    virtual char* WideToAnsi(char*,const wchar_t*,std::uint32_t)=0;
    virtual void InvalidParameter(int)=0;
    virtual void RangeFailure()=0;
};
IAppModuleRegistryPlatformNative& DefaultAppModuleRegistryPlatformNative()noexcept;
// 407C90按有符号DWORD相乘；失败不写output，不将可表示的负积改成溢出。
HRESULT MultiplyAppModuleRegistrySizeNativeAt407C90(DWORD* output,DWORD left,DWORD right)noexcept;
HRESULT InitializeAppModuleRegistryLockNativeAt439276(CRITICAL_SECTION&,BYTE& initialized,IWindowClassNativeApi&);
// object必须是fresh原生/原协议注册器，Destroy只在原正常返回路径执行，不补caller EH。
HRESULT UpdateAppModuleRegistryIdNativeAt439210(void* module,UINT,BOOL,const AppModuleRegistryMapNative8h*,
    IAppModuleRegistryObjectNative&,IAppModuleRegistryPlatformNative&);
HRESULT UpdateAppModuleRegistryNameNativeAt4396C0(void* module,const char*,BOOL,const AppModuleRegistryMapNative8h*,
    IAppModuleRegistryObjectNative&,IAppModuleRegistryPlatformNative&);
HRESULT UpdateAppModuleRegistryIdNativeAt439210(void* module,UINT,BOOL,const AppModuleRegistryMapNative8h*);
HRESULT UpdateAppModuleRegistryNameNativeAt4396C0(void* module,const char*,BOOL,const AppModuleRegistryMapNative8h*);
}
