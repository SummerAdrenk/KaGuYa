#include <Runtime/GuiControlNative.h>
#include <cstdlib>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GuiControlNativeBody50h;

void DestroyMembers(Body& body) noexcept
{
    // 4FC415/4FE575/4F7235：先退化表，再事件，最后正序销毁资源组。
    body.vptrAt00 = &GuiControlBaseVtable();
    ReleaseGuiClickedEventReferenceAt4F64D0(body.eventAt3C);
    DestroyGuiControlResourceReferencesAt4F6B00(body.resourcesAt28);
}
Body* __fastcall Delete50(Body* body, void*, std::uint32_t flags) noexcept
{
    DestroyMembers(*body);
    if ((flags & 1) != 0) ::operator delete(body, std::size_t{0x50});
    return body;
}
Body* __fastcall Delete60(Body* body, void*, std::uint32_t flags) noexcept
{
    DestroyMembers(*body);
    if ((flags & 1) != 0) ::operator delete(body, std::size_t{0x60});
    return body;
}
std::int32_t __fastcall Zero(Body*, void*) noexcept { return 0; }
Body* __fastcall Delete5C(Body* body, void*, std::uint32_t flags) noexcept
{
    DestroyMembers(*body);
    if ((flags & 1) != 0) ::operator delete(body, std::size_t{0x5C});
    return body;
}
void __fastcall ResetStatic(Body* body, void*) noexcept { body->focusAt20 = 0; body->hoverAt1C = 0; }
void __fastcall ResetCheckBox(Body* body, void*) noexcept
{
    GuiControlNativeEntries::ButtonField<0x50>(*body) = 0;
    GuiControlNativeEntries::ButtonField<0x58>(*body) = 0;
    ResetStatic(body, nullptr);
}
void __fastcall NoDraw(Body*, void*, const RECT*) noexcept {}
void __fastcall NoComposite(Body*, void*, YtlReferenceSlotRuntime target, const RECT*) noexcept
{
    auto* control = target.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (target.controlAt08) target.controlAt08->DeleteControlAtVslot04(1);
    }
}
std::int32_t __fastcall Enabled(Body* body, void*) noexcept
{ return body->enabledAt18 != 0 && body->stateAt14 != 0; }
void __fastcall Focus(Body* body, void*) noexcept { body->focusAt20 = 1; }
void __fastcall Blur(Body* body, void*) noexcept { body->focusAt20 = 0; }
std::int32_t __fastcall Hover(Body* body, void*) noexcept { return body->hoverAt1C; }
void __fastcall Enter(Body* body, void*) noexcept { body->hoverAt1C = 1; }
void __fastcall Leave(Body* body, void*) noexcept { body->hoverAt1C = 0; }
void __fastcall Noop(Body*, void*) noexcept {}
void __fastcall NoopArgument(Body*, void*, std::int32_t) noexcept {}
std::int32_t __fastcall Pure(Body*, void*) noexcept { _purecall(); std::terminate(); }
std::int32_t __fastcall StaticState(Body* body, void*) noexcept
{
    if (!body->enabledAt18) return 2;
    if (!body->stateAt14) return 1;
    if (body->hoverAt1C) return 4;
    if (body->focusAt20) return 3;
    return 0;
}
std::int32_t __fastcall CheckBoxState(Body* body, void*) noexcept
{
    if (!body->enabledAt18) return 2;
    if (!body->stateAt14) return 1;
    if (GuiControlNativeEntries::ButtonField<0x50>(*body)) return 5;
    if (body->hoverAt1C) return 4;
    if (body->focusAt20) return 3;
    return 0;
}
void __fastcall WriteState(Body* body, void*, std::int32_t value) noexcept { body->stateAt14 = value; }
std::int32_t __fastcall ReadState(Body* body, void*) noexcept { return body->stateAt14; }
void __fastcall WriteEnabled(Body* body, void*, std::int32_t value) noexcept { body->enabledAt18 = value; }
std::int32_t __fastcall ReadEnabled(Body* body, void*) noexcept { return body->enabledAt18; }
std::int32_t __fastcall Matches(Body* body, void*, std::int32_t type, std::int32_t event) noexcept
{ return body->typeAt08 == type && body->eventAt04 == event; }
std::int32_t __fastcall HasHit(Body* body, void*) noexcept
{ return GuiControlNativeEntries::ButtonField<0x54>(*body) != 7; }
std::int32_t __fastcall HitSlot(Body* body, void*) noexcept
{ return GuiControlNativeEntries::ButtonField<0x54>(*body); }
std::int32_t __fastcall Pressed(Body* body, void*) noexcept
{ return GuiControlNativeEntries::ButtonField<0x50>(*body); }
void __fastcall SetPressed(Body* body, void*, std::int32_t value) noexcept
{ GuiControlNativeEntries::ButtonField<0x50>(*body) = value; }

