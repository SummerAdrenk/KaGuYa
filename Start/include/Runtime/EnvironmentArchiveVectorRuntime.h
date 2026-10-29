#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
// Environment+30 的三指针原始存储；只复制引用，不创建或解释 IArchive 正文。
void AppendEnvironmentArchiveReferenceAt49852D(
    EnvironmentVectorStorage<YtlReferenceSlotRuntime>& storage,
    const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

void ClearEnvironmentArchiveReferencesAt498312(
    EnvironmentVectorStorage<YtlReferenceSlotRuntime>& storage) noexcept;

void DestroyEnvironmentArchiveReferencesAt497080(
    EnvironmentVectorStorage<YtlReferenceSlotRuntime>& storage,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
}
