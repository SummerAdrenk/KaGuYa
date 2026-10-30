#include <Runtime/MessageWindowLayerNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = MessageWindowLayerNativeBody50h;
using Base = LayerItemNativeBody14h;
using Slot = YtlReferenceSlotRuntime;
using Table = LayerItemNativeFrameVtable;
thread_local MessageWindowLayerNativeServices* currentServices = nullptr;

MessageWindowLayerNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("CMesWin native scope is absent");
    return *currentServices;
}
Body& Derived(Base* body) noexcept { return *reinterpret_cast<Body*>(body); }
void Dirty(Body& body)
{
    auto* const parent = body.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &body.boundsAt14);
}
bool Valid(const Body& body, std::int32_t index) noexcept
{
    return index >= 0 && index < DrawLayerReferenceCount(body.stylesAt24);
}
MessageWindowStyleNativeBody34h* At(Body& body, std::int32_t index) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(body.stylesAt24.begin) +
        static_cast<std::uint32_t>(index) * 12u;
    return ResolveMessageWindowStyleNative(*reinterpret_cast<const Slot*>(address));
}
void ClearStyles(Body& body) noexcept
{
    // 4913B0固定释放范围，结束时才重读begin；不释放backing/capacity。
    ReleaseDrawLayerRangeAt452440(body.stylesAt24.begin, body.stylesAt24.end);
    body.stylesAt24.end = body.stylesAt24.begin;
}
class ReferenceUnwind final
{
public:
    explicit ReferenceUnwind(Slot& slot) noexcept : slot_(&slot) {}
    ~ReferenceUnwind() noexcept { if (slot_) ReleaseYtlReference(*slot_); }
    void Disarm() noexcept { slot_ = nullptr; }
private:
    Slot* slot_;
};
class BodyAllocationUnwind final
{
public:
    BodyAllocationUnwind(IEnvironmentRootMemory& memory, void* body, std::size_t size) noexcept
        : memory_(memory), body_(body), size_(size) {}
    // 5965DB及489C90原new失败清理发生于展开期；不能先进入catch再delete。
    ~BodyAllocationUnwind() noexcept { if (active_) memory_.DeleteBody(body_, size_); }
    void Disarm() noexcept { active_ = false; }
private:
    IEnvironmentRootMemory& memory_;
    void* body_;
    std::size_t size_;
    bool active_ = true;
};
void FinishAppendReference(Slot& slot) noexcept
{
    // 45A416只在payload回调后重读control；这里没有null保护。
    auto* control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08)
        {
            control->DestroyPayloadAtVslot00();
            control = slot.controlAt08;
        }
        control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}