using namespace GuiControlNativeEntries;
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
// noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
// 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define CONTROL_SLOT(slot, entry) reinterpret_cast<decltype(GuiControlNativeVtable23::slot)>(&entry)
#define CHECK_SLOT(slot, entry) reinterpret_cast<decltype(GuiCheckBoxNativeVtable30::slot)>(&entry)
const GuiControlNativeVtable23 baseTable{
    CONTROL_SLOT(deletingDestructorAt00, Delete50), CONTROL_SLOT(queryAt04, Zero),
    CONTROL_SLOT(resetAt08, ResetStatic), CONTROL_SLOT(drawAt0C, NoDraw),
    CONTROL_SLOT(compositeAt10, NoComposite), CONTROL_SLOT(keyboardAt14, Zero),
    CONTROL_SLOT(mouseAt18, Zero), CONTROL_SLOT(enabledAt1C, Zero),
    CONTROL_SLOT(focusAt20, Focus), CONTROL_SLOT(blurAt24, Blur), CONTROL_SLOT(hoverAt28, Hover),
    CONTROL_SLOT(enterAt2C, Enter), CONTROL_SLOT(leaveAt30, Leave), CONTROL_SLOT(nullAt34, Noop),
    CONTROL_SLOT(resourceStateAt38, Pure), CONTROL_SLOT(rectAt3C, Rect), CONTROL_SLOT(hitAt40, HitStatic),
    CONTROL_SLOT(writeStateAt44, WriteState), CONTROL_SLOT(readStateAt48, ReadState),
    CONTROL_SLOT(writeEnabledAt4C, WriteEnabled), CONTROL_SLOT(readEnabledAt50, ReadEnabled),
    CONTROL_SLOT(nullAt54, NoopArgument), CONTROL_SLOT(matchesAt58, Matches)};
const GuiControlNativeVtable23 staticTable{
    CONTROL_SLOT(deletingDestructorAt00, Delete50), CONTROL_SLOT(queryAt04, Zero),
    CONTROL_SLOT(resetAt08, ResetStatic), CONTROL_SLOT(drawAt0C, DrawStatic),
    CONTROL_SLOT(compositeAt10, CompositeStatic), CONTROL_SLOT(keyboardAt14, Zero),
    CONTROL_SLOT(mouseAt18, Zero), CONTROL_SLOT(enabledAt1C, Enabled),
    CONTROL_SLOT(focusAt20, Focus), CONTROL_SLOT(blurAt24, Blur), CONTROL_SLOT(hoverAt28, Hover),
    CONTROL_SLOT(enterAt2C, Enter), CONTROL_SLOT(leaveAt30, Leave), CONTROL_SLOT(nullAt34, Noop),
    CONTROL_SLOT(resourceStateAt38, StaticState), CONTROL_SLOT(rectAt3C, Rect), CONTROL_SLOT(hitAt40, HitStatic),
    CONTROL_SLOT(writeStateAt44, WriteState), CONTROL_SLOT(readStateAt48, ReadState),
    CONTROL_SLOT(writeEnabledAt4C, WriteEnabled), CONTROL_SLOT(readEnabledAt50, ReadEnabled),
    CONTROL_SLOT(nullAt54, NoopArgument), CONTROL_SLOT(matchesAt58, Matches)};
const GuiCheckBoxNativeVtable30 checkTable{{
    CONTROL_SLOT(deletingDestructorAt00, Delete60), CONTROL_SLOT(queryAt04, Zero),
    CONTROL_SLOT(resetAt08, ResetCheckBox), CONTROL_SLOT(drawAt0C, DrawCheckBox),
    CONTROL_SLOT(compositeAt10, CompositeCheckBox), CONTROL_SLOT(keyboardAt14, Keyboard),
    CONTROL_SLOT(mouseAt18, Mouse), CONTROL_SLOT(enabledAt1C, Enabled),
    CONTROL_SLOT(focusAt20, Focus), CONTROL_SLOT(blurAt24, Blur), CONTROL_SLOT(hoverAt28, Hover),
    CONTROL_SLOT(enterAt2C, Enter), CONTROL_SLOT(leaveAt30, Leave), CONTROL_SLOT(nullAt34, Noop),
    CONTROL_SLOT(resourceStateAt38, CheckBoxState), CONTROL_SLOT(rectAt3C, Rect), CONTROL_SLOT(hitAt40, HitCheckBox),
    CONTROL_SLOT(writeStateAt44, WriteState), CONTROL_SLOT(readStateAt48, ReadState),
    CONTROL_SLOT(writeEnabledAt4C, WriteEnabled), CONTROL_SLOT(readEnabledAt50, ReadEnabled),
    CONTROL_SLOT(nullAt54, NoopArgument), CONTROL_SLOT(matchesAt58, Matches)},
    CHECK_SLOT(resourceAt5C, Resource), CHECK_SLOT(setHitSlotAt60, SetHitSlot),
    CHECK_SLOT(hasHitSlotAt64, HasHit), CHECK_SLOT(hitSlotAt68, HitSlot),
    CHECK_SLOT(pressedAt6C, Pressed), CHECK_SLOT(setPressedAt70, SetPressed), CHECK_SLOT(positionAt74, Position)};
