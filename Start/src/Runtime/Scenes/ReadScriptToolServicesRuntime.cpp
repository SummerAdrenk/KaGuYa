#include <Runtime/ReadScriptToolServicesRuntime.h>
#include <Runtime/ReadScriptInputPredicatesRuntime.h>
#include <Runtime/ToolWindowInteractionRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptToolServicesRuntime::ReadScriptToolServicesRuntime(DrawToolView window,ReadScriptInputPredicatesRuntime& input) noexcept
    :window_(window),input_(input){}
ReadScriptToolServicesRuntime::ReadScriptToolServicesRuntime(ToolWindowLayerRuntime& window,
    ToolWindowInteractionRuntime& interaction, ReadScriptInputPredicatesRuntime& input) noexcept
    :ReadScriptToolServicesRuntime(DrawToolView(window,interaction),input){}

void ReadScriptToolServicesRuntime::SetToolModeAt434FB0(std::int32_t value)
{
    // 4B2730 正文传0/1，4B11B0 选项传2；434FDD/435174 分别比较2/1。
    window_.SetModeAt434FB0(value);
}
void ReadScriptToolServicesRuntime::InitializeToolAt4AB910(bool value)
{
    window_.ResetSkipLatchAt4AB910(value ? 1 : 0);
}
bool ReadScriptToolServicesRuntime::PollConfiguredKeyAt4B0630(std::uint32_t fieldOffset)
{
    return input_.ConfiguredMouseAtClickOffset(fieldOffset);
}
bool ReadScriptToolServicesRuntime::ReadReplayValue() { return input_.ReplayValue(); }
std::uint32_t ReadScriptToolServicesRuntime::UpdateToolAt4BE200() { return window_.TickAt4BE200(); }
std::int32_t ReadScriptToolServicesRuntime::ToolEventAt34() { return window_.ReadEventAt34(); }
std::int32_t ReadScriptToolServicesRuntime::ToolSkipPredicateAt4B0760(bool value) { return input_.SkipAt4B0760(value); }
std::int32_t ReadScriptToolServicesRuntime::ToolAutoPredicateAt4B0470(bool value) { return input_.AutoAt4B0470(value); }
bool ReadScriptToolServicesRuntime::PollSaveAt47F830() { return input_.SaveAt47F830(); }
bool ReadScriptToolServicesRuntime::PollLoadAt47F650() { return input_.LoadAt47F650(); }
bool ReadScriptToolServicesRuntime::PollQuickSaveAt47F780() { return input_.QuickSaveAt47F780(); }
bool ReadScriptToolServicesRuntime::PollQuickLoadAt47F6D0() { return input_.QuickLoadAt47F6D0(); }
bool ReadScriptToolServicesRuntime::PollLikabilityAt47F530(std::int32_t value)
{
    return input_.LikabilityAt47F530(static_cast<std::uint8_t>(value));
}
void ReadScriptToolServicesRuntime::CaptureToolAt4C01F0() { window_.CaptureAt4C01F0(); }
void ReadScriptToolServicesRuntime::SetToolCaptureModeAt4C2300(bool value)
{
    window_.SetCaptureModeAt4C2300(value ? 1 : 0);
}
void ReadScriptToolServicesRuntime::RestoreToolAt4BF4E0() { window_.RestoreAt4BF4E0(); }
}
