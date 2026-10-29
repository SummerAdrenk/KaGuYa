#pragma once
#include <Runtime/ScriptStartupNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
// 只借caller已取得的同一root；publication/payload是宿主元数据，不进入原CCh正文。
struct ReplayScriptNativeReceiver final
{
    ScriptNativeBody* body;
    ScriptNativePublication* publication;
    RootRuntimePayload* payload=nullptr;
};
class IReplayScriptNativeExecution
{
public:
    virtual ~IReplayScriptNativeExecution()=default;
    virtual std::uint8_t LoadStartupAt4D53D0(ReplayScriptNativeReceiver,const char*,std::int32_t)=0;
    virtual std::int32_t DispatchAt4D3FF0(ReplayScriptNativeReceiver)=0;
};
}
