#include <Runtime/AtlBaseModuleNative.h>
#include <atlbase.h>
#include <bit>
#include <cerrno>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
constexpr GUID versionGuid{0xD3492828,0x4138,0x4FD4,{0xB3,0xEC,0xDB,0x99,0x13,0x5E,0xAE,0x86}};
class Diagnostics final:public IAtlBaseModuleNativeDiagnostics
{
public:
    BOOL IsDebuggerPresent()override{return ::IsDebuggerPresent();}
    void OutputDebugStringW(const wchar_t* value)override{::OutputDebugStringW(value);}
    void MoveBytesAt43D2D0(void* destination,std::uint32_t capacity,const void* source,std::uint32_t count)override
    {
        if(!count)return;
        const auto error=(!destination||!source)?EINVAL:capacity<count?ERANGE:0;
        if(error)
        {
            errno=error;::_invalid_parameter_noinfo();
            // 43D2D0仅产生22/34，4136D0均映射到80070057；handler异常不被吞掉。
            throw ATL::CAtlException(E_INVALIDARG);
        }
        std::memmove(destination,source,count);
    }
};
constinit Diagnostics diagnostics;
}
IAtlBaseModuleNativeDiagnostics& DefaultAtlBaseModuleNativeDiagnostics()noexcept{return diagnostics;}
const GUID& AtlBaseModuleNativeVersionGuid()noexcept{return versionGuid;}
WindowClassModuleNativeBody38h* ConstructAtlBaseModuleNativeAt53E893(WindowClassModuleNativeBody38h& body,
    HINSTANCE image,const GUID& guid,BYTE& failed,AtlBaseModuleNativeServices& services)
{
    std::memset(&body.resourceLockAt14,0,sizeof(body.resourceLockAt14));
    body.resourceInstancesAt2C=nullptr;body.resourceCountAt30=0;body.resourceArrayWordAt34=0;
    body.sizeAt00=0x38;body.resourceInstanceAt08=image;body.instanceAt04=image;
    body.buildVersionAt0C=0xE00;body.versionGuidAt10=&guid;
    std::uint32_t result=0;
    if(!services.windows.InitializeCriticalSectionAndSpinCount(&body.resourceLockAt14,0))
    {
        result=services.windows.GetLastError();
        if(std::bit_cast<std::int32_t>(result)>0)result=(result&0xFFFFu)|0x80070000u;
    }
    if(std::bit_cast<std::int32_t>(result)<0)
    {
        if(services.diagnostics.IsDebuggerPresent())
            services.diagnostics.OutputDebugStringW(L"ERROR : Unable to initialize critical section in CAtlBaseModule\n");
        failed=1;
    }
    return &body;
}
void ReleaseAtlBaseModuleNativeAt53E949(WindowClassModuleNativeBody38h& body,AtlBaseModuleNativeServices& services)
{
    services.windows.DeleteCriticalSection(&body.resourceLockAt14);
    if(Read(body.resourceInstancesAt2C))
    {
        services.windows.Free(Read(body.resourceInstancesAt2C));
        body.resourceInstancesAt2C=nullptr;
    }
    body.resourceCountAt30=0;body.resourceArrayWordAt34=0;
}
HINSTANCE GetAtlBaseModuleInstanceNativeAt53EA87(const WindowClassModuleNativeBody38h& body)noexcept
{return Read(body.instanceAt04);}
}
