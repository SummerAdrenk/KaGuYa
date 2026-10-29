#include "SaveLoadPopupNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadPopupNativeDetail;
void SetSaveLoadPopupSliderNativeAt537C80(Common* p, std::int32_t value)
{
    Fresh child;
    p->vptrAt00->findChildAt14(p, &child.r, 57);
    Local lifetime{child.r};
    if (Read(child.r.baseAt00))
    {
        auto* selected = Resolve<GuiControlNativeBody50h>(child.r);
        auto* slider = reinterpret_cast<GuiSliderNativeBodyD8h*>(selected);
        if (slider->maximumAtBC == 0) SetSliderRange(selected, 0);
        else reinterpret_cast<const GuiSliderNativeVtable31*>(selected->vptrAt00)->setValueAt64(selected, value);
    }
    lifetime.Current();
}
namespace SaveLoadPopupNativeDetail
{
void SetSliderRange(GuiControlNativeBody50h* control, int maximum)
{
    auto* slider = reinterpret_cast<GuiSliderNativeBodyD8h*>(control);
    if (maximum > 0)
        reinterpret_cast<const GuiSliderNativeVtable31*>(control->vptrAt00)->setRangeAt5C(control, 0, maximum);
    else
    {
        // 旧小目录不启用滚动条；共享slider的PositionFromValue不能用零范围相除。
        slider->minimumAtB8 = slider->maximumAtBC = slider->valueAtB4 = 0;
        slider->thumbPositionAt98 = {control->xAt34 + slider->travelOffsetAt74.x,
            control->yAt38 + slider->travelOffsetAt74.y};
        slider->draggingAt8C = 0;
        slider->hoveredAt90 = 0;
    }
    control->vptrAt00->writeEnabledAt4C(control, maximum > 0);
}
namespace
{
void RefreshCatalogBounds(Common* p)
{
    auto& b = Popup(p);
    const int pages = PageCount();
    int visible = 0;
    for (auto* at = b.numbersAtA0.begin; at != b.numbersAtA0.end; ++at)
        if (Resolve<Number>(*at)->pageAt04 >= 0) ++visible;
    Fresh child;
    p->vptrAt00->findChildAt14(p, &child.r, 57);
    Local lifetime{child.r};
    const auto maximum = Resolve<GuiSliderNativeBodyD8h>(child.r)->maximumAtBC;
    if (visible != VisibleRows(pages) || maximum != MaximumFirstPage(pages) ||
        b.firstPageAt80 < 0 || b.firstPageAt80 > MaximumFirstPage(pages) ||
        (pages ? b.selectedPageAt7C < 0 || b.selectedPageAt7C >= pages : b.selectedPageAt7C != -1))
    {
        // 目录增删后先撤销旧窗口中的输入，再发布新范围和有效行。
        ClearGuiClickedEventFlagsAt532200(*Manager(b));
        p->vptrAt00->resetChildrenAt68(p);
        p->vptrAt00->resetInputAt3C(p);
        b.mouseRepeatEventAtB0 = -1;
        UpdateSaveLoadPopupNativeAt52B390(p, b.firstPageAt80, 1);
    }
}
}
void __fastcall Begin(Common* p, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Manager(Popup(p)));
    p->vptrAt00->resetChildrenAt68(p);
    // 01:48F2C0由父tab提供窗口起点；现代52B390仍从当前选择页起算。
    const auto first = CurrentSaveLoadResourceProfile().popup.tiledFrame
        ? Popup(p).firstPageAt80 : Popup(p).selectedPageAt7C;
    UpdateSaveLoadPopupNativeAt52B390(p, first, 0);
    p->vptrAt00->resetInputAt3C(p);
    Popup(p).mouseRepeatEventAtB0 = -1;
    Popup(p).resultAt5C = -1;
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    Popup(p).resultAt5C = -1;
    RefreshCatalogBounds(p);
    auto& s = RequireSaveLoadPopupNativeServices().source;
    auto* input = s.InputAt418210();
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    auto* at = Read(keyboard->actionsAt14[0].begin);
    auto* const end = Read(keyboard->actionsAt14[0].end);
    const auto buffer = at != end ? Read(keyboard->bufferAt10) : 0u;
    for (; at != end; ++at)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(at);
        if (*reinterpret_cast<const volatile std::uint8_t*>(address + buffer + 1u) &&
            !*reinterpret_cast<const volatile std::uint8_t*>(address - buffer + 2u))
        { Popup(p).resultAt5C = 53; Popup(p).pendingPageAt84 = Popup(p).selectedPageAt7C; return 1; }
    }
    input = s.InputAt418210();
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if (!Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28))
    { Popup(p).resultAt5C = 53; Popup(p).pendingPageAt84 = Popup(p).selectedPageAt7C; return 1; }
    auto candidate = Read(Popup(p).firstPageAt80); // process回调之前的ESI快照。
    p->vptrAt00->processInputAt54(p);
    const auto event = PollManagerAt50FDB0(Manager(Popup(p)));
    if (event == 58) candidate = Add(candidate, -1);
    else if (event == 59) candidate = Add(candidate, 1);
    else if (event >= 60 && event < Add(Count(Popup(p).numbersAtA0), 60))
    {
        const auto selected = Add(Sub(Read(Popup(p).firstPageAt80), 60), event);
        if (selected >= 0 && selected < PageCount() &&
            Resolve<Number>(Popup(p).numbersAtA0.begin[event - 60])->pageAt04 == selected)
        {
            if (selected != Read(Popup(p).selectedPageAt7C))
            { Popup(p).resultAt5C = 54; Popup(p).pendingPageAt84 = selected; }
            else UpdateSaveLoadPopupNativeAt52B390(p, candidate, 1);
        }
    }
    else
    {
        Fresh slider;
        p->vptrAt00->findChildAt14(p, &slider.r, 57);
        Local lifetime{slider.r};
        if (Read(slider.r.baseAt00))
        {
            const auto wheel = p->vptrAt00->wheelAt78(p);
            if (wheel > 0) { if (candidate > 0) candidate = Add(candidate, -1); }
            else if (wheel < 0) { if (candidate < MaximumFirstPage(PageCount())) candidate = Add(candidate, 1); }
            else
            {
                auto* selected = Resolve<GuiControlNativeBody50h>(slider.r);
                candidate = reinterpret_cast<const GuiSliderNativeVtable31*>(selected->vptrAt00)->valueAt60(selected);
            }
        }
        lifetime.Current(true); // 52FD64是显式4F6460，不是无清写尾。
    }
    candidate = std::clamp(candidate, 0, MaximumFirstPage(PageCount()));
    if (Read(Popup(p).firstPageAt80) != candidate)
    {
        UpdateSaveLoadPopupNativeAt52B390(p, candidate, 1);
    }
    return 1;
}
}
}
