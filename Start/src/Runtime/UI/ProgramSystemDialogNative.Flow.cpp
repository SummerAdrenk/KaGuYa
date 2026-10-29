#include "ProgramSystemDialogNative.Internal.h"
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace ProgramSystemNativeDetail
{
namespace
{
void ResetControl(Common& common, std::int32_t event)
{
    Fresh child;
    Read(common.vptrAt00)->findChildAt14(&common, &child.slot, event);
    LocalReference lifetime{child.slot};
    auto* receiver = Resolve<GuiControlNativeBody50h>(child.slot);
    Read(receiver->vptrAt00)->writeStateAt44(receiver, 0);
    lifetime.Finish();
}
}
void Begin(Body& body, bool mode01)
{
    Leaves leaves;
    auto& common = body.common;
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(body.clickedManagerAt5C));
    Read(common.vptrAt00)->writeEnabledAt24(&common, 1);
    Read(common.vptrAt00)->resetChildrenAt68(&common);
    ResetControl(common, mode01 ? 0x18 : 0x31);
    ResetControl(common, mode01 ? 0x1A : 0x33);
    {
        Fresh child;
        Read(common.vptrAt00)->findChildAt14(&common, &child.slot, mode01 ? 0x1D : 0x36);
        LocalReference lifetime{child.slot};
        // 50C451/50C741：每次独立捕获Env，直接读archives[2]，不是417990调用。
        auto* archive = Archive2(leaves.services.parent.source.CaptureEnvironmentAt4180C0(
            mode01 ? 0x50C741 : 0x50C451));
        if ((Read(Field<std::uint8_t>(archive, 0x18)) & 2u) != 0)
            Resolve<GuiCheckBoxNativeBody60h>(child.slot)->checkedAt5C = 1;
        // flag为0不写checked，也不调用writeState；不能对称化为重置0。
        lifetime.Finish();
    }
    {
        Fresh child;
        Read(common.vptrAt00)->findChildAt14(&common, &child.slot, mode01 ? 0x1E : 0x37);
        LocalReference lifetime{child.slot};
        auto* archive = Archive2(leaves.services.parent.source.CaptureEnvironmentAt4180C0(
            mode01 ? 0x50C7FF : 0x50C50F));
        auto* receiver = Resolve<GuiCheckBoxNativeBody60h>(child.slot);
        receiver->checkedAt5C = (Read(Field<std::uint32_t>(archive, 0x14)) >> 1) & 1u;
        auto* control = Resolve<GuiControlNativeBody50h>(child.slot);
        Read(control->vptrAt00)->writeStateAt44(control, 0);
        lifetime.Finish();
    }
}
std::int32_t Poll(Body& body, bool mode01)
{
    Leaves leaves;
    auto& s = leaves.services;
    auto& common = body.common;
    Read(common.vptrAt00)->processInputAt54(&common);
    const auto event = s.source.PollClickedEventManagerAt50FDB0(
        Resolve<GuiClickedEventReferences>(body.clickedManagerAt5C), 1, mode01 ? 0x52D2C1 : 0x52D0E1);
    if (event < 0) return 1;
    if (event == (mode01 ? 0x1D : 0x36))
    {
        Fresh child;
        Read(common.vptrAt00)->findChildAt14(&common, &child.slot, event);
        LocalReference lifetime{child.slot};
        const auto checked = Read(Resolve<GuiCheckBoxNativeBody60h>(child.slot)->checkedAt5C);
        Resolve<GuiCheckBoxNativeBody60h>(child.slot)->checkedAt5C = checked == 0;
        auto* environment = s.parent.source.CaptureEnvironmentAt4180C0(mode01 ? 0x52D32E : 0x52D14E);
        auto* registry = s.parent.source.RegistryAt417990(environment, 2, mode01 ? 0x52D335 : 0x52D155);
        auto& flags = Field<std::uint32_t>(registry, 0x18);
        flags = Read(flags) ^ 2u;
        lifetime.active = false;
        ReleaseGuiControlReferenceAt4F6460(child.slot); // 原直接helper；释放故障终止。
        return 1;
    }
    const auto routes = mode01 ? ProgramSystem01DialogNativeRoutesSample18() : ProgramSystem00DialogNativeRoutesSample18();
    // 原switch仅52/53/56..59或27/28/31..34；skip及未知event完全不动作。
    const bool routed = mode01 ? (event == 27 || event == 28 || (event >= 31 && event <= 34)) :
        (event == 52 || event == 53 || (event >= 56 && event <= 59));
    if (!routed) return 1;
    Fresh child;
    Read(common.vptrAt00)->findChildAt14(&common, &child.slot, event);
    LocalReference lifetime{child.slot};
    auto* receiver = Resolve<GuiControlNativeBody50h>(child.slot);
    Read(receiver->vptrAt00)->resetAt08(receiver);
    for (std::uint32_t i = 0; i != 6; ++i)
    {
        if (event != Read(routes[i].eventAt00)) continue;
        s.parent.source.EnsureRootAt413BE0(mode01 ? 0x52D39A : 0x52D1BA);
        auto* root = s.parent.source.RootAt6198C4(mode01 ? 0x52D39F : 0x52D1BF);
        auto* program = Resolve<void>(s.source.RootProgramSlotAt30(root));
        auto& destination = Field<RuntimeCString>(program, 0x1C);
        const auto* name = Read(routes[i].nameAt04);
        const auto length = name ? static_cast<int>(std::strlen(name)) : 0;
        SetRuntimeCStringAt437680(destination, name, length);
        break;
    }
    lifetime.Finish();
    return 1; // 52D23E/52D41E真实常量，不代表Root整合已完成。
}
}
void BeginProgramSystem00DialogNativeAt50C300(ProgramSystemDialogNativeBody68h& b)
{ ProgramSystemNativeDetail::Begin(b, false); }
void BeginProgramSystem01DialogNativeAt50C5F0(ProgramSystemDialogNativeBody68h& b)
{ ProgramSystemNativeDetail::Begin(b, true); }
std::int32_t PollProgramSystem00DialogNativeAt52D090(ProgramSystemDialogNativeBody68h& b)
{ return ProgramSystemNativeDetail::Poll(b, false); }
std::int32_t PollProgramSystem01DialogNativeAt52D270(ProgramSystemDialogNativeBody68h& b)
{ return ProgramSystemNativeDetail::Poll(b, true); }
}
