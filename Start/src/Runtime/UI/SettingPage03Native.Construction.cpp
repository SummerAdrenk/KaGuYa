#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage03NativeDetail;
namespace
{
void Manager(Slot& slot)
{
    auto* raw = ::operator new(0x0C);
    auto* p = raw ? ::new(raw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(slot, p);
}
void Ranges(Common& p)
{
    for (auto* pos = Read(p.childrenAt0C.begin); pos != Read(p.childrenAt0C.end); ++pos)
    {
        auto* c = Resolve<Control>(*pos);
        if (Read(c->typeAt08) != 5) continue;
        c = Resolve<Control>(*pos); Slider(c).setRangeAt5C(c, 0, 100);
        c = Resolve<Control>(*pos); Slider(c).setStepAt68(c, 1);
    }
}
Block* ConstructBlock(void* raw, Common* parent, std::int32_t id, std::uint32_t kind)
{
    Block* b;
    if (kind == 4) { auto* full = ::new(raw) SettingPage03Block04NativeBody84h; b = &full->block; }
    else b = ::new(raw) Block;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->storedAt5C = 1; b->managerAt74 = {};
    b->common.vptrAt00 = &SettingPage03BlockNativeVtable(kind).common;
    struct Unwind final { Common* p; bool active = true; ~Unwind() noexcept { if (active) DestroyBlockBase(p); } } unwind{&b->common};
    Manager(b->managerAt74);
    RECT bounds{};
    if (kind == 2)
    {
        for (const auto& label : CurrentSettingPage03Profile().labels)
        {
            const auto& pair = label.images;
            const auto* descriptor = label.displayIndex < 0 || *Sounds(true)->CharacterDisplayAt84(label.displayIndex)
                ? pair.shown : pair.hidden;
            // 4E777C原分支仅在备用descriptor为空时选主descriptor，是原证行为。
            if (!descriptor) descriptor = pair.shown;
            Populate(b->common, b->managerAt74, SettingPage03ControlKind::Static, descriptor, bounds, false);
        }
    }
    constexpr std::size_t widths[]{0x24, 0x2C, 0x48, 0x6C};
    for (const auto& group : CurrentSettingPage03Profile().groups.at(kind))
        for (std::uint32_t i = 0; i != group.count; ++i)
            Populate(b->common, b->managerAt74, group.kind,
                static_cast<const std::byte*>(group.descriptors) + widths[static_cast<unsigned>(group.kind)] * i, bounds);
    b->boundsAt64 = bounds;
    if (kind != 4) Ranges(b->common);
    // +60所有块不写；04的+80也不写，515140首次建立page值。
    unwind.active = false; return b;
}
Common* NewBlock(Parent& p, unsigned kind, int id)
{
    const std::size_t size = kind == 4 ? 0x84 : kind == 3 ? 0x7C : 0x80;
    auto& memory = RequireSettingPage03NativeServices().memory;
    void* raw = memory.AllocateBody(size);
    struct Raw final { void* p; std::size_t n; IEnvironmentRootMemory& m; bool active = true;
        ~Raw() noexcept { if (active) m.DeleteBody(p, n); } } cleanup{raw, size, memory};
    Common* result = nullptr;
    if (raw)
    {
        switch (kind)
        {
        case 0: result = &ConstructSettingPage03Block00NativeAt4E6B60(raw, &p.common, id)->common; break;
        case 1: result = &ConstructSettingPage03Block01NativeAt4E7050(raw, &p.common, id)->common; break;
        case 2: result = &ConstructSettingPage03Block02NativeAt4E7540(raw, &p.common, id)->common; break;
        case 3: result = &ConstructSettingPage03Block03NativeAt4E7F30(raw, &p.common, id)->common; break;
        case 4: result = &ConstructSettingPage03Block04NativeAt4E8130(raw, &p.common, id)->block.common; break;
        }
    }
    cleanup.active = false; return result;
}
}
Block* ConstructSettingPage03Block00NativeAt4E6B60(void* p, Common* parent, std::int32_t id)
{ return ConstructBlock(p, parent, id, 0); }
Block* ConstructSettingPage03Block01NativeAt4E7050(void* p, Common* parent, std::int32_t id)
{ return ConstructBlock(p, parent, id, 1); }
Block* ConstructSettingPage03Block02NativeAt4E7540(void* p, Common* parent, std::int32_t id)
{ return ConstructBlock(p, parent, id, 2); }
SettingPage03Block04NativeBody84h* ConstructSettingPage03Block04NativeAt4E8130(void* p, Common* parent, std::int32_t id)
{ return reinterpret_cast<SettingPage03Block04NativeBody84h*>(ConstructBlock(p, parent, id, 4)); }
namespace SettingPage03NativeDetail
{
Block* ConstructRadio(void* raw, Common* parent, std::int32_t id)
{
    auto* b = ::new(raw) Block;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->storedAt5C = 1; b->managerAt74 = {}; b->common.vptrAt00 = &RadioTable();
    struct Unwind final { Common* p; bool active = true; ~Unwind() noexcept { if (active) DestroyBlockBase(p); } } unwind{&b->common};
    Manager(b->managerAt74);
    RECT bounds{};
    for (const auto& d : CurrentSettingPage03Profile().loop.buttons)
        Populate(b->common, b->managerAt74, SettingPage03ControlKind::Button, &d, bounds, false);
    b->boundsAt64 = bounds; unwind.active = false; return b;
}
}
Container* ConstructSettingPage03Block03NativeAt4E7F30(void* raw, Common* parent, std::int32_t id)
{
    auto* b = ::new(raw) Container;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->dialogsAt5C = {}; b->storedAt68 = 1;
    b->common.vptrAt00 = &SettingPage03Block03NativeVtable().common.common;
    struct Unwind final { Common* p; bool active = true; ~Unwind() noexcept { if (active) DestroyGuiDialogIntoNativeAt4F7B80(p); } } unwind{&b->common};
    RECT bounds{};
    auto& s = RequireSettingPage03NativeServices();
    auto* radioRaw = s.memory.AllocateBody(0x80); Block* radio = nullptr;
    {
        struct Raw final { void* p; IEnvironmentRootMemory& m; bool active = true;
            ~Raw() noexcept { if (active) m.DeleteBody(p, 0x80); } } cleanup{radioRaw, s.memory};
        if (radioRaw) radio = ConstructRadio(radioRaw, &b->common, CurrentSettingPage03Profile().loop.radioGroup);
        cleanup.active = false;
    }
    Fresh child; ConstructDialogRef(child.slot, radio ? &radio->common : nullptr, 5, 0x80);
    {
        RefLife life{child.slot}; auto* captured = Read(child.slot.controlAt08);
        GuiDialogIntoBaseVtable().appendDialogAt8C(&b->common, Copy(child.slot));
        RECT out; auto* selected = Resolve<Common>(child.slot);
        auto* returned = Table(selected).boundsAt8C(selected, &out);
        ::UnionRect(&bounds, &bounds, returned); life.Captured(captured);
    }
    Fresh label;
    CreateGuiStaticAt505800(label.slot, &b->common, *CurrentSettingPage03Profile().loop.label, s.loader, s.surfaces, s.strings);
    {
        RefLife life{label.slot}; auto* selected = Resolve<Control>(label.slot);
        RECT out; auto* returned = CTable(selected).rectAt3C(selected, &out);
        ::UnionRect(&bounds, &bounds, returned);
        GuiDialogBaseVtable().appendChildAt10(&b->common, Copy(label.slot)); life.Normal();
    }
    b->boundsAt6C = bounds; unwind.active = false; return b;
}
Parent* ConstructSettingPage03NativeAt4F2910(void* raw, Common* parent, std::int32_t id)
{
    auto* p = ::new(raw) Parent;
    ConstructGuiDialogAt4E0BE0(&p->common, parent, id);
    p->dialogsAt5C = {}; p->footerAt78 = {}; p->common.vptrAt00 = &SettingPage03NativeVtable().common.common;
    struct Unwind final { Parent* p; bool active = true; ~Unwind() noexcept { if (active) DestroySettingPage03NativeAt4F9840(p); } } unwind{p};
    RECT bounds{};
    for (unsigned kind : CurrentSettingPage03Profile().blockOrder)
    {
        auto* created = NewBlock(*p, kind, 15 + kind);
        Fresh child; ConstructDialogRef(child.slot, created, kind, kind == 4 ? 0x84 : kind == 3 ? 0x7C : 0x80);
        RefLife life{child.slot}; auto* captured = Read(child.slot.controlAt08);
        GuiDialogIntoBaseVtable().appendDialogAt8C(&p->common, Copy(child.slot));
        RECT out;
        auto* returned = kind == 3 ? ContainerTable(created).boundsAt94(created, &out) : Table(created).boundsAt8C(created, &out);
        ::UnionRect(&bounds, &bounds, returned); life.Captured(captured);
    }
    GuiFactoryConstructionLifetime factory; // 4F2E82的new1 + 4D7860控制块18，真实寿命。
    auto& s = RequireSettingPage03NativeServices(); Fresh footer;
    auto* returned = CreateGuiStaticAt505800(footer.slot, &p->common, *CurrentSettingPage03Profile().footer, s.loader, s.surfaces, s.strings);
    {
        RefLife life{footer.slot};
        if (&p->footerAt78 != returned) { ReleaseYtlReference(p->footerAt78); CopyGuiClickedEventReference(p->footerAt78, *returned); }
        life.Normal();
    }
    auto* selected = Resolve<Control>(p->footerAt78); RECT out;
    auto* rectangle = CTable(selected).rectAt3C(selected, &out); ::UnionRect(&bounds, &bounds, rectangle);
    p->boundsAt68 = bounds; factory.Finish(); unwind.active = false; return p;
}
}
