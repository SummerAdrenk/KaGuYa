#include "GuiPanelNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace GuiPanelNativeDetail;
template<bool Enabled> void Propagate(Body& body, std::int32_t value)
{
    if constexpr (Enabled) body.common.enabledAt18 = value;
    else body.common.stateAt14 = value;
    auto* cursor = body.idsAt50.begin;
    while (cursor != body.idsAt50.end)
    {
        auto* parent = Parent(body);
        UnwrittenReference child;
        parent->vptrAt00->findChildAt14(parent, &child.slot, *cursor);
        LocalReference lifetime{child.slot}; // 仅callee正常返回后取得资格。
        if (child.slot.baseAt00)
        {
            auto* p = ResolveGuiControl(child.slot);
            if constexpr (Enabled) p->vptrAt00->writeEnabledAt4C(p, value);
            else p->vptrAt00->writeStateAt44(p, value);
        }
        lifetime.Current(true); // state1终止；不回滚本项或更早子项。
        ++cursor;
    }
}
}
void WriteGuiPanelStateAt536CE0(GuiPanelNativeBody6Ch& body, std::int32_t value)
{ Propagate<false>(body, value); }
void WriteGuiPanelEnabledAt538670(GuiPanelNativeBody6Ch& body, std::int32_t value)
{ Propagate<true>(body, value); }
void SelectGuiPanelAt535B10(GuiPanelNativeBody6Ch& body, std::int32_t id)
{
    using namespace GuiPanelNativeDetail;
    body.selectedAt68 = -1;
    struct Message final { std::int32_t kind; const std::int32_t* selected; } message{0, &id};
    static_assert(sizeof(Message) == 8);
    auto* cursor = body.idsAt50.begin;
    while (cursor != body.idsAt50.end)
    {
        if (*cursor == id) body.selectedAt68 = *cursor;
        // 535B6A发布selected之后重读id，每轮独立重取parent。
        const auto event = *cursor;
        auto* parent = Parent(body);
        UnwrittenReference child;
        parent->vptrAt00->findChildAt14(parent, &child.slot, event);
        LocalReference lifetime{child.slot};
        if (child.slot.baseAt00)
        {
            auto* p = ResolveGuiControl(child.slot);
            p->vptrAt00->nullAt54(p, reinterpret_cast<std::int32_t>(&message));
        }
        lifetime.Current(true);
        ++cursor;
    }
}
std::int32_t KeyboardGuiPanelAt51FA60(GuiPanelNativeBody6Ch& body)
{
    using namespace GuiPanelNativeDetail;
    if (!body.common.vptrAt00->enabledAt1C(&body.common) || !body.common.focusAt20) return 0;
    auto* navigation = body.navigationAt5C.begin;
    bool pressed = false;
    for (; navigation != body.navigationAt5C.end; ++navigation)
    {
        auto* parent = Parent(body); // 先固定EBX，再解析navigation读取input。
        auto* n = ResolveEnvironmentObjectReference<GuiNavigationNativeBody>(*navigation);
        const auto input = n->inputAt04;
        if (parent->vptrAt00->pressedAt70(parent, input)) { pressed = true; break; }
    }
    // 非零回调直接跳51FAEE，不能再检查回调可能修改的end。
    if (!pressed) return 0;
    auto candidate = body.selectedAt68; // 第一次pressed回调成功后才读。
    if (candidate < 0) return 0;
    for (;;)
    {
        // pressed及每个被拒子项退役后，重新解析同一Ref槽。
        auto* n = ResolveEnvironmentObjectReference<GuiNavigationNativeBody>(*navigation);
        candidate = n->vptrAt00->nextAt00(n, candidate);
        if (candidate < 0 || candidate == body.selectedAt68) return 0;
        auto* parent = Parent(body);
        UnwrittenReference child;
        parent->vptrAt00->findChildAt14(parent, &child.slot, candidate);
        LocalReference lifetime{child.slot};
        if (!child.slot.baseAt00) throw 1;
        auto* p = ResolveGuiControl(child.slot);
        const auto state = p->vptrAt00->resourceStateAt38(p);
        if (state == 1 || state == 2)
        {
            lifetime.Current(true);
            continue; // 原版仅negative/当前selected停止，不添加循环预算或兜底。
        }
        reinterpret_cast<const GuiPanelNativeVtable26*>(body.common.vptrAt00)->selectAt64(&body.common, candidate);
        parent = Parent(body); // select可替换parent及focus槽。
        parent->vptrAt00->setActiveAt30(parent, &body.common);
        lifetime.Current(true);
        return 1;
    }
}
namespace GuiPanelNativeDetail::Entries
{
std::int32_t __fastcall Keyboard(Common* p, void*) { return KeyboardGuiPanelAt51FA60(Panel(p)); }
void __fastcall WriteState(Common* p, void*, std::int32_t v) { WriteGuiPanelStateAt536CE0(Panel(p), v); }
void __fastcall WriteEnabled(Common* p, void*, std::int32_t v) { WriteGuiPanelEnabledAt538670(Panel(p), v); }
void __fastcall Select(Common* p, void*, std::int32_t id) { SelectGuiPanelAt535B10(Panel(p), id); }
}
}
