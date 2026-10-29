#include "SettingPage01PreviewBlockNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage01PreviewBlockNativeDetail;
std::int32_t ReadSettingPage01PreviewSelectionNativeAt535E60(Common& p)
{
    for (int index = 0; index != 2; ++index)
    {
        Fresh found; Read(p.vptrAt00)->findChildAt14(&p, &found.slot,
            CurrentSettingTextDisplayProfile().preview.buttons[index].common.eventAt00);
        RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            auto* child = Resolve<Control>(found.slot);
            if (!Read(child->vptrAt00)->readStateAt48(child)) { life.Normal(); return index; }
        }
        life.Normal();
    }
    return -1;
}
void WriteSettingPage01PreviewSelectionNativeAt509940(Common& p, std::int32_t value, std::int32_t invalidate)
{
    for (int index = 0; index != 2; ++index)
    {
        Fresh found; Read(p.vptrAt00)->findChildAt14(&p, &found.slot,
            CurrentSettingTextDisplayProfile().preview.buttons[index].common.eventAt00);
        RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            auto* child = Resolve<Control>(found.slot);
            Read(child->vptrAt00)->writeStateAt44(child, index == 0 ? value == 0 : value);
            if (invalidate) InvalidateChild(p, found.slot);
        }
        life.Normal();
    }
    if (invalidate)
    {
        Fresh found; Read(p.vptrAt00)->findChildAt14(&p, &found.slot,
            CurrentSettingTextDisplayProfile().preview.sliders[2].eventAt00); RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            const auto index = ReadSettingPage01PreviewSelectionNativeAt535E60(p);
            if (index < 0) throw 1;
            auto* child = Resolve<Control>(found.slot);
            const auto* table = &Slider(child);
            table->setValueAt64(child, Signed(Read(Read(B(&p).transparencyAt80.storage.begin)[index])));
            InvalidateChild(p, found.slot);
        }
        Invalidate(p, B(&p).previewBoundsAt98);
        life.Normal();
    }
}
void SaveSettingPage01PreviewTransparencyNativeAt4FF330(Common& p)
{
    Fresh found; Read(p.vptrAt00)->findChildAt14(&p, &found.slot,
        CurrentSettingTextDisplayProfile().preview.sliders[2].eventAt00); RefLife life{found.slot};
    if (Read(found.slot.baseAt00))
    {
        const auto index = ReadSettingPage01PreviewSelectionNativeAt535E60(p);
        if (index == 0 || index == 1)
        {
            auto* child = Resolve<Control>(found.slot);
            auto* output = Read(B(&p).transparencyAt80.storage.begin); // 原ESI在value虚调用前固定。
            const auto value = Slider(child).valueAt60(child);
            output[index] = static_cast<std::uint32_t>(value);
        }
    }
    life.Normal();
}
namespace SettingPage01PreviewBlockNativeDetail
{
namespace
{
void LoadSlider(Common* p, unsigned index, bool clear)
{
    const int id = CurrentSettingTextDisplayProfile().preview.sliders[index].eventAt00;
    Fresh found; Read(p->vptrAt00)->findChildAt14(p, &found.slot, id); RefLife life{found.slot};
    if (Read(found.slot.baseAt00))
    {
        auto* child = Resolve<Control>(found.slot); const auto* table = &Slider(child);
        const auto value = index == 2 ? ReadSettingTextAlpha(0) : ReadSettingTextSlider(index);
        table->setValueAt64(child, Signed(value));
    }
    life.Normal(clear);
}
void StoreSlider(Common* p, unsigned index, bool clear)
{
    const int id = CurrentSettingTextDisplayProfile().preview.sliders[index].eventAt00;
    Fresh found; Read(p->vptrAt00)->findChildAt14(p, &found.slot, id); RefLife life{found.slot};
    if (Read(found.slot.baseAt00))
    {
        auto* child = Resolve<Control>(found.slot);
        const auto value = Slider(child).valueAt60(child); // 原value固定后才取System根。
        WriteSettingTextSlider(index, static_cast<std::uint32_t>(value));
    }
    life.Normal(clear);
}
std::int32_t ScanManager(GuiClickedEventReferences& manager)
{
    auto* position = Read(manager.begin); auto* end = Read(manager.end);
    for (; position != end; ++position)
    {
        const auto base = Read(position->baseAt00);
        if (!Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->eventValueAt08);
        auto* event = ResolveBase<GuiClickedEventNativeBody>(*position, base);
        Read(event->vptrAt00)->clearAt04(event); return value; // 原50FDB0：先固定value再clear，不扫描后项。
    }
    return -1;
}
}
void __fastcall Load(Common* p, void*)
{
    auto& b = B(p);
    LoadSlider(p, 0, false); LoadSlider(p, 1, false);
    Read(b.transparencyAt80.storage.begin)[0] = ReadSettingTextAlpha(0);
    Read(b.transparencyAt80.storage.begin)[1] = ReadSettingTextAlpha(1);
    auto* firstColor = Resolve<Color>(Read(b.previewsAt8C.begin)[0]);
    if (!ApplyColorTransWindowNativeAt46F6D0(*firstColor, 1,
        Signed(ReadSettingTextAlpha(0)), RequireSettingPage01PreviewBlockNativeServices().colors)) throw 1;
    auto* secondColor = Resolve<Color>(Read(b.previewsAt8C.begin)[1]); // 第二份先固定Color再capture。
    const auto alpha = ReadSettingTextAlpha(1);
    if (!ApplyColorTransWindowNativeAt46F6D0(*secondColor, 1, Signed(alpha),
        RequireSettingPage01PreviewBlockNativeServices().colors)) throw 1;
    LoadSlider(p, 2, true);
    WriteSettingPage01PreviewSelectionNativeAt509940(*p, 1, 0);
    b.choice.fieldAt5C = 0; // 全部成功才开放store，不对失败前缀回滚。
}
void __fastcall Store(Common* p, void*)
{
    auto& b = B(p);
    if (!Read(b.choice.fieldAt5C))
    {
        StoreSlider(p, 0, false); StoreSlider(p, 1, true);
        SaveSettingPage01PreviewTransparencyNativeAt4FF330(*p);
        auto* first = Read(b.transparencyAt80.storage.begin);
        WriteSettingTextAlpha(0, Read(first[0]));
        auto* second = Read(b.transparencyAt80.storage.begin);
        WriteSettingTextAlpha(1, Read(second[1]));
    }
    b.choice.fieldAt5C = 1;
}
std::int32_t __fastcall End(Common* p, void*) { QTable(p).storeAt94(p); return 1; } // 原5090F0，不是508BD0空End。
std::int32_t __fastcall Poll(Common* p, void*)
{
    const auto& profile = CurrentSettingTextDisplayProfile().preview;
    Fresh found; Read(p->vptrAt00)->findChildAt14(p, &found.slot, profile.sliders[2].eventAt00); RefLife life{found.slot};
    if (!Read(found.slot.baseAt00)) throw 1;
    auto* child = Resolve<Control>(found.slot); const auto before = Slider(child).valueAt60(child);
    Read(p->vptrAt00)->processInputAt54(p);
    auto* manager = Resolve<GuiClickedEventReferences>(B(p).choice.managerAt74);
    const auto event = ScanManager(*manager);
    if (event == profile.buttons[0].common.eventAt00 || event == profile.buttons[1].common.eventAt00)
    {
        SaveSettingPage01PreviewTransparencyNativeAt4FF330(*p);
        WriteSettingPage01PreviewSelectionNativeAt509940(*p, event == profile.buttons[0].common.eventAt00 ? 1 : 0, 1);
    }
    child = Resolve<Control>(found.slot);
    if (Slider(child).valueAt60(child) != before)
    {
        const auto index = ReadSettingPage01PreviewSelectionNativeAt535E60(*p);
        if (index < 0) throw 1;
        child = Resolve<Control>(found.slot);
        auto* color = Resolve<Color>(Read(B(p).previewsAt8C.begin)[index]);
        const auto value = Slider(child).valueAt60(child);
        if (!ApplyColorTransWindowNativeAt46F6D0(*color, 1, value,
            RequireSettingPage01PreviewBlockNativeServices().colors)) throw 1;
        Invalidate(*p, B(p).previewBoundsAt98);
    }
    life.Normal(); return 1;
}
}
}
