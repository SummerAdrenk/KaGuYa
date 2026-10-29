#include "CgOperationDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::CgOperationNativeDetail
{
namespace
{
using Child=GuiControlNativeBody50h;
bool IsCell(const Child& child) noexcept { return child.eventAt04>=ItemEventBase; }
bool CellAvailable(const Child& child)
{
    const auto& popup=*static_cast<const Popup*>(child.parentAt4C);
    const auto& parent=*reinterpret_cast<const Dialog*>(popup.common.parentAt04);
    const auto ordinal=Add(Mul(popup.pageAt94,Profile().pageCapacity),Sub(child.eventAt04,ItemEventBase));
    const auto count=parent.availabilityAt80.begin?parent.availabilityAt80.end-parent.availabilityAt80.begin:0;
    return ordinal>=0&&ordinal<count&&parent.availabilityAt80.begin[ordinal]!=0;
}
int __fastcall NativeState(Child* child,void*) noexcept
{
    // 向共享导航报告GUI状态1/2，避免把尾页隐藏项当作可聚焦按钮。
    if(!child->enabledAt18)return 2;
    if(!child->stateAt14)return 1;
    if(GuiControlNativeEntries::ButtonField<0x50>(*child))return 5;
    return child->hoverAt1C?4:child->focusAt20?3:0;
}
int NativeSlot(int guiSlot)
{
    switch(guiSlot){case 0:return 0;case 1:case 2:return 3;case 3:case 4:return 1;case 5:return 2;}
    throw std::out_of_range("CG operation native resource state has no GUI projection");
}
Slot* __fastcall NativeResource(Child* child,void*,Slot* out,int slot)
{
    *out={};
    if(slot>=7)return out;
    if(slot<0)throw std::out_of_range("CG operation native image slot is negative");
    if(!child->enabledAt18)return out;
    slot=NativeSlot(slot);
    const auto groupIndex=IsCell(*child)&&!CellAvailable(*child)?1u:0u;
    const auto& groups=child->resourcesAt28;
    if(!groups.begin||groups.end-groups.begin<=static_cast<std::ptrdiff_t>(groupIndex))
        throw std::logic_error("CG operation native image group is absent");
    const auto& group=RequireGuiControlResource(groups.begin[groupIndex]);
    const auto image=group.slotIndicesAt00[slot];
    if(image<0)return out;
    if(image>=GuiControlSurfaceReferenceCount(group.surfacesAt1C))
        throw std::out_of_range("CG operation native image map is invalid");
    CopyGuiControlResourceReference(*out,group.surfacesAt1C.begin[image]);
    return out;
}
int __fastcall NativeHasHit(Child* child,void*) noexcept { return !IsCell(*child); }
int __fastcall NativeHitSlot(Child* child,void*)
{
    if(child->resourcesAt28.begin==child->resourcesAt28.end)
        throw std::logic_error("CG operation native hit group is absent");
    const auto& group=RequireGuiControlResource(*child->resourcesAt28.begin);
    const auto state=NativeState(child,nullptr);
    if(group.slotIndicesAt00[NativeSlot(state)]>=0)return state;
    // 原native命中取当前态，缺席时取已加载surface[0]；不填补null normal。
    constexpr int projected[]{0,3,5,1};
    for(int i=0;i<4;++i)if(group.slotIndicesAt00[i]==0)return projected[i];
    throw std::logic_error("CG operation native hit surface is absent");
}
int __fastcall NativeHit(Child* child,void*,int x,int y)
{
    if(!child->enabledAt18||!child->stateAt14)return 0;
    return GuiControlNativeEntries::HitCheckBox(child,nullptr,x,y);
}
}
const GuiButtonNativeVtable29& NativeOperationButtonMethods() noexcept
{
    static const auto table=[]()
    {
        auto value=GuiButtonVtable();
        value.common.resourceStateAt38=reinterpret_cast<decltype(value.common.resourceStateAt38)>(&NativeState);
        value.common.hitAt40=reinterpret_cast<decltype(value.common.hitAt40)>(&NativeHit);
        value.resourceAt5C=reinterpret_cast<decltype(value.resourceAt5C)>(&NativeResource);
        value.hasHitSlotAt64=reinterpret_cast<decltype(value.hasHitSlotAt64)>(&NativeHasHit);
        value.hitSlotAt68=reinterpret_cast<decltype(value.hitSlotAt68)>(&NativeHitSlot);
        return value;
    }();
    return table;
}
}

