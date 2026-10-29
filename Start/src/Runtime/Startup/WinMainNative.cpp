#include <Runtime/WinMainNative.h>
#include <Runtime/ProcessAppModuleNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
const char installError[]=
    "\x83\x43\x83\x93\x83\x58\x83\x67\x81\x5B\x83\x8B\x90\xDD\x92\xE8"
    "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xCC\x83\x8D\x81\x5B\x83\x68"
    "\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
const char systemError[]=
    "\x83\x56\x83\x58\x83\x65\x83\x80\x83\x66\x81\x5B\x83\x5E\x82\xCC"
    "\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC"
    "\x82\xB5\x82\xBD\x81\x49";
static_assert(sizeof(installError)==49&&sizeof(systemError)==39);
BYTE* Paths(EnvironmentRootStorage* environment)noexcept
{
    // 43D74B..76C：固定首读base，control/stride在bias之前读，DWORD乘加回绕。
    const auto base=Read(environment->pathsAt48.baseAt00);
    if(!base)return nullptr;
    const auto control=Read(environment->pathsAt48.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    return reinterpret_cast<BYTE*>(base+Read(environment->pathsAt48.biasAt04)*stride);
}
}
EnvironmentWinMainNativeSource::EnvironmentWinMainNativeSource(EnvironmentStartupRuntime& environment,
    IEnvironmentPathApi& paths,IResourceVersionPlatformApi& versions,IStartFileApi& files,
    ISystemDataEnvironmentDiagnostics& diagnostics,IWinMainNativeApplication& application,
    IRuntimeStringManagerSource& strings,IEnvironmentRootMemory& memory)noexcept
    :environment_(environment),paths_(paths),versions_(versions),files_(files),diagnostics_(diagnostics),
     application_(application),strings_(strings),memory_(memory){}
EnvironmentRootStorage* EnvironmentWinMainNativeSource::EnvironmentAt4180C0()
{return environment_.RootSource().CaptureAt4180C0();}
void EnvironmentWinMainNativeSource::InitializeEnvironmentAt498280(EnvironmentRootStorage* receiver,
    HINSTANCE image,HINSTANCE previous,LPSTR line,int show)
{
    Runtime::InitializeEnvironmentAt498280(*receiver,environment_.RootSource(),environment_.ParamsRootSource(),
        image,previous,line,show,paths_,versions_,files_,diagnostics_,strings_,ProcessSettingTagErrorOwner(),memory_);
}
int EnvironmentWinMainNativeSource::RunApplicationAt4342A0(LPSTR line,int show)
{return application_.RunApplicationAt4342A0(line,show);}
int EnvironmentWinMainNativeSource::MessageBoxA(HWND window,const char* text,const char* caption,UINT flags)
{return ::MessageBoxA(window,text,caption,flags);}
WinMainNativeFramework ProcessWinMainNativeFramework()noexcept
{
    return {ProcessAppModuleNative(),ProcessAppModulePublicationNative(),ProcessAppModuleLibraryIdNative(),
        ProcessAppModuleAutoMapNative(),{DefaultEnvironmentRootMemory(),DefaultAppModuleNativePlatformApi()}};
}
int RunWinMainNativeAt43D6E0(HINSTANCE image,HINSTANCE previous,LPSTR line,int show,
    IWinMainNativeSource& source,Platform::IProcessEntryApi& platform,WinMainNativeFramework framework)
{
    platform.InitializeComApartment();platform.PrimeDefaultWindowProcedure();platform.InitializeCommonControls();
    static_cast<void>(InitializeAppModuleNativeAt418DC0(framework.module,nullptr,image,nullptr,
        framework.library,framework.automatic,framework.services));
    source.InitializeEnvironmentAt498280(source.EnvironmentAt4180C0(),image,previous,line,show);
    int result=0;
    if(!Read(*Paths(source.EnvironmentAt4180C0())))
        static_cast<void>(source.MessageBoxA(nullptr,installError,"error !!",MB_ICONERROR));
    else if(!Read(*(reinterpret_cast<const BYTE*>(ResolveEnvironmentArchiveAt417990(*source.EnvironmentAt4180C0(),0))+4)))
        static_cast<void>(source.MessageBoxA(nullptr,systemError,"error !!",MB_ICONERROR));
    else
    {
        // 43D7C6先取第四次Env的title/header；追加/运行若抛出，原调用者不清此串或mutex。
        auto* environment=source.EnvironmentAt4180C0();
        auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(Read(*reinterpret_cast<const char* const*>(&environment->titleAt10))))-1;
        auto* copied=CloneRuntimeCStringDataAt414540(header);
        const char* chars=reinterpret_cast<const char*>(copied+1);
        union Text{RuntimeCString value;Text()noexcept{}~Text()noexcept{}} name;
        std::memcpy(&name.value,&chars,sizeof(chars));
        AppendRuntimeCStringAt413550(name.value,"_Running",8);
        // 43D7F5的CMutex虚+08=432640：局部handle刚写0，仅判零，无Win32调用。
        // 原7F8的ESI固定跨CreateMutex和Run；即使回调改fresh槽也按该header释放。
        chars=name.value.GetString();
        const auto mutex=platform.CreateOwnedMutex(chars);
        if(platform.ReadLastError()!=ERROR_ALREADY_EXISTS&&platform.ReadLastError()!=ERROR_INVALID_HANDLE)
            result=source.RunApplicationAt4342A0(line,show);
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars))-1);
        if(mutex){platform.ReleaseOwnedMutex(mutex);platform.CloseKernelHandle(mutex);}
    }
    ReleaseAppModuleWindowsNativeAt43D866(framework.module,framework.publication,framework.services);
    ReleaseAppModuleMessageLoopsNativeAt43D8D9(framework.module,framework.services);
    ReleaseAppModuleFactoriesNativeAt43D922(framework.module);
    NotifyAppModuleAutoMapNativeAt43D954(framework.automatic);
    ShutdownAppModuleNativeAt438830(framework.module,framework.services);
    platform.UninitializeComApartment();return result;
}
}
