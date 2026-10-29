#include "OptionCharSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::OptionCharSelectNativeDetail
{
void __fastcall ResetInput(Body* common, void*)
{
    // 510757先清+6C，再回调+30；成功后才清base+30。
    Dialog(common).pagingLatchAt6C = 0;
    common->vptrAt00->setActiveAt30(common, nullptr);
    common->inputLatchAt30 = 0;
}
std::int32_t __fastcall ActiveInput(Body* common, void*)
{
    if (common->activeAt4C->typeAt08 != 0) return 0;
    auto& body = Dialog(common);
    const auto latch = body.pagingLatchAt6C;
    const auto result = latch != 0;
    if ((latch == 1 && !common->vptrAt00->heldAt6C(common, 9)) ||
        (latch == 2 && !common->vptrAt00->heldAt6C(common, 10)))
    {
        Slot event = Copy(common->activeAt4C->eventAt3C);
        LocalReference lifetime{event};
        if (event.baseAt00)
        {
            auto* receiver = ResolveGuiClickedEvent(event);
            receiver->vptrAt00->invokeAt00(receiver);
        }
        body.pagingLatchAt6C = 0;
        lifetime.active = false;
        ReleaseGuiClickedEventReferenceAt4F64D0(event);
    }
    return result;
}
std::int32_t __fastcall UpperInput(Body* common, void*)
{
    auto& body = Dialog(common);
    if (body.pagingLatchAt6C) return GuiDialogBaseVtable().upperInputAt34(common);
    const auto& profile = CurrentOptionCharResourceProfile();
    const auto previous = common->vptrAt00->pressedAt70(common, 9);
    const auto* table = common->vptrAt00;
    std::int32_t event, latch;
    if (previous)
    {
        event = Sub(body.focusedAt84, 1);
        if (event < body.firstVisibleAt80) event = profile.previousBoundaryEvent;
        latch = 1;
    }
    else
    {
        if (!table->pressedAt70(common, 10)) return GuiDialogBaseVtable().upperInputAt34(common);
        event = Add(body.focusedAt84, 1);
        if (event >= Add(body.firstVisibleAt80, profile.visibleCount)) event = profile.nextBoundaryEvent;
        latch = 2;
        table = common->vptrAt00;
    }
    if (Legacy(profile))
    {
        // 01–07原循环跨整个数组跳state3。全禁用原本死循环；现代边界明确拒绝。
        const int count = static_cast<int>(profile.instances.size());
        int visited = 0;
        for (;;)
        {
            auto& candidate = RequiredChild(common, event);
            if (candidate.vptrAt00->readStateAt48(&candidate) != 3) break;
            if (++visited >= count) throw std::logic_error("Option character navigation has no enabled choice");
            event = Add(event, previous ? -1 : 1);
            if (event < 0) event = profile.previousBoundaryEvent;
            else if (event >= count) event = profile.nextBoundaryEvent;
        }
    }
    Slot child;
    const auto* returned = table->findChildAt14(common, &child, event);
    LocalReference lifetime{child};
    // 51F958/51F9D3缓存父表；53DDE0使用find返回的Ref地址，不假定等于局部out。
    table = common->vptrAt00;
    auto* receiver = ResolveGuiControl(*returned);
    if (!receiver) throw std::logic_error("Option character navigation selected a missing child");
    table->setActiveAt30(common, receiver);
    lifetime.active = false;
    ReleaseGuiControlReferenceAt4F6460(child);
    receiver = common->activeAt4C;
    body.pagingLatchAt6C = latch;
    reinterpret_cast<const GuiButtonNativeVtable29*>(receiver->vptrAt00)->setPressedAt70(receiver, 1);
    return 1;
}
}
