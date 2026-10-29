#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScrVmScriptCommandState.h>
#include <Runtime/ScriptSaveCommandNative.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
// 对应 opcode 25 / 0x4D0AC0。
class ScrSaveInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrSaveInstructionHandler(IScriptSaveCommandSource& source) noexcept;
    ScrSaveInstructionHandler(
        ScrVmValueTable& values,
        const ScrVmExecutableProfile& executableProfile,
        const ScrVmScriptCommandState& scriptCommand,
        IScriptSaveCommandSource* source = nullptr) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    struct LegacyDependencies final
    {
        ScrVmValueTable& values;
        const ScrVmExecutableProfile& executableProfile;
        const ScrVmScriptCommandState& scriptCommand;
    };
    std::optional<LegacyDependencies> legacy_;
    IScriptSaveCommandSource* source_;
};
}
