#include "SaveLoadPopupNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime::SaveLoadPopupNativeDetail
{
namespace
{
class NumberControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit NumberControl(Number* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<Number*>(payloadAt14);
        if (!p) return;
        // 53DA20->4F9E80：三条拥有链按+28/+1C/+10反序退役，不碰+00/+04。
        ReleaseYtlReference(p->surfacesAt10[2]);
        ReleaseYtlReference(p->surfacesAt10[1]);
        ReleaseYtlReference(p->surfacesAt10[0]);
        ::operator delete(p, std::size_t{0x34});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(NumberControl) == 0x18);
struct Raw final
{
    void* p; std::size_t size; bool active = true;
    ~Raw() noexcept { if (active) ::operator delete(p, size); }
};
}
void NewNumberReference(Slot& out)
{
    auto* raw = ::operator new(0x34);
    auto* p = raw ? ::new(raw) Number : nullptr;
    if (p)
    {
        p->originAt08 = {};
        for (auto& surface : p->surfacesAt10) surface = {};
    }
    out = {}; // 4F0002在new34之后才清fresh输出。
    if (!p) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(p);
    auto* memory = ::operator new(0x18);
    auto* c = memory ? ::new(memory) NumberControl(p) : nullptr;
    c->referenceCountAt04 = 1;
    c->owningAt08 = 1;
    c->strideAt0C = 0x34;
    out.controlAt08 = c; // 4F0047，control失败不补删34h正文。
    c->countAt10 = 1;
}
void AppendNumberAt4D8AC0(Vector& v, const Slot& r)
{
    // 4D8AC0与4540A0是Ref12同一增长算法：1.5x、先复制插入项、再旧范围、释放后发布。
    // Payload仅经原Ref虚槽退役；Surface命名的kernel没有Surface类型转换。
    if (v.end == v.capacity) GrowGuiControlSurfaceReferencesAt4540A0(v, v.end, r);
    else { CopyGuiClickedEventReference(*v.end, r); ++v.end; }
}
void DestroyNumbersAt4F7080(Vector& v) noexcept
{ DestroyGuiControlSurfaceReferencesAt43EAA0(v); }
void CreateCheckBoxAt5043B0(Slot& out, Common* parent, const CheckDescriptor& d, SaveLoadPopupNativeServices& s)
{
    struct Result final
    {
        Slot& r; bool active = false;
        ~Result() noexcept { if (active) ReleaseYtlReference(r); }
    } result{out}; // state0只在4D7710完整成功后有资格。
    Fresh normal;
    LoadGuiControlResourceAt5047A0(normal.r, d.normal.data(), s.loader, s.surfaces, s.strings);
    Local normalLife{normal.r};
    if (!Read(normal.r.baseAt00)) throw 1;
    Fresh checked;
    LoadGuiControlResourceAt5047A0(checked.r, d.checked.data(), s.loader, s.surfaces, s.strings);
    Local checkedLife{checked.r};
    if (!Read(checked.r.baseAt00)) throw 1;
    auto* normalControl = normal.r.controlAt08;
    auto* checkedControl = checked.r.controlAt08;
    if (auto* point = Read(d.position))
    {
        MoveGuiControlResourceAt5086C0(Copy(normal.r), Read(point->x), Read(point->y));
        point = Read(d.position);
        MoveGuiControlResourceAt5086C0(Copy(checked.r), Read(point->x), Read(point->y));
    }
    auto* raw = ::operator new(0x60);
    Raw rawLife{raw, 0x60};
    GuiCheckBoxNativeBody60h* body = nullptr;
    if (raw)
    {
        Slot ownedChecked = Copy(checked.r); // 50449E先右参数，再左参数。
        Slot ownedNormal = Copy(normal.r);
        body = ConstructGuiCheckBoxAt4E0520(static_cast<GuiCheckBoxNativeBody60h*>(raw),
            parent, Read(d.event), ownedNormal, ownedChecked);
    }
    rawLife.active = false;
    ConstructGuiCheckBoxReferenceAt4D7710(out, body, 1);
    result.active = true;
    Resolve<GuiControlNativeBody50h>(out)->childIdAt0C = Read(d.childId);
    auto* child = Resolve<GuiControlNativeBody50h>(out);
    reinterpret_cast<const GuiCheckBoxNativeVtable30*>(child->vptrAt00)->setHitSlotAt60(child, Read(d.hitSlot));
    checkedLife.Captured(checkedControl);
    normalLife.Captured(normalControl);
    result.active = false;
}
void SelectSurfaceAt50B1F0(Slot& out, GuiControlResourceNativeBody38h* group, std::int32_t index)
{
    out = {};
    Local result{out};
    if (index < 7)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(group) + static_cast<std::uint32_t>(index) * 4u;
        const auto mapped = *reinterpret_cast<const volatile std::int32_t*>(address);
        if (mapped >= 0) CopyAssignGraphicsSurfaceReferenceAt465870(out, Read(group->surfacesAt1C.begin)[mapped]);
    }
    result.active = false;
}
void BindEventAt536DB0(GuiControlNativeBody50h* child, Slot owned)
{
    Local argument{owned};
    auto& target = child->eventAt3C;
    if (&target != &owned)
    {
        ReleaseCurrent(target);
        target.baseAt00 = Read(owned.baseAt00);
        target.biasAt04 = Read(owned.biasAt04);
        target.controlAt08 = Read(owned.controlAt08);
        if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
    }
    child->eventEnabledAt48 = 1;
    argument.Current();
}
void AppendEventAt4FEF80(GuiClickedEventReferences* manager, Slot owned)
{
    Local argument{owned};
    AppendGuiClickedEventReference(*manager, owned);
    argument.Current();
}
std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences* manager)
{
    auto* at = Read(manager->begin);
    auto* const end = Read(manager->end);
    for (; at != end; ++at)
    {
        const auto base = Read(at->baseAt00);
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->eventValueAt08);
        auto* selected = ResolveWithBase<GuiClickedEventNativeBody>(*at, base);
        selected->vptrAt00->clearAt04(selected);
        return value;
    }
    return -1;
}
void Blit(GraphicsSurfaceNativeBody* target, GraphicsSurfaceNativeBody* source, POINT position)
{
    // 4425C0先目标handle后源handle，零handle不Capture图形模块。
    const auto destination = Read(target->handleAt04);
    if (!destination) return;
    const auto input = Read(source->handleAt04);
    if (!input) return;
    auto& api = RequireSaveLoadPopupNativeServices().graphics.CompositionAt4182F0();
    static_cast<void>(api.BlitSurface(destination, input, position.x, position.y, nullptr, nullptr, nullptr, 0));
}
void Clear(GraphicsSurfaceNativeBody* surface)
{
    const auto handle = Read(surface->handleAt04);
    if (!handle) return;
    const auto color = Read(surface->clearColorAt20);
    auto& api = RequireSaveLoadPopupNativeServices().graphics.CompositionAt4182F0();
    api.ClearSurface(handle, color, nullptr);
}
SaveFileStateNativeArchive* Save(EnvironmentRootStorage* environment) noexcept
{
    auto* begin = Read(environment->archivesAt30.begin);
    const auto count = Count(environment->archivesAt30);
    return static_cast<std::uint32_t>(count) > 6u ? Resolve<SaveFileStateNativeArchive>(begin[6]) : nullptr;
}
}
