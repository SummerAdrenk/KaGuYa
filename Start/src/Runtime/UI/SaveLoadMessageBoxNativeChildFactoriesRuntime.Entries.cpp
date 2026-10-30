#include "SaveLoadMessageBoxNativeChildFactoriesRuntime.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadMessageBoxChildFactoriesDetail;
void DestroyCommon(Body* p) noexcept
{
    p->vptrAt00 = &GuiControlBaseVtable();
    ReleaseGuiClickedEventReferenceAt4F64D0(p->eventAt3C);
    DestroyGuiControlResourceReferencesAt4F6B00(p->resourcesAt28);
}
Body* __fastcall DeleteMultiCheckAt4FCCA0(Body* p, void*, std::uint32_t flags) noexcept
{
    DestroyCommon(p);
    if (flags & 1) ::operator delete(p, std::size_t{0x78});
    return p;
}
Body* __fastcall DeleteMultiHotkeyAt4FCD40(Body* p, void*, std::uint32_t flags) noexcept
{
    p->vptrAt00 = &SaveLoadMessageBoxMultiHotkeyNativeVtable().common;
    DestroyGuiControlSurfaceReferencesAt43EAA0(reinterpret_cast<MultiHotkey*>(p)->hotkeysAt5C);
    DestroyCommon(p);
    if (flags & 1) ::operator delete(p, std::size_t{0x68});
    return p;
}
Body* __fastcall DeleteRadioAt4FDC00(Body* p, void*, std::uint32_t flags) noexcept
{
    DestroyCommon(p);
    if (flags & 1) ::operator delete(p, std::size_t{0x5C});
    return p;
}
void __fastcall ResetMappedAt5025F0(Body* p, void*) noexcept
{
    for (auto& hit : reinterpret_cast<MultiCheck*>(p)->hitSlotsAt5C) hit = 7;
}
std::int32_t __fastcall SetMappedAt536E70(Body* p, void*, std::int32_t index, std::int32_t value) noexcept
{
    if (index >= 7) return 0;
    // 原只有有符号上界检查；负下标会写+5C之前，不擅加归一化/拒绝。
    const auto address = reinterpret_cast<std::uintptr_t>(p) + 0x5Cu + static_cast<std::uint32_t>(index) * 4u;
    *reinterpret_cast<std::int32_t*>(address) = value;
    return 1;
}
std::int32_t MappedHit(Body* p)
{
    const auto state = Read(p->vptrAt00)->resourceStateAt38(p);
    const auto address = reinterpret_cast<std::uintptr_t>(p) + 0x5Cu + static_cast<std::uint32_t>(state) * 4u;
    return Read(*reinterpret_cast<std::int32_t*>(address));
}
std::int32_t __fastcall HasMappedAt50FE80(Body* p, void*) { return MappedHit(p) != 7; }
std::int32_t __fastcall MappedAt509D10(Body* p, void*) { return MappedHit(p); }
void InvokeEvent(Body* p)
{
    Fresh event;
    CopyGuiClickedEventReference(event.slot, p->eventAt3C);
    ReferenceLifetime lifetime{event.slot};
    if (Read(event.slot.baseAt00))
    {
        auto* receiver = Resolve<GuiClickedEventNativeBody>(event.slot);
        Read(receiver->vptrAt00)->invokeAt00(receiver);
    }
    lifetime.active = false;
    ReleaseGuiClickedEventReferenceAt4F64D0(event.slot); // 正常及EH均回读Ref，释放异常终止。
}
std::int32_t __fastcall MultiCheckMouseAt5215E0(Body* p, void*)
{
    if (!Read(p->vptrAt00)->enabledAt1C(p)) return 0;
    auto* parent = Read(p->parentAt4C);
    const auto previousHover = Read(p->hoverAt1C);
    const auto* hitTable = Read(p->vptrAt00);
    POINT local;
    auto* point = ParentSlot<ParentPoint>(parent, 0x74)(parent, &local);
    const auto y = Read(point->y), x = Read(point->x);
    std::int32_t hovered = 0;
    if (hitTable->hitAt40(p, x, y))
    {
        hovered = 1;
        if (!Read(p->focusAt20))
        {
            parent = Read(p->parentAt4C);
            ParentSlot<ParentFocus>(parent, 0x30)(parent, p);
        }
    }
    // 52164F比较入口hover快照；相等才回读回调改写后的+1C。
    if (previousHover != hovered) p->hoverAt1C = hovered;
    else hovered = Read(p->hoverAt1C);
    if (!hovered) { GuiControlNativeEntries::ButtonField<0x50>(*p) = 0; return 0; }
    const auto pressed = Read(GuiControlNativeEntries::ButtonField<0x50>(*p));
    parent = Read(p->parentAt4C);
    if (pressed)
    {
        if (ParentSlot<ParentScalar>(parent, 0x84)(parent))
        {
            const auto eventEnabled = Read(p->eventEnabledAt48);
            GuiControlNativeEntries::ButtonField<0x50>(*p) = 0;
            if (eventEnabled) InvokeEvent(p);
        }
    }
    else if (ParentSlot<ParentScalar>(parent, 0x80)(parent)) GuiControlNativeEntries::ButtonField<0x50>(*p) = 1;
    return 0;
}
std::int32_t __fastcall RadioMouseAt521710(Body* p, void*)
{
    if (!Read(p->vptrAt00)->enabledAt1C(p)) return 0;
    auto* parent = Read(p->parentAt4C);
    const auto* hitTable = Read(p->vptrAt00);
    p->hoverAt1C = 0;
    POINT local;
    auto* point = ParentSlot<ParentPoint>(parent, 0x74)(parent, &local);
    const auto y = Read(point->y), x = Read(point->x);
    if (hitTable->hitAt40(p, x, y)) p->hoverAt1C = 1;
    else if (!Read(p->hoverAt1C)) { GuiControlNativeEntries::ButtonField<0x50>(*p) = 0; return 0; }
    // Radio不调用父+30聚焦；false hit允许+74回调设置hover后继续。
    const auto pressed = Read(GuiControlNativeEntries::ButtonField<0x50>(*p));
    parent = Read(p->parentAt4C);
    if (pressed)
    {
        if (ParentSlot<ParentScalar>(parent, 0x84)(parent))
        {
            const auto eventEnabled = Read(p->eventEnabledAt48);
            GuiControlNativeEntries::ButtonField<0x50>(*p) = 0;
            if (eventEnabled) InvokeEvent(p);
        }
    }
    else if (ParentSlot<ParentScalar>(parent, 0x80)(parent)) GuiControlNativeEntries::ButtonField<0x50>(*p) = 1;
    return 0;
}
void __fastcall RadioSelectAt5311D0(Body* p, void*, std::int32_t address)
{
    const auto* message = reinterpret_cast<const std::int32_t*>(address);
    if (!message || Read(message[0])) return;
    if (!Read(p->vptrAt00)->readEnabledAt50(p)) return;
    auto* selected = reinterpret_cast<const std::int32_t*>(Read(message[1]));
    const auto* table = Read(p->vptrAt00);
    const auto value = Read(*selected);
    table->writeStateAt44(p, value != Read(p->eventAt04));
}
void __fastcall ResetMultiHotkeyAt531240(Body* p, void*)
{
    ClearHotkeyStatesAt5321A0(reinterpret_cast<MultiHotkey*>(p));
    GuiControlNativeEntries::ButtonField<0x50>(*p) = 0;
    GuiControlNativeEntries::ButtonField<0x58>(*p) = 0;
    p->focusAt20 = 0;
    p->hoverAt1C = 0;
}
std::int32_t __fastcall MultiHotkeyKeyboardAt51EFD0(Body* p, void*)
{
    auto* body = reinterpret_cast<MultiHotkey*>(p);
    if (!Count(body->hotkeysAt5C)) return 0;
    if (!Read(p->vptrAt00)->enabledAt1C(p)) return 0;
    auto* begin = Read(body->hotkeysAt5C.begin);
    auto* end = Read(body->hotkeysAt5C.end);
    auto* active = begin;
    for (; active != end; ++active) if (Read(Resolve<Hotkey>(*active)->stateAt04)) break;
    const auto index = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(active) - reinterpret_cast<std::uintptr_t>(begin))) / 12;
    const auto count = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin))) / 12;
    if (index < count)
    {
        auto* parent = Read(p->parentAt4C);
        const auto key = Read(Resolve<Hotkey>(*active)->hotkeyAt00);
        if (ParentSlot<ParentAction>(parent, 0x6C)(parent, key)) return 2;
        GuiControlNativeEntries::ButtonField<0x50>(*p) = 0;
        ClearHotkeyStatesAt5321A0(body);
        if (Read(p->eventEnabledAt48)) InvokeEvent(p);
        return 1;
    }
    // 第一次结束指针缓存；每次父+70回调之后才按51F159重新取当前end。
    if (begin == end) return 0;
    auto* at = begin;
    do
    {
        auto* parent = Read(p->parentAt4C);
        const auto key = Read(Resolve<Hotkey>(*at)->hotkeyAt00);
        if (ParentSlot<ParentAction>(parent, 0x70)(parent, key))
        {
            GuiControlNativeEntries::ButtonField<0x50>(*p) = 1;
            Resolve<Hotkey>(*at)->stateAt04 = 1; // +70回调后重新解析同一Ref，不能用旧payload。
            return 2;
        }
        ++at;
    } while (at != Read(body->hotkeysAt5C.end));
    return 0;
}
#define COMMON_SLOT(field, f) reinterpret_cast<decltype(GuiControlNativeVtable23::field)>(&f)
}
namespace SaveLoadMessageBoxChildFactoriesDetail
{
void ClearHotkeyStatesAt5321A0(MultiHotkey* p)
{
    auto* at = Read(p->hotkeysAt5C.begin);
    if (at == Read(p->hotkeysAt5C.end)) return;
    do { Resolve<Hotkey>(*at)->stateAt04 = 0; ++at; } while (at != Read(p->hotkeysAt5C.end));
}
}
const SaveLoadMessageBoxMultiCheckNativeVtable31& SaveLoadMessageBoxMultiCheckNativeVtable() noexcept
{
    static const auto table = []
    {
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        SaveLoadMessageBoxMultiCheckNativeVtable31 t{GuiButtonVtable(),
            reinterpret_cast<decltype(SaveLoadMessageBoxMultiCheckNativeVtable31::resetMappedHitSlotsAt74)>(&ResetMappedAt5025F0),
            reinterpret_cast<decltype(SaveLoadMessageBoxMultiCheckNativeVtable31::setMappedHitSlotAt78)>(&SetMappedAt536E70)};
        t.button.common.deletingDestructorAt00 = COMMON_SLOT(deletingDestructorAt00, DeleteMultiCheckAt4FCCA0);
        t.button.common.mouseAt18 = COMMON_SLOT(mouseAt18, MultiCheckMouseAt5215E0);
        t.button.hasHitSlotAt64 = reinterpret_cast<decltype(t.button.hasHitSlotAt64)>(&HasMappedAt50FE80);
        t.button.hitSlotAt68 = reinterpret_cast<decltype(t.button.hitSlotAt68)>(&MappedAt509D10);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
const GuiButtonNativeVtable29& SaveLoadMessageBoxMultiHotkeyNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = GuiButtonVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt00 = COMMON_SLOT(deletingDestructorAt00, DeleteMultiHotkeyAt4FCD40);
        t.common.resetAt08 = COMMON_SLOT(resetAt08, ResetMultiHotkeyAt531240);
        t.common.keyboardAt14 = COMMON_SLOT(keyboardAt14, MultiHotkeyKeyboardAt51EFD0);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
const GuiButtonNativeVtable29& SaveLoadMessageBoxRadioNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = GuiButtonVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt00 = COMMON_SLOT(deletingDestructorAt00, DeleteRadioAt4FDC00);
        t.common.mouseAt18 = COMMON_SLOT(mouseAt18, RadioMouseAt521710);
        t.common.nullAt54 = COMMON_SLOT(nullAt54, RadioSelectAt5311D0);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
#undef COMMON_SLOT
}
