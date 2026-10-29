#pragma once
#include <Runtime/ReadScriptReturnTitleNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
struct ToolWindowNativeInteractionServices;
struct RootReadScriptReturnTitleNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeGraphicsRuntimeLibrary& graphics;
    ToolWindowNativeInteractionServices& tool;
    GeneralConfirmDialogNativeServices& confirmation;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
};
class RootReadScriptReturnTitleNativeSource final:public IReadScriptReturnTitleNativeSource
{
public:
    explicit RootReadScriptReturnTitleNativeSource(RootReadScriptReturnTitleNativeDependencies d)noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t)override;
    void EnsureRootAt413BE0(std::uint32_t)override;
    void* RootAt6198C4(std::uint32_t)override;
    std::uint8_t ReadWaitMarkAt418C10(void*,std::uint32_t)override;
    void WriteWaitMarkAt418C10(void*,std::uint8_t,std::uint32_t)override;
    void CaptureToolAt4C01F0(void*,std::uint32_t)override;
    void SetToolCaptureModeAt4C2300(void*,std::uint8_t,std::uint32_t)override;
    void RestoreToolAt4BF4E0(void*,std::uint32_t)override;
    DrawLayersView LayersAt4175A0(void*,std::uint32_t)override;
    IModalDialogRunnerServices& ModalAt4AD230(void*,std::uint32_t)override;
    BOOL PostMessage(HWND,UINT,WPARAM,LPARAM,std::uint32_t)override;
private:
    RootReadScriptReturnTitleNativeDependencies dependencies_;
};
class RootReadScriptReturnTitleNativeRuntime final:public IReadScriptReturnTitleNativeServicesSource
{
public:
    explicit RootReadScriptReturnTitleNativeRuntime(RootReadScriptReturnTitleNativeDependencies)noexcept;
    ReadScriptReturnTitleNativeServices& ReturnTitleNativeServices()override{return services_;}
    RootReadScriptReturnTitleNativeSource& Source()noexcept{return source_;}
private:
    RootReadScriptReturnTitleNativeSource source_;
    ReadScriptReturnTitleNativeServices services_;
};
}
