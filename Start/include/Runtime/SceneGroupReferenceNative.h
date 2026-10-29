#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
using SceneGroupReferencesNative=EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
// CG/Replay均调用同一原函数；只管理Ref12向量，不依赖具体按钮类型或宿主服务。
void AppendSceneGroupReferenceNativeAt499030(SceneGroupReferencesNative&,const YtlReferenceSlotRuntime&);
void DestroySceneGroupReferencesNativeAt4A3950(SceneGroupReferencesNative&)noexcept;
}
