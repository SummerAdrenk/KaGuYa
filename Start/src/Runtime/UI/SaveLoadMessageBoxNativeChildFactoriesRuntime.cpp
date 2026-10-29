#include "SaveLoadMessageBoxNativeChildFactoriesRuntime.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadMessageBoxChildFactoriesDetail;
SaveLoadMessageBoxNativeChildFactoriesRuntime::SaveLoadMessageBoxNativeChildFactoriesRuntime(
    SaveLoadMessageBoxNativeChildFactoriesDependencies dependencies) noexcept : dependencies_(dependencies) {}

Slot* SaveLoadMessageBoxNativeChildFactoriesRuntime::CreateAt5043B0(Slot* output,
    GuiDialogNativeBody5Ch* parent, const void* descriptor)
{
    const auto& d = *static_cast<const SaveLoadMessageBoxCheckBoxDescriptorNative48h*>(descriptor);
    auto& s = dependencies_;
    GraphicsSurfaceNativeScope scope(s.surfaces);
    ReferenceLifetime result{*output, false};
    Fresh normal;
    LoadGuiControlResourceAt5047A0(normal.slot, d.normalNamesAt04.data(), s.loader, s.surfaces, s.strings);
    ReferenceLifetime normalLife{normal.slot};
    if (!Read(normal.slot.baseAt00)) throw 1;
    Fresh checked;
    LoadGuiControlResourceAt5047A0(checked.slot, d.checkedNamesAt20.data(), s.loader, s.surfaces, s.strings);
    ReferenceLifetime checkedLife{checked.slot};
    if (!Read(checked.slot.baseAt00)) throw 1;
    auto* normalControl = Read(normal.slot.controlAt08);
    auto* checkedControl = Read(checked.slot.controlAt08);
    if (auto* point = Read(d.positionAt3C))
    {
        const auto y = Read(point->y), x = Read(point->x);
        MoveGuiControlResourceAt5086C0(CopyParts(Read(normal.slot.baseAt00), Read(normal.slot.biasAt04), normalControl), x, y);
        point = Read(d.positionAt3C); // 50445D重新取POINT；不能缓存第一次坐标。
        const auto checkedY = Read(point->y), checkedX = Read(point->x);
        MoveGuiControlResourceAt5086C0(CopyParts(Read(checked.slot.baseAt00), Read(checked.slot.biasAt04), checkedControl), checkedX, checkedY);
    }
    void* raw = ::operator new(0x60);
    RawLifetime allocation{raw, 0x60};
    GuiCheckBoxNativeBody60h* child = nullptr;
    if (raw)
    {
        // 50449E先右参数增持，再左参数；展开由真实4E0520消费两份拥有Ref。
        Slot right = CopyParts(Read(checked.slot.baseAt00), Read(checked.slot.biasAt04), checkedControl);
        Slot left = CopyParts(Read(normal.slot.baseAt00), Read(normal.slot.biasAt04), normalControl);
        child = ConstructGuiCheckBoxAt4E0520(static_cast<GuiCheckBoxNativeBody60h*>(raw), parent, Read(d.eventAt00), left, right);
    }
    allocation.active = false;
    ConstructGuiCheckBoxReferenceAt4D7710(*output, child, 1);
    result.active = true;
    Resolve<Body>(*output)->childIdAt0C = Read(d.childIdAt44);
    auto* receiver = Resolve<Body>(*output);
    reinterpret_cast<const GuiCheckBoxNativeVtable30*>(Read(receiver->vptrAt00))->setHitSlotAt60(receiver, Read(d.hitSlotAt40));
    checkedLife.Captured(checkedControl);
    normalLife.Captured(normalControl);
    result.active = false;
    return output;
}