// PE5BDF0C/5BE0F4逐槽核对：除删除器与资源槽外，前29槽为同一原函数。
const GuiButtonNativeVtable29 buttonTable = [] {
    GuiButtonNativeVtable29 result{checkTable.common,
        reinterpret_cast<decltype(GuiButtonNativeVtable29::resourceAt5C)>(&ButtonResource),
        checkTable.setHitSlotAt60, checkTable.hasHitSlotAt64, checkTable.hitSlotAt68,
        checkTable.pressedAt6C, checkTable.setPressedAt70};
    result.common.deletingDestructorAt00 = CONTROL_SLOT(deletingDestructorAt00, Delete5C);
    return result;
}();
#undef CHECK_SLOT
#undef CONTROL_SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

template<std::size_t BodySize>
class ControlReference final : public EnvironmentObjectReferenceControl
{
public:
    explicit ControlReference(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<Body*>(payloadAt14);
        // 53D680/53D9D0调用当前表；flags0析构退化后不能继续固定派生删除器。
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ControlReference<0x50>) == 0x18);
static_assert(sizeof(ControlReference<0x5C>) == 0x18);
static_assert(sizeof(ControlReference<0x60>) == 0x18);

void InitializeCommon(Body& body, void* parent, std::int32_t event,
    std::int32_t type, const GuiControlNativeVtable23& table) noexcept
{
    body.childIdAt0C = -1;
    body.unknownAt10 = 0;
    body.stateAt14 = 1;
    body.enabledAt18 = 1;
    body.hoverAt1C = 0;
    body.focusAt20 = 0;
    body.unknownAt24 = 0;
    body.resourcesAt28 = {};
    body.eventAt3C = {};
    body.eventEnabledAt48 = 1;
    body.parentAt4C = parent;
    body.yAt38 = 0;
    body.xAt34 = 0;
    body.eventAt04 = event;
    body.vptrAt00 = &table;
    body.typeAt08 = type;
}
struct GroupArgument final
{
    YtlReferenceSlotRuntime& value;
    bool active = true;
    ~GroupArgument() noexcept { if (active) ReleaseGuiControlResourceReferenceAt4F6380(value); }
    void Finish(bool clear) noexcept
    {
        active = false;
        auto* control = value.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (value.controlAt08) value.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { value.controlAt08 = nullptr; value.baseAt00 = 0; }
    }
};
struct BodyUnwind final
{
    Body& body;
    bool active = true;
    ~BodyUnwind() noexcept { if (active) DestroyMembers(body); }
};
}
const GuiControlNativeVtable23& GuiControlBaseVtable() noexcept { return baseTable; }
const GuiControlNativeVtable23& GuiStaticVtable() noexcept { return staticTable; }
const GuiCheckBoxNativeVtable30& GuiCheckBoxVtable() noexcept { return checkTable; }
const GuiButtonNativeVtable29& GuiButtonVtable() noexcept { return buttonTable; }

GuiButtonNativeBody5Ch* ConstructGuiButtonAt4DA700(GuiButtonNativeBody5Ch* body,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedGroup)
{
    GroupArgument parameter{ownedGroup};
    auto& common = body->common;
    InitializeCommon(common, parent, event, 0, buttonTable.common);
    body->hitSlotAt54 = 7;
    body->pressedAt50 = 0;
    body->keyboardLatchAt58 = 0;
    BodyUnwind unwind{common};
    YtlReferenceSlotRuntime copied;
    CopyGuiControlResourceReference(copied, ownedGroup);
    AppendGuiControlResourceArgumentAt4FF1F0(common.resourcesAt28, copied);
    const auto rect = RequireGuiControlResource(ownedGroup).combinedRectAt28;
    common.xAt34 = rect.left;
    common.yAt38 = rect.top;
    parameter.Finish(false);
    unwind.active = false;
    return body;
}

YtlReferenceSlotRuntime* ConstructToolButtonReferenceAt4E2C61(YtlReferenceSlotRuntime& output,
    GuiButtonNativeBody5Ch* body)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) ControlReference<0x5C>(&body->common) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x5C;
    output.controlAt08 = control;
    control->countAt10 = 1;
    return &output;
}

