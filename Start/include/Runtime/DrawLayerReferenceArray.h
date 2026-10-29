#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct LayerItemNativeBody14h;

// CDrawManager+14：只有三指针，元素自身是原 Ref12；不内嵌现代图层 owner。
using DrawLayerReferenceArray = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(DrawLayerReferenceArray) == 0x0C);
static_assert(offsetof(DrawLayerReferenceArray, begin) == 0);
static_assert(offsetof(DrawLayerReferenceArray, end) == 4);
static_assert(offsetof(DrawLayerReferenceArray, capacity) == 8);

[[nodiscard]] std::int32_t DrawLayerReferenceCount(const DrawLayerReferenceArray& array) noexcept;
[[nodiscard]] std::int32_t DrawLayerReferenceCapacity(const DrawLayerReferenceArray& array) noexcept;
[[nodiscard]] LayerItemNativeBody14h* ResolveDrawLayerReference(
    const YtlReferenceSlotRuntime& reference) noexcept;

YtlReferenceSlotRuntime* GrowDrawLayerReferencesAt454580(DrawLayerReferenceArray& array,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AppendDrawLayerReference(DrawLayerReferenceArray& array, const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 46A880：正文已经完成；control 分配失败不补删正文，局部 Ref 成立后才有展开资格。
void AppendOwnedDrawLayerAt46A880(DrawLayerReferenceArray& array, LayerItemNativeBody14h* layer,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

void ReleaseDrawLayerRangeAt452440(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
void DestroyDrawLayerReferencesAt461750(DrawLayerReferenceArray& array) noexcept;
}