void FinishStyleReference(Slot& slot, YtlReferenceControlRuntime* captured) noexcept
{
    // 45A42E固定ESI，与append临时及异常展开的释放不是同一条时序。
    if (!captured) return;
    if (--captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}
void AddStyle(Body& body, const MessageWindowStyleNativeDescriptor& descriptor,
    MessageWindowLayerNativeServices& services)
{
    auto& memory = services.panes.memory;
    void* const raw = memory.AllocateBody(0x34);
    BodyAllocationUnwind rawUnwind(memory, raw, 0x34);
    MessageWindowStyleNativeBody34h* created = nullptr;
    if (raw)
    {
        // 45A274在分配回调之后选择当时config，不能在进入new前固定外层槽。
        auto& selected = descriptor.configDiscriminatorAt20 == 0 ? body.type1At38 : body.type2At44;
        Slot argument = selected;
        if (argument.controlAt08) ++argument.controlAt08->referenceCountAt04;
        created = ConstructMessageWindowStyleNativeAt45F040(
            *::new (raw) MessageWindowStyleNativeBody34h, &descriptor, argument, services.panes);
    }
    rawUnwind.Disarm();
    Slot local;
    // 45A2BF撤销raw资格；control分配失败不可补删已完成style。
    ConstructMessageWindowStyleReferenceAt45A2C5(local, created, memory);
    auto* const fixedControl = local.controlAt08;
    ReferenceUnwind localUnwind(local);
    RECT combined{};
    if (created->bodyAt00.baseAt00)
    {
        RECT temporary;
        auto* const pane = ResolveMessageWindowPaneNative(created->bodyAt00);
        ::UnionRect(&combined, &combined, pane->vptrAt00->boundsAt04(pane, &temporary));
    }
    if (created->nameAt0C.baseAt00)
    {
        RECT temporary;
        auto* const pane = ResolveMessageWindowPaneNative(created->nameAt0C);
        ::UnionRect(&combined, &combined, pane->vptrAt00->boundsAt04(pane, &temporary));
    }
    ::UnionRect(&body.boundsAt14, &body.boundsAt14, &combined);
    Slot append{local.baseAt00, 0, fixedControl};
    if (fixedControl) ++fixedControl->referenceCountAt04;
    ReferenceUnwind appendUnwind(append);
    auto* const end = body.stylesAt24.end;
    if (end != body.stylesAt24.capacity)
    {
        end->baseAt00 = local.baseAt00;
        end->biasAt04 = 0;
        end->controlAt08 = fixedControl;
        if (fixedControl) ++fixedControl->referenceCountAt04;
        ++body.stylesAt24.end;
    }
    else
        static_cast<void>(GrowDrawLayerReferencesAt454580(body.stylesAt24, end, append, memory));
    appendUnwind.Disarm();
    FinishAppendReference(append);
    localUnwind.Disarm();
    FinishStyleReference(local, fixedControl);
}
std::int32_t ReadValue(ScriptParamsNativeBody& params, std::int32_t index,
    IScrVmCommandErrorSink& errors)
{
    if (index == -1) return ReadScriptValueAt4C73A0(params, index, errors);
    // 4C73AB..4C73E4没有count/ValueCount门控，非-1保持32位raw访问。
    const auto* values = ResolveEnvironmentObjectReference<std::byte>(params.saveParams2At30);
    const auto address = reinterpret_cast<std::uintptr_t>(values) +
        static_cast<std::uint32_t>(index) * 4u + 0x0Cu;
    std::int32_t value;
    std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
    return value;
}
void __fastcall Draw(Base* body, void*, const RECT* clip)
    { DrawMessageWindowLayerNativeAt482A10(Derived(body), clip, Services()); }
void __fastcall Composite(Base* body, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
    { CompositeMessageWindowLayerNativeAt4819C0(Derived(body), target, clip, Services()); }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags)
    { DeleteMessageWindowLayerNativeAt468D00(Derived(body), flags); return body; }
RECT* __fastcall Bounds(Base* body, void*, RECT* out)
    { *out = Derived(body).boundsAt14; return out; }
std::uint8_t __fastcall Request(Base* body, void*, std::uint8_t value, std::int32_t unused)
    { return RequestMessageWindowLayerNativeAt487030(Derived(body), value, unused, Services()); }
void __fastcall Advance(Base*, void*) {} // 46B230只有RET。
void __fastcall Reset(Base* body, void*) { ResetMessageWindowLayerNativeAt47ED90(Derived(body)); }
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(Table::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(Table::requestAt10)>(&Request),
    reinterpret_cast<decltype(Table::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(Table::resetAt18)>(&Reset)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

}

MessageWindowLayerNativeScope::MessageWindowLayerNativeScope(MessageWindowLayerNativeServices& services) noexcept
    : previous_(currentServices), panes_(services.panes) { currentServices = &services; }
MessageWindowLayerNativeScope::~MessageWindowLayerNativeScope() noexcept { currentServices = previous_; }
const LayerItemNativeFrameVtable& MessageWindowLayerNativeMethods() noexcept { return methods; }
std::span<const MessageWindowStyleNativeDescriptor> MessageWindowNativeStylesSample18At601408() noexcept
    { return {MessageWindowStylesNativeSample18At601408(), 2}; }

Body* ConstructMessageWindowLayerNativeAt459AB0(Body& body, LayerListNativeBody18h* parent,
    MessageWindowLayerNativeServices& services, std::span<const MessageWindowStyleNativeDescriptor> styles)
{
    ConstructLayerItemNativeAt441270(body.layer, parent);
    body.boundsAt14 = {};
    body.layer.vptrAt00 = methods.AsBase();
    body.stylesAt24 = {};
    body.type1At38 = {};
    body.type2At44 = {};
    MessageWindowNativePaneScope paneScope(services.panes);
    try
    {
        CreateAndReplaceMessageWindowNativeConfigReference(body.type1At38,
            MessageWindowConfigType::Type1, services.panes.memory);
        CreateAndReplaceMessageWindowNativeConfigReference(body.type2At44,
            MessageWindowConfigType::Type2, services.panes.memory);
        ApplyMessageWindowNativeConfigOverrides(body.type1At38, body.type2At44, services.config);
        ResetMessageWindowLayerNativeAt47ED90(body);
        body.boundsAt14 = {};
        for (const auto& descriptor : styles) AddStyle(body, descriptor, services);
    }
    catch (...)
    {
        // 45A46C不释放config、不清selector/enable、不撤注册，也不补删孤立base。
        body.boundsAt14 = {};
        ClearStyles(body);
    }
    return &body;
}
Body* DeleteMessageWindowLayerNativeAt468D00(Body& body, std::uint32_t flags) noexcept
{
    ReleaseYtlReference(body.type2At44);
    ReleaseYtlReference(body.type1At38);
    DestroyDrawLayerReferencesAt461750(body.stylesAt24);
    body.layer.vptrAt00 = LayerWindowNativeBaseMethods().AsBase();
    DestroyLayerItemNativeAt441310(body.layer);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x50});
    return result;
}
void DrawMessageWindowLayerNativeAt482A10(Body& body, const RECT* clip,
    MessageWindowLayerNativeServices& services)
{
    const auto selected = body.selectedAt30;
    if (!Valid(body, selected)) return;
    auto* const style = At(body, selected);
    MessageWindowNativePaneScope paneScope(services.panes);
    if (style->bodyAt00.baseAt00)
    {
        auto* const pane = ResolveMessageWindowPaneNative(style->bodyAt00);
        pane->vptrAt00->drawAt0C(pane, clip);
    }
    if (style->nameAt0C.baseAt00 && services.state.CaptureTextWindowNameBaseAt70())
    {
        PrepareMessageWindowNativeNameAt46B470(*style, services.state);
        auto* const pane = ResolveMessageWindowPaneNative(style->nameAt0C);
        pane->vptrAt00->drawAt0C(pane, clip);
    }
}
void CompositeMessageWindowLayerNativeAt4819C0(Body& body, GraphicsSurfaceNativeBody* target,
    const RECT* clip, MessageWindowLayerNativeServices& services)
{
    const auto selected = body.selectedAt30;
    if (!Valid(body, selected)) return;
    auto* const style = At(body, selected);
    MessageWindowNativePaneScope paneScope(services.panes);
    if (style->bodyAt00.baseAt00)
    {
        auto* const pane = ResolveMessageWindowPaneNative(style->bodyAt00);
        pane->vptrAt00->compositeAt10(pane, target, clip);
    }
    if (style->nameAt0C.baseAt00 && services.state.CaptureTextWindowNameBaseAt70())
    {
        PrepareMessageWindowNativeNameAt46B470(*style, services.state);
        auto* const pane = ResolveMessageWindowPaneNative(style->nameAt0C);
        pane->vptrAt00->compositeAt10(pane, target, clip);
    }
}
std::uint8_t RequestMessageWindowLayerNativeAt487030(Body& body, std::uint8_t requested,
    std::int32_t, MessageWindowLayerNativeServices& services)
{
    auto& state = services.state;
    const auto first = state.CaptureAfterEnsureAt413BE0();
    if (first.paramsAt47BDB0(first.object)->messageFormatIndexAt60 == -1)
        requested = 0;
    else
    {
        const auto gate = state.CaptureAfterEnsureAt413BE0();
        if (!state.MessageWindowAllowedAt419290(gate.drawManagerAt4175A0(gate.object))) requested = 0;
    }
    if (requested)
    {
        const auto* const names = state.CaptureSlot0At4180C0();
        const auto root = state.CaptureAfterEnsureAt413BE0();
        const auto index = state.ResolveAt498020(names, "mess_win_type");
        const auto selected = ReadValue(*root.paramsAt47BDB0(root.object), index, services.errors);
        if (body.selectedAt30 != selected)
        {
            Dirty(body);
            body.selectedAt30 = selected;
            body.rememberedNameAt34 = state.CaptureTextWindowNameBaseAt70() != 0 ? 1u : 0u;
        }
        else if (Valid(body, selected)) Dirty(body);
        if (body.layer.enabledAt0C) return 0;
        body.layer.enabledAt0C = 1;
        Dirty(body);
        return 1;
    }
    if (!body.layer.enabledAt0C) return 0;
    body.layer.enabledAt0C = 0;
    Dirty(body);
    return 1;
}
void ResetMessageWindowLayerNativeAt47ED90(Body& body) noexcept
{
    body.layer.enabledAt0C = 0;
    body.selectedAt30 = -1;
    body.rememberedNameAt34 = 0;
}
void RefreshMessageWindowStyleNativeAt5326F0(MessageWindowStyleNativeBody34h& style)
{
    if (style.bodyAt00.baseAt00)
    {
        auto* const pane = ResolveMessageWindowPaneNative(style.bodyAt00);
        pane->vptrAt00->refreshAt08(pane);
    }
    if (style.nameAt0C.baseAt00)
    {
        auto* const pane = ResolveMessageWindowPaneNative(style.nameAt0C);
        pane->vptrAt00->refreshAt08(pane);
    }
}
void RefreshMessageWindowLayerNativeAt5325F0(Body& body, std::int32_t index)
{
    if (index < 0)
    {
        for (auto* cursor = body.stylesAt24.begin; cursor != body.stylesAt24.end; ++cursor)
            RefreshMessageWindowStyleNativeAt5326F0(*ResolveMessageWindowStyleNative(*cursor));
    }
    else
    {
        if (!Valid(body, index)) return;
        RefreshMessageWindowStyleNativeAt5326F0(*At(body, index));
    }
    if (body.layer.enabledAt0C) Dirty(body);
}
MessageWindowNativeConfig14h* ResolveMessageWindowTextConfigNativeAt414C3C(Body& body) noexcept
{
    auto index = body.selectedAt30;
    if (!Valid(body, index)) index = 0;
    return ResolveMessageWindowNativeConfig(At(body, index)->configAt18);
}
Body* CreateAndAppendMessageWindowLayerNative(IDrawManagerNativeRootSource& source,
    MessageWindowLayerNativeServices& services)
{
    // 489C90:48A77B..48A7F2，raw构造与append分别Ensure，不缓存旧manager。
    auto& memory = services.panes.memory;
    void* const raw = memory.AllocateBody(0x50);
    BodyAllocationUnwind rawUnwind(memory, raw, 0x50);
    Body* created = nullptr;
    if (raw)
    {
        auto* const manager = source.CaptureDrawManagerAfter413BE0();
        auto* const parent = ResolveDrawLayerListAt47AE80(*manager);
        created = ConstructMessageWindowLayerNativeAt459AB0(*::new (raw) Body, parent, services);
    }
    rawUnwind.Disarm();
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    AppendOwnedDrawLayerAt46A880(manager->ownedLayersAt14, created ? &created->layer : nullptr, memory);
    return created;
}
}
