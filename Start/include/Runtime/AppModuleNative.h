#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
using AppModuleObjectMainNative=void(WINAPI*)(BOOL);
using AppModuleTermNative=void(WINAPI*)(DWORD);
struct AppModuleObjectMapNative24h final
{
    const void* classIdAt00;
    std::uint32_t opaqueAt04[3];
    void* factoryAt10;
    std::uint32_t opaqueAt14[3];
    AppModuleObjectMainNative objectMainAt20;
};
struct AppModuleAutoMapNative20h final
{
    std::uint32_t opaqueAt00[7];
    AppModuleObjectMainNative objectMainAt1C;
};
struct AppModuleTermNodeNative0Ch final
{
    AppModuleTermNative callbackAt00;
    DWORD argumentAt04;
    AppModuleTermNodeNative0Ch* nextAt08;
};
struct AppModuleMessageLoopsNative0Ch final
{
    DWORD* threadsAt00;
    void** loopsAt04;
    std::uint32_t countAt08;
};
struct AppModuleWindowsNative0Ch final
{
    HWND* windowsAt00;
    std::int32_t countAt04;
    std::uint32_t capacityAt08;
};
struct AppModuleBaseNativeBody30h final
{
    const void* vptrAt00;
    std::uint32_t sizeAt04;
    LONG lockCountAt08;
    AppModuleTermNodeNative0Ch* terminationAt0C;
    CRITICAL_SECTION lockAt10;
    void* gitAt28;
    AppModuleObjectMapNative24h* objectMapAt2C;
    AppModuleBaseNativeBody30h()noexcept{}
};
static_assert(sizeof(AppModuleBaseNativeBody30h)==0x30&&offsetof(AppModuleBaseNativeBody30h,gitAt28)==0x28);
// 619828为3Ch AppModule，不是61A698的38h BaseModule。此处只描述已核字段。
// vptr/CRT构造不由这些显式入口伪造；caller必须提供已开始寿命的实际正文。
struct AppModuleNativeBody3Ch final
{
    const void* vptrAt00;
    std::uint32_t sizeAt04;
    LONG lockCountAt08;
    AppModuleTermNodeNative0Ch* terminationAt0C;
    CRITICAL_SECTION lockAt10;
    void* gitAt28; // 407580创建StdGlobalInterfaceTable，不是CRegObject注册器。
    AppModuleObjectMapNative24h* objectMapAt2C;
    DWORD threadAt30;
    AppModuleMessageLoopsNative0Ch* loopsAt34;
    AppModuleWindowsNative0Ch* windowsAt38;
    AppModuleNativeBody3Ch()noexcept{}
};
static_assert(sizeof(void*)==4);
static_assert(sizeof(AppModuleObjectMapNative24h)==0x24&&offsetof(AppModuleObjectMapNative24h,objectMainAt20)==0x20);
static_assert(sizeof(AppModuleAutoMapNative20h)==0x20&&offsetof(AppModuleAutoMapNative20h,objectMainAt1C)==0x1C);
static_assert(sizeof(AppModuleTermNodeNative0Ch)==0x0C&&sizeof(AppModuleMessageLoopsNative0Ch)==0x0C);
static_assert(sizeof(AppModuleWindowsNative0Ch)==0x0C&&sizeof(AppModuleNativeBody3Ch)==0x3C);
static_assert(offsetof(AppModuleNativeBody3Ch,lockAt10)==0x10&&offsetof(AppModuleNativeBody3Ch,gitAt28)==0x28);
static_assert(offsetof(AppModuleNativeBody3Ch,objectMapAt2C)==0x2C&&offsetof(AppModuleNativeBody3Ch,loopsAt34)==0x34);
static_assert(offsetof(AppModuleNativeBody3Ch,windowsAt38)==0x38&&std::is_standard_layout_v<AppModuleNativeBody3Ch>);
static_assert(std::is_trivially_destructible_v<AppModuleNativeBody3Ch>);

class IAppModuleNativePlatformApi
{
protected:
    ~IAppModuleNativePlatformApi()=default;
public:
    virtual DWORD GetCurrentThreadId()=0;
    virtual void EnterCriticalSection(LPCRITICAL_SECTION)=0;
    virtual void LeaveCriticalSection(LPCRITICAL_SECTION)=0;
    virtual void DeleteCriticalSection(LPCRITICAL_SECTION)=0;
    virtual BOOL DestroyWindow(HWND)=0;
    virtual void Free(void*)=0;
    virtual void RaiseException(DWORD,DWORD)=0;
};
IAppModuleNativePlatformApi& DefaultAppModuleNativePlatformApi()noexcept;
struct AppModuleNativeServices final
{
    IEnvironmentRootMemory& memory;
    IAppModuleNativePlatformApi& platform;
};
// 两个auto-map边界槽是借用，可由回调改变；不能复制成固定span或缓存成vector。
struct AppModuleAutoMapNativeSource final
{
    const AppModuleAutoMapNative20h* const*& beginAt601008;
    const AppModuleAutoMapNative20h* const*& endAt60100C;
};
HRESULT InitializeAppModuleNativeAt418DC0(AppModuleNativeBody3Ch&,AppModuleObjectMapNative24h*,
    HINSTANCE unusedInstance,const GUID* optionalLibraryId,GUID& libraryIdAt619870,
    AppModuleAutoMapNativeSource,AppModuleNativeServices&);
void ReleaseAppModuleWindowsNativeAt43D866(AppModuleNativeBody3Ch& fixedGlobal,
    AppModuleNativeBody3Ch*& publicationAt6197DC,AppModuleNativeServices&);
void ReleaseAppModuleMessageLoopsNativeAt43D8D9(AppModuleNativeBody3Ch&,AppModuleNativeServices&);
void ReleaseAppModuleFactoriesNativeAt43D922(AppModuleNativeBody3Ch&);
void NotifyAppModuleAutoMapNativeAt43D954(AppModuleAutoMapNativeSource);
void ShutdownAppModuleNativeAt438830(AppModuleNativeBody3Ch&,AppModuleNativeServices&);
void ShutdownAppModuleNativeAt438830(AppModuleBaseNativeBody30h&,AppModuleNativeServices&);
}
