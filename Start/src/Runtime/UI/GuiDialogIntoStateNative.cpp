#include <Runtime/GuiDialogIntoStateNative.h>
#include "GuiDialogIntoNative.Internal.h"
#include <Runtime/GuiControlNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Common=GuiDialogNativeBody5Ch;
using Slot=YtlReferenceSlotRuntime;
using Argument=GuiDialogIntoNativeDetail::Argument;
union Fresh final { Slot value;Fresh() noexcept{}~Fresh() noexcept{} };
constexpr std::array<std::int32_t,6> hitIds{2,3,4,5,6,7};
Common* Resolve(Slot& value) noexcept { return ResolveEnvironmentObjectReference<Common>(value); }
void FindFirst(Common& parent,Slot& output)
{
    auto* table=reinterpret_cast<const GuiDialogIntoNativeVtable37*>(parent.vptrAt00);
    (void)table->findDialogAt90(&parent,&output,0);
}
void FinishCleared(Argument& argument) noexcept
{
    argument.active=false;
    ReleaseYtlReference(argument.slot);
}
}
std::span<const std::int32_t,6> NestedGuiHitIdsSample18() noexcept {return hitIds;}
std::int32_t ReadFirstNestedGuiInactiveAt5103C0(Common& body)
{
    Fresh first;FindFirst(body,first.value);
    Argument cleanup{first.value};
    std::int32_t result=0;
    if(first.value.baseAt00)
    {
        auto* child=Resolve(first.value);
        result=child->vptrAt00->readStateAt20(child)==0?1:0;
    }
    // FuncInfo5FA704 state1是terminate；normal尾不清三元槽。
    cleanup.Finish();return result;
}
std::int32_t HitNestedGuiControlsAt50BAB0(Common& body,POINT point)
{
    Fresh first;FindFirst(body,first.value);
    Argument firstCleanup{first.value};
    if(first.value.baseAt00)
    {
        auto* nested=Resolve(first.value);
        if(nested->vptrAt00->readStateAt20(nested)==0)
        {firstCleanup.Finish();return 1;}
        // 50BB65在readState回调之后重新按当前Ref解析，bounds位于实际派生+80。
        nested=Resolve(first.value);
        const auto* rectangle=reinterpret_cast<const RECT*>(reinterpret_cast<const std::byte*>(nested)+0x80);
        if(::PtInRect(rectangle,point))
        {
            Fresh control;
            for(auto id:hitIds)
            {
                // EDI receiver跨本循环保持；不因子Find/Hit回调重解nested。
                (void)nested->vptrAt00->findChildAt14(nested,&control.value,id);
                Argument controlCleanup{control.value};
                if(control.value.baseAt00)
                {
                    auto* child=ResolveGuiControl(control.value);
                    if(child->vptrAt00->hitAt40(child,point.x,point.y))
                    {
                        controlCleanup.Finish();
                        // 50BC95显式调用4F65B0：这一normal路径清parent槽。
                        FinishCleared(firstCleanup);return 1;
                    }
                }
                // 50BBE3 state4：继续循环的normal child尾清base/control、保留bias。
                FinishCleared(controlCleanup);
            }
        }
    }
    firstCleanup.Finish();return 0;
}
}
