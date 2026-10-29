#include "NameRegistrationDialogNative.Internal.h"
#include <initializer_list>

namespace KaGuYa::Start::Runtime::NameRegistrationDialogNativeDetail
{
std::int32_t FocusEdit(Body& b)
{
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    for (int i = 0; i != 2; ++i)
    {
        UnwrittenReference r;
        auto* editRoot = Root(s);
        s.FindEditAt479DF0(editRoot, &r.slot, b.editsAt8C[i]);
        // 532F50没有Find后的EH态；IsWindowEnabled失败不凭空取得Ref清理资格。
        LocalReference lifetime{r.slot, false};
        if (s.IsWindowEnabled(Window(Edit(r.slot))))
        {
            (void)s.SetFocus(EditWindow(Edit(r.slot)));
            lifetime.Reload(false);
            return 1;
        }
        lifetime.Reload();
    }
    return 0;
}
std::int32_t Outside(Common* p)
{
    POINT point;
    p->vptrAt00->pointerAt74(p, &point);
    constexpr int ids[]{6, 7, 8, 9, 10}; // PE609314。
    for (int id : ids)
    {
        UnwrittenReference r;
        Find(p, r.slot, id);
        LocalReference lifetime{r.slot};
        if (r.slot.baseAt00)
        {
            auto* child = ResolveGuiControl(r.slot);
            if (child->vptrAt00->hitAt40(child, point.x, point.y))
            { lifetime.Reload(false); return 0; }
        }
        lifetime.Reload();
    }
    for (const auto& rect : NameRegistrationDialogNativeEditRectanglesSample18())
        if (::PtInRect(&rect, point)) return 0;
    return 1;
}
std::int32_t __fastcall ActiveInput(Common* p, void*)
{
    auto& b = Dialog(p);
    auto* child = p->activeAt4C;
    const auto type = child->typeAt08;
    if (type != 0 && type != 2)
        return type == 11 && child->vptrAt00->keyboardAt14(child) ? 1 : 0;
    const auto latched = b.keyboardLatchAt70;
    if (!latched)
    {
        if (p->vptrAt00->pressedAt70(p, 5))
        {
            b.keyboardLatchAt70 = 1;
            child = p->activeAt4C;
            reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
            return 1;
        }
        return 0;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot event = Copy(p->activeAt4C->eventAt3C);
        LocalReference lifetime{event};
        if (event.baseAt00)
        {
            auto& selected = RequireGuiClickedEvent(event);
            (void)selected.vptrAt00->invokeAt00(&selected);
        }
        p->vptrAt00->resetInputAt3C(p);
        lifetime.Reload();
    }
    return latched;
}
std::int32_t __fastcall UpperInput(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    if (p->vptrAt00->movedAt7C(p) || p->vptrAt00->mousePressAt80(p))
    { p->vptrAt00->resetInputAt3C(p); return 0; }
    if (p->activeAt4C && p->vptrAt00->activeInputAt38(p)) return 1;
    int fallback = 0;
    bool tab = false;
    for (int i = 0; i != 2; ++i)
    {
        UnwrittenReference r;
        auto* editRoot = Root(s);
        s.FindEditAt479DF0(editRoot, &r.slot, b.editsAt8C[i]);
        LocalReference lifetime{r.slot, false}; // 原0/1态是正常尾terminate，不保护Win32叶。
        if (s.IsWindowEnabled(Window(Edit(r.slot))))
        {
            if (tab)
            {
                (void)s.SetFocus(EditWindow(Edit(r.slot)));
                fallback = 3;
                lifetime.Reload(false);
                break;
            }
            auto& event = Field<std::uint32_t>(Edit(r.slot), 0x48);
            tab = event == 3;
            event = 0;
        }
        lifetime.Reload();
    }
    constexpr int order[]{2, 5, 6, 7, 8}; // 609608:2->5->6->7->8->2。
    bool fromTab = tab && !fallback;
    if (fromTab)
    {
        (void)s.SetFocus(s.MainWindowAt6198C8());
        UnwrittenReference r;
        auto* returned = p->vptrAt00->findChildAt14(p, &r.slot, 2);
        LocalReference lifetime{r.slot};
        p->vptrAt00->setActiveAt30(p, ResolveGuiControl(*returned));
        lifetime.Reload(false);
        if (!p->activeAt4C) throw std::logic_error("NameRegist missing child2");
    }
    else
    {
        if (p->inputLatchAt30) fallback = 2;
        if (!p->vptrAt00->pressedAt70(p, 11)) return fallback;
    }
    if (!p->activeAt4C)
    {
        for (int id : order)
        {
            UnwrittenReference r;
            Find(p, r.slot, id);
            LocalReference lifetime{r.slot};
            if (!r.slot.baseAt00) throw 1;
            auto* child = ResolveGuiControl(r.slot);
            const auto state = child->vptrAt00->resourceStateAt38(child);
            if (state == 1 || state == 2) { lifetime.Reload(); continue; }
            const bool edit = id == 2 && FocusEdit(b);
            if (id == 2 && !edit) { lifetime.Reload(); continue; }
            p->vptrAt00->resetChildrenAt68(p);
            p->vptrAt00->setActiveAt30(p, edit ? nullptr : ResolveGuiControl(r.slot));
            p->inputLatchAt30 = !edit;
            lifetime.Reload(false);
            return 3;
        }
        return fallback;
    }
    const auto first = p->activeAt4C->eventAt04;
    auto id = first;
    for (;;)
    {
        int next = -1;
        for (int i = 0; i != 5; ++i) if (order[i] == id) { next = order[(i + 1) % 5]; break; }
        id = next;
        if (id < 0 || id == first) return fallback;
        UnwrittenReference r;
        Find(p, r.slot, id);
        LocalReference lifetime{r.slot};
        if (!r.slot.baseAt00) throw 1;
        auto* child = ResolveGuiControl(r.slot);
        const auto state = child->vptrAt00->resourceStateAt38(child);
        if (state == 1 || state == 2) { lifetime.Reload(); continue; }
        const bool edit = id == 2 && FocusEdit(b);
        if (id == 2 && !edit) { lifetime.Reload(); continue; }
        p->vptrAt00->resetChildrenAt68(p);
        p->vptrAt00->setActiveAt30(p, edit ? nullptr : ResolveGuiControl(r.slot));
        p->inputLatchAt30 = !edit;
        lifetime.Reload(false);
        return 3;
    }
}
void __fastcall ProcessInput(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    b.cancelAt84 = 0;
    b.outsideAt88 = Outside(p);
    auto* input = s.InputAt418210();
    auto* keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    if (!keyboard) throw std::logic_error("NameRegist missing native keyboard");
    const auto buffer = keyboard->bufferAt10;
    const auto* end = keyboard->actionsAt14[0].end;
    bool cancel = false;
    for (auto* binding = keyboard->actionsAt14[0].begin; binding != end; ++binding)
    {
        // 519913/19：+10是双缓冲索引，不是数据指针；保留原buffer而非归一bool。
        auto* bytes = reinterpret_cast<const std::uint8_t*>(binding);
        if (bytes[buffer + 1] && !bytes[2 - buffer]) { cancel = true; break; }
    }
    if (!cancel && b.outsideAt88)
    {
        input = s.InputAt418210();
        auto* mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
        if (!mouse) throw std::logic_error("NameRegist missing native mouse");
        cancel = !mouse->previousAt30[1] && mouse->pressedAt10[1] && !mouse->suppressionAt28;
    }
    if (cancel)
    {
        b.cancelAt84 = 1;
        input = s.InputAt418210();
        keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18);
        if (!keyboard) throw std::logic_error("NameRegist missing native keyboard");
        void* raw = Resolve(keyboard->rawAt04);
        using Reset = void (__thiscall*)(void*);
        auto* table = Field<const std::byte*>(raw, 0);
        Field<Reset>(const_cast<std::byte*>(table), 8)(raw);
        return;
    }
    ProcessGuiDialogInputAt518C30(*p);
}
}
