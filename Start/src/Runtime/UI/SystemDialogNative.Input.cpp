#include "SystemDialogNative.Internal.h"
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime
{
using namespace SystemDialogNativeDetail;
std::int32_t TriggerSystemDialogAt5104A0(Common* p)
{
    auto& b = Dialog(p);
    // 门控前不得取system、HWND或Root；任一正常失败尾才清84，异常不清。
    if (p->vptrAt00->readStateAt20(p) && !Read(b.suppressOnceAt84))
    {
        auto& s = RequireSystemDialogNativeServices().source;
        auto* system = s.SystemAt4180C0();
        if (!s.IsIconic(s.SystemWindowAt2C(system)))
        {
            FreshReference menu;
            p->vptrAt00->findChildAt14(p, &menu.slot, 0);
            LocalReference lifetime{menu.slot};
            if (Read(menu.slot.baseAt00))
            {
                POINT point;
                const auto* returned = p->vptrAt00->pointerAt74(p, &point);
                const auto x = returned->x, y = returned->y;
                auto* fixedRoot = Root(s);
                RECT logical;
                s.LogicalRectangleAt478E50(fixedRoot, &logical);
                if (x >= logical.left && x < logical.right)
                {
                    auto* child = Resolve<GuiControlNativeBody50h>(menu.slot);
                    RECT scratch, bounds;
                    ::CopyRect(&bounds, child->vptrAt00->rectAt3C(child, &scratch));
                    // 01:4DF960允许整条顶部；02-18还要求frame.left。bottom包含等号。
                    if (y >= logical.top && y <= bounds.bottom &&
                        (!CompiledGameProfile().topMenuRequiresFrameLeft || x >= bounds.left))
                    { lifetime.Reload(); return 1; }
                }
            }
            lifetime.Reload();
        }
    }
    b.suppressOnceAt84 = 0;
    return 0;
}
namespace SystemDialogNativeDetail
{
std::int32_t __fastcall ActiveInput(Common* p, void*)
{
    if (Read(p->activeAt4C->typeAt08) != 0) return 0;
    auto& b = Dialog(p);
    const auto latch = Read(b.keyboardLatchAt70);
    if (!latch)
    {
        if (!p->vptrAt00->pressedAt70(p, 5)) return 0;
        b.keyboardLatchAt70 = 1;
        auto* child = p->activeAt4C;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot event = Copy(p->activeAt4C->eventAt3C);
        LocalReference lifetime{event};
        if (Read(event.baseAt00))
        {
            auto* selected = Resolve<GuiClickedEventNativeBody>(event);
            (void)selected->vptrAt00->invokeAt00(selected);
        }
        b.keyboardLatchAt70 = 0; // 不调用派生reset3C、不清active/30。
        lifetime.Reload();
    }
    return latch;
}
void __fastcall ProcessInput(Common* p, void*)
{
    if (!TriggerSystemDialogAt5104A0(p)) { Dialog(p).resultAt68 = 0; return; }
    ProcessGuiDialogInputAt518C30(*p); // 原直接调用基类，不重新派生54分派。
    auto& events = Dialog(p).eventsAt74;
    auto* const end = Read(events.end); // 51B13B只捕获一次end。
    for (auto* r = Read(events.begin); r != end; ++r)
    {
        const auto base = Read(r->baseAt00);
        auto* selected = ResolveWithBase<GuiClickedEventNativeBody>(*r, base);
        if (!Read(selected->clickedFlagAt04)) continue;
        // 命中后重读control/bias，但保留第一轮ESI base；不清点击flag。
        auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r->controlAt08));
        const auto stride = c ? Read(c->strideAt0C) : 0u;
        auto* second = reinterpret_cast<GuiClickedEventNativeBody*>(base + Read(r->biasAt04) * stride);
        Dialog(p).resultAt68 = Read(second->eventValueAt08);
        return;
    }
}
}
}
