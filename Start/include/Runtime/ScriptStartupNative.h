#pragma once
#include <Runtime/RestoredScriptNative.h>
#include <Runtime/ScriptNativeBody.h>

namespace KaGuYa::Start::Runtime
{
// 共用已核的Root/Env/Draw/Graphics/命名值/Loader边界，不另造事务状态或Script。
class IScriptStartupNativeServices:public IRestoredScriptNativeServices
{
public:
    // receiver是caller刚捕获的同一根；这里只481720，不再次413BE0。
    virtual void ReplaceCapturedParamsAt481720(RestoredScriptNativeRootReceiver)=0;
    virtual std::int32_t FindScriptIdAt47A020(const char* name)=0;
};
std::uint8_t LoadStartupScriptNativeAt4D53D0(ScriptNativeBody&,const char* name,
    std::int32_t entry,IScriptStartupNativeServices&);
}
