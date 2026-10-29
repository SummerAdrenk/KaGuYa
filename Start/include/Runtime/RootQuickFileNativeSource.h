#pragma once
#include <Runtime/SaveFileFactoryNativeIo.h>
#include <Runtime/AutoSaveOperationsNative.h>
#include <Runtime/SaveFileCatalogItemNative.h>
#include <Runtime/RootSaveLoadNativeSource.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct RootQuickFileNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    RootSaveLoadNativeSource& persistence;
    decltype(&::DeleteFileA) deleteFile=&::DeleteFileA;
};
// 仅连接原0C/10h工厂和临时70h目录刷新叶，不拥有目录、catalog或CParams快照。
// 删除API显式注入；只读/内存测试不得借persistence的真实Win32删除叶。
class RootQuickFileNativeSource final:public IAutoSaveOperationsNativeSource,public ISaveFileCatalogItemNativeSource
{
public:
    explicit RootQuickFileNativeSource(RootQuickFileNativeDependencies d)noexcept:dependencies_(d){}
    ISaveFileFactoryNativeIoSource& FileIo()noexcept{return *this;}
    ISaveFileCatalogItemNativeSource& Catalog()noexcept{return *this;}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0()override;
    IRuntimeStringManagerSource& Strings()override;
    IEnvironmentArchiveRuntime* RegistryAt417990(EnvironmentRootStorage*,std::uint32_t)override;
    BOOL DeleteFileA(const char*)override;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    void* ParamsAt47BDB0(void*)override;
    std::uint32_t WriteContainerAt4C7B10(void*,const char*)override;
    std::uint32_t ReadContainerAt4C7420(void*,const char*,std::uint32_t)override;
    std::uint32_t ReadContainerAt4C7420(ScriptParamsNativeBody&,const char*,std::uint32_t)override;
    void CapturePreviewAt48DBE0(void*)override;
    std::uint32_t SavePreviewAt489420(void*,const char*)override;
private:
    RootQuickFileNativeDependencies dependencies_;
};
}
