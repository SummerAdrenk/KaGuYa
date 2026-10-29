#include <Runtime/RootQuickFileNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootQuickFileNativeSource::CaptureEnvironmentAt4180C0()
{return dependencies_.environment.CaptureAt4180C0();}
IRuntimeStringManagerSource& RootQuickFileNativeSource::Strings(){return dependencies_.persistence.Strings();}
IEnvironmentArchiveRuntime* RootQuickFileNativeSource::RegistryAt417990(EnvironmentRootStorage* environment,std::uint32_t index)
{return ResolveEnvironmentArchiveAt417990(*environment,index);}
BOOL RootQuickFileNativeSource::DeleteFileA(const char* path){return dependencies_.deleteFile(path);}
void RootQuickFileNativeSource::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootQuickFileNativeSource::RootAt6198C4(){return dependencies_.root.ReadPublishedAt6198C4();}
void* RootQuickFileNativeSource::ParamsAt47BDB0(void* root)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
std::uint32_t RootQuickFileNativeSource::WriteContainerAt4C7B10(void* params,const char* name)
{return dependencies_.persistence.WriteContainerAt4C7B10(params,name);}
std::uint32_t RootQuickFileNativeSource::ReadContainerAt4C7420(void* params,const char* name,std::uint32_t mode)
{return dependencies_.persistence.ReadContainerAt4C7420(params,name,mode);}
std::uint32_t RootQuickFileNativeSource::ReadContainerAt4C7420(ScriptParamsNativeBody& params,const char* name,std::uint32_t mode)
{return dependencies_.persistence.ReadContainerAt4C7420(params,name,mode);}
void RootQuickFileNativeSource::CapturePreviewAt48DBE0(void* root){dependencies_.persistence.CapturePreviewAt48DBE0(root);}
std::uint32_t RootQuickFileNativeSource::SavePreviewAt489420(void* root,const char* name)
{return dependencies_.persistence.SavePreviewAt489420(root,name);}
}