Slot* SaveLoadMessageBoxNativeChildFactoriesRuntime::CreateAt503BB0(Slot* output,
    GuiDialogNativeBody5Ch* parent, const void* descriptor)
{
    const auto& d = *static_cast<const SaveLoadMessageBoxMultiCheckDescriptorNative44h*>(descriptor);
    auto& s = dependencies_;
    GraphicsSurfaceNativeScope scope(s.surfaces);
    ReferenceLifetime result{*output, false};
    // 503BED..503C14在loader之前抄七个名字，hit值则在+78调用前逐个晚读。
    std::array<const char*, 7> names;
    for (std::size_t i = 0; i != 7; ++i) names[i] = Read(d.itemsAt04[i].nameAt00);
    Fresh group;
    LoadGuiControlResourceAt5047A0(group.slot, names.data(), s.loader, s.surfaces, s.strings);
    ReferenceLifetime groupLife{group.slot};
    if (!Read(group.slot.baseAt00)) throw 1;
    auto* captured = Read(group.slot.controlAt08);
    if (auto* point = Read(d.positionAt3C))
    {
        const auto y = Read(point->y), x = Read(point->x);
        MoveGuiControlResourceAt5086C0(CopyParts(Read(group.slot.baseAt00), Read(group.slot.biasAt04), captured), x, y);
    }
    void* raw = ::operator new(0x78);
    RawLifetime allocation{raw, 0x78};
    Body* child = nullptr;
    if (raw)
    {
        Slot extra = CopyParts(Read(group.slot.baseAt00), Read(group.slot.biasAt04), captured);
        ReferenceLifetime extraLife{extra};
        const auto event = Read(d.eventAt00);
        Slot argument = CopyParts(Read(group.slot.baseAt00), Read(group.slot.biasAt04), captured);
        child = &ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw), parent, event, argument)->common;
        child->vptrAt00 = &SaveLoadMessageBoxMultiCheckNativeVtable().button.common;
        for (auto& hit : reinterpret_cast<MultiCheck*>(child)->hitSlotsAt5C) hit = 7;
        extraLife.Captured(captured);
    }
    allocation.active = false;
    ConstructReference<MultiCheckControl, 0x78>(*output, child);
    result.active = true;
    Resolve<Body>(*output)->childIdAt0C = Read(d.childIdAt40);
    auto* receiver = Resolve<Body>(*output); // 503DDF固定receiver，七次只重读其当前表。
    for (std::int32_t i = 0; i != 7; ++i)
        reinterpret_cast<const SaveLoadMessageBoxMultiCheckNativeVtable31*>(Read(receiver->vptrAt00))->
            setMappedHitSlotAt78(receiver, i, Read(d.itemsAt04[i].hitSlotAt04));
    groupLife.Captured(Read(group.slot.controlAt08)); // 503DF7回读局部control，不再用旧ESI。
    result.active = false;
    return output;
}

