#include "LegacyMenuSelectionNative.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMenuSelectionDetail
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
        if (p) p->common.vptrAt00->deletingDestructorAt04(&p->common, 1);
    }
    void DeleteControlAtVslot04(unsigned flags) override { RetireEnvironmentObjectControl(this, flags); }
};
void Append(Panel& p, Ref& child, bool clickable)
{
    auto* control = ResolveGuiControl(child.slot);
    if (!control) throw std::logic_error("Menu control construction returned no payload");
    RECT rect;
    control->vptrAt00->rectAt3C(control, &rect);
    ::UnionRect(&p.bounds, &p.bounds, &rect);
    if (clickable)
    {
        Ref event;
        CreatePopupClickedEventAt4EA7B3(event.slot, control->eventAt04);
        BindPopupGuiControlEventAt4EA856(*control, Copy(event.slot), event.slot);
        AppendGuiClickedEventReference(p.events, event.slot);
    }
    AppendGuiControlChildReference(p.common.childrenAt0C, child.slot);
}
void CheckBox(Panel& p, const LegacyMenuSelectionCheckDescriptor& descriptor)
{
    auto& s = Services(p);
    Ref normal, checked, child;
    LoadGuiControlResourceAt5047A0(normal.slot, descriptor.normal.data(), s.resources, s.surfaces, s.strings);
    LoadGuiControlResourceAt5047A0(checked.slot, descriptor.checked.data(), s.resources, s.surfaces, s.strings);
    if (!normal.slot.baseAt00 || !checked.slot.baseAt00) throw 1;
    if (descriptor.position)
    {
        MoveGuiControlResourceAt5086C0(Copy(normal.slot), descriptor.position->x, descriptor.position->y);
        MoveGuiControlResourceAt5086C0(Copy(checked.slot), descriptor.position->x, descriptor.position->y);
    }
    struct CheckOwner final
    {
        GuiCheckBoxNativeBody60h* raw=new GuiCheckBoxNativeBody60h;
        bool ready=false;
        ~CheckOwner() noexcept
        {
            if (!raw) return;
            if (ready) raw->common.vptrAt00->deletingDestructorAt00(&raw->common,1);
            else ::operator delete(raw,sizeof(*raw));
        }
    } owner;
    auto* body = ConstructGuiCheckBoxAt4E0520(owner.raw, &p.common, descriptor.event,
        Copy(normal.slot), Copy(checked.slot));
    owner.ready=true;
    body->common.childIdAt0C = descriptor.childId;
    GuiCheckBoxVtable().setHitSlotAt60(&body->common, descriptor.hitSlot);
    ConstructGuiCheckBoxReferenceAt4D7710(child.slot, owner.raw, 1);
    owner.raw=nullptr;
    Append(p, child, true);
}
}
void NewPanel(Slot& out, Top& top, bool system)
{
    auto& s = *top.services;
    Leaves leaves(s);
    auto raw = std::make_unique<Panel>();
    auto& p = *raw;
    p.owner = &top; p.system = system;
    ConstructGuiDialogAt4E0BE0(&p.common, &top.into.common, system ? 0 : 1);
    p.common.vptrAt00 = &PanelMethods();
    try
    {
        const auto& profile = Profile(p);
        const auto buttons = [&](std::span<const GuiButtonDescriptorNative2Ch> rows)
        {
            for (const auto& row : rows)
            {
                Ref child;
                CreateGuiButtonAt5041D0(child.slot, &p.common, row, s.resources, s.surfaces, s.strings);
                if (!system)
                {
                    RECT rect;
                    auto* c = ResolveGuiControl(child.slot);
                    c->vptrAt00->rectAt3C(c, &rect);
                    const auto i = static_cast<std::size_t>(row.common.eventAt00 - 13);
                    p.labelPoints.at(i) = {rect.left + profile.labelOffset.x, rect.top + profile.labelOffset.y};
                }
                Append(p, child, true);
            }
        };
        if (system)
        {
            // 原0x4EAF20/0x565560：四按钮、两checkbox、四按钮，不能按event排序。
            buttons(profile.systemBeforeChecks);
            for (const auto& row : profile.systemChecks) CheckBox(p, row);
            buttons(profile.systemAfterChecks);
        }
        else
        {
            for (const auto& row : profile.statics)
            {
                Ref child;
                CreateGuiStaticAt505800(child.slot, &p.common, row, s.resources, s.surfaces, s.strings);
                Append(p, child, false);
            }
            // 原0x4EB9B0先取三按钮union作为透明文字画布，再与静态层合并bounds。
            const auto staticBounds = p.bounds;
            p.bounds = {};
            buttons(profile.choices);
            AdoptGraphicsSurfaceAt407A80(p.canvas, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
            auto* image = reinterpret_cast<GraphicsSurfaceNativeBody*>(p.canvas.baseAt00);
            if (!image->CreateAt415010(s.surfaces, p.bounds.right-p.bounds.left, p.bounds.bottom-p.bounds.top, 1, 1)) throw 1;
            image->originXAt10 = p.bounds.left; image->originYAt14 = p.bounds.top;
            Ref layer;
            CreateGuiStaticFromSurfaceAt505570(layer.slot, &p.common, profile.textLayerEvent, Copy(p.canvas), s.surfaces);
            Append(p, layer, false);
            ::UnionRect(&p.bounds, &p.bounds, &staticBounds);
            for (std::size_t i = 0; i < 3; ++i) p.colors[i] = s.source.ReadColor(profile.colorTags[i]);
            for (const auto& nav : profile.navigation)
                CreateAndAppendGuiNavigation(p.common.navigationAt24, nav.input, nav.pairs);
            AdoptGuiInputAt536EA0(p.common.inputAt18, NewGameGuiInputAt4EAAED());
        }
        auto control = std::make_unique<PanelControl>(&p);
        control->referenceCountAt04 = 1; control->owningAt08 = 1;
        control->strideAt0C = sizeof(Panel); control->countAt10 = 1;
        out = {reinterpret_cast<std::uintptr_t>(&p), 0, control.release()};
        raw.release();
    }
    catch (...) { DestroyPanel(p); throw; }
}
void DestroyPanel(Panel& p) noexcept
{
    Leaves leaves(Services(p));
    ReleaseYtlReference(p.canvas);
    DestroyGuiClickedEventReferencesAt4F6D10(p.events);
    DestroyGuiDialogAt4F7AC0(&p.common);
}
}
namespace KaGuYa::Start::Runtime
{
std::size_t LegacyMenuSelectionDialogStorageSize() noexcept { return sizeof(LegacyMenuSelectionDialog); }
GuiDialogNativeBody5Ch* ConstructLegacyMenuSelectionDialog(void* raw, LegacyMenuSelectionServices& services)
{
    using namespace LegacyMenuSelectionDetail;
    RequireLegacyMenuSelectionProfile(services.profile);
    Leaves leaves(services);
    auto* b = ::new(raw) Top;
    b->services = &services;
    ConstructGuiDialogIntoNativeAt4E0BE0(&b->into, nullptr, -1);
    b->into.common.vptrAt00 = &TopMethods().common;
    try
    {
        for (bool system : {false, true})
        {
            Ref panel;
            NewPanel(panel.slot, *b, system);
            auto* p = reinterpret_cast<Panel*>(panel.slot.baseAt00);
            TopMethods().appendDialogAt8C(&b->into.common, Copy(panel.slot));
            (system ? b->system : b->main) = p;
            ::UnionRect(&b->bounds, &b->bounds, &p->bounds);
        }
        AdoptGuiInputAt536EA0(b->into.common.inputAt18, NewGameGuiInputAt4EAAED());
        return &b->into.common;
    }
    catch (...) { DestroyGuiDialogIntoNativeAt4F7B80(&b->into.common); throw; }
}
}
