#include "SettingPage01PreviewBlockNative.Internal.h"
#include <Runtime/PopupNativeLifetime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage01PreviewBlockNativeDetail;
namespace SettingPage01PreviewBlockNativeDetail
{
void PopulateSlider(Body& b, const GuiSliderDescriptorNative6Ch& d, RECT& bounds)
{
    auto& s = RequireSettingPage02NativeServices();
    Fresh child; child.slot = {}; RefLife output{child.slot}; Fresh typed;
    CreateGuiSliderAt5052C0(typed.slot, &b.choice.common, d, s.loader, s.surfaces, s.strings);
    RefLife temporary{typed.slot};
    ReleaseYtlReference(child.slot); CopyGuiClickedEventReference(child.slot, typed.slot);
    temporary.Normal(); output.active = false;
    RefLife childLife{child.slot};
    auto* selected = Resolve<Control>(child.slot);
    RECT out; auto* returned = Read(selected->vptrAt00)->rectAt3C(selected, &out);
    ::UnionRect(&bounds, &bounds, returned);
    Fresh event;
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot); RefLife eventLife{event.slot};
    auto* capturedEvent = Read(event.slot.controlAt08);
    Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        RefLife life{binding}; selected = Resolve<Control>(child.slot);
        auto& target = selected->eventAt3C;
        ReleaseOld(target);
        target.baseAt00 = Read(event.slot.baseAt00); target.biasAt04 = 0; target.controlAt08 = capturedEvent;
        if (capturedEvent) ++capturedEvent->referenceCountAt04;
        selected->eventEnabledAt48 = 1; life.Captured(capturedEvent);
    }
    Slot upcast = Copy(child.slot);
    {
        RefLife life{upcast}; auto* captured = Read(upcast.controlAt08);
        auto& children = b.choice.common.childrenAt0C;
        const bool grow = children.end == children.capacity;
        AppendGuiControlChildReference(children, upcast);
        if (grow) captured = Read(upcast.controlAt08);
        life.Captured(captured);
    }
    Slot argument{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        RefLife life{argument}; auto* captured = capturedEvent;
        // 控件虚调用之后重解74h manager，不缓存先前receiver。
        AppendPopupClickedEventReferenceAt4EA97F(*Resolve<GuiClickedEventReferences>(b.choice.managerAt74), argument, captured);
        life.Captured(captured);
    }
    eventLife.Captured(capturedEvent); childLife.Normal(true);
}
}
Body* ConstructSettingPage01PreviewBlockNativeAt4E5510(void* storage, Common* parent, std::int32_t id)
{
    const auto& profile = CurrentSettingTextDisplayProfile().preview;
    auto* b = ::new(storage) Body;
    ::new(&b->choice) SettingPageChoiceNativeBody80h;
    ConstructGuiDialogAt4E0BE0(&b->choice.common, parent, id);
    b->choice.fieldAt5C = 1; b->choice.managerAt74 = {};
    b->choice.common.vptrAt00 = &SettingPage01PreviewBlockNativeVtable().common;
    ::new(&b->transparencyAt80) ScriptDwordVectorStorage;
    b->previewsAt8C = {}; b->previewBoundsAt98 = {}; b->textAtA8 = {};
    struct Unwind final
    { Body* p; bool active = true; ~Unwind() noexcept { if (active) DestroyMembers(*p); } } unwind{b};
    auto* rawManager = ::operator new(0x0C);
    auto* manager = rawManager ? ::new(rawManager) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(b->choice.managerAt74, manager);
    GuiFactoryConstructionLifetime factory;
    RECT bounds{};
    for (const auto& d : profile.sliders)
    { static_cast<void>(Resolve<void>(factory.Reference())); PopulateSlider(*b, d, bounds); }
    for (const auto& d : profile.buttons)
    { static_cast<void>(Resolve<void>(factory.Reference())); PopulateButton(b->choice.common, b->choice.managerAt74, d, bounds, true); }
    b->transparencyAt80.clear(); ::SetRectEmpty(&b->previewBoundsAt98);
    for (const auto* name : profile.images)
    {
        auto& services = RequireSettingPage01PreviewBlockNativeServices().colors;
        auto* raw = ::operator new(0x38);
        Color* created = nullptr;
        {
            struct Raw final
            { void* p; bool active = true; ~Raw() noexcept { if (active) ::operator delete(p, std::size_t{0x38}); } } cleanup{raw};
            if (raw) created = ConstructColorTransWindowNativeAt45E970(*::new(raw) Color, nullptr, name, 0, services);
            cleanup.active = false; // state15撤除；随后new18失败保持孤立38h，不补删。
        }
        Slot color{}; AdoptColorTransWindowNativeAt45F3B7(color, created);
        RefLife colorLife{color}; auto* captured = Read(color.controlAt08);
        if (!Read(created->processedAt1C.baseAt00)) throw 1;
        if (!ApplyColorTransWindowNativeAt46F6D0(*created, 1, 100, services)) throw 1;
        const auto base = Read(created->processedAt1C.baseAt00);
        auto* size = ResolveBase<Surface>(created->processedAt1C, base);
        const auto width = Read(size->widthAt08), height = Read(size->heightAt0C);
        auto* origin = ResolveBase<Surface>(created->processedAt1C, base);
        const auto x = Read(origin->originXAt10), y = Read(origin->originYAt14);
        RECT rectangle{x,y,Signed(static_cast<std::uint32_t>(x)+width),Signed(static_cast<std::uint32_t>(y)+height)};
        ::UnionRect(&b->previewBoundsAt98, &b->previewBoundsAt98, &rectangle);
        Slot upcast{Read(color.baseAt00),0,captured}; if (captured) ++captured->referenceCountAt04;
        {
            RefLife life{upcast}; auto* tail = captured;
            const bool grow = b->previewsAt8C.end == b->previewsAt8C.capacity;
            AppendGuiControlChildReference(b->previewsAt8C, upcast);
            if (grow) tail = Read(upcast.controlAt08);
            life.Captured(tail);
        }
        b->transparencyAt80.Push(100);
        colorLife.Captured(captured);
    }
    ::UnionRect(&bounds, &bounds, &b->previewBoundsAt98);
    BuildSettingPage01PreviewTextNativeAt53AA70(*b, 1, 1);
    b->choice.boundsAt64 = bounds;
    for (std::size_t index = 0; index != profile.sliders.size(); ++index)
    {
        const auto childId = profile.sliders[index].eventAt00;
        Fresh found;
        GuiDialogBaseVtable().findChildAt14(&b->choice.common, &found.slot, childId); // 原509D20直接基类调用。
        RefLife life{found.slot}; auto* captured = Read(found.slot.controlAt08);
        if (Read(found.slot.baseAt00))
        {
            const auto base = Read(found.slot.baseAt00);
            auto* child = Resolve<Control>(found.slot);
            Slider(child).setRangeAt5C(child, profile.minimum[index], profile.maximum[index]);
            if (profile.step[index] > 0)
            {
                // 4E5D97仍用EDI/ESI；bias与control.stride在setRange之后重读。
                const Slot current{base,Read(found.slot.biasAt04),captured};
                child = Resolve<Control>(current); Slider(child).setStepAt68(child, profile.step[index]);
            }
        }
        life.Captured(captured);
    }
    factory.Finish(); unwind.active = false; return b;
}
}
