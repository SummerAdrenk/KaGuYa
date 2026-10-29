#include "GraffitiCountDialogNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiCountDialogNativeDetail;
namespace
{
struct CountUnwind final
{
    Common& body;
    bool armed = true;
    ~CountUnwind() noexcept
    {
        if (!armed) return;
        // 5FCFD4/5FD10C：state2 -> 1 -> 0，展开不是完整派生析构。
        DestroyGuiControlSurfaceReferencesAt43EAA0(body.digitsAt84);
        DestroyGuiControlSurfaceReferencesAt43EAA0(body.backgroundsAt78);
        DestroyGraffitiCountDialogBaseAt4F7A40(body);
    }
};
class CanvasControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit CanvasControl(GraphicsSurfaceNativeBody* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GraphicsSurfaceNativeBody*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructor(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(CanvasControl) == 0x18);
void ResetCanvasAt4FE910(Slot& slot)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    ReleaseYtlReference(slot);
    auto* created = NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings);
    // 4FE98E：分配/构造回调后的第二次检查只删除当前control。
    if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) CanvasControl(created) : nullptr;
    slot.controlAt08 = control;
    // 原null control写故障不改为伪成功；24h正文已发布，无额外回滚资格。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x24;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
RECT SurfaceRectangle(const Slot& slot) noexcept
{
    auto* surface = Resolve<GraphicsSurfaceNativeBody>(slot);
    const auto width = Read(surface->widthAt08), height = Read(surface->heightAt0C);
    surface = Resolve<GraphicsSurfaceNativeBody>(slot);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    return {x, y, Add(x, width), Add(y, height)};
}
void LoadSequence(Common& body, bool weight, bool digits, RECT& combined)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    const auto count = digits ? (weight ? 11u : 10u) : (weight ? 2u : 3u);
    for (std::uint32_t i = 0; i != count; ++i)
    {
        Slot temporary{};
        RefUnwind lifetime{temporary}; // state3/6，Capture和24h构造也在展开范围内。
        void* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        const char* name = digits ? DigitName(i) : BackgroundName(weight, i);
        AdoptGraphicsSurfaceAt407A80(temporary, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
        auto* cachedControl = Read(temporary.controlAt08);
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<GraphicsSurfaceNativeBody>(temporary), name)) throw 1;
        const Slot selected{Read(temporary.baseAt00), Read(temporary.biasAt04), cachedControl};
        if (!digits)
        {
            const auto rectangle = SurfaceRectangle(selected);
            ::UnionRect(&combined, &combined, &rectangle);
        }
        auto& vector = digits ? body.digitsAt84 : body.backgroundsAt78;
        const bool slow = Read(vector.end) == Read(vector.capacity);
        // 4D83D0与4540A0均经43EA10释放旧Ref，3/2增长和32-byte大块对齐相同。
        if (slow) static_cast<void>(GrowGuiControlSurfaceReferencesAt4540A0(vector, Read(vector.end), temporary));
        else AppendGuiControlSurfaceReference(vector, selected);
        if (slow) cachedControl = Read(temporary.controlAt08);
        lifetime.Finish(cachedControl); // state5/8，缓存control跨正常尾。
        if (cachedControl) { temporary.controlAt08 = nullptr; temporary.baseAt00 = 0; }
    }
}
struct RawStaticUnwind final
{
    void* raw;
    bool armed = true;
    ~RawStaticUnwind() noexcept { if (armed) ::operator delete(raw, std::size_t{0x50}); }
};
void AddTrainingWidget(Common& body, RECT& combined)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    Slot held{};
    RefUnwind heldLifetime{held}; // state15 bit0对应var60。
    Slot returned;
    CreateGuiStaticAt505800(returned, &body.dialog, WeightTrainingDescriptor(), s.loader, s.surfaces, s.strings);
    RefUnwind returnedLifetime{returned}; // state16，仅factory成功后才获得资格。
    held = Copy(returned);
    auto* cachedControl = Read(held.controlAt08);
    returnedLifetime.armed = false;
    FinishCurrentWithoutClearing(returned); // state17，slot重读Delete。
    auto* child = Resolve<GuiControlNativeBody50h>(held);
    RECT output;
    const auto* rectangle = Read(child->vptrAt00)->rectAt3C(child, &output);
    ::UnionRect(&combined, &combined, rectangle);
    // 4F5260直接4FF020，不借当前append虚槽。
    GuiDialogBaseVtable().appendChildAt10(&body.dialog, Copy(held));
    heldLifetime.Finish(cachedControl); // state32。
}
}
namespace GraffitiCountDialogNativeDetail
{
void ConstructCountPrefix(Common& body, bool weight)
{
    ConstructGuiDialogAt4E0BE0(&body.dialog, nullptr, -1);
    ::new(static_cast<void*>(&body.canvasAt6C)) Slot{};
    body.dialog.vptrAt00 = weight ? &GraffitiWeightCountDialogVtable() : &GraffitiLikableCountDialogVtable();
    body.backgroundsAt78 = {};
    body.digitsAt84 = {};
}
void ConstructCountWidgets(Common& body, RECT& combined)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    ResetCanvasAt4FE910(body.canvasAt6C);
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt6C);
    if (!canvas->CreateAt415010(s.surfaces, Sub(combined.right, combined.left),
        Sub(combined.bottom, combined.top), 1, 1)) throw 1;
    canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt6C);
    canvas->originXAt10 = combined.left;
    canvas->originYAt14 = combined.top;
    Slot group;
    ConstructGuiControlResourceReferenceAt4D76A0(group, NewGuiControlResourceAt5047DD(), 1);
    RefUnwind groupLifetime{group}; // state10，control失败不回收38h正文。
    const auto groupBase = Read(group.baseAt00);
    auto* cachedGroupControl = Read(group.controlAt08);
    const Slot selectedGroup{groupBase, Read(group.biasAt04), cachedGroupControl};
    auto argument = Copy(body.canvasAt6C);
    RefUnwind argumentLifetime{argument};
    auto* resource = Resolve<GuiControlResourceNativeBody38h>(selectedGroup);
    const auto index = GuiControlSurfaceReferenceCount(resource->surfacesAt1C);
    AppendGuiControlSurfaceReference(resource->surfacesAt1C, argument);
    resource->slotIndicesAt00[0] = index;
    argumentLifetime.armed = false;
    FinishCurrentWithoutClearing(argument); // 538350按值参数。
    const auto rectangle = SurfaceRectangle(body.canvasAt6C);
    ::CopyRect(&Resolve<GuiControlResourceNativeBody38h>(selectedGroup)->combinedRectAt28, &rectangle);
    void* raw = ::operator new(0x50);
    RawStaticUnwind rawLifetime{raw}; // state11。
    GuiControlNativeBody50h* child = nullptr;
    if (raw)
    {
        const Slot currentBias{groupBase, Read(group.biasAt04), cachedGroupControl};
        child = ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(raw), &body.dialog, 0, Copy(currentBias));
    }
    rawLifetime.armed = false;
    Slot widget;
    ConstructGuiStaticReferenceAt4D7940(widget, child, 1);
    RefUnwind widgetLifetime{widget}; // state12，未成功发布control时不补删50h。
    auto* cachedWidgetControl = Read(widget.controlAt08);
    GuiDialogBaseVtable().appendChildAt10(&body.dialog, Copy(widget));
    widgetLifetime.Finish(cachedWidgetControl); // state13。
    groupLifetime.Finish(cachedGroupControl); // state14。
}
}
Likable* ConstructGraffitiLikableAt4E13A0(Likable* body)
{
    ::new(static_cast<void*>(&body->common)) Common;
    ConstructCountPrefix(body->common, false);
    CountUnwind lifetime{body->common};
    RECT combined{};
    LoadSequence(body->common, false, false, combined);
    LoadSequence(body->common, false, true, combined);
    ConstructCountWidgets(body->common, combined);
    body->categoryAt90 = ReadNamed("prog_param01") != 0 ? 2 : 1;
    body->initialAt94 = ReadNamed("prog_param02");
    const auto addition = ReadNamed("prog_param03");
    body->targetAt98 = Add(Read(body->initialAt94), addition);
    if (Read(body->targetAt98) >= 100) body->targetAt98 = 100;
    body->common.rectangleAt5C = combined;
    AdoptGuiInputAt536EA0(body->common.dialog.inputAt18, NewGameGuiInputAt4EAAED());
    lifetime.armed = false;
    return body;
}
Weight* ConstructGraffitiWeightAt4F4BC0(Weight* body)
{
    ::new(static_cast<void*>(&body->common)) Common;
    ConstructCountPrefix(body->common, true);
    CountUnwind lifetime{body->common};
    RECT combined{};
    LoadSequence(body->common, true, false, combined);
    LoadSequence(body->common, true, true, combined);
    ConstructCountWidgets(body->common, combined);
    AddTrainingWidget(body->common, combined);
    body->initialAt90 = ReadNamed("prog_param01");
    const auto reduction = ReadNamed("prog_param02");
    body->targetAt94 = Sub(Read(body->initialAt90), reduction);
    if (Read(body->targetAt94) < 0) body->targetAt94 = 0;
    body->trainingAt9C = ReadNamed("prog_param03") != 0;
    body->common.rectangleAt5C = combined;
    AdoptGuiInputAt536EA0(body->common.dialog.inputAt18, NewGameGuiInputAt4EAAED());
    lifetime.armed = false;
    return body;
}
}
