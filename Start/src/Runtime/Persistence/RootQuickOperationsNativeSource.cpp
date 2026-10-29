#include <Runtime/RootQuickOperationsNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootQuickOperationsNativeSource::CaptureEnvironmentAt4180C0()
{return dependencies_.environment.CaptureAt4180C0();}
IRuntimeStringManagerSource& RootQuickOperationsNativeSource::Strings(){return dependencies_.files.Strings();}
BOOL RootQuickOperationsNativeSource::DeleteFileA(const char* path){return dependencies_.files.DeleteFileA(path);}
void RootQuickOperationsNativeSource::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootQuickOperationsNativeSource::RootAt6198C4(){return dependencies_.root.ReadPublishedAt6198C4();}
void* RootQuickOperationsNativeSource::ParamsAt47BDB0(void* root)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
std::uint32_t RootQuickOperationsNativeSource::WriteContainerAt4C7B10(void* params,const char* name)
{return dependencies_.files.WriteContainerAt4C7B10(params,name);}
std::uint32_t RootQuickOperationsNativeSource::ReadContainerAt4C7420(void* params,const char* name,std::uint32_t mode)
{return dependencies_.files.ReadContainerAt4C7420(params,name,mode);}
void RootQuickOperationsNativeSource::CapturePreviewAt48DBE0(void* root){dependencies_.files.CapturePreviewAt48DBE0(root);}
std::uint32_t RootQuickOperationsNativeSource::SavePreviewAt489420(void* root,const char* name)
{return dependencies_.files.SavePreviewAt489420(root,name);}
IEnvironmentArchiveRuntime* RootQuickOperationsNativeSource::RegistryAt417990(EnvironmentRootStorage* env,unsigned index)
{return ResolveEnvironmentArchiveAt417990(*env,index);}
YtlReferenceSlotRuntime& RootQuickOperationsNativeSource::TransitionFrameAt1F0(void* root)
{return static_cast<RootRuntimePayload*>(root)->TransitionFrameSlotAt1F0();}
IModalDialogRunnerServices& RootQuickOperationsNativeSource::ModalForProgramAt3C(void* root)
{
    // 原485DEE/4860CE仍先解析选定Root+3C；5071D0不消费该ECX，不能新增对象要求。
    const auto& slot=static_cast<RootRuntimePayload*>(root)->InformationDisplaySlotAt3C();
    const auto base=*static_cast<const volatile std::uintptr_t*>(&slot.baseAt00);
    if(base)
    {
        const auto* control=*static_cast<YtlReferenceControlRuntime* const volatile*>(&slot.controlAt08);
        const auto stride=control?*reinterpret_cast<const volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+0x0Cu):0u;
        const auto bias=*static_cast<const volatile unsigned*>(&slot.biasAt04);
        static_cast<void>(base+bias*stride);
    }
    return dependencies_.confirmation.modal;
}
int RootQuickOperationsNativeSource::MessageBoxA(HWND window,const char* text,const char* caption,UINT type)
{return dependencies_.messageBox(window,text,caption,type);}
}
