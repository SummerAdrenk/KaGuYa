#include "ProgramProfileDialogNative.Internal.h"
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <memory>

namespace KaGuYa::Start::Runtime::ProgramProfileDetail
{
namespace
{
class PanelControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PanelControl(Panel* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<Panel*>(payloadAt14);
        if (p) p->base.common.vptrAt00->deletingDestructorAt04(&p->base.common, 1);
    }
    void DeleteControlAtVslot04(unsigned flags) override { RetireEnvironmentObjectControl(this, flags); }
};
void AppendBackground(Panel& p)
{
    Ref child;
    CreateGuiStaticFromSurfaceAt505570(child.slot, &p.base.common, 2,
        Copy(p.base.backgroundAt74), RequireProgramSelectDialogNativeServices().surfaces);
    GuiDialogBaseVtable().appendChildAt10(&p.base.common, Copy(child.slot));
}
}
void NewPanel(Slot& out, Top& owner, const ProgramPanelProfile& profile, Role role, int id)
{
    Leaves leaves;
    auto* p = new Panel;
    bool baseReady = false;
    try
    {
        ConstructGuiDialogAt4E0BE0(&p->base.common, role == Role::Confirmation ? nullptr : &Base(owner), id);
        p->base.managerAt5C = {}; p->base.reservedReferenceAt68 = {}; p->base.backgroundAt74 = {};
        baseReady = true;
        p->owner = &owner; p->data = &profile; p->role = role;
        p->base.selectionAt80 = -1; p->base.confirmLatchAt84 = 0;
        p->base.common.vptrAt00 = &PanelTable();
        AdoptGuiClickedEventManagerAt4D7B90(p->base.managerAt5C, new GuiClickedEventReferences{});
        if (profile.background)
        {
            Load(p->base.backgroundAt74, profile.background);
            Calendar(*p);
            AppendBackground(*p);
        }
        for (const auto& group : profile.groups) ProgramMain01NativeDetail::Populate(p->base, group);
        if (profile.staticSelector)
        {
            const int index = Value(profile.staticSelector);
            if (index < 0 || static_cast<std::size_t>(index) >= profile.selectedStatics.size())
                throw std::out_of_range("Program static selector is outside its original table");
            ProgramMain01NativeDetail::Populate(p->base, {0, 1, &profile.selectedStatics[index]});
        }
        for (const auto& nav : profile.navigation) ProgramMain01NativeDetail::PopulateNavigation(p->base, nav);
        AdoptGuiInputAt536EA0(p->base.common.inputAt18, NewGameGuiInputAt4EAAED());
        auto* control = new PanelControl(p);
        control->referenceCountAt04 = 1; control->owningAt08 = 1;
        control->strideAt0C = sizeof(Panel); control->countAt10 = 1;
        out = {reinterpret_cast<std::uintptr_t>(p), 0, control};
    }
    catch (...)
    {
        if (baseReady) ProgramMain01NativeDetail::DestroyMembers(p->base);
        ::operator delete(p, sizeof(Panel));
        throw;
    }
}
Panel& ChildPanel(Top& b, int id)
{
    Ref r;
    TopTable().inherited.findDialogAt90(&Base(b), &r.slot, id);
    auto* p = Resolve<Panel>(r.slot);
    if (!p) throw std::logic_error("Required Program panel is missing");
    return *p; // The parent owns the panel; the temporary query reference is released.
}
void DestroyTop(Top& b) noexcept
{
    if (b.modal)
    {
        RequireProgramSelectDialogNativeServices().source.SetProgramModalFlag(false);
        b.modal = false;
    }
    ReleaseYtlReference(b.confirmation);
    DestroyProgramSelectDialogNativeAt4F8D50(b.base);
}
}
namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* ConstructProgramProfileDialog(void* raw, const ProgramModeProfile& mode,
    ProgramSelectDialogNativeServices& services)
{
    using namespace ProgramProfileDetail;
    if (mode.implementation != ProgramDialogImplementation::Configured)
        throw std::invalid_argument("Configured Program constructor received a baseline mode");
    ProgramSelectDialogNativeScope scope(services);
    Leaves leaves;
    auto* b = ::new(raw) Top;
    ConstructProgramSelectDialogNativeAt4EAB70(&b->base);
    b->mode = &mode; b->confirmation = {}; b->pendingChoice = nullptr; b->modal = false;
    Base(*b).vptrAt00 = &TopTable().inherited.common;
    try
    {
        auto* env = services.source.CaptureEnvironmentAt4180C0(0);
        b->base.boundsAt74 = {0, 0, static_cast<LONG>(env->widthAt18), static_cast<LONG>(env->heightAt1C)};
        for (auto role : {Role::Main, Role::System})
        {
            Ref panel;
            NewPanel(panel.slot, *b, role == Role::Main ? mode.main : mode.system.panel, role, role == Role::Main ? 1 : 0);
            TopTable().inherited.appendDialogAt8C(&Base(*b), Copy(panel.slot));
        }
        if (!mode.confirmation.events.empty()) NewPanel(b->confirmation, *b, mode.confirmation.panel, Role::Confirmation, -1);
        Load(b->base.blackAt98, "bg_black.bgd");
        AdoptGraphicsSurfaceAt407A80(b->base.canvasAtA4, NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings));
        if (!Resolve<Surface>(b->base.canvasAtA4)->CreateAt415010(services.surfaces,
            b->base.boundsAt74.right, b->base.boundsAt74.bottom, 0, 1)) throw 1;
        AdoptGuiInputAt536EA0(Base(*b).inputAt18, NewGameGuiInputAt4EAAED());
        return &Base(*b);
    }
    catch (...) { DestroyTop(*b); throw; }
}
}
