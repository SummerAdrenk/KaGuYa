#include "OptionCharSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace OptionCharSelectNativeDetail;
using Child = GuiControlNativeBody50h;
bool CurrentLegacyChoice(const Child& child) noexcept
{ return child.eventAt04 == Dialog(static_cast<Body*>(child.parentAt4C)).focusedAt84; }
int __fastcall LegacyEnabled(Child* child, void*) noexcept
{ return child->enabledAt18 && child->stateAt14 != 3 && !CurrentLegacyChoice(*child); }
int __fastcall LegacyResourceState(Child* child, void*) noexcept
{
    // 01原4A4260/4A3A60不绘制或命中当前分类；null普通态不能补成On图。
    if (CurrentLegacyChoice(*child)) return 7;
    if (!child->enabledAt18 || child->stateAt14 == 3) return 3;
    if (GuiControlNativeEntries::ButtonField<0x50>(*child)) return 2;
    return child->hoverAt1C || child->focusAt20 ? 1 : 0;
}
int __fastcall LegacyHasHit(Child*, void*) noexcept { return 1; }
int __fastcall LegacyHitSlot(Child* child, void*)
{
    if (child->resourcesAt28.begin == child->resourcesAt28.end)
        throw std::logic_error("Native option character has no resource group");
    const auto& group = RequireGuiControlResource(*child->resourcesAt28.begin);
    const int slot = LegacyResourceState(child, nullptr);
    if (slot >= 0 && slot < 7 && group.slotIndicesAt00[slot] >= 0) return slot;
    // 原native命中在当前态无图时取已加载surface[0]；不是造一个normal资源。
    for (int i = 0; i < 7; ++i) if (group.slotIndicesAt00[i] == 0) return i;
    throw std::logic_error("Native option character has no first hit surface");
}
int __fastcall LegacyHit(Child* child, void*, int x, int y)
{
    if (!LegacyEnabled(child, nullptr)) return 0;
    return GuiControlNativeEntries::HitCheckBox(child, nullptr, x, y);
}
const GuiButtonNativeVtable29& LegacyButtonTable()
{
    static const auto table = []
    {
        auto value = GuiButtonVtable();
        value.common.enabledAt1C = reinterpret_cast<decltype(value.common.enabledAt1C)>(&LegacyEnabled);
        value.common.resourceStateAt38 = reinterpret_cast<decltype(value.common.resourceStateAt38)>(&LegacyResourceState);
        value.common.hitAt40 = reinterpret_cast<decltype(value.common.hitAt40)>(&LegacyHit);
        value.hasHitSlotAt64 = reinterpret_cast<decltype(value.hasHitSlotAt64)>(&LegacyHasHit);
        value.hitSlotAt68 = reinterpret_cast<decltype(value.hitSlotAt68)>(&LegacyHitSlot);
        return value;
    }();
    return table;
}
struct ConstructionUnwind final
{
    OptionCharSelectDialogNativeBody8Ch& body;
    bool active = true;
    ~ConstructionUnwind() noexcept
    {
        if (!active) return;
        // 5FA4A8 state1->0：先4F63F0，再4F7AC0，不调用派生deleting destructor。
        ReleaseYtlReference(body.clickedManagerAt5C);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
void AppendButton(OptionCharSelectDialogNativeBody8Ch& body, const GuiButtonDescriptorNative2Ch& descriptor,
    bool legacy, RECT& bounds)
{
    auto& dependencies = RequireOptionCharSelectDialogNativeDependencies();
    UnwrittenReference childStorage;
    auto& child = childStorage.slot;
    CreateGuiButtonAt5041D0(child, &body.common, descriptor,
        dependencies.loader, dependencies.surfaces, dependencies.strings);
    LocalReference childLifetime{child};
    auto* receiver = ResolveGuiControl(child);
    if (legacy)
    {
        receiver->vptrAt00 = &LegacyButtonTable().common;
        receiver->stateAt14 = 0;
    }
    RECT output;
    const auto* rectangle = receiver->vptrAt00->rectAt3C(receiver, &output);
    ::UnionRect(&bounds, &bounds, rectangle);

    // 与4E2F5A同序：new事件成功后才解析child/+04；control失败没有event展开资格。
    UnwrittenReference eventStorage;
    auto& event = eventStorage.slot;
    CreateToolClickedEventAt4E2F5A(event, child);
    LocalReference eventLifetime{event};
    auto* const eventControl = event.controlAt08; // 原ESI跨绑定/增长保持。
    Slot binding{event.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    BindPopupGuiControlEventAt4EA856(*ResolveGuiControl(child), binding, event);

    Slot upcast = Copy(child);
    LocalReference upcastLifetime{upcast};
    auto* upcastControl = upcast.controlAt08;
    auto& children = body.common.childrenAt0C;
    if (children.end == children.capacity)
    {
        GrowGuiControlChildrenAt4D93B0(children, children.end, upcast);
        upcastControl = upcast.controlAt08; // 4E4E9C增长后重读var_80。
    }
    else AppendGuiControlChildReference(children, upcast);
    upcastLifetime.Captured(upcastControl);

    Slot eventArgument{event.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    LocalReference eventArgumentLifetime{eventArgument};
    auto* argumentControl = eventControl;
    // 4E4EF5在复制增持之后才解析manager，不能缓存构造开始时的裸地址。
    AppendPopupClickedEventReferenceAt4EA97F(*Manager(body.clickedManagerAt5C),
        eventArgument, argumentControl);
    eventArgumentLifetime.Captured(argumentControl);
    eventLifetime.Captured(eventControl);
    childLifetime.Reload();
}
}
OptionCharSelectDialogNativeBody8Ch* ConstructOptionCharSelectDialogAt4E4A20(
    OptionCharSelectDialogNativeBody8Ch* body)
{
    const auto& profile = CurrentOptionCharResourceProfile();
    ValidateOptionCharResourceProfile(profile);
    auto& dependencies = RequireOptionCharSelectDialogNativeDependencies();
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->common.vptrAt00 = &OptionCharSelectDialogNativeVtable().common;
    body->clickedManagerAt5C = {};
    ConstructionUnwind construction{*body};
    body->boundsAt70 = {};
    auto* manager = dependencies.source.NewClickedEventManagerAt4E4AAD();
    dependencies.source.AdoptClickedEventManagerAt4D7B90(body->clickedManagerAt5C, manager);
    RECT translatedBounds{}, resourceBounds{};
    for (const auto& instance : profile.instances)
    {
        if (Legacy(profile))
        {
            const auto& native = profile.nativeRecords[instance.normalRecord];
            GuiButtonDescriptorNative2Ch descriptor{};
            descriptor.common.eventAt00 = instance.event;
            for (int i = 0; i < 7; ++i) descriptor.common.namesAt04[i] = native.namesAt00[i];
            descriptor.childIdAt28 = -1;
            // 01–07原loader setPosition=0：不应用record的零POINT，保留资源natural原点。
            AppendButton(*body, descriptor, true, resourceBounds);
        }
        else
        {
            int record = instance.normalRecord;
            if (instance.characterDisplayIndex >= 0)
            {
                const auto available = *dependencies.source.AvailabilityAt4E4C74(instance.characterDisplayIndex);
                if (!available && instance.unavailableRecord >= 0) record = instance.unavailableRecord;
            }
            AppendButton(*body, profile.records[record], false, resourceBounds);
        }
    }
    if (Legacy(profile)) translatedBounds = resourceBounds;
    else
    {
        const auto width = Sub(resourceBounds.right, resourceBounds.left);
        const auto height = Sub(resourceBounds.bottom, resourceBounds.top);
        for (const auto& point : profile.positions)
        {
            RECT translated{point.x, point.y, Add(point.x, width), Add(point.y, height)};
            ::UnionRect(&translatedBounds, &translatedBounds, &translated);
        }
    }
    // 08原4F0A85：两个导航descriptor有自己的POINT，并参与整体边界。
    for (const auto& descriptor : profile.navigationButtons) AppendButton(*body, descriptor, false, translatedBounds);
    body->boundsAt70 = translatedBounds;
    // 4E505C先发布bounds，再new4/536EA0；input control失败不回滚新base。
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    construction.active = false;
    return body;
}
}
