#pragma once
#include <Runtime/QuickOperationsNative.h>
#include <Runtime/RootQuickFileNativeSource.h>

namespace KaGuYa::Start::Runtime
{
struct RootQuickOperationsNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    RootQuickFileNativeSource& files;
    GeneralConfirmDialogNativeServices& confirmation;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};
class RootQuickOperationsNativeSource final:public IQuickOperationsNativeSource
{
public:
    explicit RootQuickOperationsNativeSource(RootQuickOperationsNativeDependencies d)noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0()override;
    IRuntimeStringManagerSource& Strings()override;
    BOOL DeleteFileA(const char*)override;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    void* ParamsAt47BDB0(void*)override;
    std::uint32_t WriteContainerAt4C7B10(void*,const char*)override;
    std::uint32_t ReadContainerAt4C7420(void*,const char*,std::uint32_t)override;
    void CapturePreviewAt48DBE0(void*)override;
    std::uint32_t SavePreviewAt489420(void*,const char*)override;
    IEnvironmentArchiveRuntime* RegistryAt417990(EnvironmentRootStorage*,unsigned)override;
    YtlReferenceSlotRuntime& TransitionFrameAt1F0(void*)override;
    IModalDialogRunnerServices& ModalForProgramAt3C(void*)override;
    int MessageBoxA(HWND,const char*,const char*,UINT)override;
private:
    RootQuickOperationsNativeDependencies dependencies_;
};
// 只持借用适配器；Root/Environment/文件/确认框服务必须长于所有Quick入口和末Ref。
class RootQuickOperationsNativeServicesRuntime final
{
public:
    explicit RootQuickOperationsNativeServicesRuntime(RootQuickOperationsNativeDependencies d)noexcept
        :source_(d),services_{source_,d.files.Catalog(),d.confirmation,d.memory}{}
    RootQuickOperationsNativeServicesRuntime(const RootQuickOperationsNativeServicesRuntime&)=delete;
    RootQuickOperationsNativeServicesRuntime& operator=(const RootQuickOperationsNativeServicesRuntime&)=delete;
    QuickOperationsNativeServices& Services()noexcept{return services_;}
    RootQuickOperationsNativeSource& Source()noexcept{return source_;}
private:
    RootQuickOperationsNativeSource source_;
    QuickOperationsNativeServices services_;
};
}
