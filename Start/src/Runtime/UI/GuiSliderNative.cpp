#include <Runtime/GuiSliderNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <emmintrin.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiSliderNativeBodyD8h;
using Common = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
using Group = GuiControlResourceNativeBody38h;

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
Body& Slider(Common* body) noexcept { return *reinterpret_cast<Body*>(body); }
const GuiSliderNativeVtable31& Table(Common* body) noexcept
{ return *reinterpret_cast<const GuiSliderNativeVtable31*>(body->vptrAt00); }
GuiDialogNativeBody5Ch* Parent(Body& body) noexcept
{ return static_cast<GuiDialogNativeBody5Ch*>(body.common.parentAt4C); }

// 5317E6/531867等正常尾固定寄存器control，异常直接终止，不再次展开同一Ref。
void FinishCaptured(Control* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
void FinishCurrent(Slot& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
struct Temporary final
{
    Slot& slot;
    bool active = true;
    ~Temporary() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(Control* control) noexcept { active = false; FinishCaptured(control); }
    void Current() noexcept { active = false; FinishCurrent(slot); }
    void Clear() noexcept { active = false; ReleaseYtlReference(slot); }
};

GraphicsSurfaceNativeBody* Surface(std::uintptr_t base, std::uint32_t bias, Control* control) noexcept
{
    Slot view{base, bias, control};
    return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(view);
}
GraphicsSurfaceNativeBody* Surface(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(slot); }

// 50B1F0只有signed slot<7和映射非负检查；负slot及坏正文不加现代安全兜底。
std::int32_t ResourceIndex(Group* group, std::int32_t slot) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(group) + static_cast<std::uint32_t>(slot) * 4u;
    return *reinterpret_cast<const std::int32_t*>(address);
}
void SelectSurface(Slot& output, Group* group, std::int32_t slot)
{
    output = {};
    Temporary cleanup{output};
    if (slot < 7)
    {
        const auto index = ResourceIndex(group, slot);
        if (index >= 0) CopyAssignGraphicsSurfaceReferenceAt465870(output, group->surfacesAt1C.begin[index]);
    }
    cleanup.active = false;
}

// 503520/5035E0/50A260只在读取RECT后启用terminate尾态，不增加可抛业务回调。
RECT GroupRect(Body& body, unsigned index)
{
    Slot selected;
    CopyGuiControlResourceReference(selected, body.common.resourcesAt28.begin[index]);
    auto* captured = selected.controlAt08;
    const RECT rectangle = ResolveGuiControlResource(selected)->combinedRectAt28;
    FinishCaptured(captured);
    return rectangle;
}
RECT Reanchor(RECT rectangle, LONG x, LONG y) noexcept
{
    rectangle.right = Add(rectangle.right, Sub(x, rectangle.left));
    rectangle.bottom = Add(rectangle.bottom, Sub(y, rectangle.top));
    rectangle.left = x;
    rectangle.top = y;
    return rectangle;
}
RECT Travel(Body& body)
{
    auto rectangle = Reanchor(body.travelRectAt7C, body.common.xAt34, body.common.yAt38);
    ::OffsetRect(&rectangle, body.travelOffsetAt74.x, body.travelOffsetAt74.y);
    return rectangle;
}
RECT Interaction(Body& body)
{
    auto rectangle = Reanchor(body.interactionRectAt64, body.common.xAt34, body.common.yAt38);
    ::OffsetRect(&rectangle, body.interactionOffsetAt5C.x, body.interactionOffsetAt5C.y);
    return rectangle;
}
RECT* ThumbRectAt509C40(Body& body, RECT* output)
{
    // 509C99先写调用者输出，再消费临时Ref；回调可观察这一时点。
    Slot selected;
    CopyGuiControlResourceReference(selected, body.common.resourcesAt28.begin[0]);
    auto* captured = selected.controlAt08;
    *output = ResolveGuiControlResource(selected)->combinedRectAt28;
    FinishCaptured(captured);
    *output = Reanchor(*output, body.thumbPositionAt98.x, body.thumbPositionAt98.y);
    return output;
}
POINT TrackPositionAt503520(Body& body, LONG x, LONG y)
{
    const auto rectangle = GroupRect(body, 1);
    return {Add(Sub(Add(body.trackOffsetAtC4.x, body.common.xAt34), rectangle.left), x),
        Add(Sub(Add(body.trackOffsetAtC4.y, body.common.yAt38), rectangle.top), y)};
}
POINT ThumbPositionAt5035E0(Body& body, LONG x, LONG y)
{
    const auto rectangle = GroupRect(body, 0);
    return {Add(Sub(body.thumbPositionAt98.x, rectangle.left), x),
        Add(Sub(body.thumbPositionAt98.y, rectangle.top), y)};
}
RECT* __fastcall RectAt50A260(Common* common, void*, RECT* output)
{
    auto& body = Slider(common);
    auto travel = Travel(body);
    const auto thumb = GroupRect(body, 0);
    if (!body.orientationAt50) travel.right = Add(travel.right, Sub(thumb.right, thumb.left));
    else travel.bottom = Add(travel.bottom, Sub(thumb.bottom, thumb.top));
    auto track = GroupRect(body, 1);
    track = Reanchor(track, Add(body.trackOffsetAtC4.x, common->xAt34),
        Add(body.trackOffsetAtC4.y, common->yAt38));
    RECT current;
    ThumbRectAt509C40(body, &current);
    RECT combined{};
    ::UnionRect(&combined, &track, &current);
    RECT result{};
    ::UnionRect(&result, &combined, &travel);
    *output = result;
    return output;
}

__m128 Float(std::int32_t value) noexcept { return _mm_cvtepi32_ps(_mm_cvtsi32_si128(value)); }
void PositionFromValue(Body& body, std::int32_t value)
{
    const auto rectangle = Travel(body);
    const auto extent = body.orientationAt50 ? Sub(rectangle.bottom, rectangle.top) : Sub(rectangle.right, rectangle.left);
    const auto relative = Sub(value, body.minimumAtB8);
    const auto range = Sub(body.maximumAtBC, body.minimumAtB8);
    const auto offset = _mm_cvttss_si32(_mm_div_ss(_mm_mul_ss(Float(relative), Float(extent)), Float(range)));
    body.thumbPositionAt98 = body.orientationAt50
        ? POINT{rectangle.left, Add(rectangle.top, offset)}
        : POINT{Add(rectangle.left, offset), rectangle.top};
}
std::int32_t CoordinateAt53AE10(Body& body, std::int32_t coordinate)
{
    const auto range = Sub(body.maximumAtBC, body.minimumAtB8);
    const auto rectangle = Travel(body);
    const auto extent = body.orientationAt50 ? Sub(rectangle.bottom, rectangle.top) : Sub(rectangle.right, rectangle.left);
    const auto relative = Sub(coordinate, body.orientationAt50 ? rectangle.top : rectangle.left);
    const auto scale = _mm_div_ss(Float(range), Float(extent));
    const auto base = _mm_add_ss(Float(body.minimumAtB8), _mm_set_ss(0.5F));
    const auto result = _mm_cvttss_si32(_mm_add_ss(_mm_mul_ss(Float(relative), scale), base));
    return body.reverseAt54 ? Add(Sub(body.minimumAtB8, result), body.maximumAtBC) : result;
}
std::int32_t Remainder(std::int32_t value, std::int32_t divisor)
{
    // 53853A的cdq/idiv：保留零除及INT_MIN/-1硬件异常，不引入C++有符号除法UB。
    std::int32_t result;
    __asm
    {
        mov eax, value
        cdq
        idiv divisor
        mov result, edx
    }
    return result;
}
void __fastcall SetValueAt538520(Common* common, void*, std::int32_t value)
{
    auto& body = Slider(common);
    value = Sub(value, Remainder(value, body.stepAtC0));
    if (value < body.minimumAtB8) value = body.minimumAtB8;
    if (value > body.maximumAtBC) value = body.maximumAtBC;
    body.valueAtB4 = value;
    PositionFromValue(body, body.reverseAt54 ? Add(body.minimumAtB8, Sub(body.maximumAtBC, value)) : value);
}
void __fastcall SetRangeAt537680(Common* body, void*, std::int32_t minimum, std::int32_t maximum)
{
    const auto previous = Slider(body).valueAtB4;
    Slider(body).minimumAtB8 = minimum;
    Slider(body).maximumAtBC = maximum;
    Table(body).setValueAt64(body, previous);
}
void __fastcall SetStepAt5384F0(Common* common, void*, std::int32_t step)
{
    auto& body = Slider(common);
    const auto range = Sub(body.maximumAtBC, body.minimumAtB8);
    body.stepAtC0 = step > range ? range : step;
}
void __fastcall SetHitSlotAt5361E0(Common* common, void*, std::int32_t index)
{
    Slot group;
    CopyGuiControlResourceReference(group, common->resourcesAt28.begin[0]);
    Temporary groupLifetime{group};
    auto* capturedGroup = group.controlAt08;
    auto* selected = ResolveGuiControlResource(group);
    Slot surface;
    surface = {};
    Temporary surfaceLifetime{surface};
    if (index < 7)
    {
        const auto mapped = ResourceIndex(selected, index);
        if (mapped >= 0) CopyAssignGraphicsSurfaceReferenceAt465870(surface, selected->surfacesAt1C.begin[mapped]);
    }
    auto* capturedSurface = surface.controlAt08;
    groupLifetime.Captured(capturedGroup);
    Slider(common).hitSlotAt94 = surface.baseAt00 ? index : 7;
    surfaceLifetime.Captured(capturedSurface);
}
std::int32_t HitThumbAt502CE0(Body& body, POINT point)
{
    RECT rectangle;
    ThumbRectAt509C40(body, &rectangle);
    if (!::PtInRect(&rectangle, point)) return 0;
    if (body.hitSlotAt94 == 7) return 1;
    Slot group;
    CopyGuiControlResourceReference(group, body.common.resourcesAt28.begin[0]);
    Temporary groupLifetime{group};
    auto* captured = group.controlAt08;
    Slot source;
    SelectSurface(source, ResolveGuiControlResource(group), body.hitSlotAt94);
    Temporary sourceLifetime{source};
    groupLifetime.Captured(captured);
    auto* selected = Surface(source);
    if (selected->alphaAt18)
    {
        const auto origin = ThumbPositionAt5035E0(body, selected->originXAt10, selected->originYAt14);
        const auto x = Sub(point.x, origin.x);
        const auto y = Sub(point.y, origin.y);
        const auto handle = Surface(source)->handleAt04;
        auto& api = RequireGuiControlGraphicsSource().PixelsAt4182F0();
        if (!(api.GetSurfacePixel(handle, x, y) & 0xFF000000u))
        {
            sourceLifetime.Clear(); // 502E4B显式40D2F0。
            return 0;
        }
    }
    sourceLifetime.Current();
    return 1;
}

void RenderPart(Body& body, bool thumb, Slot* target, const RECT* clip)
{
    Slot group;
    CopyGuiControlResourceReference(group, body.common.resourcesAt28.begin[thumb ? 0 : 1]);
    Temporary groupLifetime{group};
    auto* capturedGroup = group.controlAt08;
    // 531787先解析track组，5318F5在+38回调后才解析thumb组。
    auto* selected = thumb ? nullptr : ResolveGuiControlResource(group);
    auto index = body.common.vptrAt00->resourceStateAt38(&body.common);
    if (thumb)
    {
        if (index != 2 && index != 1)
            index = body.draggingAt8C ? 5 : body.hoveredAt90 ? 4 : index == 3 ? 3 : 0;
        selected = ResolveGuiControlResource(group);
    }
    Slot source;
    source = {};
    Temporary sourceLifetime{source};
    if (thumb || index < 7)
    {
        const auto mapped = ResourceIndex(selected, index);
        if (mapped >= 0) CopyAssignGraphicsSurfaceReferenceAt465870(source, selected->surfacesAt1C.begin[mapped]);
    }
    auto* capturedSurface = source.controlAt08;
    auto capturedBase = source.baseAt00;
    groupLifetime.Captured(capturedGroup);
    if (!thumb) capturedBase = source.baseAt00;
    if (capturedBase)
    {
        const auto bias = source.biasAt04;
        auto* surface = Surface(capturedBase, bias, capturedSurface);
        const auto x = surface->originXAt10;
        const auto y = surface->originYAt14;
        const auto position = thumb ? ThumbPositionAt5035E0(body, x, y) : TrackPositionAt503520(body, x, y);
        surface = Surface(capturedBase, thumb ? bias : source.biasAt04, capturedSurface);
        if (target)
        {
            const auto destination = Surface(*target)->handleAt04;
            if (destination)
            {
                const auto sourceHandle = surface->handleAt04;
                if (sourceHandle)
                {
                    auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
                    static_cast<void>(api.BlitSurface(destination, sourceHandle, position.x, position.y,
                        nullptr, nullptr, clip, 0));
                }
            }
        }
        else
        {
            const auto handle = surface->handleAt04;
            if (handle)
            {
                auto& api = RequireGuiControlGraphicsSource().CompositionAt4182F0();
                static_cast<void>(api.DrawSurface(handle, position.x, position.y, nullptr, nullptr, clip, 0));
            }
        }
    }
    sourceLifetime.Captured(capturedSurface);
}
void __fastcall DrawAt531730(Common* body, void*, const RECT* clip)
{
    RenderPart(Slider(body), false, nullptr, clip);
    RenderPart(Slider(body), true, nullptr, clip);
}
void __fastcall CompositeAt531A00(Common* body, void*, Slot target, const RECT* clip)
{
    Temporary parameter{target};
    RenderPart(Slider(body), false, &target, clip);
    RenderPart(Slider(body), true, &target, clip);
    parameter.Current();
}
std::int32_t __fastcall InputAt521820(Common* common, void*)
{
    auto& body = Slider(common);
    if (!common->vptrAt00->enabledAt1C(common)) return 0;
    auto* parent = Parent(body);
    body.changedAtB0 = 0;
    POINT output;
    const auto* pointer = parent->vptrAt00->pointerAt74(parent, &output);
    const auto x = pointer->x;
    const auto y = pointer->y;
    if (body.draggingAt8C)
    {
        parent = Parent(body);
        if (parent->vptrAt00->mouseReleaseAt84(parent)) { body.draggingAt8C = 0; return 0; }
        const auto coordinate = body.orientationAt50 ? Sub(y, body.dragOffsetAtD0.y) : Sub(x, body.dragOffsetAtD0.x);
        const auto* table = &Table(common);
        const auto value = CoordinateAt53AE10(body, coordinate);
        table->setValueAt64(common, value);
        body.changedAtB0 = 1;
        return 0;
    }
    common->vptrAt00->leaveAt30(common);
    bool focusCandidate = HitThumbAt502CE0(body, {x, y}) != 0;
    if (focusCandidate) common->vptrAt00->enterAt2C(common);
    else
    {
        const auto rectangle = Interaction(body);
        focusCandidate = ::PtInRect(&rectangle, {x, y}) != 0;
        if (focusCandidate) common->hoverAt1C = 1;
    }
    // 521939由命中分支到达；+2C回调改写hover不能取消原来的focus调用。
    if (focusCandidate && !common->focusAt20)
    {
        parent = Parent(body);
        parent->vptrAt00->setActiveAt30(parent, common);
    }
    if (!common->hoverAt1C && !body.hoveredAt90) { body.pressedAtCC = 0; return 0; }
    parent = Parent(body);
    if (body.pressedAtCC)
    {
        if (parent->vptrAt00->mouseReleaseAt84(parent)) body.pressedAtCC = 0;
        return 0;
    }
    if (parent->vptrAt00->mousePressAt80(parent))
    {
        if (body.hoveredAt90)
        {
            RECT rectangle;
            ThumbRectAt509C40(body, &rectangle);
            body.dragOffsetAtD0.y = Sub(y, rectangle.top);
            body.dragOffsetAtD0.x = Sub(x, rectangle.left);
            body.draggingAt8C = 1;
            return 0;
        }
        const auto coordinate = body.orientationAt50 ? y : x;
        const auto* table = &Table(common);
        const auto value = CoordinateAt53AE10(body, coordinate);
        table->setValueAt64(common, value);
        body.pressedAtCC = 1;
        return 0;
    }
    if (!body.stepInputAt58) return 0;
    parent = Parent(body);
    const auto direction = parent->vptrAt00->wheelAt78(parent);
    if (!direction) return 0;
    const auto value = direction > 0 ? Sub(body.valueAtB4, body.stepAtC0) : Add(body.stepAtC0, body.valueAtB4);
    Table(common).setValueAt64(common, value);
    body.changedAtB0 = 1;
    return 0;
}

// 5BE170的标量槽保持原直接写字段，不转调现代控件接口。
Common* __fastcall Delete(Common* body, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteGuiSliderAt4FE4B0(&Slider(body), flags)); }
std::int32_t __fastcall Zero(Common*, void*) { return 0; } // 51E640/51E730。
void __fastcall Reset(Common* common, void*)
{
    auto& body = Slider(common);
    body.hoveredAt90 = 0; body.draggingAt8C = 0; body.changedAtB0 = 0;
    body.pressedAtCC = 0; common->focusAt20 = 0; common->hoverAt1C = 0;
}
std::int32_t __fastcall Enabled(Common* body, void*) { return body->enabledAt18 != 0 && body->stateAt14 != 0; }
void __fastcall Focus(Common* body, void*) { body->focusAt20 = 1; }
void __fastcall Blur(Common* body, void*) { body->focusAt20 = 0; }
std::int32_t __fastcall Hover(Common* body, void*) { return body->hoverAt1C; }
void __fastcall Enter(Common* common, void*) { Slider(common).hoveredAt90 = 1; common->hoverAt1C = 1; }
void __fastcall Leave(Common* common, void*) { Slider(common).hoveredAt90 = 0; common->hoverAt1C = 0; }
void __fastcall Noop(Common*, void*) {} // 51E630。
std::int32_t __fastcall ResourceState(Common* common, void*)
{
    if (!common->enabledAt18) return 2;
    if (!common->stateAt14) return 1;
    if (Slider(common).pressedAtCC) return 5;
    if (common->hoverAt1C) return 4;
    return common->focusAt20 ? 3 : 0;
}
std::int32_t __fastcall Hit(Common*, void*, std::int32_t, std::int32_t) { return 0; } // 503080。
void __fastcall WriteState(Common* body, void*, std::int32_t value) { body->stateAt14 = value; }
std::int32_t __fastcall ReadState(Common* body, void*) { return body->stateAt14; }
void __fastcall WriteEnabled(Common* body, void*, std::int32_t value) { body->enabledAt18 = value; }
std::int32_t __fastcall ReadEnabled(Common* body, void*) { return body->enabledAt18; }
void __fastcall NoopArgument(Common*, void*, std::int32_t) {} // 5311C0。
std::int32_t __fastcall Matches(Common* body, void*, std::int32_t type, std::int32_t event)
{ return body->typeAt08 == type && body->eventAt04 == event; }
std::int32_t __fastcall Value(Common* body, void*) { return Slider(body).valueAtB4; }
std::int32_t __fastcall Changed(Common* body, void*) { return Slider(body).changedAtB0; }
std::int32_t __fastcall Reverse(Common* body, void*) { return Slider(body).reverseAt54; }
void __fastcall StepInput(Common* body, void*, std::int32_t value) { Slider(body).stepInputAt58 = value; }

// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
// noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
// 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BASE(slot, function) reinterpret_cast<decltype(GuiControlNativeVtable23::slot)>(&function)
#define EXTRA(slot, function) reinterpret_cast<decltype(GuiSliderNativeVtable31::slot)>(&function)
const GuiSliderNativeVtable31 table{{
    BASE(deletingDestructorAt00, Delete), BASE(queryAt04, Zero), BASE(resetAt08, Reset),
    BASE(drawAt0C, DrawAt531730), BASE(compositeAt10, CompositeAt531A00), BASE(keyboardAt14, Zero),
    BASE(mouseAt18, InputAt521820), BASE(enabledAt1C, Enabled), BASE(focusAt20, Focus), BASE(blurAt24, Blur),
    BASE(hoverAt28, Hover), BASE(enterAt2C, Enter), BASE(leaveAt30, Leave), BASE(nullAt34, Noop),
    BASE(resourceStateAt38, ResourceState), BASE(rectAt3C, RectAt50A260), BASE(hitAt40, Hit),
    BASE(writeStateAt44, WriteState), BASE(readStateAt48, ReadState), BASE(writeEnabledAt4C, WriteEnabled),
    BASE(readEnabledAt50, ReadEnabled), BASE(nullAt54, NoopArgument), BASE(matchesAt58, Matches)},
    EXTRA(setRangeAt5C, SetRangeAt537680), EXTRA(valueAt60, Value), EXTRA(setValueAt64, SetValueAt538520),
    EXTRA(setStepAt68, SetStepAt5384F0), EXTRA(changedAt6C, Changed), EXTRA(reverseAt70, Reverse),
    EXTRA(setStepInputAt74, StepInput), EXTRA(setHitSlotAt78, SetHitSlotAt5361E0)};
