#pragma once
#include <Runtime/GraffitiCountDialogNative.h>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct GraffitiCountMultiRow final
{
    std::int32_t selector, current, target, upperLimit;
    std::int32_t pixelsPerValue, remainder, pixelProgress;
};
// 现代状态，不复制原DC字节对象及其Ref/EH结构。
struct GraffitiCountMultiNativeBody final
{
    GraffitiCountDialogNativeBody90h common;
    const CountMultiResourceData* data = nullptr;
    std::vector<GraphicsSurfaceReference> gauges, icons, digits;
    std::vector<GraffitiCountMultiRow> rows;
    POINT layoutOrigin{};
    SIZE canvasSize{};
    std::int32_t lifecycle = 0, animation = 0, skipResume = 2, delay = 0;
    bool skip = false;
};
GuiDialogNativeBody5Ch* NewGraffitiCountMultiNative(GraffitiCountDialogNativeServices&);
const GuiDialogNativeVtable35& GraffitiCountMultiNativeVtable() noexcept;
void RefreshGraffitiCountMultiNative(GraffitiCountMultiNativeBody&);
}
