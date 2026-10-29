#include <Runtime/ToolDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Base = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
struct ReferenceLifetime final
{
    Slot& slot;
    bool active = true;
    ~ReferenceLifetime() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish() noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
    }
    void FinishCaptured(YtlReferenceControlRuntime* control) noexcept
    {
        active = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};
void Invalidate(Base& parent, GuiControlNativeBody50h* child)
{
    // 508640/531436/530E05：先捕获Dialog表，rect回调后才读取该表+64槽。
    const auto* table = parent.vptrAt00;
    RECT output;
    const auto* rect = child->vptrAt00->rectAt3C(child, &output);
    table->invalidateAt64(&parent, *rect);
}
void ResetAndClearEvent(Slot& selected, bool capturedRelease)
{
    auto* child = ResolveGuiControl(selected);
    child->vptrAt00->resetAt08(child);
    child = ResolveGuiControl(selected);
    Slot event;
    CopyGuiClickedEventReference(event, child->eventAt3C);
    auto* captured = event.controlAt08;
    ReferenceLifetime lifetime{event};
    auto* receiver = ResolveGuiClickedEvent(event);
    receiver->vptrAt00->clearAt04(receiver);
    if (capturedRelease) lifetime.FinishCaptured(captured); // 5313C0，state2 terminate。
    // 5085C7直接4F64D0：正常也清局部Ref；异常展开同一当前槽。
}
}
void SetToolControlEnabledNativeAt508470(Base& body, std::int32_t id, std::int32_t enabled)
{
    Slot selected;
    body.vptrAt00->findChildAt14(&body, &selected, id);
    ReferenceLifetime lifetime{selected};
    if (selected.baseAt00)
    {
        auto* child = ResolveGuiControl(selected);
        if (child->vptrAt00->readEnabledAt50(child))
        {
            child = ResolveGuiControl(selected);
            const auto previous = child->vptrAt00->readStateAt48(child);
            if (enabled) ResetAndClearEvent(selected, false);
            child = ResolveGuiControl(selected);
            child->vptrAt00->writeStateAt44(child, enabled ? 1 : 0);
            child = ResolveGuiControl(selected);
            if (child->vptrAt00->readStateAt48(child) != previous)
                Invalidate(body, ResolveGuiControl(selected));
        }
    }
    lifetime.Finish();
}
void SetToolControlCheckedNativeAt501FA0(Base& body, std::int32_t id, std::int32_t checked)
{
    Slot selected;
    body.vptrAt00->findChildAt14(&body, &selected, id);
    ReferenceLifetime lifetime{selected};
    if (selected.baseAt00)
    {
        auto* child = ResolveGuiControl(selected);
        if (child->vptrAt00->readEnabledAt50(child))
        {
            child = ResolveGuiControl(selected);
            if (child->typeAt08 == 2)
            {
                auto* box = reinterpret_cast<GuiCheckBoxNativeBody60h*>(ResolveGuiControl(selected));
                const auto previous = box->checkedAt5C;
                // 53DDE0(index=0)按当前三字Ref重解析，原参数比较完整DWORD。
                box = reinterpret_cast<GuiCheckBoxNativeBody60h*>(ResolveGuiControl(selected));
                box->checkedAt5C = checked ? 1 : 0;
                box = reinterpret_cast<GuiCheckBoxNativeBody60h*>(ResolveGuiControl(selected));
                if (box->checkedAt5C != previous) Invalidate(body, ResolveGuiControl(selected));
            }
        }
    }
    lifetime.Finish();
}
void ResetToolControlNativeAt5312B0(Base& body, std::int32_t id)
{
    Slot selected;
    body.vptrAt00->findChildAt14(&body, &selected, id);
    ReferenceLifetime lifetime{selected};
    if (selected.baseAt00)
    {
        auto* child = ResolveGuiControl(selected);
        if (child->vptrAt00->readEnabledAt50(child))
        {
            child = ResolveGuiControl(selected);
            const auto previous = child->vptrAt00->resourceStateAt38(child);
            ResetAndClearEvent(selected, true);
            child = ResolveGuiControl(selected);
            if (child->vptrAt00->resourceStateAt38(child) != previous)
                Invalidate(body, ResolveGuiControl(selected));
        }
    }
    lifetime.Finish();
}
void InvalidateToolControlsNativeAt530DC0(Base& body)
{
    for (auto* item = body.childrenAt0C.begin; item != body.childrenAt0C.end; ++item)
        Invalidate(body, ResolveGuiControl(*item));
}
std::int32_t ScanToolControlStateNativeAt50FD20(Base& body)
{
    for (auto* item = body.childrenAt0C.begin; item != body.childrenAt0C.end; ++item)
    {
        auto* child = ResolveGuiControl(*item);
        if (!child->vptrAt00->enabledAt1C(child)) continue;
        child = ResolveGuiControl(*item);
        const auto state = child->vptrAt00->resourceStateAt38(child);
        if (state == 4) return 1;
        if (state == 5) return 2;
    }
    return 0;
}
}
