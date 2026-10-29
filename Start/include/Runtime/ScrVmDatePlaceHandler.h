#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IScriptDateCommandSource;
class IScrVmCommandErrorSink;
// 对应 opcode 23 / 0x4D1CE0。无 operand，按 replay 同步 date_win。
class ScrDateWindowInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrDateWindowInstructionHandler(IScriptDateCommandSource& source, IScrVmCommandErrorSink& errors) noexcept;
    // 显式离线Execute接口；不能作为生产入口的图层状态兜底。
    ScrDateWindowInstructionHandler(
        ScrVmValueTable& values,
        const ScrVmExecutableProfile& executableProfile) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState&, IEnvironmentRootMemory&) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable* values_ = nullptr;
    const ScrVmExecutableProfile* executableProfile_ = nullptr;
    IScriptDateCommandSource* source_ = nullptr;
    IScrVmCommandErrorSink* errors_ = nullptr;
};

// 对应 opcode 24 / 0x4D1BF0。
class ScrDatePlaceResetInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrDatePlaceResetInstructionHandler(IScriptDateCommandSource& source, IScrVmCommandErrorSink& errors) noexcept;
    ScrDatePlaceResetInstructionHandler(
        ScrVmValueTable& values,
        const ScrVmExecutableProfile& executableProfile) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState&, IEnvironmentRootMemory&) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable* values_ = nullptr;
    const ScrVmExecutableProfile* executableProfile_ = nullptr;
    IScriptDateCommandSource* source_ = nullptr;
    IScrVmCommandErrorSink* errors_ = nullptr;
};
}
