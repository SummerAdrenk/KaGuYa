#include "CgOperationDialogNative.Internal.h"

#include <algorithm>

namespace KaGuYa::Start::Runtime::CgOperationNativeDetail
{
void ClearSurfaceAt4143A0(Surface* surface)
{
    const auto handle = surface->handleAt04;
    if (!handle) return;
    const auto color = surface->clearColorAt20;
    Services().composition.ClearSurface(handle, color, nullptr);
}
void BlitAt4425C0(Surface* destination, Surface* source, std::int32_t x, std::int32_t y, const RECT* clip)
{
    const auto destinationHandle = destination->handleAt04;
    if (!destinationHandle) return;
    const auto sourceHandle = source->handleAt04;
    if (!sourceHandle) return;
    static_cast<void>(Services().composition.BlitSurface(destinationHandle, sourceHandle,
        x, y, nullptr, nullptr, clip, 0));
}
Slot DigitFor(Dialog& parent, std::int32_t digit, CgOperationDigitUse use)
{
    if(digit<0||digit>=10)throw std::out_of_range("CG operation decimal digit is outside 0..9");
    const auto slot=Profile().digitSlots[static_cast<std::size_t>(use)][digit];
    const auto count = GuiControlSurfaceReferenceCount(parent.digitsAt8C);
    if (slot<0||slot>=count)throw std::out_of_range("CG operation digit surface is outside the loaded atlas");
    return Copy(parent.digitsAt8C.begin[slot]);
}
void ApplySelectionAt537010(Buttons& body, std::int32_t selection)
{
    ClearSurfaceAt4143A0(Resolve<Surface>(body.surfacesAt68[0]));
    ClearSurfaceAt4143A0(Resolve<Surface>(body.surfacesAt68[1]));
    const auto& profile=Profile();
    // 01原4A1DB4/4A1DBB为无符号cmp/jnb；18原53708D后为signed jge，负数域不能混同。
    if(profile.unsignedPrimaryGuard?static_cast<std::uint32_t>(selection)>=1000u:selection>=1000)return;
    auto* target = Resolve<Surface>(body.surfacesAt68[0]);
    const auto originX = target->originXAt10;
    const auto originY = target->originYAt14;
    auto remaining = Add(selection, 1);
    for (int place = 2; place >= 0; --place)
    {
        const auto divisor = place == 2 ? 100 : place == 1 ? 10 : 1;
        const auto digit = remaining / divisor;
        const auto x = Sub(profile.primaryPoints[place].x, originX);
        const auto y = Sub(profile.primaryPoints[place].y, originY);
        for (int variant = 0; variant != 2; ++variant)
        {
            Slot resource = DigitFor(*reinterpret_cast<Dialog*>(body.common.parentAt04), digit,
                variant==0?CgOperationDigitUse::Primary:CgOperationDigitUse::PrimaryInactive);
            RefGuard resourceGuard{resource};
            auto* captured = resource.controlAt08;
            auto* source = Resolve<Surface>(resource);
            auto* destination = Resolve<Surface>(body.surfacesAt68[variant]);
            BlitAt4425C0(destination, source, x, y);
            resourceGuard.Captured(captured);
        }
        remaining = Sub(remaining, Mul(digit, divisor));
    }
}
void AppendPopupItemAt4FEAE0(Popup& body, std::int32_t index, std::int32_t available)
{
    const auto& profile=Profile();
    if(index<0)throw std::out_of_range("CG operation popup ordinal is negative");
    const auto page = index / profile.pageCapacity;
    if (page >= GuiControlSurfaceReferenceCount(body.pagesAt88))
    {
        if(page!=GuiControlSurfaceReferenceCount(body.pagesAt88))
            throw std::logic_error("CG operation popup append is not sequential");
        Slot created;
        CompatibleSurfaceAt444470(created, *Resolve<Surface>(body.pagesAt88.begin[0]));
        RefGuard createdGuard{created};
        auto* first = Resolve<Surface>(body.pagesAt88.begin[0]);
        auto* target = Resolve<Surface>(created);
        target->originXAt10 = first->originXAt10;
        target->originYAt14 = first->originYAt14;
        // 01原4A6CA0/4A6E90每页先铺整张板；旧50格没有现代逐格stamp。
        if(NativeVisuals())BlitAt4425C0(target,Resolve<Surface>(body.backgroundAt7C),0,0);
        AppendSurface(body.pagesAt88, created);
        createdGuard.Reload();
    }
    const auto item = index % profile.pageCapacity;
    auto* const selected = body.pagesAt88.begin + page;
    auto* target = Resolve<Surface>(*selected);
    const auto originX = target->originXAt10;
    const auto originY = target->originYAt14;
    const auto* coordinates = profile.popupPoints[item].data();
    if(!NativeVisuals())
    {
        auto* background = Resolve<Surface>(body.backgroundAt7C);
        if(!background)throw std::logic_error("CG operation popup stamp is absent");
        target = Resolve<Surface>(*selected);
        BlitAt4425C0(target, background, Sub(Sub(coordinates[2].x, originX),profile.cellOffset.x),
            Sub(Sub(coordinates[2].y, originY),profile.cellOffset.y));
        if (index >= 1000) return;
    }
    auto remaining = Add(index, 1);
    for (int place = 2; place >= 0; --place)
    {
        const auto divisor = place == 2 ? 100 : place == 1 ? 10 : 1;
        const auto digit = remaining / divisor;
        const auto x = Sub(coordinates[place].x, originX);
        const auto y = Sub(coordinates[place].y, originY);
        Slot resource = DigitFor(*reinterpret_cast<Dialog*>(body.common.parentAt04), digit,
            available?CgOperationDigitUse::Popup:CgOperationDigitUse::PopupUnavailable);
        RefGuard resourceGuard{resource};
        auto* captured = resource.controlAt08;
        if (resource.baseAt00)
        {
            auto* source = Resolve<Surface>(resource);
            target = Resolve<Surface>(body.pagesAt88.begin[page]);
            BlitAt4425C0(target, source, x, y);
        }
        remaining = Sub(remaining, Mul(digit, divisor));
        resourceGuard.Captured(captured);
    }
}
void SelectPageAt527500(Popup& body, std::int32_t page)
{
    if(page<0||page>=GuiControlSurfaceReferenceCount(body.pagesAt88))
        throw std::out_of_range("CG operation popup page is outside the cache");
    if (body.pageAt94 == page) return;
    auto& common = body.common;
    if (page < GuiControlSurfaceReferenceCount(body.pagesAt88))
    {
        Slot child;
        common.vptrAt00->findChildAt14(&common, &child, 9);
        RefGuard childGuard{child};
        if (child.baseAt00)
        {
            Slot group = Copy(Resolve<GuiControlNativeBody50h>(child)->resourcesAt28.begin[0]);
            RefGuard groupGuard{group};
            if (group.baseAt00)
            {
                auto* selectedGroup = Resolve<GuiControlResourceNativeBody38h>(group);
                auto& surfaces = selectedGroup->surfacesAt1C;
                ReleaseGuiControlSurfaceRangeAt43EA10(surfaces.begin, surfaces.end);
                surfaces.end = surfaces.begin;
                Slot argument = Copy(body.pagesAt88.begin[page]);
                AppendGuiControlSurfaceArgumentAt538350(*Resolve<GuiControlResourceNativeBody38h>(group), 0, argument);
            }
            groupGuard.active = false;
            ReleaseGuiControlResourceReferenceAt4F6380(group);
        }
        childGuard.Reload();
    }
    body.pageAt94 = page;
    SyncPopupControls(body);
    common.vptrAt00->resetInputAt3C(&common);
}
void SyncPopupControls(Popup& body)
{
    const auto& profile=Profile();
    auto& common=body.common;
    auto* parent = reinterpret_cast<Dialog*>(common.parentAt04);
    const auto bytes = Sub(std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(parent->availabilityAt80.end))),
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(parent->availabilityAt80.begin))));
    const auto count = bytes / 4;
    const auto start = Mul(body.pageAt94, profile.pageCapacity);
    const auto visible = Sub((std::min)(count, Add(start, profile.pageCapacity)), start);
    for (std::int32_t i = 0; i != profile.pageCapacity; ++i)
    {
        Slot child;
        common.vptrAt00->findChildAt14(&common, &child, Add(i, ItemEventBase));
        RefGuard childGuard{child};
        if (child.baseAt00)
        {
            auto* receiver = Resolve<GuiControlNativeBody50h>(child);
            receiver->vptrAt00->writeEnabledAt4C(receiver, i < visible);
        }
        childGuard.Reload(true);
    }
    const auto pages=GuiControlSurfaceReferenceCount(body.pagesAt88);
    for(int i=0;i<2;++i)
    {
        Slot child;
        common.vptrAt00->findChildAt14(&common,&child,PreviousPageEvent+i);
        RefGuard guard{child};
        if(auto* receiver=Resolve<GuiControlNativeBody50h>(child))
        {
            const bool clamped=profile.pagePolicy==CgOperationPagePolicy::ClampedButtonsAndWheel;
            const bool visible=!clamped||(pages>1&&(i==0?body.pageAt94>0:body.pageAt94+1<pages));
            receiver->vptrAt00->writeEnabledAt4C(receiver,visible);
            receiver->vptrAt00->writeStateAt44(receiver,clamped?1:pages>1);
        }
    }
}
void ResetPopupAt5021C0(Popup& body)
{
    SelectPageAt527500(body, 0);
    Slot first = Copy(body.pagesAt88.begin[0]);
    RefGuard firstGuard{first};
    auto* captured = first.controlAt08;
    ClearSurfaceAt4143A0(Resolve<Surface>(first));
    if(NativeVisuals())BlitAt4425C0(Resolve<Surface>(first),Resolve<Surface>(body.backgroundAt7C),0,0);
    if (static_cast<std::uint32_t>(GuiControlSurfaceReferenceCount(body.pagesAt88)) > 1u)
    {
        ReleaseGuiControlSurfaceRangeAt43EA10(body.pagesAt88.begin, body.pagesAt88.end);
        body.pagesAt88.end = body.pagesAt88.begin;
        if (body.pagesAt88.end != body.pagesAt88.capacity)
        {
            CopyGuiControlResourceReference(*body.pagesAt88.end, first);
            ++body.pagesAt88.end;
        }
        else
        {
            GrowSurfacesAt4D83D0(body.pagesAt88, body.pagesAt88.end, first);
            captured = first.controlAt08; // 50229C仅增长路重取。
        }
    }
    firstGuard.Captured(captured);
    SyncPopupControls(body);
}
}
