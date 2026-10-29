#pragma once
#include <Runtime/ReplayScriptNativeServices.h>
#include <Runtime/RootScriptStartupNativeServices.h>

namespace KaGuYa::Start::Runtime
{
// 不长期缓存某一代Root；每次调用只借caller选中receiver的payload/publication。
class RootReplayScriptNativeServices final:public IReplayScriptNativeExecution
{
public:
    RootReplayScriptNativeServices(RootRestoredScriptNativeDependencies dependencies,const IScrResourceLookupSource& names)noexcept
        :dependencies_(dependencies),names_(names){}
    std::uint8_t LoadStartupAt4D53D0(ReplayScriptNativeReceiver,const char*,std::int32_t)override;
    std::int32_t DispatchAt4D3FF0(ReplayScriptNativeReceiver)override;
private:
    RootRestoredScriptNativeDependencies dependencies_;
    const IScrResourceLookupSource& names_;
};
}
