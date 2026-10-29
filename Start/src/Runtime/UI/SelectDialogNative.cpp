#include <Runtime/SelectDialogNative.h>
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/RootRuntimeOwner.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SelectDialogNativeBody88h;
using Base = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
thread_local const SelectDialogNativeProfile* profileSource;
thread_local ISelectDialogNativeSource* menuSource;
struct RefLifetime final
{
    Slot& slot;
    bool active = true;
    ~RefLifetime() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish(bool clear = true) noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Captured(YtlReferenceControlRuntime* control) noexcept
    {
        active = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
GuiDirtyRectNativeBody80h* __fastcall Dirty(Base* base, void*) noexcept
{ return ResolveGuiDirtyRectReference(base->dirtyAt50); }
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Base*>(DeleteSelectDialogNativeAt4FE1E0(reinterpret_cast<Body*>(base), flags)); }
void __fastcall ResetInput(Base* base, void*)
{
    const auto* table = base->vptrAt00;
    reinterpret_cast<Body*>(base)->confirmLatchAt68 = 0;
    table->setActiveAt30(base, nullptr);
    base->inputLatchAt30 = 0;
}
std::int32_t __fastcall Confirm(Base* base, void*)
{
    if (base->activeAt4C->typeAt08 != 0) return 0;
    auto& body = *reinterpret_cast<Body*>(base);
    const auto previous = body.confirmLatchAt68;
    const auto* table = base->vptrAt00;
    if (!previous)
    {
        if (!table->pressedAt70(base, 5)) return previous;
        auto* child = base->activeAt4C;
        body.confirmLatchAt68 = 1;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!table->heldAt6C(base, 5))
    {
        Slot event;
        CopyGuiClickedEventReference(event, base->activeAt4C->eventAt3C);
        RefLifetime lifetime{event};
        if (event.baseAt00)
        {
            auto* receiver = ResolveGuiClickedEvent(event);
            receiver->vptrAt00->invokeAt00(receiver);
        }
        body.confirmLatchAt68 = 0;
        // 5114E1正常尾直接4F64D0；若invoke抛出，锁存不清但局部Ref仍释放。
    }
    return previous;
}
void __fastcall Reset(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    base->vptrAt00->writeEnabledAt24(base, 1);
    ClearGuiClickedEventFlagsAt532200(body.eventsAt7C);
    for (auto* item = base->childrenAt0C.begin; item != base->childrenAt0C.end; ++item)
    {
        auto* child = ResolveGuiControl(*item);
        child->vptrAt00->resetAt08(child);
        child = ResolveGuiControl(*item);
        child->vptrAt00->writeEnabledAt4C(child, 0);
        child = ResolveGuiControl(*item);
        child->vptrAt00->writeStateAt44(child, 0);
    }
    base->vptrAt00->resetInputAt3C(base);
    base->activeAt4C = nullptr;
    if (!menuSource || !profileSource) throw std::logic_error("SelectDialogNativeScope is not installed");
    const auto& candidates = menuSource->CandidatesAt50F119();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(candidates.storage.end) -
        reinterpret_cast<std::uintptr_t>(candidates.storage.begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) >> 2;
    ::SetRectEmpty(&body.boundsAt6C);
    if (count < 0 || count >= 4) throw 1;
    const auto* ids = profileSource->visibleIdsAt6070E8[static_cast<std::size_t>(count)];
    if (!ids) throw 1;
    for (std::int32_t index = 0; index < count; ++index)
    {
        Slot selected;
        base->vptrAt00->findChildAt14(base, &selected, ids[index]);
        RefLifetime lifetime{selected};
        if (selected.baseAt00)
        {
            auto* child = ResolveGuiControl(selected);
            child->vptrAt00->writeEnabledAt4C(child, 1);
            child = ResolveGuiControl(selected);
            child->vptrAt00->writeStateAt44(child, 1);
            child = ResolveGuiControl(selected);
            RECT output;
            const auto* rect = child->vptrAt00->rectAt3C(child, &output);
            ::UnionRect(&body.boundsAt6C, &body.boundsAt6C, rect);
        }
        lifetime.Finish();
    }
    void* raw = ::operator new(1);
    Slot factory;
    ConstructGuiFactoryReferenceAt4D7860(factory, raw, 1);
    RefLifetime factoryLifetime{factory};
    const auto* navigation = profileSource->navigationAt607180[static_cast<std::size_t>(count)];
    if (!navigation) throw 1;
    auto* factoryControl = factory.controlAt08;
    for (const auto* descriptor = navigation; descriptor->inputAt00 != 0xFF; ++descriptor)
    {
        Slot value;
        CreateGuiNavigationAt504CE0(value, descriptor->inputAt00, descriptor->pairsAt04);
        RefLifetime valueLifetime{value};
        AppendGuiNavigationReferenceAt4D9D60(base->navigationAt24, value);
        valueLifetime.Finish();
    }
    factoryLifetime.Captured(factoryControl);
    body.phaseAt64 = 3;
    body.resultAt60 = -1;
}
std::int32_t __fastcall Poll(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    switch (body.phaseAt64)
    {
    case 0: case 1: case 2: return 1;
    case 3:
    {
        base->vptrAt00->processInputAt54(base);
        auto* item = body.eventsAt7C.begin;
        const auto* end = body.eventsAt7C.end;
        for (; item != end; ++item)
        {
            if (ResolveGuiClickedEvent(*item)->clickedFlagAt04 == 0) continue;
            body.resultAt60 = ResolveGuiClickedEvent(*item)->eventValueAt08;
            body.phaseAt64 = 4;
            break;
        }
        return 1;
    }
    case 4:
    {
        auto* dirty = base->vptrAt00->dirtyAt00(base);
        dirty->vptrAt00->addRectAt08(dirty, &body.boundsAt6C);
        ClearGuiClickedEventFlagsAt532200(body.eventsAt7C);
        for (auto* item = base->childrenAt0C.begin; item != base->childrenAt0C.end; ++item)
        {
            auto* child = ResolveGuiControl(*item);
            if (child->vptrAt00->readEnabledAt50(child) == 0) continue;
            child = ResolveGuiControl(*item);
            child->vptrAt00->resetAt08(child);
        }
        base->vptrAt00->resetInputAt3C(base);
        base->activeAt4C = nullptr;
        body.phaseAt64 = -1;
        return 1;
    }
    default: return 0;
    }
}
std::int32_t __fastcall End(Base* base, void*)
{ base->vptrAt00->writeEnabledAt24(base, 0); return 1; }
std::int32_t __fastcall Result(Base* base, void*)
{
    std::uint32_t ordinal = 0;
    for (auto* item = base->childrenAt0C.begin; item != base->childrenAt0C.end; ++item)
    {
        auto* child = ResolveGuiControl(*item);
        if (!child->vptrAt00->readEnabledAt50(child)) continue;
        child = ResolveGuiControl(*item);
        if (child->eventAt04 == reinterpret_cast<Body*>(base)->resultAt60) break;
        ++ordinal;
    }
    return std::bit_cast<std::int32_t>(ordinal);
}
}
const GuiDialogNativeVtable35& SelectDialogNativeVtable() noexcept
{
    static const GuiDialogNativeVtable35 table = [] {
        auto result = GuiDialogBaseVtable();
#define SELECT_SLOT(slot, function) result.slot = reinterpret_cast<decltype(result.slot)>(&function)
        SELECT_SLOT(dirtyAt00, Dirty);
        SELECT_SLOT(deletingDestructorAt04, Delete);
        SELECT_SLOT(activeInputAt38, Confirm);
        SELECT_SLOT(resetInputAt3C, ResetInput);
        SELECT_SLOT(beginAt44, Reset);
        SELECT_SLOT(pollAt48, Poll);
        SELECT_SLOT(endAt4C, End);
        SELECT_SLOT(resultAt50, Result);
#undef SELECT_SLOT
        return result;
    }();
    return table;
}
SelectDialogNativeBody88h* DeleteSelectDialogNativeAt4FE1E0(Body* body, std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &SelectDialogNativeVtable();
    DestroyGuiClickedEventReferencesAt4F6D10(body->eventsAt7C);
    DestroyGuiDialogAt4F7AC0(&body->common);
    if (flags & 1U) ::operator delete(body, std::size_t{0x88});
    return body;
}
RECT* SelectVisibleBoundsNativeAt50AD40(Body& body, RECT* output, std::int32_t requested)
{
    std::uint32_t ordinal = 0;
    for (auto* item = body.common.childrenAt0C.begin; item != body.common.childrenAt0C.end; ++item)
    {
        auto* child = ResolveGuiControl(*item);
        if (!child->vptrAt00->readEnabledAt50(child)) continue;
        if (ordinal == static_cast<std::uint32_t>(requested))
        {
            child = ResolveGuiControl(*item);
            child->vptrAt00->rectAt3C(child, output);
            return output; // 50ADCF不采用子函数的返回指针。
        }
        ++ordinal;
    }
    *output = {};
    return output;
}
SelectDialogNativeScope::SelectDialogNativeScope(const SelectDialogNativeProfile& profile,
    ISelectDialogNativeSource& source) noexcept : previousProfile_(profileSource), previousSource_(menuSource)
{ profileSource = &profile; menuSource = &source; }
SelectDialogNativeScope::~SelectDialogNativeScope() noexcept
{ profileSource = previousProfile_; menuSource = previousSource_; }
const ScriptDwordVectorStorage& RootSelectDialogNativeSource::CandidatesAt50F119()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().ParamsAt47BDB0().menuCandidatesAt64;
}
}