#undef EXTRA
#undef BASE
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

struct BodyUnwind final
{
    Body* body;
    bool active = true;
    ~BodyUnwind() noexcept { if (active) DestroyGuiSliderAt4F79A0(body); }
};
struct RawUnwind final
{
    void* raw;
    bool active = true;
    ~RawUnwind() noexcept { if (active) ::operator delete(raw, std::size_t{0xD8}); }
};
class SliderControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SliderControl(Body* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Common*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1); // 53D9C0当前表。
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SliderControl) == 0x18);
}

const GuiSliderNativeVtable31& GuiSliderVtable() noexcept { return table; }
void DestroyGuiSliderAt4F79A0(Body* body) noexcept
{
    body->common.vptrAt00 = &GuiControlBaseVtable();
    ReleaseGuiClickedEventReferenceAt4F64D0(body->common.eventAt3C);
    DestroyGuiControlResourceReferencesAt4F6B00(body->common.resourcesAt28);
}
Body* DeleteGuiSliderAt4FE4B0(Body* body, std::uint32_t flags) noexcept
{
    DestroyGuiSliderAt4F79A0(body);
    if (flags & 1) ::operator delete(body, std::size_t{0xD8});
    return body;
}
Body* ConstructGuiSliderAt4F3C00(Body* body, void* parent, std::int32_t event,
    const GuiSliderConstructionNative40h& configuration)
{
    auto& common = body->common;
    common.childIdAt0C = -1; common.unknownAt10 = 0; common.stateAt14 = 1;
    common.enabledAt18 = 1; common.hoverAt1C = 0; common.focusAt20 = 0; common.unknownAt24 = 0;
    common.resourcesAt28 = {}; common.eventAt3C = {}; common.eventEnabledAt48 = 1;
    common.parentAt4C = parent; common.yAt38 = 0; common.xAt34 = 0;
    common.vptrAt00 = &table.common;
    body->interactionOffsetAt5C = {}; body->interactionRectAt64 = {};
    body->travelOffsetAt74 = {}; body->travelRectAt7C = {};
    body->thumbPositionAt98.y = 0;
    for (auto& value : body->zeroAtA0) value = 0;
    body->hoveredAt90 = 0; body->draggingAt8C = 0; body->hitSlotAt94 = 7;
    body->thumbPositionAt98.y = 0; body->thumbPositionAt98.x = 0;
    body->trackOffsetAtC4 = {}; body->dragOffsetAtD0 = {};
    common.typeAt08 = 5; common.eventAt04 = event;
    BodyUnwind cleanup{body};
    Slot copied;
    CopyGuiControlResourceReference(copied, configuration.resourcesAt00[0]);
    AppendGuiControlResourceArgumentAt4FF1F0(common.resourcesAt28, copied);
    CopyGuiControlResourceReference(copied, configuration.resourcesAt00[1]);
    AppendGuiControlResourceArgumentAt4FF1F0(common.resourcesAt28, copied);
    body->orientationAt50 = configuration.orientationAt18;
    body->reverseAt54 = configuration.reverseAt1C;
    body->interactionRectAt64 = configuration.interactionRectAt20;
    body->travelRectAt7C = configuration.interactionRectAt20;
    body->travelRectAt7C.left = Add(body->travelRectAt7C.left, configuration.travelDeltaAt30.left);
    body->travelRectAt7C.right = Add(body->travelRectAt7C.right, configuration.travelDeltaAt30.right);
    body->travelRectAt7C.top = Add(body->travelRectAt7C.top, configuration.travelDeltaAt30.top);
    body->travelRectAt7C.bottom = Add(body->travelRectAt7C.bottom, configuration.travelDeltaAt30.bottom);
    RECT combined = ResolveGuiControlResource(configuration.resourcesAt00[1])->combinedRectAt28;
    ::UnionRect(&combined, &combined, &body->interactionRectAt64);
    ::UnionRect(&combined, &combined, &body->travelRectAt7C);
    common.yAt38 = combined.top; common.xAt34 = combined.left;
    const auto track = ResolveGuiControlResource(configuration.resourcesAt00[1])->combinedRectAt28;
    body->trackOffsetAtC4 = {Sub(track.left, combined.left), Sub(track.top, combined.top)};
    body->interactionOffsetAt5C = {Sub(body->interactionRectAt64.left, common.xAt34), Sub(body->interactionRectAt64.top, combined.top)};
    body->interactionRectAt64 = Reanchor(body->interactionRectAt64, common.xAt34, common.yAt38);
    body->travelOffsetAt74 = {Sub(body->travelRectAt7C.left, common.xAt34), Sub(body->travelRectAt7C.top, common.yAt38)};
    body->travelRectAt7C = Reanchor(body->travelRectAt7C, common.xAt34, common.yAt38);
    body->stepInputAt58 = 1; body->stepAtC0 = 1;
    body->minimumAtB8 = 0; body->maximumAtBC = 100; body->valueAtB4 = 0;
    PositionFromValue(*body, body->reverseAt54 ? 100 : 0);
    cleanup.active = false;
    return body;
}
Slot* ConstructGuiSliderReferenceAt505440(Slot& output, Body* body)
{
    output = {};
    if (!body) return &output;
    output.biasAt04 = 0;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) SliderControl(body) : nullptr;
    output.controlAt08 = control;
    // 505485原null control写+4；不替换为现代throw或invalid_parameter。
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0xD8;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    return &output;
}
Body* ResolveGuiSlider(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<Body>(reference); }

