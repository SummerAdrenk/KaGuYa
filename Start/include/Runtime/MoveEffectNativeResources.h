#pragma once

#include <Runtime/MoveEffectNative.h>

namespace KaGuYa::Start::Runtime
{
// 49CDE4..49D1FC：调用方已通过49CDDE快照门控。
// sourcePattern是49C32D保存的var_30，不从item.patternAt24重新取源。
// itemSlot仅借用：入口保存EDI/base及ESI/control，原索引固定为0；不增持或接管。
// 正常提交snapshot/offset/镜像修正；异常保留Sprite临时坐标与Pattern+54h，交外层展开。
void CaptureMoveSnapshotAt49CDE4(YtlReferenceSlotRuntime& itemSlot,
    PatternLayerNativeBody80h& sourcePattern, std::uint32_t mirror,
    PatternLayerNativeRenderServices& services);
}
