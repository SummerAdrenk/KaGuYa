#include "SettingPage02Native.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage02NativeDetail;
namespace
{
void NewManager(Slot& slot)
{
    auto* raw = ::operator new(0x0C);
    auto* manager = raw ? ::new(raw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(slot, manager);
}
Choice* ConstructChoice(void* raw, Common* parent, std::int32_t id,
    std::span<const GuiButtonDescriptorNative2Ch> buttons, bool block02)
{
    auto* b = ::new(raw) Choice;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->fieldAt5C = 1; b->managerAt74 = {};
    b->common.vptrAt00 = &(block02 ? SettingPage02Block01NativeVtable() : SettingFixRadioButtonsNativeVtable()).common;
    struct Unwind final { Choice* p; bool active = true; ~Unwind() noexcept { if (active) DestroyChoice(p); } } unwind{b};
    NewManager(b->managerAt74);
    RECT bounds{};
    for (const auto& d : buttons)
        PopulateButton(b->common, b->managerAt74, d, bounds, block02);
    b->boundsAt64 = bounds; // 全部控件完成后才写；60构造不写。
    unwind.active = false; return b;
}
}
Choice* ConstructSettingFixRadioButtonsNativeAt4E07B0(void* raw, Common* parent, std::int32_t id,
    std::span<const GuiButtonDescriptorNative2Ch> buttons)
{ return ConstructChoice(raw, parent, id, buttons, false); }
Choice* ConstructSettingPage02Block01NativeAt4E6070(void* raw, Common* parent, std::int32_t id)
{ return ConstructChoice(raw, parent, id, CurrentSettingTextDisplayProfile().rightClick.buttons, true); }
Mode* ConstructSettingPage02Block02NativeAt4E64D0(void* raw, Common* parent, std::int32_t id)
{
    const auto& profile = CurrentSettingTextDisplayProfile().mode;
    auto* b = ::new(raw) Mode;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->common.vptrAt00 = &SettingPage02Block02NativeVtable();
    struct Unwind final
    {
        Mode* p; bool manager = false, active = true;
        ~Unwind() noexcept
        { if (active) { if (manager) ReleaseYtlReference(p->managerAt6C); DestroyGuiDialogAt4F7AC0(&p->common); } }
    } unwind{b};
    b->managerAt6C = {}; unwind.manager = true;
    NewManager(b->managerAt6C);
    RECT bounds{};
    for (const auto& d : profile.buttons)
        PopulateButton(b->common, b->managerAt6C, d, bounds, true);
    if (profile.label) PopulateStatic(b->common, b->managerAt6C, *profile.label, bounds);
    b->boundsAt5C = bounds; // 78..7B在ctor均不写，Begin才写单byte78。
    unwind.active = false; return b;
}
namespace SettingPage02NativeDetail
{
Choice* ConstructBoundChoice(void* raw, Common* parent, const SettingTextChoiceRow& row)
{ return ConstructChoice(raw, parent, row.id, row.buttons, true); }
Choice* ConstructBooleanRow(void* raw, Common* parent, const SettingTextBoolRow& row)
{
    if (!row.checkbox) return ConstructChoice(raw, parent, row.id, row.buttons, false);
    auto* b = ::new(raw) Choice;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, row.id);
    b->fieldAt5C = 1; b->managerAt74 = {};
    b->common.vptrAt00 = &SettingCheckboxRowTable().common;
    struct Unwind final { Choice* p; bool active = true; ~Unwind() noexcept { if (active) DestroyChoice(p); } } unwind{b};
    NewManager(b->managerAt74);
    RECT bounds{};
    PopulateCheckbox(b->common, b->managerAt74, *row.checkbox, bounds);
    b->boundsAt64 = bounds;
    unwind.active = false; return b;
}
Container* ConstructContainer(void* raw, Common* parent, std::int32_t id, SettingPageRadioContainerKind kind)
{
    const auto& profile = CurrentSettingTextDisplayProfile();
    const bool choices = kind == SettingPageRadioContainerKind::SceneChoices;
    const auto rows = choices ? std::span<const SettingTextBoolRow>{} : BooleanRows(kind);
    const auto count = choices ? profile.sceneChoices.size() : rows.size();
    auto& s = RequireSettingPage02NativeServices();
    auto* b = ::new(raw) Container;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->dialogsAt5C = {}; b->storedAt68 = 1;
    b->common.vptrAt00 = &SettingPageRadioContainerNativeVtable(kind).common.common;
    struct Unwind final
    { Container* p; bool active = true; ~Unwind() noexcept { if (active) DestroyGuiDialogIntoNativeAt4F7B80(&p->common); } } unwind{b};
    RECT bounds{};
    for (std::size_t index = 0; index != count; ++index)
    {
        auto* rawChoice = s.memory.AllocateBody(0x80);
        Choice* created = nullptr;
        {
            struct Raw final
            { void* p; IEnvironmentRootMemory& m; bool active = true; ~Raw() noexcept { if (active) m.DeleteBody(p, 0x80); } } cleanup{rawChoice, s.memory};
            if (!rawChoice) throw std::bad_alloc();
            created = choices ? ConstructBoundChoice(rawChoice, &b->common, profile.sceneChoices[index])
                              : ConstructBooleanRow(rawChoice, &b->common, rows[index]);
            cleanup.active = false;
        }
        Fresh child;
        ConstructDialogRef(child.slot, created ? &created->common : nullptr, 0x80);
        RefLife childLife{child.slot}; // ConstructDialogRef owns the fresh body on failure too.
        auto* capturedChild = Read(child.slot.controlAt08);
        Slot upcast = Copy(child.slot);
        {
            RefLife life{upcast}; auto* captured = Read(upcast.controlAt08);
            const bool grow = b->dialogsAt5C.end == b->dialogsAt5C.capacity;
            AppendNestedGuiDialogReference(b->dialogsAt5C, upcast);
            if (grow) captured = Read(upcast.controlAt08);
            life.Captured(captured);
        }
        RECT out;
        auto* returned = QTable(&created->common).boundsAt8C(&created->common, &out);
        ::UnionRect(&bounds, &bounds, returned);
        childLife.Captured(capturedChild); // 原ESI缓存尾，不改成重选control。
    }
    b->boundsAt6C = bounds;
    unwind.active = false; return b;
}
}
Container* ConstructSettingPage02Block00NativeAt4E5E40(void* raw, Common* parent, std::int32_t id)
{ return ConstructContainer(raw, parent, id, SettingPageRadioContainerKind::Page02Block00); }
Container* ConstructSettingPage02Block03NativeAt4E6930(void* raw, Common* parent, std::int32_t id)
{ return ConstructContainer(raw, parent, id, SettingPageRadioContainerKind::Page02Block03); }
}
