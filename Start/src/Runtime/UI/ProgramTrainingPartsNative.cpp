#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
class PartsControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PartsControl(Parts* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if (auto* p = static_cast<Parts*>(payloadAt14))
        {
            DestroyProgramTrainingPartsNativeAt4F9D40(*p);
            ::operator delete(p, std::size_t{0x74});
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(PartsControl) == 0x18);
// 4F5FC9：仅构造typed18的vptr/payload，计数和发布时序由原caller控制。
class SurfaceControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SurfaceControl(GraphicsSurfaceNativeBody* p) noexcept
        : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if (auto* p = static_cast<GraphicsSurfaceNativeBody*>(payloadAt14))
            Read(p->vptrAt00)->deletingDestructor(p, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SurfaceControl) == 0x18);
struct PartsFrame final
{
    Parts& p;
    bool active = true;
    ~PartsFrame() noexcept { if (active) DestroyProgramTrainingPartsNativeAt4F9D40(p); }
};
void UnionSurface(RECT& bounds, const Slot& slot)
{ const auto rectangle = SurfaceRect(slot); ::UnionRect(&bounds, &bounds, &rectangle); }
}
void DestroyProgramTrainingPartsNativeAt4F9D40(ProgramTrainingPartsNativeBody74h& p) noexcept
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    ReleaseReload(p.levelAt68, true);
    ReleaseReload(p.decreaseAt5C, true);
    ReleaseReload(p.increaseAt50, true);
    ReleaseReload(p.effectAt44, true);
    DestroyGuiControlSurfaceReferencesAt43EAA0(p.symbolsAt28);
    DestroyGuiControlSurfaceReferencesAt43EAA0(p.stressAt0C);
    DestroyGuiControlSurfaceReferencesAt43EAA0(p.canvasesAt00);
}
void AdoptProgramTrainingPartsNativeAt4DD0E0(YtlReferenceSlotRuntime& output,
    ProgramTrainingPartsNativeBody74h* body)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    ReleaseReload(output, true);
    if (!body) return;
    output.biasAt04 = 0;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    output.controlAt08 = raw ? ::new(raw) PartsControl(body) : nullptr;
    output.controlAt08->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x74;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
}
ProgramTrainingPartsNativeBody74h* ConstructProgramTrainingPartsNativeAt4F55A0(void* raw74)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    auto* p = ::new(raw74) Parts;
    p->canvasesAt00 = {}; p->stressAt0C = {}; p->stressBoundsAt18 = {};
    p->symbolsAt28 = {}; p->numberBoundsAt34 = {};
    ::new(&p->effectAt44) Slot;
    ::new(&p->increaseAt50) Slot;
    ::new(&p->decreaseAt5C) Slot;
    ::new(&p->levelAt68) Slot;
    PartsFrame members{*p};
    for (std::uint32_t i = 0; i != 20; ++i)
    {
        Slot temporary;
        RefFrame frame{temporary};
        LoadFresh(temporary, Sample18::resources[i]);
        auto* control = Read(temporary.controlAt08);
        auto& destination = i < 4 ? p->stressAt0C : p->symbolsAt28;
        // 4D83D0与4540A0同一Ref复制/3:2增长/旧范围清理协议，复用已有原生叶。
        if (Read(destination.end) == Read(destination.capacity))
        {
            GrowGuiControlSurfaceReferencesAt4540A0(destination, Read(destination.end), temporary);
            control = Read(temporary.controlAt08);
        }
        else AppendGuiControlSurfaceReference(destination, temporary);
        frame.Captured(control);
    }
    Slot* slots[]{&p->effectAt44, &p->increaseAt50, &p->decreaseAt5C, &p->levelAt68};
    for (std::uint32_t i = 0; i != 4; ++i)
    {
        const auto ensure = 0x4F5932u + i * 0x48u;
        auto* root = Root(ensure, ensure + 5);
        auto& source = leaves.services.shared.source;
        auto* draw = Resolve<void>(source.RootDrawSlotAt18(root));
        if (!source.LoadSurfaceAt480000(draw, slots[i], Sample18::markers[i], 0x4F5968u + i * 0x48u)) throw 1;
    }
    RECT bounds{};
    for (auto* at = Read(p->stressAt0C.begin); at != Read(p->stressAt0C.end); ++at)
        UnionSurface(p->stressBoundsAt18, *at);
    ::UnionRect(&bounds, &bounds, &p->stressBoundsAt18);
    for (std::int32_t i = 0; i != 3; ++i) UnionSurface(bounds, Read(p->symbolsAt28.begin)[i]);
    for (std::int32_t i = 3; i != 5; ++i) UnionSurface(bounds, Read(p->symbolsAt28.begin)[i]);
    RECT digitBounds{};
    for (std::int32_t i = 5; i != 15; ++i) UnionSurface(digitBounds, Read(p->symbolsAt28.begin)[i]);
    const auto width = Sub(digitBounds.right, digitBounds.left);
    const auto height = Sub(digitBounds.bottom, digitBounds.top);
    for (const auto& position : Sample18::numberPoints)
    {
        const RECT rectangle{position.x, position.y, Add(position.x, width), Add(position.y, height)};
        ::UnionRect(&p->numberBoundsAt34, &p->numberBoundsAt34, &rectangle);
    }
    UnionSurface(p->numberBoundsAt34, Read(p->symbolsAt28.begin)[15]);
    ::UnionRect(&bounds, &bounds, &p->numberBoundsAt34);
    for (auto* slot : slots) UnionSurface(bounds, *slot);
    for (std::uint32_t i = 0; i != 3; ++i)
    {
        auto* surface = NewGraphicsSurfaceAt40C2E0(leaves.services.shared.surfaces, leaves.services.shared.strings);
        Slot temporary;
        // 4F5F9A..5FFD是fresh Ref内联构造，不执行407A80的旧槽释放。
        if (surface)
        {
            temporary.baseAt00 = reinterpret_cast<std::uintptr_t>(surface);
            void* raw = ::operator new(0x18);
            auto* control = raw ? ::new(raw) SurfaceControl(surface) : nullptr;
            control->referenceCountAt04 = 1;
            control->owningAt08 = 1;
            control->strideAt0C = 0x24;
            temporary.controlAt08 = control;
            control->countAt10 = 1;
        }
        RefFrame frame{temporary};
        auto* captured = Read(temporary.controlAt08);
        const auto canvasWidth = Sub(bounds.right, bounds.left), canvasHeight = Sub(bounds.bottom, bounds.top);
        // 4F6015..60E1不是415010：成功先width/height，最后alpha；保留此发布序。
        auto* selected = Resolve<GraphicsSurfaceNativeBody>(temporary);
        selected->ResetAt432670(leaves.services.shared.surfaces);
        selected->handleAt04 = leaves.services.shared.surfaces.CreateSurface(canvasWidth, canvasHeight, 1, 1);
        if (!Read(selected->handleAt04)) throw 1;
        selected->widthAt08 = canvasWidth;
        selected->heightAt0C = canvasHeight;
        selected->alphaAt18 = 1;
        surface->originXAt10 = bounds.left;
        surface->originYAt14 = bounds.top;
        if (Read(p->canvasesAt00.end) == Read(p->canvasesAt00.capacity))
        {
            GrowGuiControlSurfaceReferencesAt4540A0(p->canvasesAt00, Read(p->canvasesAt00.end), temporary);
            captured = Read(temporary.controlAt08);
        }
        else AppendGuiControlSurfaceReference(p->canvasesAt00, temporary);
        frame.Captured(captured);
    }
    members.active = false;
    return p;
}
}
