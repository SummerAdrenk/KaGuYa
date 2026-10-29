#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
std::uint8_t CurrentMode()
{
    auto& s = RequireSettingPage02NativeServices().source;
    s.EnsureRootAt413BE0(); auto* root = s.RootAt6198C4();
    auto* mode = Resolve<void>(s.RootWindowModeSlotAt0C(root));
    return static_cast<std::uint8_t>(Read(Field<std::uint8_t>(mode, 4)) != 0);
}
void ModeControls(Common* p, std::uint8_t mode, bool invalidate)
{
    const auto& profile = CurrentSettingTextDisplayProfile().mode;
    // 01:4EAD50，02:4F0FB0，03:4F0760：画面设置只在全屏时可操作。
    // 按显式命令表定位extra，不把01的59或02/03的64套到其它profile。
    for (const auto& command : profile.commands)
    {
        if (command.command != 0x801D) continue;
        Fresh found;
        auto* returned = Read(p->vptrAt00)->findChildAt14(p, &found.slot, command.event);
        RefLife life{found.slot};
        auto* child = Resolve<GuiControlNativeBody50h>(*returned);
        Read(child->vptrAt00)->writeStateAt44(child, mode != 0);
        life.Normal();
    }
    const int events[]{profile.windowEvent, profile.fullscreenEvent};
    for (int index = 0; index != 2; ++index)
    {
        Fresh found;
        auto* returned = Read(p->vptrAt00)->findChildAt14(p, &found.slot, events[index]);
        RefLife life{found.slot};
        // 原509710直接解Ref，不将缺少必需按钮转换为空成功。
        auto* child = Resolve<GuiControlNativeBody50h>(*returned);
        Read(child->vptrAt00)->writeStateAt44(child, index == 0 ? mode : mode == 0);
        life.Normal(index == 1 && mode == 0);
    }
    if (invalidate) Read(p->vptrAt00)->invalidateAt64(p, Field<RECT>(p, 0x5C));
}
int PollManager(Mode& b)
{
    auto* manager = Resolve<GuiClickedEventReferences>(b.managerAt6C);
    auto* position = Read(manager->begin); const auto* end = Read(manager->end);
    for (; position != end; ++position)
    {
        const auto base = Read(position->baseAt00);
        if (!Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->clickedFlagAt04)) continue;
        const auto value = Read(ResolveBase<GuiClickedEventNativeBody>(*position, base)->eventValueAt08);
        auto* event = ResolveBase<GuiClickedEventNativeBody>(*position, base); Read(event->vptrAt00)->clearAt04(event);
        return value;
    }
    return -1;
}
}
void __fastcall BeginMode(Common* p, void*)
{
    auto& b = *reinterpret_cast<Mode*>(p);
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(b.managerAt6C));
    Read(p->vptrAt00)->writeEnabledAt24(p, 1); Read(p->vptrAt00)->resetChildrenAt68(p);
    b.modeAt78 = CurrentMode(); ModeControls(p, Read(b.modeAt78), false);
}
std::int32_t __fastcall PollMode(Common* p, void*)
{
    auto& b = *reinterpret_cast<Mode*>(p);
    const auto current = CurrentMode();
    if (Read(b.modeAt78) != current)
    {
        // 原变化分支第二次Ensure/选根/取mode，不能使用首次捕获值代替。
        b.modeAt78 = CurrentMode(); ModeControls(p, Read(b.modeAt78), true); return 1;
    }
    Read(p->vptrAt00)->processInputAt54(p);
    const auto event = PollManager(b);
    for (const auto& command : CurrentSettingTextDisplayProfile().mode.commands)
    {
        if (event != command.event) continue;
        if (command.updateModeButtons)
        {
            Read(p->vptrAt00)->resetChildrenAt68(p);
            ModeControls(p, static_cast<std::uint8_t>(command.fullscreen), true);
        }
        auto& source = RequireSettingPage02NativeServices().source;
        auto* system = source.CaptureEnvironmentAt4180C0();
        const auto window = Read(system->windowAt2C);
        // Original callers ignore the BOOL result. Poll's 1 means input processed, not mode applied.
        (void)source.PostMessageA(reinterpret_cast<HWND>(window), WM_COMMAND, command.command, 0);
        // 发送后不写缓存78，也不在本页伪造真实Root模式切换完成。
        break;
    }
    return 1;
}
}
