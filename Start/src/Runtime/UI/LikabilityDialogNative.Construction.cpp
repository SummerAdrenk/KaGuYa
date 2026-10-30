#include "LikabilityDialogNative.Internal.h"
#include "LikabilityDialogNative.Profile.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilityNativeDetail;
struct ConstructorFrame final
{
    Body& b;
    Slot group, child;
    void* raw = nullptr;
    int state = -1;
    ~ConstructorFrame() noexcept
    {
        const auto map = ConstructorUnwind();
        while (state >= 0)
        {
            const auto at = state;
            state = map[static_cast<unsigned>(at)].previous;
            switch (at)
            {
            case 0: DestroyGuiDialogAt4F7AC0(&b.common); break;
            case 1: ReleaseYtlReference(b.clickedManagerAt6C); break;
            case 2: ReleaseYtlReference(b.backgroundAt78); break;
            case 3: ReleaseYtlReference(b.canvasAt84); break;
            case 4: ::operator delete(raw, std::size_t{0x38}); break;
            case 5: ReleaseGuiControlResourceReferenceAt4F6380(group); break;
            case 6: ::operator delete(raw, std::size_t{0x50}); break;
            case 7: ReleaseGuiControlReferenceAt4F6460(child); break;
            case 10: ReleaseGuiControlReferenceAt4F6460(group); break;
            case 11: ReleaseGuiClickedEventReferenceAt4F64D0(child); break;
            default: std::terminate();
            }
        }
    }
};
Slot CapturedCopy(const Slot& from, std::uintptr_t base, Control* c) noexcept
{
    Slot r{base, Read(from.biasAt04), c};
    if (c) ++c->referenceCountAt04;
    return r;
}
void CreateButtonAt5045C0(Slot& out, Common& parent)
{
    out = {};
    Local output{out};
    auto& s = RequireLikabilityDialogNativeServices();
    Slot temp;
    CreateGuiButtonAt5041D0(temp, &parent, LikabilityDialogNativeButtonSample18(), s.loader, s.surfaces, s.strings);
    Local temporary{temp};
    Assign(out, temp, false);
    temporary.active = false;
    ReleaseGuiControlReferenceAt4F6460(temp);
    output.active = false;
}
}
LikabilityDialogNativeBody94h* ConstructLikabilityDialogNativeAt4E0E30(LikabilityDialogNativeBody94h& b)
{
    using namespace LikabilityNativeDetail;
    auto& s = RequireLikabilityDialogNativeServices();
    const auto& profile = GameLikabilityResourceProfile();
    RequireUsableLikabilityProfile(profile);
    if (!std::holds_alternative<LikabilityBaseline18Data>(profile.paint))
    {
        if (!s.profileHost) throw std::logic_error("Likability profile host is not bound");
        b.profileState = ConstructLikabilityProfileGui(b, profile, *s.profileHost);
        return &b;
    }
    ConstructGuiDialogAt4E0BE0(&b.common, nullptr, -1);
    b.common.vptrAt00 = &LikabilityDialogNativeVtable();
    ConstructorFrame f{b, {}, {}};
    f.state = 0;
    ::new(&b.clickedManagerAt6C) Slot;
    ::new(&b.backgroundAt78) Slot;
    ::new(&b.canvasAt84) Slot;
    f.state = 3;
    BorrowArchive0(s.source.CaptureEnvironmentAt4180C0(0x4E0ECF));
    s.source.EnsureRootAt413BE0(0x4E0F1B);
    auto* root = s.source.RootAt6198C4(0x4E0F26);
    const auto variant = NamedValue(root, ExtraNames()[0], 0x4E0F36) == 0 ? 1u : 0u;
    auto* receiver = CaptureDraw(0x4E0F56, 0x4E0F5B);
    if (!LoadGuiControlSurfaceReferenceAt480000(b.canvasAt84, receiver,
        LikabilityDialogNativeBackgroundNamesSample18()[variant], s.loader, s.surfaces, s.strings)) throw 1;
    auto* managerRaw = ::operator new(0x0C);
    auto* manager = managerRaw ? ::new(managerRaw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(b.clickedManagerAt6C, manager);
    RECT bounds{};
    f.raw = ::operator new(0x38);
    f.state = 4;
    auto* groupBody = static_cast<GuiControlResourceNativeBody38h*>(f.raw);
    if (groupBody) ConstructGuiControlResourceBodyAt4DABB0(*groupBody);
    f.state = 3; // 4D76A0返回前没有局部Ref展开资格。
    ConstructGuiControlResourceReferenceAt4D76A0(f.group, groupBody, 1);
    f.state = 5;
    auto canvasArgument = Copy(b.canvasAt84);
    const auto initialGroupBase = Read(f.group.baseAt00);
    auto* const groupControl = Read(f.group.controlAt08);
    AppendGuiControlSurfaceArgumentAt538350(*ResolveCaptured<GuiControlResourceNativeBody38h>(
        initialGroupBase, Read(f.group.biasAt04), groupControl), 0, canvasArgument);
    const auto canvasBase = Read(b.canvasAt84.baseAt00);
    auto* canvas = ResolveWithBase<Surface>(b.canvasAt84, canvasBase);
    const auto width = Read(canvas->widthAt08);
    const auto height = Read(canvas->heightAt0C);
    canvas = ResolveWithBase<Surface>(b.canvasAt84, canvasBase);
    const auto x = Read(canvas->originXAt10), y = Read(canvas->originYAt14);
    const RECT rectangle{x, y, Add(x, width), Add(y, height)};
    const auto groupBase = Read(f.group.baseAt00);
    auto* fixedGroup = ResolveCaptured<GuiControlResourceNativeBody38h>(groupBase, Read(f.group.biasAt04), groupControl);
    ::CopyRect(&fixedGroup->combinedRectAt28, &rectangle);
    f.raw = ::operator new(0x50);
    f.state = 6;
    auto* child = f.raw ? ConstructGuiStaticAt4F4050(static_cast<GuiControlNativeBody50h*>(f.raw),
        &b.common, 0, CapturedCopy(f.group, groupBase, groupControl)) : nullptr;
    f.state = 5;
    ConstructGuiStaticReferenceAt4D7940(f.child, child, 1);
    f.state = 7;
    auto* const childControl = Read(f.child.controlAt08);
    auto* selectedChild = Resolve<GuiControlNativeBody50h>(f.child);
    RECT scratch;
    ::UnionRect(&bounds, &bounds, selectedChild->vptrAt00->rectAt3C(selectedChild, &scratch));
    GuiDialogBaseVtable().appendChildAt10(&b.common, CapturedCopy(f.child, Read(f.child.baseAt00), childControl));
    f.state = 8;
    ReleaseCaptured(childControl);
    f.state = 9;
    ReleaseCaptured(groupControl);
    f.state = 3;
    CreateButtonAt5045C0(f.group, b.common); // 复用原局部槽，旧位模式不再有释放资格。
    f.state = 10;
    CreateToolClickedEventAt4E2F5A(f.child, f.group);
    f.state = 11;
    auto* const eventControl = Read(f.child.controlAt08);
    auto eventArgument = CapturedCopy(f.child, Read(f.child.baseAt00), eventControl);
    BindEventAt536DB0(Resolve<GuiControlNativeBody50h>(f.group), eventArgument);
    GuiDialogBaseVtable().appendChildAt10(&b.common, Copy(f.group));
    eventArgument = CapturedCopy(f.child, Read(f.child.baseAt00), eventControl);
    AppendEventAt4FEF80(Resolve<GuiClickedEventReferences>(b.clickedManagerAt6C), eventArgument);
    f.state = 12;
    ReleaseCaptured(eventControl);
    f.state = 13;
    ReleaseCurrent(f.group);
    f.state = 3;
    b.boundsAt5C = bounds;
    AdoptGuiInputAt536EA0(b.common.inputAt18, NewGameGuiInputAt4EAAED());
    f.state = -1;
    return &b; // 原4E1368不写+90。
}
}
