#include "NameRegistrationDialogNative.Internal.h"
#include <new>
#include <initializer_list>

namespace KaGuYa::Start::Runtime::NameRegistrationDialogNativeDetail
{
namespace
{
class BrushControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit BrushControl(HBRUSH* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<HBRUSH*>(payloadAt14);
        if (p)
        {
            if (*p) (void)RequireNameRegistrationDialogNativeServices().source.DeleteObject(*p);
            ::operator delete(p, std::size_t{4});
        }
    }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
static_assert(sizeof(BrushControl) == 0x18);
void AddBackgroundBounds(Common* p)
{
    // 52DBC0两次解析background；分别缓存x/y与width/height，dirty回调不能提前。
    auto& b = Dialog(p);
    auto* first = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(b.backgroundAt5C);
    const auto width = Field<std::int32_t>(first, 8), height = Field<std::int32_t>(first, 12);
    auto* second = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(b.backgroundAt5C);
    const auto x = Field<std::int32_t>(second, 0x10), y = Field<std::int32_t>(second, 0x14);
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}
void InvalidateChild(Common* p, std::int32_t id)
{
    UnwrittenReference r;
    Find(p, r.slot, id);
    LocalReference lifetime{r.slot};
    if (r.slot.baseAt00)
    {
        auto* child = ResolveGuiControl(r.slot);
        const auto* table = p->vptrAt00;
        RECT out;
        auto* rectangle = child->vptrAt00->rectAt3C(child, &out);
        table->invalidateAt64(p, *rectangle);
    }
    lifetime.Reload();
}
void SetDefaultText(Body& b, int index, bool second)
{
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    UnwrittenReference edit;
    auto* fixedRoot = Root(s);
    s.FindEditAt479DF0(fixedRoot, &edit.slot, b.editsAt8C[index]);
    LocalReference lifetime{edit.slot};
    auto* params = GameSystem(s);
    const auto* text = second ? s.GameSystemDefaultNameAt34(params) : s.GameSystemDefaultNameAt30(params);
    const auto hwnd = EditWindow(Edit(edit.slot));
    if (hwnd) (void)s.SetWindowTextA(hwnd, text);
    lifetime.Reload();
}
}
void SetEditBackground(void* edit, COLORREF color)
{
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    auto& brush = Field<Slot>(edit, 0x8C);
    if (!brush.baseAt00)
    {
        ReleaseYtlReference(brush);
        void* raw = ::operator new(4);
        auto* created = raw ? ::new(raw) HBRUSH(nullptr) : nullptr;
        if (brush.controlAt08) brush.controlAt08->DeleteControlAtVslot04(1);
        brush.biasAt04 = 0;
        brush.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
        void* storage = ::operator new(0x18);
        auto* control = storage ? ::new(storage) BrushControl(created) : nullptr;
        brush.controlAt08 = control;
        control->referenceCountAt04 = 1;
        brush.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(brush.controlAt08)->strideAt0C = 4;
        static_cast<EnvironmentObjectReferenceControl*>(brush.controlAt08)->countAt10 = 1;
    }
    if (*ResolveEnvironmentObjectReference<HBRUSH>(brush))
    {
        auto* selected = ResolveEnvironmentObjectReference<HBRUSH>(brush);
        if (s.DeleteObject(*selected)) *selected = nullptr;
    }
    auto* selected = ResolveEnvironmentObjectReference<HBRUSH>(brush);
    *selected = s.CreateSolidBrush(color); // 失败的NULL同样发布，不默填成功。
    Field<COLORREF>(edit, 0x88) = color;
}
void SetRadio(Body& b, std::int32_t id, std::int32_t invalidate)
{
    auto* p = &b.common;
    UnwrittenReference selected;
    Find(p, selected.slot, 5);
    LocalReference lifetime{selected.slot};
    if (selected.slot.baseAt00)
    {
        auto* child = ResolveGuiControl(selected.slot);
        using Select = void (__thiscall*)(Child*, std::int32_t);
        Field<Select>(const_cast<GuiControlNativeVtable23*>(child->vptrAt00), 0x64)(child, id);
        if (invalidate)
        {
            child = ResolveGuiControl(selected.slot);
            const auto* table = p->vptrAt00;
            RECT out;
            const auto* rect = child->vptrAt00->rectAt3C(child, &out);
            table->invalidateAt64(p, *rect);
        }
        p->vptrAt00->setActiveAt30(p, ResolveGuiControl(selected.slot));
    }
    lifetime.Reload(false);
    if (!invalidate) return;
    InvalidateChild(p, 9);
    InvalidateChild(p, 10);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    (void)s.SetFocus(s.SystemWindowAt2C(s.SystemAt4180C0()));
}
std::int32_t RadioChoice(Body& b)
{
    if (!b.radioEnabledAt80) return -1;
    for (int id : {9, 10})
    {
        UnwrittenReference r;
        Find(&b.common, r.slot, id);
        LocalReference lifetime{r.slot};
        if (r.slot.baseAt00)
        {
            auto* child = ResolveGuiControl(r.slot);
            if (!child->vptrAt00->readStateAt48(child))
            { lifetime.Reload(false); return id == 9 ? 0 : 1; }
        }
        lifetime.Reload(false);
    }
    return -1;
}
void BeginModal(Common* p)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    if (!b.backgroundAt5C.baseAt00)
    {
        auto* fixedRoot = Root(s);
        auto* graphics = Resolve(s.RootDrawSlotAt18(fixedRoot));
        UnwrittenReference background;
        auto* returned = s.CaptureBackgroundAt4ACDD0(graphics, &background.slot);
        LocalReference lifetime{background.slot};
        CopyAssignGraphicsSurfaceReferenceAt465870(b.backgroundAt5C, *returned);
        lifetime.Reload(false);
    }
    p->vptrAt00->writeEnabledAt24(p, 1);
    ClearGuiClickedEventFlagsAt532200(b.eventsAt74);
    b.phaseAt6C = 0;
    p->vptrAt00->resetInputAt3C(p);
    p->vptrAt00->resetChildrenAt68(p);
    p->activeAt4C = nullptr;
    Table(p).beginExtraAt8C(p);
    s.RootModalFlagAt1B8(Root(s)) = 1;
}
void __fastcall Begin(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    {
        UnwrittenReference panel;
        Find(p, panel.slot, 5);
        LocalReference lifetime{panel.slot};
        if (panel.slot.baseAt00)
        {
            auto* child = ResolveGuiControl(panel.slot);
            child->vptrAt00->writeEnabledAt4C(child, b.radioEnabledAt80);
            using Select = void (__thiscall*)(Child*, std::int32_t);
            child = ResolveGuiControl(panel.slot);
            Field<Select>(const_cast<GuiControlNativeVtable23*>(child->vptrAt00), 0x64)(child, 9);
        }
        lifetime.Reload(false);
    }
    const int disabled[]{int((~(s.GameSystemFlagsAt2C(GameSystem(s)) >> 1)) & 1),
        int((~s.GameSystemFlagsAt2C(GameSystem(s))) & 1)};
    const char* names[2];
    names[0] = s.RegistryNameAt5C(Registry1(s)).GetString();
    names[1] = s.RegistryNameAt58(Registry1(s)).GetString();
    bool focused = false;
    for (int i = 0; i != 2; ++i)
    {
        b.editsAt8C[i] = s.NewEditAt46FA30(Root(s), disabled[i], 1, nullptr);
        const auto rectangle = NameRegistrationDialogNativeEditRectanglesSample18()[i];
        UnwrittenReference edit;
        auto* editRoot = Root(s);
        s.FindEditAt479DF0(editRoot, &edit.slot, b.editsAt8C[i]);
        LocalReference lifetime{edit.slot}; // state2；失败不移除root已发布edit。
        ::CopyRect(&Field<RECT>(Edit(edit.slot), 0x4C), &rectangle);
        auto* layoutRoot = Root(s);
        s.ApplyEditAt4875F0(layoutRoot, b.editsAt8C[i]);
        auto* selected = Edit(edit.slot);
        const auto window = EditWindow(selected);
        if (window)
        {
            Field<std::int32_t>(selected, 0x7C) = i == 0 ? 4 : 8;
            (void)s.SetWindowTextA(window, names[i]);
        }
        if (disabled[i])
        {
            Field<COLORREF>(Edit(edit.slot), 0x84) = 0x808080;
            selected = Edit(edit.slot);
            SetEditBackground(selected, 0xC2B4B9);
            UnwrittenReference label;
            Find(p, label.slot, i + 3);
            LocalReference labelLifetime{label.slot};
            if (label.slot.baseAt00)
            { auto* child = ResolveGuiControl(label.slot); child->vptrAt00->writeStateAt44(child, 0); }
            labelLifetime.Reload();
        }
        selected = Edit(edit.slot);
        (void)s.ShowWindow(Window(selected), 5);
        Field<std::uint32_t>(selected, 0x48) = 0;
        UnwrittenText text;
        s.ReadEditTextAt50B000(selected, &text.text);
        LocalText textLifetime{text.text, false}; // 50D646无TEXT展开态，只正常消费。
        (void)s.SendMessageA(EditWindow(selected), 0xB1, 0, text.text.GetLength());
        textLifetime.Finish();
        if (!focused && !disabled[i])
        { (void)s.SetFocus(EditWindow(Edit(edit.slot))); focused = true; }
        lifetime.Reload();
    }
    BeginModal(p);
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    auto& b = Dialog(p);
    switch (b.phaseAt6C)
    {
    case 0:
        AddBackgroundBounds(p);
        b.phaseAt6C = 3;
        break;
    case 3:
        p->vptrAt00->processInputAt54(p);
        if (Table(p).pollExtraAt90(p)) b.phaseAt6C = 4;
        break;
    case 4:
        AddBackgroundBounds(p);
        p->vptrAt00->writeEnabledAt24(p, 0);
        Table(p).endExtraAt94(p);
        b.phaseAt6C = -1;
        p->vptrAt00->flushDirtyAt58(p);
        return 1;
    default: return 0;
    }
    p->vptrAt00->flushDirtyAt58(p);
    return 1;
}
std::int32_t __fastcall End(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    if (b.resultAt68 == 6)
    {
        const auto choice = RadioChoice(b);
        if (choice >= 0) s.RegistryNameChangedAt54(Registry1(s)) = choice != 0;
    }
    for (int i = 0; i != 2; ++i)
    {
        if (b.resultAt68 == 6)
        {
            UnwrittenReference edit;
            auto* editRoot = Root(s);
            s.FindEditAt479DF0(editRoot, &edit.slot, b.editsAt8C[i]);
            LocalReference lifetime{edit.slot};
            UnwrittenText text;
            s.ReadEditTextAt50B000(Edit(edit.slot), &text.text);
            LocalText textLifetime{text.text, true};
            if (text.text.GetLength())
            {
                auto* registry = Registry1(s);
                auto& destination = i == 0 ? s.RegistryNameAt5C(registry) : s.RegistryNameAt58(registry);
                AssignRuntimeCStringAt410220(destination, text.text);
            }
            textLifetime.Finish();
            lifetime.Reload();
        }
        auto* removeRoot = Root(s);
        s.RemoveEditAt486B60(removeRoot, b.editsAt8C[i]);
    }
    ReleaseYtlReference(b.backgroundAt5C);
    s.RootModalFlagAt1B8(Root(s)) = 0;
    return 1;
}
void __fastcall FlushDirty(Common* p, void*)
{
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = p->vptrAt00->dirtyAt00(p);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = p->vptrAt00->dirtyAt00(p);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (int i = 0; i < count; ++i)
    {
        const auto handle = Field<std::int32_t>(Resolve(Dialog(p).backgroundAt5C), 4);
        if (handle)
            (void)RequireNameRegistrationDialogNativeServices().graphics.CompositionAt4182F0().DrawSurface(
                handle, 0, 0, nullptr, nullptr, rectangles + i, 0);
        p->vptrAt00->drawAt60(p, rectangles + i);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangles + i);
    }
    dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->clearAt00(dirty);
}
std::int32_t __fastcall PollExtra(Common* p, void*)
{
    auto& b = Dialog(p);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    if (b.cancelAt84) { b.resultAt68 = 7; return 1; }
    int event = -1;
    for (auto* r = b.eventsAt74.begin; r != b.eventsAt74.end; ++r)
    {
        if (!RequireGuiClickedEvent(*r).clickedFlagAt04) continue;
        event = RequireGuiClickedEvent(*r).eventValueAt08;
        auto& selected = RequireGuiClickedEvent(*r);
        selected.vptrAt00->clearAt04(&selected);
        break;
    }
    switch (event)
    {
    case 9: case 10: SetRadio(b, event, 1); return 0;
    case 6:
        if (s.GameSystemFlagsAt2C(GameSystem(s)) & 1) SetDefaultText(b, 1, false);
        if (s.GameSystemFlagsAt2C(GameSystem(s)) & 2) SetDefaultText(b, 0, true);
        return 0;
    case 7:
        for (int i = 0; i != 2; ++i)
        {
            UnwrittenReference edit;
            auto* editRoot = Root(s);
            s.FindEditAt479DF0(editRoot, &edit.slot, b.editsAt8C[i]);
            LocalReference lifetime{edit.slot};
            UnwrittenText text;
            s.ReadEditTextAt50B000(Edit(edit.slot), &text.text);
            LocalText textLifetime{text.text, false}; // 原state2只保护edit，不保护TEXT。
            const bool nonempty = text.text.GetLength() != 0;
            textLifetime.Finish();
            lifetime.Reload();
            if (!nonempty) goto focusOutside;
        }
        b.resultAt68 = 6;
        return 1;
    case 8: b.resultAt68 = 7; return 1;
    default: break;
    }
focusOutside:
    if (b.outsideAt88)
    {
        auto* input = s.InputAt418210();
        auto* mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
        if (!mouse) throw std::logic_error("NameRegist missing native mouse");
        if (!mouse->previousAt30[0] && mouse->pressedAt10[0] && !mouse->suppressionAt28)
            (void)s.SetFocus(s.SystemWindowAt2C(s.SystemAt4180C0()));
    }
    return 0;
}
}
