#include <Runtime/RootScriptStartupNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Logging/Logger.h>
#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
RestoredScriptNativeRootReceiver RootScriptStartupNativeServices::CaptureAfterEnsureAt413BE0()
{return common_.CaptureAfterEnsureAt413BE0();}
void RootScriptStartupNativeServices::ReplaceCapturedParamsAt481720(RestoredScriptNativeRootReceiver receiver)
{static_cast<RootRuntimePayload*>(receiver.object)->ReplaceParamsAt481720();}
EnvironmentRootStorage* RootScriptStartupNativeServices::CaptureEnvironmentAt4180C0()
{return common_.CaptureEnvironmentAt4180C0();}
std::int32_t RootScriptStartupNativeServices::FindScriptIdAt47A020(const char* name)
{return resources_.FindResourceIdAt47A020(name);}
std::uint8_t RootScriptStartupNativeServices::LoadScriptAt4D3C90(ScriptNativeBody& body,std::int32_t id)
{
    const auto result=common_.LoadScriptAt4D3C90(body,id);
    const auto error=GetLastError();
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,"Script startup load id=%d result=%u",id,unsigned(result));
    SetLastError(error);return result;
}
void RootScriptStartupNativeServices::ResetDrawOwnersAt47E940(DrawLayersView draw)
{common_.ResetDrawOwnersAt47E940(draw);}
void RootScriptStartupNativeServices::SetGraphicsTextFirstNameAt4D6CD0(const char* name)
{common_.SetGraphicsTextFirstNameAt4D6CD0(name);}
void RootScriptStartupNativeServices::SetGraphicsTextSecondNameAt4D6DB0(const char* name)
{common_.SetGraphicsTextSecondNameAt4D6DB0(name);}
std::int32_t RootScriptStartupNativeServices::ResolveNamedValueAt498020(IEnvironmentArchiveRuntime* archive,const char* name)
{return common_.ResolveNamedValueAt498020(archive,name);}
void RootScriptStartupNativeServices::WriteValueAt4C8220(ScriptParamsNativeBody& params,std::int32_t index,std::int32_t value)
{common_.WriteValueAt4C8220(params,index,value);}
std::int32_t RootScriptStartupNativeServices::DispatchAt4D3FF0(ScriptNativeBody& body)
{return common_.DispatchAt4D3FF0(body);}
void RootScriptStartupNativeServices::OnPublishCommandFields(ScriptNativeBody&,unsigned mask) noexcept
{publication_.commandMask|=mask;}
}
