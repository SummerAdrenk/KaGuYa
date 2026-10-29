#include <Runtime/ToolWindowInteractionServicesRuntime.h>

#include <Runtime/MessageWindowLayerRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ToolWindowInteractionServicesRuntime::ToolWindowInteractionServicesRuntime(
    const UnSaveDataRuntime& unsave, DrawLayersView layers,
    const ScrVmValueTable& values, const ScrVmExecutableProfile& profile,
    Platform::MouseView mouse, Platform::KeyboardView keyboard) noexcept
    : unsave_(unsave), layers_(layers), values_(values), profile_(profile), input_(mouse, keyboard) {}

std::uint32_t ToolWindowInteractionServicesRuntime::ReadUnSaveModeAt04() { return unsave_.ModeAt04(); }
std::uint8_t ToolWindowInteractionServicesRuntime::ReadUnSaveByteAt08() { return unsave_.BytesAt08()[0]; }
std::uint8_t ToolWindowInteractionServicesRuntime::ReadUnSaveByteAt09() { return unsave_.BytesAt08()[1]; }
std::uint8_t ToolWindowInteractionServicesRuntime::ReadUnSaveByteAt0C()
{
    return static_cast<std::uint8_t>(unsave_.DwordAt0C());
}
std::uint32_t ToolWindowInteractionServicesRuntime::ReadUnSaveFlagsAt14() { return unsave_.FlagsAt14(); }
std::uint32_t ToolWindowInteractionServicesRuntime::ReadUnSaveFlagsAt18() { return unsave_.FlagsAt18(); }
std::uint32_t ToolWindowInteractionServicesRuntime::ReadUnSaveFlagsAt1C() { return unsave_.FlagsAt1C(); }
std::uint32_t ToolWindowInteractionServicesRuntime::ReadUnSaveFlagsAt20() { return unsave_.FlagsAt20(); }
bool ToolWindowInteractionServicesRuntime::HasVoiceArrayStorageAt3C()
{
    // 419AC0 只检查原 +3C 是否为空；空 vector 的已分配 owner 同样通过。
    return static_cast<bool>(unsave_.ArrayReferenceAt3C().StorageAt3C());
}
std::int32_t ToolWindowInteractionServicesRuntime::ReadTextGlyphCursorAt50()
{
    return layers_.TextAtSlotThree().GlyphCursorAt50();
}
std::uint8_t ToolWindowInteractionServicesRuntime::ReadMessageWindowEnabledAt0C()
{
    return layers_.MessageWindowEnabledAtSlotOne();
}
bool ToolWindowInteractionServicesRuntime::IsSkipKeyHeldAt4198D0()
{
    return input_.ReadHeldAtVslot00(6);
}
std::int32_t ToolWindowInteractionServicesRuntime::ReadReplayValueAt4C73A0()
{
    return values_.ReadSigned(profile_.ResolveNamedValue("replay"));
}
std::int32_t ToolWindowInteractionServicesRuntime::ReadAffectionDisplayFlagAt4C73A0()
{
    constexpr char name[] = "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    return values_.ReadSigned(profile_.ResolveNamedValue(name));
}
}
