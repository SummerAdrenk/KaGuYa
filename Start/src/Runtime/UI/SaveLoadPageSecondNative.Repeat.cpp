#include "SaveLoadPageSecondNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageSecondNativeDetail;
std::int32_t RepeatState(const Slot& r)
{
    if (!Read(r.baseAt00)) return 7;
    auto* child = Resolve<GuiControlNativeBody50h>(r);
    if (!child->vptrAt00->readStateAt48(child)) return 7;
    child = Resolve<GuiControlNativeBody50h>(r);
    return child->vptrAt00->resourceStateAt38(child);
}
void Repeat(Body& b, const Slot& child, std::int32_t before, std::int32_t after, std::int32_t event)
{
    if (before != after)
    {
        if (after == 5 && Read(b.repeatEventAtA4) == -1)
        {
            b.repeatCountAtA0 = 20;
            b.repeatEventAtA4 = event;
        }
        return; // 状态转出不在这一帧清repeat；原519E75..EA4。
    }
    if (Read(b.repeatEventAtA4) != event) return;
    if (after != 5) { b.repeatEventAtA4 = -1; return; }
    const auto count = Read(b.repeatCountAtA0);
    if (count) { b.repeatCountAtA0 = Add(count, -1); return; }
    Slot clicked = Copy(Resolve<GuiControlNativeBody50h>(child)->eventAt3C);
    LocalReference lifetime{clicked};
    Invoke(clicked);
    b.repeatCountAtA0 = 1; // 触发成功返回后才写；下一次是减1，再下一次触发。
    lifetime.Reload();
}
}
void ProcessInputSaveLoadPageSecondNativeAt519CE0(Common* p)
{
    FreshReference next;
    p->vptrAt00->findChildAt14(p, &next.slot, 51);
    LocalReference nextLifetime{next.slot};
    const auto nextBefore = RepeatState(next.slot);
    FreshReference previous;
    p->vptrAt00->findChildAt14(p, &previous.slot, 50);
    LocalReference previousLifetime{previous.slot};
    const auto previousBefore = RepeatState(previous.slot);
    ProcessGuiDialogInputAt518C30(*p); // 实际GuiDialog原核，绝不再次分派+54或委托source。
    const auto nextAfter = RepeatState(next.slot);
    const auto previousAfter = RepeatState(previous.slot);
    Repeat(Page(p), next.slot, nextBefore, nextAfter, 51);
    Repeat(Page(p), previous.slot, previousBefore, previousAfter, 50);
    previousLifetime.Reload();
    nextLifetime.Reload(false);
}
}
