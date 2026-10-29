#include "CgOperationDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace CgOperationNativeDetail;
void FindDialog(Dialog& body, Slot& output, std::int32_t id)
{
    auto& common = body.into.common;
    reinterpret_cast<const GuiDialogIntoNativeVtable37*>(common.vptrAt00)->findDialogAt90(&common, &output, id);
}
std::int32_t ReadResult(Slot& reference)
{
    auto* receiver = Resolve<Common>(reference);
    return receiver->vptrAt00->resultAt50(receiver);
}
void InvalidateChild(Dialog& body, Slot& reference)
{
    auto* child = Resolve<Common>(reference);
    auto* const dialogTable = body.into.common.vptrAt00;
    RECT rectangle;
    const auto* childTable = reinterpret_cast<const Vtable36*>(child->vptrAt00);
    const auto* result = childTable->boundsAt8C(child, &rectangle);
    dialogTable->invalidateAt64(&body.into.common, *result);
}
void InvalidateDialog(Dialog& body)
{
    const auto rectangle = body.boundsAt68;
    body.into.common.vptrAt00->invalidateAt64(&body.into.common, rectangle);
}
void ClosePopup(Dialog& body, Slot& primary, Slot& popup)
{
    body.resultAt78 = 3;
    auto* child = Resolve<Common>(primary);
    child->vptrAt00->writeStateAt1C(child, 1);
    child = Resolve<Common>(primary);
    child->vptrAt00->resetChildrenAt68(child);
    child = Resolve<Common>(popup);
    child->vptrAt00->writeEnabledAt24(child, 0);
    InvalidateDialog(body);
}
bool PopupCloseInput()
{
    // 52C5DE与52C613分别取根；先keyboard，只有false才取mouse，不能提前缓存两者。
    auto* root = Services().input.CaptureAt418210();
    if (ReadKeyboardPressedAt47F4E0(*Resolve<KeyboardManagerNativeBody438h>(root->keyboardAt18), 0)) return true;
    root = Services().input.CaptureAt418210();
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(root->mouseAt00);
    return !mouse->previousAt30[1] && mouse->pressedAt10[1] && !mouse->suppressionAt28;
}
std::int32_t AvailabilityCount(Dialog& body) noexcept
{
    const auto bytes = Sub(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(body.availabilityAt80.end))),
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(body.availabilityAt80.begin))));
    return bytes / 4;
}
int ApplyPrimaryAction(Dialog& body,Slot& primary,CgOperationAction action)
{
    auto* receiver=Resolve<Common>(primary);
    switch(action)
    {
    case CgOperationAction::Return:body.resultAt78=0;break;
    case CgOperationAction::Hide:body.resultAt78=1;break;
    case CgOperationAction::OpenPopup:
    {
        body.resultAt78=2;
        receiver->vptrAt00->writeStateAt1C(receiver,0);
        Slot popup;FindDialog(body,popup,1);RefGuard guard{popup};
        if(popup.baseAt00)
        {
            auto* child=Resolve<Common>(popup);child->vptrAt00->writeEnabledAt24(child,1);
            SelectPageAt527500(*Resolve<Popup>(popup),body.selectionAt7C/Profile().pageCapacity);
            InvalidateDialog(body);
        }
        break;
    }
    case CgOperationAction::Previous:case CgOperationAction::Next:
    {
        const int count=AvailabilityCount(body);
        if(count<=0)break;
        if(body.selectionAt7C<0||body.selectionAt7C>=count)
            throw std::out_of_range("CG operation selection is outside availability");
        const int step=action==CgOperationAction::Previous?-1:1;
        int next=body.selectionAt7C;
        for(int visited=0;visited<count;++visited)
        {
            next=Add(next,step);
            if(next<0)next=count-1;else if(next>=count)next=0;
            if(next==body.selectionAt7C)break;
            if(!body.availabilityAt80.begin[next])continue;
            ApplySelectionAt537010(*Resolve<Buttons>(primary),next);
            InvalidateChild(body,primary);body.selectionAt7C=next;body.resultAt78=4;break;
        }
        break;
    }
    default:throw std::logic_error("Unknown CG operation action");
    }
    return body.resultAt78;
}
}

namespace CgOperationNativeDetail
{
int DispatchAction(Dialog& body,CgOperationAction action)
{
    body.resultAt78=-1;
    Slot primary;FindDialog(body,primary,0);RefGuard guard{primary};
    if(!primary.baseAt00)throw std::logic_error("CG operation primary dialog is absent");
    return ApplyPrimaryAction(body,primary,action);
}
}