Slot* SaveLoadMessageBoxNativeChildFactoriesRuntime::CreateAt503E60(Slot* output,
    GuiDialogNativeBody5Ch* parent, const void* descriptor)
{
    const auto& d = *static_cast<const SaveLoadMessageBoxMultiHotkeyDescriptorNative2Ch*>(descriptor);
    auto& s = dependencies_;
    GraphicsSurfaceNativeScope scope(s.surfaces);
    ReferenceLifetime result{*output, false};
    Fresh group;
    LoadGuiControlResourceAt5047A0(group.slot, d.common.namesAt04.data(), s.loader, s.surfaces, s.strings);
    ReferenceLifetime groupLife{group.slot};
    const auto base = Read(group.slot.baseAt00);
    if (!base) throw 1;
    auto* captured = Read(group.slot.controlAt08);
    if (auto* point = Read(d.common.positionAt20))
    {
        const auto y = Read(point->y), x = Read(point->x);
        MoveGuiControlResourceAt5086C0(CopyParts(base, Read(group.slot.biasAt04), captured), x, y);
    }
    void* raw = ::operator new(0x68);
    RawLifetime allocation{raw, 0x68};
    Body* child = nullptr;
    if (raw)
    {
        Slot extra = CopyParts(Read(group.slot.baseAt00), Read(group.slot.biasAt04), captured);
        ReferenceLifetime extraLife{extra};
        const auto event = Read(d.common.eventAt00);
        Slot argument = CopyParts(base, Read(group.slot.biasAt04), captured);
        child = &ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw), parent, event, argument)->common;
        child->vptrAt00 = &SaveLoadMessageBoxMultiHotkeyNativeVtable().common;
        reinterpret_cast<MultiHotkey*>(child)->hotkeysAt5C = {}; // 4D8140(0,0)为空范围，无其它原语。
        extraLife.Captured(captured);
    }
    allocation.active = false;
    ConstructReference<MultiHotkeyControl, 0x68>(*output, child);
    result.active = true;
    auto* keys = Read(d.terminatedHotkeysAt28);
    if (keys && Read(keys[0]) != -1)
    {
        std::uint32_t index = 0;
        do
        {
            const auto key = Read(keys[index]);
            auto* receiver = Resolve<MultiHotkey>(*output); // 504035在new8之前固定receiver。
            void* hotkeyRaw = ::operator new(8);
            auto* hotkey = hotkeyRaw ? ::new(hotkeyRaw) Hotkey{key, 0} : nullptr;
            Fresh item;
            // 504078失败时8h未取得展开资格，保持原分配前缀，不补删。
            ConstructReference<HotkeyAndStateControl, 8>(item.slot, hotkey);
            ReferenceLifetime itemLife{item.slot};
            AppendHotkey(receiver->hotkeysAt5C, item.slot);
            auto* itemControl = Read(item.slot.controlAt08); // 慢路返回后回读，快路等于缓存ESI。
            itemLife.Captured(itemControl, true);
            ++index;
            keys = Read(d.terminatedHotkeysAt28); // 504139允许叶回调改变后续键表。
        } while (Read(keys[index]) != -1);
        captured = Read(group.slot.controlAt08); // 504146仅非空循环路径回读。
    }
    auto* receiver = Resolve<Body>(*output);
    reinterpret_cast<const GuiButtonNativeVtable29*>(Read(receiver->vptrAt00))->setHitSlotAt60(receiver, Read(d.hitSlotAt24));
    // 原503E60从不写childId，+0C继续为4DA700的-1。
    groupLife.Captured(captured);
    result.active = false;
    return output;
}

Slot* SaveLoadMessageBoxNativeChildFactoriesRuntime::CreateAt505090(Slot* output,
    GuiDialogNativeBody5Ch* parent, const void* descriptor)
{
    const auto& d = *static_cast<const SaveLoadMessageBoxRadioDescriptorNative2Ch*>(descriptor);
    auto& s = dependencies_;
    GraphicsSurfaceNativeScope scope(s.surfaces);
    ReferenceLifetime result{*output, false};
    Fresh group;
    LoadGuiControlResourceAt5047A0(group.slot, d.common.namesAt04.data(), s.loader, s.surfaces, s.strings);
    ReferenceLifetime groupLife{group.slot};
    const auto base = Read(group.slot.baseAt00);
    if (!base) throw 1;
    auto* captured = Read(group.slot.controlAt08);
    if (auto* point = Read(d.common.positionAt20))
    {
        const auto y = Read(point->y), x = Read(point->x);
        MoveGuiControlResourceAt5086C0(CopyParts(base, Read(group.slot.biasAt04), captured), x, y);
    }
    void* raw = ::operator new(0x5C);
    RawLifetime allocation{raw, 0x5C};
    Body* child = nullptr;
    if (raw)
    {
        Slot extra = CopyParts(Read(group.slot.baseAt00), Read(group.slot.biasAt04), captured);
        ReferenceLifetime extraLife{extra};
        const auto event = Read(d.common.eventAt00);
        Slot argument = CopyParts(base, Read(group.slot.biasAt04), captured);
        child = &ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw), parent, event, argument)->common;
        child->vptrAt00 = &SaveLoadMessageBoxRadioNativeVtable().common;
        child->typeAt08 = 3;
        extraLife.Captured(captured);
    }
    allocation.active = false;
    ConstructReference<RadioControl, 0x5C>(*output, child);
    result.active = true;
    Resolve<Body>(*output)->childIdAt0C = Read(d.childIdAt28);
    auto* receiver = Resolve<Body>(*output);
    reinterpret_cast<const GuiButtonNativeVtable29*>(Read(receiver->vptrAt00))->setHitSlotAt60(receiver, Read(d.hitSlotAt24));
    groupLife.Captured(captured);
    result.active = false;
    return output;
}
}
