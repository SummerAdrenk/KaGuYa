#include <Runtime/RootRestoredScriptNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
RootRestoredScriptNativeServices::RootRestoredScriptNativeServices(
    RootRestoredScriptNativeDependencies dependencies) noexcept:dependencies_(dependencies){}
RestoredScriptNativeRootReceiver RootRestoredScriptNativeServices::CaptureAfterEnsureAt413BE0()
{
    dependencies_.root.EnsureCreatedAt413BE0();
    return {dependencies_.root.Published(),
        [](void* root){return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();},
        [](void* root){return static_cast<RootRuntimePayload*>(root)->PostWindow().DrawLayersAt18();}};
}
EnvironmentRootStorage* RootRestoredScriptNativeServices::CaptureEnvironmentAt4180C0()
{return dependencies_.environment.CaptureAt4180C0();}
std::uint8_t RootRestoredScriptNativeServices::LoadScriptAt4D3C90(ScriptNativeBody& receiver,std::int32_t id)
{return dependencies_.execution.LoadAt4D3C90(receiver,id);}
void RootRestoredScriptNativeServices::ResetDrawOwnersAt47E940(DrawLayersView receiver)
{receiver.ResetOwnedLayersAt47E940(dependencies_.graphics);}
void RootRestoredScriptNativeServices::SetGraphicsTextFirstNameAt4D6CD0(const char* name)
{dependencies_.graphics.SetTextFirstName(name);}
void RootRestoredScriptNativeServices::SetGraphicsTextSecondNameAt4D6DB0(const char* name)
{dependencies_.graphics.SetTextSecondName(name);}
std::int32_t RootRestoredScriptNativeServices::ResolveNamedValueAt498020(IEnvironmentArchiveRuntime*,const char* name)
{return ResolveScriptNamedValueNativeAt498020(name,dependencies_.names,dependencies_.strings);}
void RootRestoredScriptNativeServices::WriteValueAt4C8220(ScriptParamsNativeBody& receiver,std::int32_t index,std::int32_t value)
{WriteScriptValueAt4C8220(receiver,index,value,dependencies_.errors);}
std::int32_t RootRestoredScriptNativeServices::DispatchAt4D3FF0(ScriptNativeBody& receiver)
{return dependencies_.execution.DispatchAt4D3FF0(receiver);}
}
