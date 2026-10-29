#include "SaveLoadMessageBoxNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <vector>
#include <Runtime/GuiPanelNative.h>
#include <Runtime/GuiSliderNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadMessageBoxNativeDetail;
Slot* CreateChild(FreshReference& result, Common* parent, std::int32_t type,
    const void* descriptor, SaveLoadMessageBoxNativeServices& s)
{
    switch (type)
    {
    case 0: return CreateGuiStaticAt505800(result.slot, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, s.surfaces, s.strings);
    case 1: return CreateGuiButtonAt5041D0(result.slot, parent,
        *static_cast<const GuiButtonDescriptorNative2Ch*>(descriptor), s.loader, s.surfaces, s.strings);
    case 2: return s.childFactories.CreateAt5043B0(&result.slot, parent, descriptor);
    case 3: return CreateGuiSliderAt5052C0(result.slot, parent,
        *static_cast<const GuiSliderDescriptorNative6Ch*>(descriptor), s.loader, s.surfaces, s.strings);
    case 4: return s.childFactories.CreateAt503BB0(&result.slot, parent, descriptor);
    case 5: return s.childFactories.CreateAt503E60(&result.slot, parent, descriptor);
    case 6: return s.childFactories.CreateAt505090(&result.slot, parent, descriptor);
    case 7: return CreateGuiPanelAt504E90(result.slot, parent,
        *static_cast<const GuiStaticDescriptorNative24h*>(descriptor), s.loader, s.surfaces, s.strings);
    default: throw 1; // 原4ED156，不能以null child或未知modern控件伪成功。
    }
}
constexpr std::uint32_t strides[]{0x24, 0x2C, 0x48, 0x6C, 0x44, 0x2C, 0x2C, 0x24};

