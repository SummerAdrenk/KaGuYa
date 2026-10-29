#pragma once
#include <Platform/ProcessEntryCoordinator.h>
#include <Runtime/AppModuleNative.h>
#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IWinMainNativeApplication
{
public:
    virtual ~IWinMainNativeApplication()=default;
    virtual int RunApplicationAt4342A0(LPSTR commandLine,int showCommand)=0;
};
class IWinMainNativeSource:public IWinMainNativeApplication
{
public:
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual void InitializeEnvironmentAt498280(EnvironmentRootStorage*,HINSTANCE,HINSTANCE,LPSTR,int)=0;
    virtual int MessageBoxA(HWND,const char*,const char*,UINT)=0;
};
// 这里只借长期生产对象；不预先Capture Environment或用现代CAppModule建立第二模块。
class EnvironmentWinMainNativeSource final:public IWinMainNativeSource
{
public:
    EnvironmentWinMainNativeSource(EnvironmentStartupRuntime&,IEnvironmentPathApi&,IResourceVersionPlatformApi&,
        IStartFileApi&,ISystemDataEnvironmentDiagnostics&,IWinMainNativeApplication&,
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory())noexcept;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    void InitializeEnvironmentAt498280(EnvironmentRootStorage*,HINSTANCE,HINSTANCE,LPSTR,int)override;
    int RunApplicationAt4342A0(LPSTR,int)override;
    int MessageBoxA(HWND,const char*,const char*,UINT)override;
private:
    EnvironmentStartupRuntime& environment_;
    IEnvironmentPathApi& paths_;
    IResourceVersionPlatformApi& versions_;
    IStartFileApi& files_;
    ISystemDataEnvironmentDiagnostics& diagnostics_;
    IWinMainNativeApplication& application_;
    IRuntimeStringManagerSource& strings_;
    IEnvironmentRootMemory& memory_;
};
struct WinMainNativeFramework final
{
    AppModuleNativeBody3Ch& module;
    AppModuleNativeBody3Ch*& publication;
    GUID& library;
    AppModuleAutoMapNativeSource automatic;
    AppModuleNativeServices services;
};
WinMainNativeFramework ProcessWinMainNativeFramework()noexcept;
// 原43D6E0没有caller EH；字符串/mutex及框架的正常释放仅在原控制流抵达时执行。
// 此入口不是旧ProcessEntryCoordinator的安全/DTO域，不能给失败路径补RAII或null兜底。
int RunWinMainNativeAt43D6E0(HINSTANCE,HINSTANCE,LPSTR,int,IWinMainNativeSource&,
    Platform::IProcessEntryApi&,WinMainNativeFramework);
}