GuiControlNativeBody50h* ConstructGuiStaticAt4F4050(Body* body, void* parent,
    std::int32_t event, YtlReferenceSlotRuntime ownedGroup)
{
    GroupArgument parameter{ownedGroup};
    InitializeCommon(*body, parent, event, 1, staticTable);
    BodyUnwind unwind{*body};
    YtlReferenceSlotRuntime copied;
    CopyGuiControlResourceReference(copied, ownedGroup);
    AppendGuiControlResourceArgumentAt4FF1F0(body->resourcesAt28, copied);
    const auto rect = RequireGuiControlResource(ownedGroup).combinedRectAt28;
    body->xAt34 = rect.left;
    body->yAt38 = rect.top;
    parameter.Finish(false);
    unwind.active = false;
    return body;
}
GuiCheckBoxNativeBody60h* ConstructGuiCheckBoxAt4E0520(GuiCheckBoxNativeBody60h* body,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedNormal,
    YtlReferenceSlotRuntime ownedChecked)
{
    // EH：正文先退化/析构，其后normal、checked；正常尾同样normal先消费。
    GroupArgument checkedParameter{ownedChecked};
    GroupArgument normalParameter{ownedNormal};
    auto& common = body->common;
    InitializeCommon(common, parent, event, 2, checkTable.common);
    body->hitSlotAt54 = 7;
    body->checkedAt5C = 0;
    body->pressedAt50 = 0;
    body->keyboardLatchAt58 = 0;
    BodyUnwind unwind{common};
    YtlReferenceSlotRuntime copied;
    CopyGuiControlResourceReference(copied, ownedNormal);
    AppendGuiControlResourceArgumentAt4FF1F0(common.resourcesAt28, copied);
    CopyGuiControlResourceReference(copied, ownedChecked);
    AppendGuiControlResourceArgumentAt4FF1F0(common.resourcesAt28, copied);
    const RECT checkedRect = RequireGuiControlResource(ownedChecked).combinedRectAt28;
    const RECT normalRect = RequireGuiControlResource(ownedNormal).combinedRectAt28;
    RECT combined{};
    ::UnionRect(&combined, &normalRect, &checkedRect);
    ::CopyRect(&RequireGuiControlResource(ownedNormal).combinedRectAt28, &combined);
    ::CopyRect(&RequireGuiControlResource(ownedChecked).combinedRectAt28, &combined);
    common.xAt34 = combined.left;
    common.yAt38 = combined.top;
    normalParameter.Finish(true);
    checkedParameter.Finish(false);
    unwind.active = false;
    return body;
}
YtlReferenceSlotRuntime* ConstructGuiStaticReferenceAt4D7940(YtlReferenceSlotRuntime& output,
    Body* body, std::uint8_t owning)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) ControlReference<0x50>(body) : nullptr;
    output.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = owning;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x50;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    return &output;
}
YtlReferenceSlotRuntime* ConstructPopupCheckBoxReferenceAt4EA73C(YtlReferenceSlotRuntime& output,
    GuiCheckBoxNativeBody60h* body)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) ControlReference<0x60>(&body->common) : nullptr;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x60;
    output.controlAt08 = control;
    control->countAt10 = 1;
    return &output;
}
YtlReferenceSlotRuntime* ConstructGuiButtonReferenceAt504298(YtlReferenceSlotRuntime& output,
    GuiButtonNativeBody5Ch* body)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) ControlReference<0x5C>(&body->common) : nullptr;
    // 5042DE先发布control，再初始化；Tool内联4E2C61的顺序不同。
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x5C;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    return &output;
}
YtlReferenceSlotRuntime* ConstructGuiCheckBoxReferenceAt4D7710(YtlReferenceSlotRuntime& output,
    GuiCheckBoxNativeBody60h* body, std::uint8_t owning)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = ::operator new(0x18);
    auto* control = allocation ? ::new (allocation) ControlReference<0x60>(&body->common) : nullptr;
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = owning;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x60;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    return &output;
}
void ReleaseGuiControlReferenceAt4F6460(YtlReferenceSlotRuntime& reference) noexcept
{ ReleaseYtlReference(reference); }
Body* ResolveGuiControl(const YtlReferenceSlotRuntime& reference) noexcept
{ return ResolveEnvironmentObjectReference<Body>(reference); }
Body& RequireGuiControl(const YtlReferenceSlotRuntime& reference)
{
    auto* body = ResolveGuiControl(reference);
    if (!body) throw std::invalid_argument("native GuiControl body is null");
    return *body;
}
}
