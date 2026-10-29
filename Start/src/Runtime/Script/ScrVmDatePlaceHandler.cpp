#include <Runtime/ScrVmDatePlaceHandler.h>
#include <Runtime/ScriptDateCommandNative.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScriptOwnedStateNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ScrDateWindowInstructionHandler::ScrDateWindowInstructionHandler(IScriptDateCommandSource& source,
    IScrVmCommandErrorSink& errors) noexcept : source_(&source), errors_(&errors) {}
std::int32_t ScrDateWindowInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState&, IEnvironmentRootMemory&)
{
    if (!source_) throw std::logic_error("DATE_WINDOW native entry requires an explicit source");
    return ExecuteScriptDateWindowAt4D1CE0(*source_, *errors_);
}
ScrDateWindowInstructionHandler::ScrDateWindowInstructionHandler(
    ScrVmValueTable& values,
    const ScrVmExecutableProfile& executableProfile) noexcept
    : values_(&values),
      executableProfile_(&executableProfile)
{
}

std::int32_t ScrDateWindowInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView&)
{
    if (!values_) throw std::logic_error("DATE_WINDOW offline entry requires an explicit value table");
    const std::int32_t replay = values_->ReadSigned(
        executableProfile_->ResolveNamedValue("replay"));
    values_->WriteSigned(
        executableProfile_->ResolveNamedValue("date_win"),
        replay != 0 ? -1 : 0);
    return 0;
}

ScrDatePlaceResetInstructionHandler::ScrDatePlaceResetInstructionHandler(IScriptDateCommandSource& source,
    IScrVmCommandErrorSink& errors) noexcept : source_(&source), errors_(&errors) {}
std::int32_t ScrDatePlaceResetInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory&)
{
    if (!source_) throw std::logic_error("DATE_PLACE native entry requires an explicit source");
    const auto result = ExecuteScriptDatePlaceAt4D1BF0(*source_, *errors_);
    if (result == 3)
    {
        // Ero Zemi 4C6C10 -> 4C4AA0，与共享ResetScriptEffect是同一状态协议。
        auto* effect = ResolveEnvironmentObjectReference<ScriptEffectNative>(runtime.NativeBody().effectAt0C);
        if (!effect) throw std::logic_error("DATE_PLACE effect storage is absent");
        ResetScriptEffectAt4CD180(*effect);
    }
    return result;
}
ScrDatePlaceResetInstructionHandler::ScrDatePlaceResetInstructionHandler(
    ScrVmValueTable& values,
    const ScrVmExecutableProfile& executableProfile) noexcept
    : values_(&values),
      executableProfile_(&executableProfile)
{
}

std::int32_t ScrDatePlaceResetInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView&)
{
    // 0x4D1BF0 的顺序固定；第二次写入失败时不回滚 date_win。
    if (!values_) throw std::logic_error("DATE_PLACE offline entry requires an explicit value table");
    values_->WriteSigned(
        executableProfile_->ResolveNamedValue("date_win"),
        -1);
    values_->WriteSigned(
        executableProfile_->ResolveNamedValue("place"),
        -1);
    return 0;
}
}
