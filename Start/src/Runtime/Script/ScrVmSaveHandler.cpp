#include <Runtime/ScrVmSaveHandler.h>

#include <Runtime/ScrVmControlHandlers.h>

#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ScrSaveInstructionHandler::ScrSaveInstructionHandler(IScriptSaveCommandSource& source) noexcept
    : source_(&source)
{
}

ScrSaveInstructionHandler::ScrSaveInstructionHandler(
    ScrVmValueTable& values,
    const ScrVmExecutableProfile& executableProfile,
    const ScrVmScriptCommandState& scriptCommand, IScriptSaveCommandSource* source) noexcept
    : legacy_(LegacyDependencies{values, executableProfile, scriptCommand}), source_(source)
{
}

std::int32_t ScrSaveInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    // 原生入口只借显式source，不构造独立绑定或读取legacy profile。
    if (source_)
        return ExecuteScriptSaveAt4D0AC0(runtime.NativeParserAtA0(), *source_, memory);
    if (!legacy_)
        throw std::logic_error("SAVE native entry requires an explicit source or legacy dependencies");

    class BoundSource final : public IScriptSaveCommandSource
    {
    public:
        BoundSource(ScrScriptRuntimeState& state, ScrVmValueTable& values,
            const ScrVmExecutableProfile& profile, const ScrVmScriptCommandState& command)
            : state_(state), values_(values), profile_(profile), command_(command) {}
        std::uint32_t ReadFlagsAt04() override { return command_.ReadFlagsAt04(); }
        std::int32_t ReadReplayAt4D0AF3() override
        { return values_.ReadSigned(profile_.ResolveNamedValue("replay")); }
        void WriteGateAt25(bool value) override { state_.SetSaveCommandGate(value); }
        ScriptSaveRootReceiver CaptureAfterEnsureAt413BE0() override
        {
            return {&state_, [](void* p) { return static_cast<ScrScriptRuntimeState*>(p)->ProgramCounter(); },
                [](void* p, std::int32_t value) { static_cast<ScrScriptRuntimeState*>(p)->SetCurrentSaveSlot(value); }};
        }
    private:
        ScrScriptRuntimeState& state_;
        ScrVmValueTable& values_;
        const ScrVmExecutableProfile& profile_;
        const ScrVmScriptCommandState& command_;
    } bound(runtime, legacy_->values, legacy_->executableProfile, legacy_->scriptCommand);
    // 仅旧构造且未提供source时，独立runtime显式借绑定槽。
    return ExecuteScriptSaveAt4D0AC0(runtime.NativeParserAtA0(), bound, memory);
}

std::int32_t ScrSaveInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    // source-only没有离线依赖，必须在门控、取值和save slot写入之前拒绝。
    if (!legacy_)
        throw std::logic_error("SAVE offline Execute requires explicit legacy dependencies");

    // 4D0AEA 的 +04 属于 CScript；SAVE 正文是恢复索引，不参与这个门控。
    bool gate = false;
    if ((legacy_->scriptCommand.ReadFlagsAt04() & 0x02u) == 0u)
    {
        gate = legacy_->values.ReadSigned(
            legacy_->executableProfile.ResolveNamedValue("replay")) == 0;
    }
    runtime.SetSaveCommandGate(gate);

    if (instruction.body.size() < sizeof(std::int32_t))
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM save slot is truncated");
    }

    std::int32_t slot = 0;
    std::memcpy(&slot, instruction.body.data(), sizeof(slot));

    // 0x4D0B38 先写 CScript+0x25，0x4D0C56 后写当前 save slot。
    runtime.SetCurrentSaveSlot(slot);
    return 0;
}
}
