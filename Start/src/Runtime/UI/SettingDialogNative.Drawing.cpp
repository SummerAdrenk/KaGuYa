#include "SettingDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::SettingDialogNativeDetail
{
void __fastcall FlushDirty(Common* p, void*)
{
    auto* dirty = Read(p->vptrAt00)->dirtyAt00(p);
    Read(dirty->vptrAt00)->coalesceAt04(dirty);
    dirty = Read(p->vptrAt00)->dirtyAt00(p);
    const auto count = std::bit_cast<std::int32_t>(Read(dirty->vptrAt00)->countAt18(dirty));
    if (!count) return;
    dirty = Read(p->vptrAt00)->dirtyAt00(p);
    auto* rectangles = Read(dirty->vptrAt00)->rectsAt1C(dirty);
    for (int index = 0; index < count; ++index)
    {
        const auto handle = Read(Resolve<GraphicsSurfaceNativeBody>(Dialog(p).backgroundAt68)->handleAt04);
        if (handle) RequireSettingDialogNativeServices().source.DrawBackgroundAt5272BF(handle, rectangles + index);
        Read(p->vptrAt00)->drawAt60(p, rectangles + index);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangles + index);
    }
    dirty = Read(p->vptrAt00)->dirtyAt00(p);
    Read(dirty->vptrAt00)->clearAt00(dirty);
}
}
