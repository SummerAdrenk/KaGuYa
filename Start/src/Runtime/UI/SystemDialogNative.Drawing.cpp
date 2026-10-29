#include "SystemDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::SystemDialogNativeDetail
{
void __fastcall FlushDirty(Common* p, void*)
{
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = p->vptrAt00->dirtyAt00(p);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = p->vptrAt00->dirtyAt00(p);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (int i = 0; i < count; ++i)
    {
        const auto handle = Read(Resolve<GraphicsSurfaceNativeBody>(Dialog(p).backgroundAt5C)->handleAt04);
        if (handle)
            RequireSystemDialogNativeServices().source.DrawBackgroundAt52687F(handle, rectangles + i);
        p->vptrAt00->drawAt60(p, rectangles + i);
        // 同一个dirty数组元素地址；不复制clip，不因DrawSurface false跳过Draw/提交。
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangles + i);
    }
    dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->clearAt00(dirty);
}
}
