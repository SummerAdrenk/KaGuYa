#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ScrContainer;

using ScriptResultLayerReferenceList = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(ScriptResultLayerReferenceList) == 0x0C);

// 4CB066..4CB205：首清三指针，不释放旧内容；失败清本次列表，成功由调用者接管。
// 只收集真实Ref12，不生成DTO，不提前验证层数或重复的byte04。
void CollectScriptResultLayersNativeAt4CB08D(const ScrContainer& container,
    std::int32_t layerGroupIndex, ScriptResultLayerReferenceList& out,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void DestroyScriptResultLayersNativeAt4C9C70(ScriptResultLayerReferenceList& list) noexcept;
}