std::int32_t PollCgOperationDialogNativeAt52C2D0(CgOperationDialogNativeBodyA8h& body)
{
    using namespace CgOperationNativeDetail;
    body.resultAt78 = -1;
    Slot primary;
    FindDialog(body, primary, 0);
    RefGuard primaryGuard{primary};
    if (primary.baseAt00)
    {
        auto* receiver = Resolve<Common>(primary);
        if (receiver->vptrAt00->readStateAt20(receiver))
        {
            receiver = Resolve<Common>(primary);
            receiver->vptrAt00->pollAt48(receiver);
            // 五次结果读取是原可观察调用次序，不能合并为switch一次读取。
            if (ReadResult(primary) == 7) ApplyPrimaryAction(body,primary,CgOperationAction::Return);
            else if (ReadResult(primary) == 6) ApplyPrimaryAction(body,primary,CgOperationAction::Hide);
            else if (ReadResult(primary) == 5) ApplyPrimaryAction(body,primary,CgOperationAction::Next);
            else if (ReadResult(primary) == 3) ApplyPrimaryAction(body,primary,CgOperationAction::Previous);
            else if (ReadResult(primary) == 4) ApplyPrimaryAction(body,primary,CgOperationAction::OpenPopup);
        }
        else
        {
            Slot popup;
            FindDialog(body, popup, 1);
            RefGuard popupGuard{popup};
            if (popup.baseAt00)
            {
                if (PopupCloseInput()) ClosePopup(body, primary, popup);
                else
                {
                    receiver = Resolve<Common>(popup);
                    receiver->vptrAt00->pollAt48(receiver);
                    auto event = ReadResult(popup);
                    const auto& profile=Profile();
                    // 旧CPopup先处理item，只有无item结果时才处理滚轮；负前页、正后页。
                    if(event<0&&profile.pagePolicy==CgOperationPagePolicy::ClampedButtonsAndWheel)
                    {
                        auto* input=Services().input.CaptureAt418210();
                        auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
                        const auto wheel=mouse->secondaryAt2C?0:mouse->wheelAt34;
                        if(wheel<0)event=PreviousPageEvent;else if(wheel>0)event=NextPageEvent;
                    }
                    if (event == PreviousPageEvent||event == NextPageEvent)
                    {
                        auto* pageOwner = Resolve<Popup>(popup);
                        const auto count = GuiControlSurfaceReferenceCount(pageOwner->pagesAt88);
                        auto page=Add(pageOwner->pageAt94,event==PreviousPageEvent?-1:1);
                        if(profile.pagePolicy==CgOperationPagePolicy::WrappingButtons)
                        {if(page<0)page=count-1;else if(page>=count)page=0;}
                        else {if(page<0)page=0;else if(page>=count)page=count-1;}
                        if(page!=pageOwner->pageAt94)
                        {SelectPageAt527500(*pageOwner,page);InvalidateChild(body,popup);}
                    }
                    else if (static_cast<std::uint32_t>(Sub(event,ItemEventBase)) < static_cast<std::uint32_t>(profile.pageCapacity))
                    {
                        const auto page = Resolve<Popup>(popup)->pageAt94;
                        const auto index = Add(Sub(event,ItemEventBase), Mul(page,profile.pageCapacity));
                        // 原52C754只有signed上界检查，不能附加负数修正或跳到第一页。
                        if (index < AvailabilityCount(body) && body.availabilityAt80.begin[index])
                        {
                            if (index == body.selectionAt7C)
                            {
                                Slot target;
                                receiver = Resolve<Common>(popup);
                                receiver->vptrAt00->findChildAt14(receiver, &target, event);
                                RefGuard targetGuard{target};
                                if (target.baseAt00)
                                {
                                    receiver = Resolve<Common>(popup);
                                    receiver->vptrAt00->resetChildrenAt68(receiver);
                                    receiver = Resolve<Common>(popup);
                                    receiver->vptrAt00->setActiveAt30(receiver, nullptr);
                                    receiver = Resolve<Common>(popup);
                                    auto* const table = receiver->vptrAt00;
                                    table->setActiveAt30(receiver, Resolve<GuiControlNativeBody50h>(target));
                                }
                                targetGuard.active = false;
                                ReleaseGuiControlReferenceAt4F6460(target);
                                InvalidateDialog(body);
                            }
                            else
                            {
                                ApplySelectionAt537010(*Resolve<Buttons>(primary), index);
                                body.selectionAt7C = index;
                                body.resultAt78 = 4;
                                receiver = Resolve<Common>(primary);
                                receiver->vptrAt00->writeStateAt1C(receiver, 1);
                                receiver = Resolve<Common>(primary);
                                receiver->vptrAt00->resetChildrenAt68(receiver);
                                receiver = Resolve<Common>(popup);
                                receiver->vptrAt00->writeEnabledAt24(receiver, 0);
                                InvalidateDialog(body);
                            }
                        }
                    }
                }
            }
            popupGuard.Reload(true);
        }
    }
    primaryGuard.Reload();
    return 1;
}
}
