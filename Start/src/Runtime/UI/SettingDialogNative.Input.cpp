#include "SettingDialogNative.Internal.h"
#include <Runtime/SettingShellResourceProfile.h>

namespace KaGuYa::Start::Runtime::SettingDialogNativeDetail
{
namespace
{
void ResetCommandControl(Body& b, const Slot& system, int event)
{
    auto* nested = Resolve<Common>(system);
    FreshReference child;
    Read(nested->vptrAt00)->findChildAt14(nested, &child.slot, event);
    LocalReference lifetime{child.slot};
    if (Read(child.slot.baseAt00))
    {
        auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
        Read(selected->vptrAt00)->resetAt08(selected);
        selected = Resolve<GuiControlNativeBody50h>(child.slot);
        const auto* parentTable = Read(b.common.vptrAt00); // 原先固定parent table，再调用子rect。
        RECT out;
        const auto* returned = Read(selected->vptrAt00)->rectAt3C(selected, &out);
        parentTable->invalidateAt64(&b.common, *returned);
    }
    lifetime.Reload();
}
void PollCurrentPage(Body& b)
{
    const auto selection = Read(b.pageAt78);
    const auto& profile = CurrentSettingShellProfile();
    bool available = false;
    for (const auto kind : profile.pageVisitOrder)
        if (static_cast<int>(kind) == selection) { available = true; break; }
    if (!available) return;
    FreshReference page;
    Table(&b.common).findDialogAt90(&b.common, &page.slot, selection + 1);
    LocalReference lifetime{page.slot};
    auto* nested = Resolve<Common>(page.slot);
    (void)Read(nested->vptrAt00)->pollAt48(nested);
    if (selection == static_cast<int>(SettingShellPage::Other) && profile.nameResult >= 0)
    {
        nested = Resolve<Common>(page.slot);
        if (Read(nested->vptrAt00)->resultAt50(nested) == profile.nameResult)
        { b.phaseAt74 = 8; b.confirmationAtA8 = 0; }
    }
    lifetime.Reload(false);
}
}
void __fastcall ProcessInput(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& source = RequireSettingDialogNativeServices().source;
    auto* input = source.InputAt418210();
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    auto* position = Read(keyboard->actionsAt14[0].begin);
    const auto* end = Read(keyboard->actionsAt14[0].end);
    if (position != end)
    {
        const auto buffer = Read(keyboard->bufferAt10);
        for (; position != end; ++position)
        {
            const auto address = reinterpret_cast<std::uintptr_t>(position);
            if (*reinterpret_cast<const volatile std::uint8_t*>(address + buffer + 1u) &&
                !*reinterpret_cast<const volatile std::uint8_t*>(address - buffer + 2u))
            { b.phaseAt74 = 4; return; }
        }
    }
    input = source.InputAt418210(); // 键盘取消短路时不能捕获mouse。
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if (!Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28))
    { b.phaseAt74 = 4; return; }
    FreshReference system;
    Table(p).findDialogAt90(p, &system.slot, 0);
    LocalReference lifetime{system.slot};
    if (Read(system.slot.baseAt00))
    {
        auto* nested = Resolve<Common>(system.slot);
        (void)Read(nested->vptrAt00)->pollAt48(nested);
        const auto result = [&] {
            auto* current = Resolve<Common>(system.slot);
            return Read(current->vptrAt00)->resultAt50(current);
        };
        // 原反复调用Result，不缓存第一次值。每条分支按原顺序解析、写phase和释放。
        const auto& profile = CurrentSettingShellProfile();
        for (const auto& command : profile.commands)
        {
            if (result() != command.button) continue;
            switch (command.action)
            {
            case SettingShellAction::Close:
                b.phaseAt74 = 4; lifetime.Reload(); return;
            case SettingShellAction::ExitGame: b.phaseAt74 = 5; break;
            case SettingShellAction::ReturnToTitle:
                b.phaseAt74 = 6; b.confirmationAtA8 = 0; break;
            case SettingShellAction::RestoreDefaults:
                b.phaseAt74 = 7; b.confirmationAtA8 = 0; break;
            }
            ResetCommandControl(b, system.slot, command.button);
            lifetime.Reload(); return;
        }
        int switchedTo = -1;
        for (const auto& tab : profile.tabs)
        {
            if (result() != tab.button) continue;
            const auto selection = static_cast<int>(tab.page);
            if (Read(b.pageAt78) != selection)
            { SelectSettingDialogNativePageAt5352F0(b, selection); switchedTo = selection; }
            break;
        }
        if (switchedTo != -1)
        {
            auto* root = Root(source);
            source.RootPageAt214(root) = switchedTo;
            AddBackgroundBounds(b);
            lifetime.Reload(); return;
        }
    }
    lifetime.Reload(false);
    PollCurrentPage(b);
}
}
