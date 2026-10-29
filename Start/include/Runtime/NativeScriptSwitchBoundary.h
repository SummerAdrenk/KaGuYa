#pragma once
#include <Runtime/ScrVmFileCallHandlers.h>
#include <Runtime/ScriptLoadNative.h>

namespace KaGuYa::Start::Runtime
{
// CmdExec/Call/Return/Follow/End选定原CScript后，直接4D3C90，不走现代runtime.LoadScript。
class NativeScriptSwitchBoundary final:public IScrVmScriptSwitchBoundary
{
public:
    explicit NativeScriptSwitchBoundary(ScriptLoadNativeServices& services)noexcept:services_(services){}
    ScrScriptSwitchResult Switch(ScrScriptRuntimeState&,std::int32_t)override;
private:
    ScriptLoadNativeServices& services_;
};
}
