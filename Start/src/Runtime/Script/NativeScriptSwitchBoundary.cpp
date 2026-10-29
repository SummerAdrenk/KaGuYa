#include <Runtime/NativeScriptSwitchBoundary.h>

namespace KaGuYa::Start::Runtime
{
ScrScriptSwitchResult NativeScriptSwitchBoundary::Switch(ScrScriptRuntimeState& runtime,int id)
{
    ScrScriptSwitchResult result;
    result.success=LoadScriptNativeAt4D3C90(runtime.NativeBody(),id,services_)!=0;
    result.error=result.success?ScrScriptSwitchError::None:ScrScriptSwitchError::ResourceLoadFailed;
    // 原4D3C90失败已有自己的MessageBox；caller只走原throw/report后缀，不重复显示。
    result.failureAlreadyReported=true;return result;
}
}
