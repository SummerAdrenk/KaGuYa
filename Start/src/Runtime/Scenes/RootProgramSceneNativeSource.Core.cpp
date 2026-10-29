#include <Runtime/RootProgramSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/ProgramProfileDialogNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootProgramSceneNativeSource::CaptureEnvironmentAt4180C0(unsigned)
{return dependencies_.environment.CaptureAt4180C0();}
void RootProgramSceneNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootProgramSceneNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootProgramSceneNativeSource::RootDrawSlotAt18(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootProgramSceneNativeSource::RootScenesSlotAt30(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
YtlReferenceSlotRuntime& RootProgramSceneNativeSource::RootInformationSlotAt3C(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->InformationDisplaySlotAt3C();}
ScriptParamsNativeBody* RootProgramSceneNativeSource::ParamsAt47BDB0(void* root,unsigned)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
ScriptNativeBody* RootProgramSceneNativeSource::ScriptAt47CD00(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->ScriptAt47CD00();}
int RootProgramSceneNativeSource::ResolveNamedAt498020(void*,const char* name,unsigned)
{return ResolveScriptNamedValueNativeAt498020(name,dependencies_.names,dependencies_.strings);}
int RootProgramSceneNativeSource::ReadValueAt4C73A0(ScriptParamsNativeBody* body,int index,unsigned)
{return ReadScriptValueAt4C73A0(*body,index,dependencies_.errors);}
GuiDialogNativeBody5Ch* RootProgramSceneNativeSource::ConstructDialog00At4DE1E0(void* raw)
{return &ConstructProgramSelectDialogNativeAt4DE1E0(raw,dependencies_.dialog)->into.common;}
GuiDialogNativeBody5Ch* RootProgramSceneNativeSource::ConstructDialog01At4DF0E0(void* raw)
{return &ConstructProgramSelectDialogNativeAt4DF0E0(raw,dependencies_.dialog)->into.common;}
GuiDialogNativeBody5Ch* RootProgramSceneNativeSource::ConstructProfileDialog(void* raw, const ProgramModeProfile& mode)
{ return ConstructProgramProfileDialog(raw, mode, dependencies_.dialog); }
std::uint8_t RootProgramSceneNativeSource::InputPredicate(void*,unsigned function,unsigned argument,unsigned)
{
    // 五个原callee均不读传入ECX。caller已取过Root，不能在这里省略callee自身的取源。
    auto& env=dependencies_.environment;auto& input=dependencies_.input.RootSource();
    switch(function)
    {
    case 0x47F830:return ReadScriptSaveKeyAt47F830(env,input);
    case 0x47F650:return ReadScriptLoadKeyAt47F650(env,input);
    case 0x47F780:return ReadScriptQuickSaveKeyAt47F780(env,input,dependencies_.root,dependencies_.errors,dependencies_.names,dependencies_.strings);
    case 0x47F6D0:return ReadScriptQuickLoadKeyAt47F6D0(env,input,dependencies_.root,dependencies_.errors,dependencies_.names,dependencies_.strings);
    case 0x47F530:return ReadScriptLikabilityKeyAt47F530(env,input,dependencies_.root,argument,dependencies_.errors,dependencies_.names,dependencies_.strings);
    default:throw std::invalid_argument("Unknown native Program input function");
    }
}
RootProgramSceneServicesRuntime::RootProgramSceneServicesRuntime(RootProgramSceneNativeDependencies d)noexcept
    :source_(d),scene_{source_,d.memory,&d.dialog,&d.confirmation}{}
}
