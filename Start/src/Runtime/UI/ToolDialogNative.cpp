#include <Runtime/ToolDialogNative.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ToolDialogNativeBody7Ch;
using Base = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
thread_local const ToolDialogNativeRelation* relationSource = nullptr;

struct ChildLifetime final
{
    Slot& slot;
    bool active = true;
    ~ChildLifetime() noexcept { if (active) ReleaseGuiControlReferenceAt4F6460(slot); }
    void Finish(bool clear) noexcept
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
};
GuiDirtyRectNativeBody80h* __fastcall Dirty(Base* body, void*) noexcept
{ return ResolveGuiDirtyRectReference(body->dirtyAt50); }
Base* __fastcall Delete(Base* body, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Base*>(DeleteToolDialogNativeAt4FCA80(reinterpret_cast<Body*>(body), flags)); }
void __fastcall Reset(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    base->vptrAt00->writeEnabledAt24(base, 1);
    ClearGuiClickedEventFlagsAt532200(body.eventsAt70);
    ResetToolDialogControlsAt532480(*base);
    body.eventAt5C = -1;
    base->activeAt4C = nullptr;
}
std::int32_t __fastcall One(Base*, void*) noexcept { return 1; }
std::int32_t __fastcall Result(Base* body, void*) noexcept
{ return reinterpret_cast<Body*>(body)->eventAt5C; }
void __fastcall Process(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    body.eventAt5C = -1;
    ProcessGuiDialogInputAt518C30(*base);
    ReconcileToolDialogControlsAt527720(*base);
    // 519836/39只在遍历前取begin/end；首个clicked才清除，其余保留到下次更新。
    auto* position = body.eventsAt70.begin;
    const auto* end = body.eventsAt70.end;
    for (; position != end; ++position)
    {
        if (ResolveGuiClickedEvent(*position)->clickedFlagAt04 == 0) continue;
        body.eventAt5C = ResolveGuiClickedEvent(*position)->eventValueAt08;
        auto* event = ResolveGuiClickedEvent(*position);
        event->vptrAt00->clearAt04(event);
        return;
    }
}
}

const GuiDialogNativeVtable35& ToolDialogNativeVtable() noexcept
{
    static const GuiDialogNativeVtable35 table = [] {
        auto result = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define TOOL_SLOT(slot, function) result.slot = reinterpret_cast<decltype(result.slot)>(&function)
        TOOL_SLOT(dirtyAt00, Dirty);
        TOOL_SLOT(deletingDestructorAt04, Delete);
        TOOL_SLOT(beginAt44, Reset);
        TOOL_SLOT(pollAt48, One);
        TOOL_SLOT(endAt4C, One);
        TOOL_SLOT(resultAt50, Result);
        TOOL_SLOT(processInputAt54, Process);
#undef TOOL_SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return result;
    }();
    return table;
}
void ResetToolDialogControlsAt532480(Base& body)
{
    for (auto* position = body.childrenAt0C.begin; position != body.childrenAt0C.end; ++position)
    {
        auto* child = ResolveGuiControl(*position);
        child->vptrAt00->resetAt08(child);
        child = ResolveGuiControl(*position);
        if (child->typeAt08 == 2)
            reinterpret_cast<GuiCheckBoxNativeBody60h*>(ResolveGuiControl(*position))->checkedAt5C = 0;
        child = ResolveGuiControl(*position);
        if (child->typeAt08 == 1)
        {
            child = ResolveGuiControl(*position);
            child->vptrAt00->writeEnabledAt4C(child, 0);
        }
    }
}
void ReconcileToolDialogControlsAt527720(Base& body)
{
    if (!relationSource) throw std::logic_error("ToolDialog relation source is not bound");
    Slot primary;
    body.vptrAt00->findChildAt14(&body, &primary, relationSource->controlIdAt606FE0);
    ChildLifetime primaryLifetime{primary};
    if (primary.baseAt00)
    {
        auto* child = ResolveGuiControl(primary);
        if (child->vptrAt00->readStateAt48(child) != 0)
        {
            Slot related;
            body.vptrAt00->findChildAt14(&body, &related, relationSource->relatedIdAt606FDC);
            ChildLifetime relatedLifetime{related};
            std::int32_t value = 0;
            if (related.baseAt00)
            {
                child = ResolveGuiControl(related);
                if (child->vptrAt00->enabledAt1C(child))
                    value = body.activeAt4C == ResolveGuiControl(related);
            }
            child = ResolveGuiControl(primary);
            if (child->vptrAt00->readEnabledAt50(child) != value)
            {
                child = ResolveGuiControl(primary);
                child->vptrAt00->writeEnabledAt4C(child, value);
                child = ResolveGuiControl(primary);
                const auto* table = body.vptrAt00; // 5278C1先取Dialog表，再调用child.rect。
                RECT output;
                const auto* rectangle = child->vptrAt00->rectAt3C(child, &output);
                table->invalidateAt64(&body, *rectangle);
            }
            relatedLifetime.Finish(true);
        }
    }
    primaryLifetime.Finish(false);
}
ToolDialogNativeBody7Ch* DeleteToolDialogNativeAt4FCA80(Body* body, std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &ToolDialogNativeVtable();
    DestroyGuiClickedEventReferencesAt4F6D10(body->eventsAt70);
    DestroyGuiDialogAt4F7AC0(&body->common);
    if ((flags & 1) != 0) ::operator delete(body, std::size_t{0x7C});
    return body;
}
ToolDialogNativeScope::ToolDialogNativeScope(const ToolDialogNativeRelation& relation) noexcept
    : previous_(relationSource) { relationSource = &relation; }
ToolDialogNativeScope::~ToolDialogNativeScope() noexcept { relationSource = previous_; }
}
