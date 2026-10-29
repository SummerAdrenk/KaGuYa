#include <Runtime/AppModuleRegistryNative.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <Runtime/WindowClassNative.h>
#include <atlbase.h>
#include <statreg.h>
#include <bit>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <intrin.h>
#include <malloc.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
HRESULT LastErrorResult(IAppModuleRegistryPlatformNative& api)
{
    auto value=api.GetLastError();if(std::bit_cast<std::int32_t>(value)>0)value=(value&0xFFFFu)|0x80070000u;
    return std::bit_cast<HRESULT>(value);
}
class Platform final:public IAppModuleRegistryPlatformNative
{
public:
    HINSTANCE ModuleInstance()override{return GetAtlBaseModuleInstanceNativeAt53EA87(ProcessAtlBaseModuleNative());}
    DWORD GetModuleFileNameA(HMODULE h,char* out,DWORD size)override{return ::GetModuleFileNameA(h,out,size);}
    HMODULE GetModuleHandleA(const char* name)override{return ::GetModuleHandleA(name);}
    DWORD GetLastError()override{return ::GetLastError();}
    bool StackAvailable(std::uint32_t bytes)override{return ATL::_ATL_SAFE_ALLOCA_IMPL::_AtlVerifyStackAvailable(bytes);}
    void* Allocate(std::uint32_t bytes)override{return std::malloc(bytes);}
    void Free(void* p)override{std::free(p);}
    wchar_t* AnsiToWide(wchar_t* out,const char* source,std::uint32_t size)override
    {
        if(!out||!source)return nullptr;out[0]=0;
        return ::MultiByteToWideChar(CP_THREAD_ACP,0,source,-1,out,static_cast<int>(size))?out:nullptr;
    }
    char* WideToAnsi(char* out,const wchar_t* source,std::uint32_t size)override
    {
        if(!out||!source)return nullptr;out[0]=0;
        return ::WideCharToMultiByte(CP_THREAD_ACP,0,source,-1,out,static_cast<int>(size),nullptr,nullptr)?out:nullptr;
    }
    void InvalidParameter(int error)override{errno=error;::_invalid_parameter_noinfo();}
    void RangeFailure()override{__fastfail(FAST_FAIL_RANGE_CHECK_FAILURE);}
};
constinit Platform platform;
// 正常核心暂复用ATL注册器；全部RGS/内部EH资格仍见独立审计，不借现代模块槽。
class Registrar final:public ATL::CRegObject
{
public:
    HRESULT InitializeNativeLock()
    {
        static_assert(offsetof(Registrar,m_csMap)==0x10&&sizeof(m_csMap)==0x1C);
        // 原initialized byte位于+28；经对象表示写入，保留库析构对该flag的正常读取。
        auto& initialized=*reinterpret_cast<BYTE*>(reinterpret_cast<std::uintptr_t>(this)+0x28u);
        return InitializeAppModuleRegistryLockNativeAt439276(m_csMap.m_sec,initialized,DefaultWindowClassNativeApi());
    }
    HRESULT Apply(const wchar_t* file,const char* name,const char* type,BOOL registering)
    {return RegisterFromResource(file,name,type,registering);}
};
static_assert(sizeof(Registrar)==0x2C);
class Object final:public IAppModuleRegistryObjectNative
{
    union Storage{Registrar value;Storage()noexcept{}~Storage()noexcept{}} storage_;
public:
    void Construct()override{::new(&storage_.value)Registrar;}
    HRESULT InitializeLock()override{return storage_.value.InitializeNativeLock();}
    void* RegistrarInterface()noexcept override{return static_cast<IRegistrarBase*>(&storage_.value);}
    HRESULT AddReplacement(const wchar_t* key,const wchar_t* value)override{return storage_.value.AddReplacement(key,value);}
    HRESULT ApplyResource(const wchar_t* file,const char* name,const char* type,BOOL registering)override
    {return storage_.value.Apply(file,name,type,registering);}
    void Destroy()override{storage_.value.~Registrar();}
};
struct Buffers final
{
    void* head=nullptr;
    IAppModuleRegistryPlatformNative& api;
    void* Allocate(std::uint32_t bytes)
    {
        if(~bytes<8u)throw ATL::CAtlException(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
        auto* p=api.Allocate(bytes+8u);if(!p)return nullptr;
        *static_cast<void**>(p)=head;head=p;
        return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p)+8u);
    }
    void Clear()
    {while(head){auto* p=head;head=Read(*static_cast<void* const*>(p));api.Free(p);}}
};
void Escape(wchar_t* destination,std::uint32_t capacity,const wchar_t* source)
{
    if(!capacity)return;std::uint32_t used=0;
    while(used<capacity-1u&&Read(*source))
    {
        *destination++=Read(*source);
        if(Read(*source)==L'\''&&++used<capacity-1u)*destination++=L'\'';
        ++used;++source;
    }
    *destination=0;
}
std::uint32_t WideBytes(std::size_t length)
{
    const auto characters=static_cast<std::uint32_t>(length)+1u;
    DWORD result;
    return SUCCEEDED(MultiplyAppModuleRegistrySizeNativeAt407C90(&result,characters,2))?result:0u;
}
HRESULT Common(void* raw,void* registrar)
{
    using Method=HRESULT(__thiscall*)(void*,void*);
    auto* table=Read(*static_cast<Method* const*>(raw));
    return Read(table[5])(raw,registrar);
}
#define APP_REG_BUFFER(bytes,arena) ((std::bit_cast<std::int32_t>(static_cast<DWORD>(bytes))<=0x400&&api.StackAvailable(bytes))?_alloca(bytes):(arena).Allocate(bytes))
HRESULT Update(void* raw,UINT id,const char* name,bool byName,BOOL registering,const AppModuleRegistryMapNative8h* entries,
    IAppModuleRegistryObjectNative& object,IAppModuleRegistryPlatformNative& api)
{
    object.Construct();const auto initialized=object.InitializeLock();
    if(FAILED(initialized)){object.Destroy();return initialized;}
    if(entries)
    {
        while(Read(entries->keyAt00))
        {
            const auto value=Read(entries->valueAt04),key=Read(entries->keyAt00);
            static_cast<void>(object.AddReplacement(key,value));
            entries=reinterpret_cast<const AppModuleRegistryMapNative8h*>(reinterpret_cast<std::uintptr_t>(entries)+8u);
        }
    }
    auto result=Common(raw,object.RegistrarInterface());
    if(FAILED(result)){object.Destroy();return result;}
    Buffers buffers{nullptr,api};char filename[260];
    const auto instance=api.ModuleInstance();const auto length=api.GetModuleFileNameA(instance,filename,260);
    if(!length){result=LastErrorResult(api);buffers.Clear();object.Destroy();return result;}
    if(length==260){buffers.Clear();object.Destroy();return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);}
    const auto pathBytes=WideBytes(std::strlen(filename));
    auto* wide=pathBytes?api.AnsiToWide(static_cast<wchar_t*>(APP_REG_BUFFER(pathBytes,buffers)),filename,pathBytes/2u):nullptr;
    if(!wide){buffers.Clear();object.Destroy();return E_OUTOFMEMORY;}
    wchar_t escaped[520],quoted[524];Escape(escaped,520,wide);const wchar_t* module=escaped;
    if(!instance||instance==api.GetModuleHandleA(nullptr))
    {
        quoted[0]=L'"';const auto bytes=WideBytes(std::wcslen(escaped));
        if(bytes>0x416u){api.InvalidParameter(ERANGE);buffers.Clear();object.Destroy();return E_FAIL;}
        std::memmove(quoted+1,escaped,bytes);const auto used=std::wcslen(quoted);quoted[used]=L'"';
        if((used+1u)*2u>=0x418u)api.RangeFailure();quoted[used+1u]=0;module=quoted;
    }
    result=object.AddReplacement(L"Module",module);
    if(SUCCEEDED(result))result=object.AddReplacement(L"Module_Raw",escaped);
    if(FAILED(result)){buffers.Clear();object.Destroy();return result;}
    Buffers resourceBuffers{nullptr,api};char* resource=nullptr;
    if(byName)
    {
        if(!name){buffers.Clear();object.Destroy();return E_OUTOFMEMORY;}
        const auto nameBytes=WideBytes(std::strlen(name));
        auto* nameWide=nameBytes?api.AnsiToWide(static_cast<wchar_t*>(APP_REG_BUFFER(nameBytes,buffers)),name,nameBytes/2u):nullptr;
        if(!nameWide){buffers.Clear();object.Destroy();return E_OUTOFMEMORY;}
        const auto narrowBytes=WideBytes(std::wcslen(nameWide));
        resource=narrowBytes?api.WideToAnsi(static_cast<char*>(APP_REG_BUFFER(narrowBytes,resourceBuffers)),nameWide,narrowBytes):nullptr;
    }
    else resource=reinterpret_cast<char*>(static_cast<std::uintptr_t>(static_cast<WORD>(id)));
    // 439AFC/439BD5即使name转换失败，也仍尝试REGISTRY转换后才检查两个结果。
    auto* type=api.WideToAnsi(static_cast<char*>(APP_REG_BUFFER(18u,resourceBuffers)),L"REGISTRY",18);
    if((byName&&!resource)||!type)result=E_OUTOFMEMORY;
    else result=object.ApplyResource(wide,resource,type,registering?TRUE:FALSE);
    resourceBuffers.Clear();buffers.Clear();object.Destroy();return result;
}
#undef APP_REG_BUFFER
}
IAppModuleRegistryPlatformNative& DefaultAppModuleRegistryPlatformNative()noexcept{return platform;}
HRESULT MultiplyAppModuleRegistrySizeNativeAt407C90(DWORD* output,DWORD left,DWORD right)noexcept
{
    const auto product=static_cast<std::int64_t>(std::bit_cast<std::int32_t>(left))*
        static_cast<std::int64_t>(std::bit_cast<std::int32_t>(right));
    if(product>0x7FFFFFFFll||product<(-0x7FFFFFFFll-1))return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
    *output=static_cast<DWORD>(product);return S_OK;
}
HRESULT InitializeAppModuleRegistryLockNativeAt439276(CRITICAL_SECTION& section,BYTE& initialized,IWindowClassNativeApi& api)
{
    DWORD result=0;
    if(!api.InitializeCriticalSectionAndSpinCount(&section,0))
    {
        result=api.GetLastError();
        if(std::bit_cast<std::int32_t>(result)>0)result=(result&0xFFFFu)|0x80070000u;
    }
    const auto hr=std::bit_cast<HRESULT>(result);
    if(SUCCEEDED(hr))initialized=1;return hr;
}
HRESULT UpdateAppModuleRegistryIdNativeAt439210(void* module,UINT id,BOOL registering,const AppModuleRegistryMapNative8h* entries,
    IAppModuleRegistryObjectNative& object,IAppModuleRegistryPlatformNative& api)
{return Update(module,id,nullptr,false,registering,entries,object,api);}
HRESULT UpdateAppModuleRegistryNameNativeAt4396C0(void* module,const char* name,BOOL registering,const AppModuleRegistryMapNative8h* entries,
    IAppModuleRegistryObjectNative& object,IAppModuleRegistryPlatformNative& api)
{return Update(module,0,name,true,registering,entries,object,api);}
HRESULT UpdateAppModuleRegistryIdNativeAt439210(void* module,UINT id,BOOL registering,const AppModuleRegistryMapNative8h* entries)
{Object object;return UpdateAppModuleRegistryIdNativeAt439210(module,id,registering,entries,object,platform);}
HRESULT UpdateAppModuleRegistryNameNativeAt4396C0(void* module,const char* name,BOOL registering,const AppModuleRegistryMapNative8h* entries)
{Object object;return UpdateAppModuleRegistryNameNativeAt4396C0(module,name,registering,entries,object,platform);}
}
