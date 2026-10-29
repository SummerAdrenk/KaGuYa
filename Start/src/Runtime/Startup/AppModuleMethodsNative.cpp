#include <Runtime/AppModuleMethodsNative.h>
#include <Objbase.h>
#include <ObjIdl.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
thread_local AppModuleNativeVirtualServices* active=nullptr;
class Com final:public IAppModuleNativeComApi
{
public:
    HRESULT CreateGlobalInterfaceTable(void** output)override
    {return ::CoCreateInstance(CLSID_StdGlobalInterfaceTable,nullptr,CLSCTX_INPROC_SERVER,IID_IGlobalInterfaceTable,output);}
};
constinit Com com;
AppModuleNativeVirtualServices& Services()noexcept
{
    static AppModuleNativeVirtualServices services{{DefaultEnvironmentRootMemory(),DefaultAppModuleNativePlatformApi()},com};
    return active?*active:services;
}
LONG* Count(void* body)noexcept{return reinterpret_cast<LONG*>(reinterpret_cast<std::uintptr_t>(body)+8u);}
LONG __fastcall Lock(void* p,void*){return ::InterlockedIncrement(Count(p));}
LONG __fastcall Unlock(void* p,void*){return ::InterlockedDecrement(Count(p));}
LONG __fastcall LockCount(void* p,void*){return Read(*Count(p));}
HRESULT __fastcall Git(void* p,void*,void** out){return GetAppModuleGitNativeAt407580(p,out,Services().com);}
HRESULT __fastcall Replacements(void* p,void*,void* registrar){return AddAppModuleCommonReplacementsNativeAt4075D0(p,registrar);}
void* __fastcall DeleteBase(void* p,void*,std::uint32_t flags)
{
    auto& body=*static_cast<AppModuleBaseNativeBody30h*>(p);body.vptrAt00=&AppModuleBaseNativeMethods();
    ShutdownAppModuleNativeAt438830(body,Services().lifetime);
    if(flags&1u)Services().lifetime.memory.DeleteBody(p,0x30);return p;
}
void* __fastcall DeleteApp(void* p,void*,std::uint32_t flags)
{
    auto& body=*static_cast<AppModuleNativeBody3Ch*>(p);body.vptrAt00=&AppModuleBaseNativeMethods();
    ShutdownAppModuleNativeAt438830(body,Services().lifetime);
    if(flags&1u)Services().lifetime.memory.DeleteBody(p,0x3C);return p;
}
HRESULT WINAPI Deprecated(void*,std::uintptr_t,BOOL,const AppModuleRegistryMapNative8h*){return E_FAIL;}
HRESULT WINAPI UpdateId(void* p,UINT id,BOOL start,const AppModuleRegistryMapNative8h* map)
{return UpdateAppModuleRegistryIdNativeAt439210(p,id,start,map);}
HRESULT WINAPI UpdateName(void* p,const char* name,BOOL start,const AppModuleRegistryMapNative8h* map)
{return UpdateAppModuleRegistryNameNativeAt4396C0(p,name,start,map);}
const AppModuleNativeVtable10 base{
    reinterpret_cast<decltype(base.deleteAt00)>(&DeleteBase),reinterpret_cast<decltype(base.lockAt04)>(&Lock),
    reinterpret_cast<decltype(base.unlockAt08)>(&Unlock),reinterpret_cast<decltype(base.lockCountAt0C)>(&LockCount),
    reinterpret_cast<decltype(base.gitAt10)>(&Git),reinterpret_cast<decltype(base.replacementsAt14)>(&Replacements),
    Deprecated,Deprecated,UpdateId,UpdateName};
const AppModuleNativeVtable10 app{
    reinterpret_cast<decltype(app.deleteAt00)>(&DeleteApp),base.lockAt04,base.unlockAt08,base.lockCountAt0C,
    base.gitAt10,base.replacementsAt14,Deprecated,Deprecated,UpdateId,UpdateName};
}
IAppModuleNativeComApi& DefaultAppModuleNativeComApi()noexcept{return com;}
AppModuleNativeVirtualScope::AppModuleNativeVirtualScope(AppModuleNativeVirtualServices& services)noexcept:previous_(active){active=&services;}
AppModuleNativeVirtualScope::~AppModuleNativeVirtualScope()noexcept{active=previous_;}
const AppModuleNativeVtable10& AppModuleNativeMethods()noexcept{return app;}
const AppModuleNativeVtable10& AppModuleBaseNativeMethods()noexcept{return base;}
HRESULT GetAppModuleGitNativeAt407580(void* raw,void** output,IAppModuleNativeComApi& api)
{
    if(!output)return E_POINTER;
    auto** slot=reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(raw)+0x28u);HRESULT result=S_OK;
    if(!Read(*slot))result=api.CreateGlobalInterfaceTable(slot);
    if(SUCCEEDED(result))
    {
        *output=Read(*slot);auto* selected=Read(*slot);
        using AddRef=ULONG(WINAPI*)(void*);
        auto* table=Read(*static_cast<AddRef* const*>(selected));Read(table[1])(selected);
    }
    return result;
}
HRESULT AddAppModuleCommonReplacementsNativeAt4075D0(void*,void* registrar)
{
    using Add=HRESULT(WINAPI*)(void*,const wchar_t*,const wchar_t*);
    auto* table=Read(*static_cast<Add* const*>(registrar));
    return Read(table[3])(registrar,L"APPID",L"");
}
}
