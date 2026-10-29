#include <Runtime/RootScriptNativeExecutionRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ScriptNativeExecutionSelectionScope RootScriptNativeExecutionRuntime::BindSelectedReceiver(
    RootRuntimePayload& selected,ScriptNativeBody& body)
{
    // 选定的原Ref必须匹配；不Ensure，不重新47CD00，不跟随6198C4的新发布根。
    if(ResolveEnvironmentObjectReference<ScriptNativeBody>(selected.ScriptSlotAt1CC())!=&body)
        throw std::invalid_argument("selected Root does not publish this CScript receiver");
    auto* services=selected.BorrowScriptReceiverServices();
    if(!services)
    {
        const auto& d=dependencies_;
        services=&selected.AdoptScriptReceiverServices(std::make_unique<RootScriptReceiverNativeServicesRuntime>(
            RootScriptReceiverNativeDependencies{d.root,d.environment,d.params,d.input,d.graphics,d.draws,d.effects,
                body,selected.ScriptPublication(),selected.ParamsSlotAt1C0(),d.archives,d.files,d.key,d.codecs,d.readers,
                d.random,d.errors,d.transaction,d.names,d.memory,d.strings,d.messageBox}));
    }
    auto& state=services->State();
    if(&state.NativeBody()!=&body||&state.Publication()!=&selected.ScriptPublication()||
        state.paramsGeneration.get()!=&selected.ParamsSlotAt1C0())
        throw std::invalid_argument("installed CScript services differ from the selected receiver tuple");
    return ScriptNativeExecutionSelectionScope(*this,services->Execution(),body);
}
IRestoredScriptNativeExecution& RootScriptNativeExecutionRuntime::RequireSelected(ScriptNativeBody& body)const
{
    const auto* binding=ScriptNativeExecutionSelectionScope::CurrentFor(*this);
    if(!binding)throw std::logic_error("CScript execution requires an explicit caller selection scope");
    if(binding->body!=&body)throw std::invalid_argument("CScript execution scope has a different receiver");
    return *binding->execution;
}
std::uint8_t RootScriptNativeExecutionRuntime::LoadAt4D3C90(ScriptNativeBody& body,std::int32_t id)
{return RequireSelected(body).LoadAt4D3C90(body,id);}
std::int32_t RootScriptNativeExecutionRuntime::DispatchAt4D3FF0(ScriptNativeBody& body)
{return RequireSelected(body).DispatchAt4D3FF0(body);}
}
