#include "CgOperationDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace CgOperationNativeDetail
{
void __fastcall DrawButtons(Common* common, void*, const RECT* clip)
{
    // 522ED0先直接调用5230B0；此处不能再经派生+60递归分派。
    GuiDialogBaseVtable().drawAt60(common, clip);
    if (!common->enabledAt38) return;
    auto& body = *reinterpret_cast<Buttons*>(common);
    const auto& selected = body.surfacesAt68[common->stateAt34 ? 0 : 1];
    if (!selected.baseAt00) return;
    auto* surface = Resolve<Surface>(selected);
    const auto x = surface->originXAt10;
    const auto y = surface->originYAt14;
    surface = Resolve<Surface>(selected);
    const auto handle = surface->handleAt04;
    if (handle) static_cast<void>(Services().composition.DrawSurface(handle, x, y, nullptr, nullptr, clip, 0));
}
void __fastcall CompositeButtons(Common* common, void*, Slot destination, const RECT* clip)
{
    RefGuard destinationGuard{destination};
    GuiDialogBaseVtable().compositeAt5C(common, Copy(destination), clip);
    if (common->enabledAt38)
    {
        auto& body = *reinterpret_cast<Buttons*>(common);
        const auto& selected = body.surfacesAt68[common->stateAt34 ? 0 : 1];
        if (selected.baseAt00)
        {
            auto* surface = Resolve<Surface>(selected);
            const auto x = surface->originXAt10;
            const auto y = surface->originYAt14;
            auto* target = Resolve<Surface>(destination);
            surface = Resolve<Surface>(selected);
            BlitAt4425C0(target, surface, x, y, clip);
        }
    }
    destinationGuard.Reload();
}
}

void DrawCgOperationDialogNativeAt523370(CgOperationDialogNativeBodyA8h& body, const RECT* clip)
{
    // 已有Into核就是523370；绘制异常不会恢复临时置零的nested state。
    GuiDialogIntoBaseVtable().common.drawAt60(&body.into.common, clip);
}
void CompositeCgOperationDialogNativeAt523480(CgOperationDialogNativeBodyA8h& body,
    YtlReferenceSlotRuntime destination, const RECT* clip)
{
    GuiDialogIntoBaseVtable().common.compositeAt5C(&body.into.common, destination, clip);
}
}
