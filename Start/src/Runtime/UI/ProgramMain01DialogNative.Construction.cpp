#include "ProgramMain01DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain01NativeDetail;
class Main01Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Main01Control(Body* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<Common*>(payloadAt14);
        if (p) Read(p->vptrAt00)->deletingDestructorAt04(p, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(Main01Control) == 0x18);
void AppendBackground(Body& b)
{
    Fresh group;
    ConstructGuiControlResourceReferenceAt4D76A0(group.slot, NewGuiControlResourceAt5047DD(), 1);
    RefLife groupLife{group.slot};
    auto* capturedGroup = Read(group.slot.controlAt08);
    Slot surfaceArgument = Copy(b.backgroundAt74);
    auto* groupReceiver = Resolve<GuiControlResourceNativeBody38h>(group.slot);
    AppendGuiControlSurfaceArgumentAt538350(*groupReceiver, 0, surfaceArgument);
    auto* surface = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
    const auto width = Read(surface->widthAt08), height = Read(surface->heightAt0C);
    surface = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    const RECT rect{x, y, Add(x, width), Add(y, height)};
    const auto capturedBase = Read(group.slot.baseAt00);
    auto* resource = ResolveCached<GuiControlResourceNativeBody38h>(group.slot, capturedBase, capturedGroup);
    ::CopyRect(&resource->combinedRectAt28, &rect);
    auto* raw = static_cast<Child*>(::operator new(0x50));
    Child* control = nullptr;
    try
    {
        if (raw)
        {
            Slot argument{capturedBase, Read(group.slot.biasAt04), capturedGroup};
            if (capturedGroup) ++capturedGroup->referenceCountAt04;
            control = ConstructGuiStaticAt4F4050(raw, &b.common, 2, argument);
        }
    }
    catch (...) { ::operator delete(raw, std::size_t{0x50}); throw; }
    Fresh child;
    ConstructGuiStaticReferenceAt4D7940(child.slot, control, 1);
    RefLife childLife{child.slot};
    auto* capturedChild = Read(child.slot.controlAt08);
    GuiDialogBaseVtable().appendChildAt10(&b.common, Copy(child.slot));
    childLife.Captured(capturedChild);
    groupLife.Captured(capturedGroup);
}
}
Body* ConstructProgramMain01DialogNativeAt4DE520(void* raw, Common* parent, std::int32_t id)
{
    LeafScope leaves;
    auto* b = ::new(raw) Body;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->common.vptrAt00 = &ProgramMain01DialogNativeVtable();
    b->managerAt5C = {}; b->reservedReferenceAt68 = {}; b->backgroundAt74 = {};
    struct Unwind final
    {
        Body& b; bool active = true;
        ~Unwind() noexcept { if (active) DestroyMembers(b); }
    } unwind{*b};
    auto& source = leaves.services.source;
    source.EnsureRootAt413BE0(0x4DE5C7);
    auto* root = source.RootAt6198C4(0x4DE5CC);
    auto* draw = Resolve(source.RootDrawSlotAt18(root));
    if (!source.LoadSurfaceAt480000(draw, &b->backgroundAt74,
        ProgramMain01DialogNativeBackgroundSample18(), 0x4DE5FA)) throw 1;
    ComposeProgramMain01DialogNativeAt5281F0(*b);
    AppendBackground(*b);
    auto* manager = static_cast<GuiClickedEventReferences*>(::operator new(0x0C));
    if (manager) ::new(manager) GuiClickedEventReferences{};
    AdoptGuiClickedEventManagerAt4D7B90(b->managerAt5C, manager);
    for (const auto& group : ProgramMain01DialogNativeGroupsSample18()) Populate(*b, group);
    for (const auto& group : ProgramMain01DialogNativeNavigationSample18()) PopulateNavigation(*b, group);
    unwind.active = false;
    return b;
}
Body* ConstructProgramMain01DialogNativeAt4DE520(void* raw, Common* parent,
    std::int32_t id, ProgramMain01DialogNativeServices& services)
{
    ProgramMain01DialogNativeScope scope(services);
    return ConstructProgramMain01DialogNativeAt4DE520(raw, parent, id);
}
Slot* ConstructProgramMain01DialogNativeReferenceAt4DF170(Slot& out, Body* body)
{
    out = {};
    if (body)
    {
        out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* raw = ::operator new(0x18);
        auto* control = raw ? ::new(raw) Main01Control(body) : nullptr;
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x88;
        out.controlAt08 = control;
        control->countAt10 = 1;
    }
    return &out;
}
namespace ProgramMain01NativeDetail
{
void DestroyMembers(Body& b) noexcept
{
    ReleaseYtlReference(b.backgroundAt74);
    ReleaseYtlReference(b.reservedReferenceAt68);
    ReleaseYtlReference(b.managerAt5C);
    DestroyGuiDialogAt4F7AC0(&b.common);
}
}
void DestroyProgramMain01DialogNative(Body& b) noexcept
{
    LeafScope leaves;
    b.common.vptrAt00 = &ProgramMain01DialogNativeVtable();
    DestroyMembers(b);
}
Body* DeleteProgramMain01DialogNativeAt4FC170(Body* b, std::uint32_t flags) noexcept
{
    DestroyProgramMain01DialogNative(*b);
    if (flags & 1u) ::operator delete(b, std::size_t{0x88});
    return b;
}
}
