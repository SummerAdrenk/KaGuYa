#pragma once
#include <Runtime/AppModuleNative.h>
#include <Runtime/AppModuleRegistryNative.h>
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
struct AppModuleNativeVtable10 final
{
    void*(__thiscall* deleteAt00)(void*,std::uint32_t);
    LONG(__thiscall* lockAt04)(void*);
    LONG(__thiscall* unlockAt08)(void*);
    LONG(__thiscall* lockCountAt0C)(void*);
    HRESULT(__thiscall* gitAt10)(void*,void**);
    HRESULT(__thiscall* replacementsAt14)(void*,void*);
    HRESULT(WINAPI* deprecatedAt18)(void*,std::uintptr_t,BOOL,const AppModuleRegistryMapNative8h*);
    HRESULT(WINAPI* deprecatedAt1C)(void*,std::uintptr_t,BOOL,const AppModuleRegistryMapNative8h*);
    HRESULT(WINAPI* updateIdAt20)(void*,UINT,BOOL,const AppModuleRegistryMapNative8h*);
    HRESULT(WINAPI* updateNameAt24)(void*,const char*,BOOL,const AppModuleRegistryMapNative8h*);
};
static_assert(sizeof(AppModuleNativeVtable10)==0x28&&offsetof(AppModuleNativeVtable10,updateIdAt20)==0x20);
class IAppModuleNativeComApi
{
protected:
    ~IAppModuleNativeComApi()=default;
public:
    virtual HRESULT CreateGlobalInterfaceTable(void**)=0;
};
IAppModuleNativeComApi& DefaultAppModuleNativeComApi()noexcept;
struct AppModuleNativeVirtualServices final
{
    AppModuleNativeServices lifetime;
    IAppModuleNativeComApi& com;
};
class AppModuleNativeVirtualScope final
{
public:
    explicit AppModuleNativeVirtualScope(AppModuleNativeVirtualServices&)noexcept;
    ~AppModuleNativeVirtualScope()noexcept;
    AppModuleNativeVirtualScope(const AppModuleNativeVirtualScope&)=delete;
    AppModuleNativeVirtualScope& operator=(const AppModuleNativeVirtualScope&)=delete;
private:
    AppModuleNativeVirtualServices* previous_;
};
const AppModuleNativeVtable10& AppModuleNativeMethods()noexcept;
const AppModuleNativeVtable10& AppModuleBaseNativeMethods()noexcept;
HRESULT GetAppModuleGitNativeAt407580(void*,void**,IAppModuleNativeComApi&);
HRESULT AddAppModuleCommonReplacementsNativeAt4075D0(void*,void* registrar);
}
