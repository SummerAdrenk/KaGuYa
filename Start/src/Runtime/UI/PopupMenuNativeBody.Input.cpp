#include "PopupMenuNativeBody.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace PopupNativeDetail;
void __fastcall PopupMenuNativeEntries::Reset(GuiDialogNativeBody5Ch* common, void*)
{
    const auto* table = common->vptrAt00;
    Popup(common).confirmAt78 = 0;
    table->setActiveAt30(common, nullptr);
    common->inputLatchAt30 = 0;
}
std::int32_t __fastcall PopupMenuNativeEntries::ActiveInput(GuiDialogNativeBody5Ch* common, void*)
{
    auto* active = common->activeAt4C;
    if (!active) throw std::logic_error("510FA0 requires active control");
    if (active->typeAt08 != 2) return 0;
    auto& body = Popup(common);
    const auto previous = body.confirmAt78;
    const auto* table = common->vptrAt00;
    if (!previous)
    {
        if (!table->pressedAt70(common, 5)) return 0;
        active = common->activeAt4C;
        if (!active) throw std::logic_error("511072 requires late active control");
        body.confirmAt78 = 1;
        const auto* childTable = reinterpret_cast<const GuiCheckBoxNativeVtable30*>(active->vptrAt00);
        childTable->setPressedAt70(active, 1);
        return 1;
    }
    if (table->heldAt6C(common, 5)) return previous;
    active = common->activeAt4C;
    if (!active) throw std::logic_error("510FDA requires late active control");
    Slot event;
    CopyGuiClickedEventReference(event, active->eventAt3C);
    LocalReference lifetime{event};
    if (event.baseAt00)
    {
        auto& selected = RequireGuiClickedEvent(event);
        selected.vptrAt00->invokeAt00(&selected);
    }
    body.confirmAt78 = 0;
    lifetime.Finish(true); // 511061调用4F64D0，不是保留局部bits的内联尾。
    return previous;
}
void __fastcall PopupMenuNativeEntries::Input(GuiDialogNativeBody5Ch* common, void*)
{
    auto& body = Popup(common);
    auto& cancel = RequirePopupMenuNativeDependencies().cancel;
    const auto& keyboard = cancel.KeyboardAt5199FF();
    const auto bindings = keyboard.BindingCount(0);
    for (std::size_t index = 0; index != bindings; ++index)
        if (keyboard.CurrentState(0, index) && !keyboard.PreviousState(0, index))
        { body.phaseAt6C = 4; return; }
    if (cancel.RightButtonAt519A5A()) { body.phaseAt6C = 4; return; }
    ProcessGuiDialogInputAt518C30(*common);
    for (auto* it = common->childrenAt0C.begin; it != common->childrenAt0C.end; ++it)
    {
        const auto hintId = HintFor(RequireGuiControl(*it).eventAt04);
        if (hintId < 0) continue;
        Slot hint;
        common->vptrAt00->findChildAt14(common, &hint, hintId);
        LocalReference lifetime{hint};
        if (hint.baseAt00)
        {
            auto& selected = RequireGuiControl(hint);
            if (selected.vptrAt00->readStateAt48(&selected))
            {
                auto& source = RequireGuiControl(*it);
                if (source.vptrAt00->enabledAt1C(&source))
                {
                    const std::int32_t visible = common->activeAt4C && common->activeAt4C == ResolveGuiControl(*it);
                    auto& target = RequireGuiControl(hint);
                    if (target.vptrAt00->readEnabledAt50(&target) != visible)
                    {
                        WriteVisible(hint, visible);
                        InvalidateChild(*common, hint);
                    }
                }
            }
        }
        lifetime.Finish(true);
    }
    const auto* end = body.eventsAt80.end;
    for (auto* it = body.eventsAt80.begin; it != end; ++it)
        if (RequireGuiClickedEvent(*it).clickedFlagAt04)
        {
            body.resultAt68 = RequireGuiClickedEvent(*it).eventValueAt08;
            body.phaseAt6C = 4;
            return;
        }
}
}
