#include <Runtime/ScenePlaybackScriptRuntime.h>

namespace KaGuYa::Start::Runtime
{
ScenePlaybackScriptRuntime::ScenePlaybackScriptRuntime(
    const ParamsRuntimeProfile& profile, const IScrResourceNameSource& names,
    StartupScriptTransaction& startup, const ScrVmCore& vm,
    ScrScriptRuntimeState& script, ScrVmScriptCommandState& command) noexcept
    : catalog_(profile), names_(&names), startup_(&startup), vm_(&vm),
      script_(&script), command_(&command)
{
}

ScenePlaybackSelectionRef ScenePlaybackScriptRuntime::FindSelectionAt4ADAE0(
    const std::int32_t group, const std::int32_t item, const std::int32_t segment)
{
    return catalog_.FindSelection(group, item, segment);
}

std::string ScenePlaybackScriptRuntime::ResolveScriptNameAt4AD1B0(
    const ScenePlaybackSelection& selection)
{
    const auto names = names_->CapturePatternNames();
    const char* const rawName = names.FindNameAt4AD1B0(selection.fileIndex);
    // 4B894A 的 40B530 先做 CString 拷贝，越界 null 经 437680(0,0) 变空串。
    return rawName ? std::string(rawName) : std::string{};
}

void ScenePlaybackScriptRuntime::LoadReplayScriptAt4D53D0(
    const char* const name, const std::int32_t entry)
{
    // 4B8990 不检查返回值，下一调用必定是 VM；异常仍直接传播。
    static_cast<void>(startup_->Execute(name, entry));
}

std::int32_t ScenePlaybackScriptRuntime::ExecuteScriptAt4D3FF0()
{
    return vm_->ExecuteUntilYield(*script_);
}

void ScenePlaybackScriptRuntime::WriteScriptDwordAt04(const std::uint32_t value)
{
    command_->WriteFlagsAt04(value);
}

void ScenePlaybackScriptRuntime::WriteScriptByteAt08(const std::uint8_t value)
{
    // 4B8A2F/4B8C0A 只写 +08 低 byte，保留高三 byte 的原值和有效性。
    command_->WriteReloadByteAt08(value);
}
}
