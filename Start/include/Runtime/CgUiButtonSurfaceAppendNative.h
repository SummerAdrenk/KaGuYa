#pragma once

#include <Runtime/GuiControlResourceNative.h>

namespace KaGuYa::Start::Runtime
{
// 4C2180 的借用 Ref 指针参数，不是 4FF1F0 的已增持按值参数。
// 只补 499920 的局部 Ref / EH / 正常尾；三指针、分配、增长、退休均用原有 kernels。
// 调用方继续提供正文退休所需的 GraphicsSurfaceNativeScope；本入口不另取 root/memory。
void AppendCgUiButtonSurfaceReferenceAt499920(GuiControlSurfaceReferences& references,
    const YtlReferenceSlotRuntime& source);
}
