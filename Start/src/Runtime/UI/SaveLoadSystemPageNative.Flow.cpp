#include "SaveLoadSystemPageNative.Internal.h"
#include <Runtime/SaveLoadPageSecondNative.h>

namespace KaGuYa::Start::Runtime::SaveLoadSystemPageNativeDetail
{
void __fastcall Begin(Common* p, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Manager(Page(p)));
    p->vptrAt00->resetChildrenAt68(p);
    for (const auto event : {13, 10})
    {
        FreshReference child;
        p->vptrAt00->findChildAt14(p, &child.slot, event);
        LocalReference lifetime{child.slot};
        if (child.slot.baseAt00)
        {
            auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
            auto& s = RequireSaveLoadSystemPageNativeServices().source;
            s.EnsureRootAt413BE0();
            auto* root = s.RootAt6198C4();
            const auto* table = selected->vptrAt00;
            const auto value = s.RootByteAt218(root);
            table->writeStateAt44(selected, value != 0);
        }
        lifetime.Reload(false); // 50ECCA/50ED50正常尾不清参数位模式。
    }
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    Page(p).resultAt5C = -1;
    auto& s = RequireSaveLoadSystemPageNativeServices().source;
    auto* input = s.InputAt418210();
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    // 52FDD1..52FDF9：固定begin/end/buffer，第一组action0。直接双缓冲，不加bounds。
    auto* position = Read(keyboard->actionsAt14[0].begin);
    auto* const end = Read(keyboard->actionsAt14[0].end);
    const auto buffer = position != end ? Read(keyboard->bufferAt10) : 0u;
    for (; position != end; ++position)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(position);
        const auto current = *reinterpret_cast<const volatile std::uint8_t*>(address + buffer + 1u);
        if (current && !*reinterpret_cast<const volatile std::uint8_t*>(address - buffer + 2u))
        { Page(p).resultAt5C = 11; return 1; }
    }
    // 未取消才第二次Capture；mouse只取右键previous/current和suppression，不读取secondary。
    input = s.InputAt418210();
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if (!Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28))
    { Page(p).resultAt5C = 11; return 1; }
    p->vptrAt00->processInputAt54(p);
    Page(p).resultAt5C = PollManagerAt50FDB0(Manager(Page(p)), 1);
    return 1;
}
std::int32_t __fastcall End(Common*, void*) { return 1; } // 5094C0原只有mov eax,1;ret。
std::int32_t __fastcall Result(Common* p, void*) { return Read(Page(p).resultAt5C); }
void __fastcall Draw(Common* p, void*, const RECT* clip)
{
    DrawSaveLoadPageBaseNativeAt524960(p,clip);
}
void __fastcall Composite(Common* p, void*, Slot owned, const RECT* clip)
{
    CompositeSaveLoadPageBaseNativeAt5249C0(p,owned,clip);
}
}
