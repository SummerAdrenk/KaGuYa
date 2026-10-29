#include "ProgramMain01DialogNative.Internal.h"
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain01NativeDetail;
void SetAvailable(Common& parent, std::int32_t id, std::int32_t value)
{
    Fresh child;
    Read(parent.vptrAt00)->findChildAt14(&parent, &child.slot, id);
    RefLife life{child.slot};
    if (Read(child.slot.baseAt00))
    {
        auto* selected = Resolve<Child>(child.slot);
        Read(selected->vptrAt00)->writeStateAt44(selected, value);
    }
    life.Current(true); // 501A76/501B00/501D26/501DB0，正常尾异常terminate。
}
void Blit(ProgramMain01DialogNativeServices& s, GraphicsSurfaceNativeBody* destination,
    GraphicsSurfaceNativeBody* source, std::int32_t x, std::int32_t y)
{
    // 4425E6..442602：两个handle先门控，再由既有GraphicsGui叶晚取4182F0。
    const auto target = Read(destination->handleAt04);
    if (!target) throw 1;
    const auto from = Read(source->handleAt04);
    if (!from) throw 1;
    if (!s.shared.graphics.CompositionAt4182F0().BlitSurface(target, from, x, y, nullptr, nullptr, nullptr, 0)) throw 1;
}
std::int32_t ReadInitialParameter(IProgramMain01DialogNativeSource& source, const char* name,
    std::uint32_t envSite, std::uint32_t ensureSite, std::uint32_t rootSite,
    std::uint32_t nameSite, std::uint32_t paramsSite, std::uint32_t readSite)
{
    auto* archive = Archive0(source.CaptureEnvironmentAt4180C0(envSite));
    source.EnsureRootAt413BE0(ensureSite);
    auto* root = source.RootAt6198C4(rootSite);
    const auto index = source.ResolveNamedAt498020(archive, name, nameSite);
    auto* params = source.ParamsAt47BDB0(root, paramsSite);
    return source.ReadValueAt4C73A0(params, index, readSite);
}
std::int32_t ConsumeFirstEvent(Slot& managerSlot)
{
    auto* manager = Resolve<GuiClickedEventReferences>(managerSlot);
    auto* p = Read(manager->begin);
    auto* end = Read(manager->end); // 50FDB3..50FDB5固定范围，不重取manager或end。
    for (; p != end; ++p)
    {
        const auto base = Read(p->baseAt00);
        auto* event = ResolveCached<GuiClickedEventNativeBody>(*p, base, Read(p->controlAt08));
        if (!Read(event->clickedFlagAt04)) continue;
        event = ResolveCached<GuiClickedEventNativeBody>(*p, base, Read(p->controlAt08));
        const auto result = Read(event->eventValueAt08);
        event = ResolveCached<GuiClickedEventNativeBody>(*p, base, Read(p->controlAt08));
        Read(event->vptrAt00)->clearAt04(event);
        return result;
    }
    return -1;
}
}
void BeginProgramMain01DialogNativeAt50C2A0(Body& b)
{
    LeafScope leaves;
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(b.managerAt5C));
    Read(b.common.vptrAt00)->writeEnabledAt24(&b.common, 1);
    Read(b.common.vptrAt00)->resetInputAt3C(&b.common);
    Read(b.common.vptrAt00)->resetChildrenAt68(&b.common);
    RefreshProgramMain01DialogNativeAt501880(b);
    b.selectionAt80 = -1;
}
std::int32_t PollProgramMain01DialogNativeAt52D040(Body& b)
{
    LeafScope leaves;
    Read(b.common.vptrAt00)->processInputAt54(&b.common);
    const auto event = ConsumeFirstEvent(b.managerAt5C);
    if (event >= 0)
    {
        const auto selection = Add(event, -17);
        if (std::uint32_t(selection) <= 5u) { b.selectionAt80 = selection; return 0; }
    }
    return 1;
}
void RefreshProgramMain01DialogNativeAt501880(Body& b)
{
    LeafScope leaves;
    auto& source = leaves.services.source;
    for (const auto& row : ProgramMain01DialogNativeAvailabilitySample18())
    {
        // 内联Environment锁/构造与4180C0同体；每轮都捕获，不缓存Root或参数receiver。
        auto* archive = Archive0(source.CaptureEnvironmentAt4180C0(0x5018D0));
        source.EnsureRootAt413BE0(0x501A01);
        auto* root = source.RootAt6198C4(0x501A0B);
        const auto index = source.ResolveNamedAt498020(archive, Read(row.nameAt00), 0x501A17);
        auto* params = source.ParamsAt47BDB0(root, 0x501A1F);
        const auto value = source.ReadValueAt4C73A0(params, index, 0x501A26);
        SetAvailable(b.common, Read(row.buttonAt04), value >= 0);
        for (std::int32_t i = 0; i < 3; ++i) SetAvailable(b.common, Read(row.symbolsAt08[i]), value == i);
    }
    const auto& row = ProgramMain01DialogNativeLastAvailabilitySample18();
    auto* archive = Archive0(source.CaptureEnvironmentAt4180C0(0x501B80));
    source.EnsureRootAt413BE0(0x501CB1);
    auto* root = source.RootAt6198C4(0x501CBB);
    const auto index = source.ResolveNamedAt498020(archive, Read(row.nameAt00), 0x501CC7);
    auto* params = source.ParamsAt47BDB0(root, 0x501CCF);
    const auto value = source.ReadValueAt4C73A0(params, index, 0x501CD6);
    SetAvailable(b.common, Read(row.buttonAt04), value >= 0);
    for (std::int32_t i = 0; i < 2; ++i) SetAvailable(b.common, Read(row.symbolsAt08[i]), value == i);
}
void ComposeProgramMain01DialogNativeAt5281F0(Body& b)
{
    LeafScope leaves;
    auto& s = leaves.services;
    auto& source = s.source;
    const auto time = ReadInitialParameter(source, "time", 0x52821D, 0x528269,
        0x52826E, 0x52827B, 0x528283, 0x52828A);
    {
        Slot image{}; RefLife imageLife{image}; // FI0，从480000之前起生效。
        source.EnsureRootAt413BE0(0x5282B2);
        auto* root = source.RootAt6198C4(0x5282B7);
        auto* draw = Resolve(source.RootDrawSlotAt18(root));
        const auto* name = Read(ProgramMain01DialogNativeTimeNamesSample18()[time]);
        if (!source.LoadSurfaceAt480000(draw, &image, name, 0x5282E9)) throw 1;
        auto* capturedImage = Read(image.controlAt08);
        auto* picture = Resolve<GraphicsSurfaceNativeBody>(image);
        const auto x = Read(picture->originXAt10), y = Read(picture->originYAt14);
        picture = Resolve<GraphicsSurfaceNativeBody>(image);
        auto* destination = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
        Blit(s, destination, picture, x, y);
        imageLife.Captured(capturedImage); // FI1，正常释放故障terminate。
    }
    GuiControlSurfaceReferences digits{};
    struct VectorLife final
    {
        GuiControlSurfaceReferences& vector;
        ~VectorLife() noexcept { DestroyGuiControlSurfaceReferencesAt43EAA0(vector); }
    } digitsLife{digits}; // FI2
    Slot* cachedEnd = nullptr; // 5283A2的EDI跨Root/Load/Ref释放回调保持，不提前重取end。
    for (std::int32_t i = 0; i < 10; ++i)
    {
        Slot image{}; RefLife imageLife{image}; // FI3
        // 捕获draw后才选名称/new24；不可让loader在分配后重新取Root。
        auto* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        const auto* name = Read(ProgramMain01DialogNativeDigitsSample18().data()[i]);
        auto* picture = NewGraphicsSurfaceAt40C2E0(s.shared.surfaces, s.shared.strings); // FI4由该叶处理raw24。
        AdoptGraphicsSurfaceAt407A80(image, picture);
        auto* captured = Read(image.controlAt08);
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<GraphicsSurfaceNativeBody>(image), name)) throw 1;
        if (cachedEnd != Read(digits.capacity))
        {
            cachedEnd->baseAt00 = Read(image.baseAt00);
            cachedEnd->biasAt04 = Read(image.biasAt04);
            cachedEnd->controlAt08 = captured;
            if (captured) ++captured->referenceCountAt04;
            ++cachedEnd;
            digits.end = cachedEnd;
        }
        else
        {
            // 4D83D0的53C120/53BED0/53B4B0与4540A0同一分配、复制增持、释放发布协议。
            GrowGuiControlSurfaceReferencesAt4540A0(digits, cachedEnd, image);
            cachedEnd = Read(digits.end);
            captured = Read(image.controlAt08);
        }
        // 5284A4仅增长路重读；快路ESI保持4800A0前所选control。
        imageLife.Captured(captured, true); // FI5
    }
    const auto month = ReadInitialParameter(source, "month", 0x5284EE, 0x52853A,
        0x52853F, 0x52854C, 0x528554, 0x52855B);
    auto* first = Read(digits.begin);
    auto* monthImage = Resolve<GraphicsSurfaceNativeBody>(first[month % 10]);
    auto* destination = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
    Blit(s, destination, monthImage, 11, 27); // 608F84/608F88
    source.EnsureRootAt413BE0(0x5285DD);
    auto* root = source.RootAt6198C4(0x5285E2);
    auto* env = source.CaptureEnvironmentAt4180C0(0x5285EF);
    auto* archive = source.RegistryAt417990(env, 0, 0x5285F6);
    const auto index = source.ResolveNamedAt498020(archive, "date", 0x5285FD);
    auto* params = source.ParamsAt47BDB0(root, 0x528605);
    const auto date = source.ReadValueAt4C73A0(params, index, 0x52860C);
    const auto tens = date / 10;
    auto* dayImage = Resolve<GraphicsSurfaceNativeBody>(first[date % 10]);
    destination = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
    Blit(s, destination, dayImage, 79, 27); // 608F8C/608F90
    if (tens)
    {
        auto* tensImage = Resolve<GraphicsSurfaceNativeBody>(Read(digits.begin)[tens]);
        destination = Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt74);
        Blit(s, destination, tensImage, 55, 27); // 608F94/608F98
    }
}
}
