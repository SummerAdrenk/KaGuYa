#include <Runtime/RootReplayScriptNativeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
std::uint8_t RootReplayScriptNativeServices::LoadStartupAt4D53D0(ReplayScriptNativeReceiver receiver,const char* name,int entry)
{
    if(!receiver.payload)throw std::logic_error("Root Replay startup requires selected payload");
    if(!receiver.body||!receiver.publication)throw std::logic_error("Root Replay startup requires selected body/publication");
    if(receiver.publication!=&receiver.payload->ScriptPublication())
        throw std::invalid_argument("Root Replay startup publication differs from selected payload");
    auto selection=dependencies_.execution.BindSelectedReceiver(*receiver.payload,*receiver.body);
    RootScriptStartupNativeServices services(dependencies_,names_,*receiver.publication);
    return LoadStartupScriptNativeAt4D53D0(*receiver.body,name,entry,services);
}
std::int32_t RootReplayScriptNativeServices::DispatchAt4D3FF0(ReplayScriptNativeReceiver receiver)
{
    if(!receiver.payload)throw std::logic_error("Root Replay dispatch requires selected payload");
    if(!receiver.body||!receiver.publication)throw std::logic_error("Root Replay dispatch requires selected body/publication");
    if(receiver.publication!=&receiver.payload->ScriptPublication())
        throw std::invalid_argument("Root Replay dispatch publication differs from selected payload");
    auto selection=dependencies_.execution.BindSelectedReceiver(*receiver.payload,*receiver.body);
    return dependencies_.execution.DispatchAt4D3FF0(*receiver.body);
}
}