Slot* CreateGuiSliderAt5052C0(Slot& output, void* parent, const GuiSliderDescriptorNative6Ch& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope graphics(surfaces);
    // 5052F9的state0是最外层：+78抛出先清配置/track/thumb，最后清返回Ref。
    Temporary outputLifetime{output, false};
    Slot thumb;
    LoadGuiControlResourceAt5047A0(thumb, descriptor.thumbNamesAt04.data(), loader, surfaces, strings);
    Temporary thumbLifetime{thumb};
    if (!thumb.baseAt00) throw 1;
    Slot track;
    LoadGuiControlResourceAt5047A0(track, descriptor.trackNamesAt20.data(), loader, surfaces, strings);
    Temporary trackLifetime{track};
    if (!track.baseAt00) throw 1;
    auto* capturedThumb = thumb.controlAt08; // 505348在可选移动之前固定EBX。
    if (const auto* position = descriptor.positionAt3C)
    {
        const auto x = position->x, y = position->y;
        Slot copied;
        CopyGuiControlResourceReference(copied, thumb);
        MoveGuiControlResourceAt5086C0(copied, x, y);
        position = descriptor.positionAt3C;
        const auto secondX = position->x, secondY = position->y;
        CopyGuiControlResourceReference(copied, track);
        MoveGuiControlResourceAt5086C0(copied, secondX, secondY);
    }
    GuiSliderConstructionNative40h configuration;
    Temporary configurationThumb{configuration.resourcesAt00[0]};
    Temporary configurationTrack{configuration.resourcesAt00[1]};
    configuration.interactionRectAt20 = {}; configuration.travelDeltaAt30 = {};
    // 4F9FB0赋值：原先释放目标，再逐DWORD复制增持；两个初始目标均为空。
    ReleaseYtlReference(configuration.resourcesAt00[0]);
    CopyGuiControlResourceReference(configuration.resourcesAt00[0], thumb);
    ReleaseYtlReference(configuration.resourcesAt00[1]);
    CopyGuiControlResourceReference(configuration.resourcesAt00[1], track);
    configuration.orientationAt18 = descriptor.orientationAt44;
    configuration.reverseAt1C = descriptor.reverseAt48;
    ::CopyRect(&configuration.interactionRectAt20, &descriptor.interactionRectAt4C);
    ::CopyRect(&configuration.travelDeltaAt30, &descriptor.travelDeltaAt5C);
    void* raw = ::operator new(0xD8);
    RawUnwind rawLifetime{raw};
    Body* body = raw ? ConstructGuiSliderAt4F3C00(::new (raw) Body, parent, descriptor.eventAt00, configuration) : nullptr;
    rawLifetime.active = false;
    ConstructGuiSliderReferenceAt505440(output, body);
    outputLifetime.active = true; // 5054A9之后才取得返回槽展开资格。
    body = ResolveGuiSlider(output);
    Table(&body->common).setHitSlotAt78(&body->common, descriptor.hitSlotAt40);
    configurationTrack.Clear();
    configurationThumb.Clear();
    auto* capturedTrack = track.controlAt08; // 5054EC在配置正常尾之后读取ESI。
    trackLifetime.Captured(capturedTrack);
    thumbLifetime.Captured(capturedThumb);
    outputLifetime.active = false;
    return &output;
}
}
