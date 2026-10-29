#include "SettingPage02Native.Internal.h"
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <new>

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
std::size_t ChildSize(SettingTextDisplayBlock block)
{
    if (block == SettingTextDisplayBlock::Preview) return 0xB4;
    if (block == SettingTextDisplayBlock::RightClick) return 0x80;
    return 0x7C;
}
Common* NewChild(Parent& p, const SettingTextDisplayChild& child, ISettingPage01Block02NativeConstruction* preview)
{
    auto& s = RequireSettingPage02NativeServices();
    const auto bytes = ChildSize(child.block);
    auto* raw = s.memory.AllocateBody(bytes);
    struct Raw final
    { void* p; std::size_t bytes; IEnvironmentRootMemory& m; bool active = true;
      ~Raw() noexcept { if (active) m.DeleteBody(p, bytes); } } cleanup{raw, bytes, s.memory};
    if (!raw) throw std::bad_alloc();
    Common* result;
    switch (child.block)
    {
    case SettingTextDisplayBlock::TextOptions: result = &ConstructSettingPage01Block00NativeAt4E50B0(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::SkipOptions: result = &ConstructSettingPage01Block01NativeAt4E52E0(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::Preview:
        if (!preview) throw std::logic_error("Text settings preview constructor is missing");
        result = preview->ConstructBlock02At4E5510(raw, &p.common, child.id); break;
    case SettingTextDisplayBlock::ConfirmOptions: result = &ConstructSettingPage02Block00NativeAt4E5E40(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::RightClick: result = &ConstructSettingPage02Block01NativeAt4E6070(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::DisplayMode: result = &ConstructSettingPage02Block02NativeAt4E64D0(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::SaveOptions: result = &ConstructSettingPage02Block03NativeAt4E6930(raw, &p.common, child.id)->common; break;
    case SettingTextDisplayBlock::SceneChoices: result = &ConstructContainer(raw, &p.common, child.id, SettingPageRadioContainerKind::SceneChoices)->common; break;
    default: throw std::logic_error("Unknown text/display settings block");
    }
    cleanup.active = false; return result;
}
void Append(Parent& p, const SettingTextDisplayChild& descriptor, ISettingPage01Block02NativeConstruction* preview, RECT& bounds)
{
    auto* created = NewChild(p, descriptor, preview);
    Fresh child;
    ConstructDialogRef(child.slot, created, static_cast<std::uint32_t>(ChildSize(descriptor.block)));
    RefLife life{child.slot};
    Slot owned = Copy(child.slot);
    GuiDialogIntoBaseVtable().appendDialogAt8C(&p.common, owned); // 直接4FF130，按值参数由callee消费。
    RECT out;
    const RECT* returned;
    if (descriptor.block == SettingTextDisplayBlock::DisplayMode)
    { out = Field<RECT>(created, 0x5C); returned = &out; } // 4F26DB直接读5C，不虚调用bounds。
    else if (descriptor.block == SettingTextDisplayBlock::Preview || descriptor.block == SettingTextDisplayBlock::RightClick)
        returned = QTable(created).boundsAt8C(created, &out);
    else returned = CTable(created).boundsAt94(created, &out);
    ::UnionRect(&bounds, &bounds, returned);
    auto* captured = Read(child.slot.controlAt08); life.Captured(captured);
}
void Footer(Parent& p, const GuiStaticDescriptorNative24h& descriptor, RECT& bounds, bool factoryHeap)
{
    auto& s = RequireSettingPage02NativeServices();
    // Page01原有new1/control18；Page02使用栈内空factory，不新增这两次分配。
    auto populate = [&] {
        Fresh temporary;
        auto* returned = CreateGuiStaticAt505800(temporary.slot, &p.common, descriptor, s.loader, s.surfaces, s.strings);
        RefLife life{temporary.slot};
        AssignRef(p.footerAt78, *returned); life.Normal();
        auto* selected = Resolve<GuiControlNativeBody50h>(p.footerAt78);
        RECT out; auto* rect = Read(selected->vptrAt00)->rectAt3C(selected, &out);
        ::UnionRect(&bounds, &bounds, rect);
    };
    if (factoryHeap) { GuiFactoryConstructionLifetime factory; populate(); factory.Finish(); }
    else populate();
}
void Reload(Common* p, bool page01)
{
    for (const auto& child : CurrentSettingTextDisplayProfile().pages[page01 ? 0 : 1])
    {
        if (child.block == SettingTextDisplayBlock::DisplayMode) continue;
        Fresh found; Into(p).findDialogAt90(p, &found.slot, child.id);
        RefLife life{found.slot};
        if (Read(found.slot.baseAt00))
        {
            auto* selected = Resolve<Common>(found.slot);
            if (child.block == SettingTextDisplayBlock::Preview)
            {
                // 01:4E8890 / 18:506460 preserve the scene selection across a reload.
                const auto previous = ReadSettingPage01PreviewSelectionNativeAt535E60(*selected);
                QTable(selected).loadAt90(selected);
                WriteSettingPage01PreviewSelectionNativeAt509940(*selected, previous == 0, 1);
            }
            else if (child.block == SettingTextDisplayBlock::RightClick)
                QTable(selected).loadAt90(selected);
            else CTable(selected).loadAt98(selected);
        }
        life.Normal();
    }
}
}
Parent* ConstructParent(void* raw, Common* parent, std::int32_t id, bool page01, ISettingPage01Block02NativeConstruction* preview)
{
    const auto& profile = CurrentSettingTextDisplayProfile();
    const auto index = page01 ? 0u : 1u;
    auto* p = ::new(raw) Parent;
    ConstructGuiDialogAt4E0BE0(&p->common, parent, id);
    p->dialogsAt5C = {}; p->footerAt78 = {};
    p->common.vptrAt00 = &(page01 ? SettingPage01NativeVtable() : SettingPage02NativeVtable()).common.common;
    struct Unwind final
    { Parent* p; bool active = true; ~Unwind() noexcept { if (active) DestroySettingPageNativeAt4F9840(p); } } unwind{p};
    RECT bounds{};
    for (const auto& child : profile.pages[index]) Append(*p, child, preview, bounds);
    if (!profile.footers[index]) throw std::logic_error("Text/display page footer is missing");
    Footer(*p, *profile.footers[index], bounds, page01);
    p->boundsAt68 = bounds;
    unwind.active = false; return p;
}
void __fastcall ParentDraw(Common* p, void*, const RECT* clip)
{
    if (Read(p->enabledAt38))
    {
        auto* footer = Resolve<GuiControlNativeBody50h>(reinterpret_cast<Parent*>(p)->footerAt78);
        Read(footer->vptrAt00)->drawAt0C(footer, clip);
    }
    GuiDialogIntoBaseVtable().common.drawAt60(p, clip); // 原无论门控如何，之后直接523370。
}
void __fastcall ParentComposite(Common* p, void*, Slot owned, const RECT* clip)
{
    RefLife lifetime{owned};
    if (Read(p->enabledAt38))
    {
        auto* footer = Resolve<GuiControlNativeBody50h>(reinterpret_cast<Parent*>(p)->footerAt78);
        Slot copied = Copy(owned); Read(footer->vptrAt00)->compositeAt10(footer, copied, clip);
    }
    Slot nested = Copy(owned); GuiDialogIntoBaseVtable().common.compositeAt5C(p, nested, clip);
    lifetime.Normal();
}
RECT* __fastcall ParentBounds(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x68); return out; }
void __fastcall Reload01(Common* p, void*) { Reload(p, true); }
void __fastcall Reload02(Common* p, void*) { Reload(p, false); }
}
namespace KaGuYa::Start::Runtime
{
SettingPage02NativeBody84h* ConstructSettingPage02NativeAt4F23B0(void* raw, GuiDialogNativeBody5Ch* parent, std::int32_t id)
{ return SettingPage02NativeDetail::ConstructParent(raw, parent, id, false, nullptr); }
}
