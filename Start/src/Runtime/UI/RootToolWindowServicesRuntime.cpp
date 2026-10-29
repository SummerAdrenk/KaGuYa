#include <Runtime/RootToolWindowServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootToolWindowServicesRuntime::RootToolWindowServicesRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, const Platform::IInputRuntime& input, ScriptNamedValueNativeTables tables,
    decltype(&::MessageBoxA) messageBox,ToolWindowNativeInteractionServices* native,IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), root_(root), named_(environment, root, tables, messageBox,strings), input_(input),native_(native) {}

DrawLayersView RootToolWindowServicesRuntime::CaptureLayers()
{
    root_.EnsureCreatedAt413BE0();
    const auto manager = root_.RequirePublished().PostWindow().DrawLayersAt18();
    if (!manager) throw std::logic_error("tool query accesses absent DrawManager (modern safety boundary)");
    return manager;
}
void RootToolWindowServicesRuntime::Invoke434FB0(std::int32_t argument)
{
    // 46D55C..46D593：先取根+18的owning索引2；查询期间不重新选择本次CToolWin。
    BorrowToolAtSlotTwo(CaptureLayers()).SetModeAt434FB0(argument);
}
std::int32_t RootToolWindowServicesRuntime::ReadReplayAt417B50()
{
    // 417B58先slot0，417BA4才取根；与419A22先根的顺序相反。
    return named_.ReadAfterSlot0At4C73A0("replay");
}
std::uint32_t RootToolWindowServicesRuntime::ReadUnSaveModeAt04()
{ return environment_.Archives().UnSaveData().ModeAt04(); }
std::uint8_t RootToolWindowServicesRuntime::ReadUnSaveByteAt08()
{ return environment_.Archives().UnSaveData().BytesAt08()[0]; }
std::uint8_t RootToolWindowServicesRuntime::ReadUnSaveByteAt09()
{ return environment_.Archives().UnSaveData().BytesAt08()[1]; }
std::uint8_t RootToolWindowServicesRuntime::ReadUnSaveByteAt0C()
{ return static_cast<std::uint8_t>(environment_.Archives().UnSaveData().DwordAt0C()); }
std::uint32_t RootToolWindowServicesRuntime::ReadUnSaveFlagsAt14()
{ return environment_.Archives().UnSaveData().FlagsAt14(); }
std::uint32_t RootToolWindowServicesRuntime::ReadUnSaveFlagsAt18()
{ return environment_.Archives().UnSaveData().FlagsAt18(); }
std::uint32_t RootToolWindowServicesRuntime::ReadUnSaveFlagsAt1C()
{ return environment_.Archives().UnSaveData().FlagsAt1C(); }
std::uint32_t RootToolWindowServicesRuntime::ReadUnSaveFlagsAt20()
{ return environment_.Archives().UnSaveData().FlagsAt20(); }
bool RootToolWindowServicesRuntime::HasVoiceArrayStorageAt3C()
{ return static_cast<bool>(environment_.Archives().UnSaveData().ArrayReferenceAt3C().StorageAt3C()); }
std::int32_t RootToolWindowServicesRuntime::ReadTextGlyphCursorAt50()
{
    return CaptureLayers().TextAtSlotThree().GlyphCursorAt50();
}
std::uint8_t RootToolWindowServicesRuntime::ReadMessageWindowEnabledAt0C()
{
    return CaptureLayers().MessageWindowEnabledAtSlotOne();
}
bool RootToolWindowServicesRuntime::IsSkipKeyHeldAt4198D0()
{ return input_.ReadHeldAtVslot00(6); }
std::int32_t RootToolWindowServicesRuntime::ReadReplayValueAt4C73A0()
{ return named_.ReadAfterRootAt4C73A0("replay"); }
std::int32_t RootToolWindowServicesRuntime::ReadAffectionDisplayFlagAt4C73A0()
{
    // 419A6A -> 5AE270的CP932常量，保留全角空格。
    constexpr char name[] = "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    return named_.ReadAfterRootAt4C73A0(name);
}
}