void Populate(Body& b, const SaveLoadMessageBoxNativeControlGroup& group,
    SaveLoadMessageBoxNativeServices& s)
{
    for (std::int32_t index = 0; index < Read(group.countAt04); ++index)
    {
        // state3已保护零child；factory返回后才取得其fresh Ref的展开资格。
        Slot child{};
        LocalReference childLifetime{child};
        const auto type = Read(group.typeAt00);
        if (std::uint32_t(type) > 7u) throw 1;
        const auto* descriptor = reinterpret_cast<const void*>(
            reinterpret_cast<std::uintptr_t>(Read(group.descriptorsAt08)) + std::uint32_t(index) * strides[type]);
        FreshReference factory;
        auto* returned = CreateChild(factory, &b.common, type, descriptor, s);
        LocalReference factoryLifetime{factory.slot}; // states4/6/8/10/12/14/16/18。
        if (&child != returned)
        {
            // 原逐字段拷贝返回值，再增持；不要用赋值setter释放零child。
            child.controlAt08 = returned->controlAt08;
            child.baseAt00 = returned->baseAt00;
            child.biasAt04 = returned->biasAt04;
            if (child.controlAt08) ++child.controlAt08->referenceCountAt04;
        }
        const Slot capturedChild = child; // EBX/EDI/ESI在factory尾释放之前固定。
        factoryLifetime.Current(); // states5/7/.../19：尾析构异常terminate，不重试。

        auto* selected = Resolve<GuiControlNativeBody50h>(capturedChild);
        if (Read(selected->typeAt08) != 1)
        {
            FreshReference event;
            // 原new0C之后才重解cachedChild并读取+04；new18失败不补删孤立正文。
            CreateToolClickedEventAt4E2F5A(event.slot, capturedChild);
            LocalReference eventLifetime{event.slot}; // state20。
            auto* capturedEvent = event.slot.controlAt08;
            Slot binding{event.slot.baseAt00, 0, capturedEvent};
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
            {
                LocalReference bindingLifetime{binding}; // state21。
                selected = Resolve<GuiControlNativeBody50h>(capturedChild);
                auto& target = selected->eventAt3C;
                if (&target != &binding)
                {
                    ReleaseWithoutClearing(target);
                    target.baseAt00 = event.slot.baseAt00; // 原旧release后才读var30 base。
                    target.biasAt04 = 0;
                    target.controlAt08 = capturedEvent;
                    if (capturedEvent) ++capturedEvent->referenceCountAt04;
                }
                selected->eventEnabledAt48 = 1;
                bindingLifetime.Captured(capturedEvent); // state22。
            }
            // 原events追加在children之前；快路用缓存EDI，增长路返回后才更新EDI。
            AppendPopupClickedEventReferenceAt4EA97F(b.eventsAt74, event.slot, capturedEvent);
            eventLifetime.Captured(capturedEvent); // state23。
        }

        Slot upcast{child.baseAt00, child.biasAt04, capturedChild.controlAt08};
        if (capturedChild.controlAt08) ++capturedChild.controlAt08->referenceCountAt04;
        {
            LocalReference upcastLifetime{upcast}; // state24。
            auto* captured = upcast.controlAt08;
            if (b.common.childrenAt0C.end == b.common.childrenAt0C.capacity)
            {
                GrowGuiControlChildrenAt4D93B0(b.common.childrenAt0C, b.common.childrenAt0C.end, upcast);
                captured = upcast.controlAt08;
            }
            else
            {
                // 原快路用EBX/ESI与当时var1C+4，而非复制可能被回调改过的upcast。
                const Slot fast{capturedChild.baseAt00, child.biasAt04, capturedChild.controlAt08};
                AppendGuiControlChildReference(b.common.childrenAt0C, fast);
            }
            upcastLifetime.Captured(captured); // state25。
        }
        childLifetime.Captured(capturedChild.controlAt08); // state26。
    }
}
void PopulateNavigation(Body& b, const SaveLoadMessageBoxNativeNavigationGroup& group)
{
    FreshReference navigation;
    CreateGuiNavigationAt504CE0(navigation.slot, group.inputAt00, group.pairsAt04);
    LocalReference lifetime{navigation.slot}; // state27，工厂内ex=0原普通分支。
    AppendGuiNavigationReferenceAt4D9D60(b.common.navigationAt24, navigation.slot);
    lifetime.Current(); // state28。
}
}
Body* ConstructSaveLoadMessageBoxNativeAt4EC8A0(void* storage,
    std::span<const SaveLoadMessageBoxNativeControlGroup> groups,
    std::span<const SaveLoadMessageBoxNativeNavigationGroup> navigation)
{
    auto& s = RequireSaveLoadMessageBoxNativeServices();
    auto* b = ::new(storage) Body;
    ConstructGuiDialogAt4E0BE0(&b->common, nullptr, -1);
    b->backgroundAt5C = {};
    b->eventsAt74 = {};
    b->common.vptrAt00 = &SaveLoadMessageBoxNativeVtable().common;
    // 原两次4DA6C0均无可抛叶；不值初始化整个A8h，68..73和80..83仍是旧位模式。
    b->pageAndSlotBaseAt84 = {};
    b->pageOnlyBaseAt90 = {};
    b->digitsAt9C = {};
    struct Unwind final
    { Body& b; bool active = true; ~Unwind() noexcept { if (active) DestroyCompletedMembers(b); } } unwind{*b};
    AdoptGuiInputAt536EA0(b->common.inputAt18, NewGameGuiInputAt4EAAED());
    for (const auto& group : groups) Populate(*b, group, s);
    for (const auto& group : navigation) PopulateNavigation(*b, group);
    unwind.active = false;
    return b;
}
Body* ConstructSaveLoadMessageBoxNativeAt4EC8A0(void* storage)
{
    const auto& controls=CurrentSaveLoadResourceProfile().deletion.controls;
    const SaveLoadMessageBoxNativeControlGroup groups[]{
        {0,static_cast<int>(controls.statics.size()),controls.statics.data()},
        {1,static_cast<int>(controls.buttons.size()),controls.buttons.data()}};
    std::vector<SaveLoadMessageBoxNativeNavigationGroup> navigation;
    for(const auto& item:controls.navigation)navigation.push_back({item.input,item.pairs.data()});
    return ConstructSaveLoadMessageBoxNativeAt4EC8A0(storage,groups,navigation);
}
Common* ConstructSaveLoadMessageBoxAt4EC8A0(void* storage)
{ return &ConstructSaveLoadMessageBoxNativeAt4EC8A0(storage)->common; }
}
