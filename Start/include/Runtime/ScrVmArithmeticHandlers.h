#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScriptInvalidCommandNative.h>

#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
class IScriptArithmeticCommandSource;

enum class ScrVmArithmeticError : std::uint8_t
{
    DivideByZero,
    SignedDivisionOverflow,
};

class ScrVmArithmeticException final : public std::runtime_error
{
public:
    ScrVmArithmeticException(ScrVmArithmeticError error, const char* message);
    [[nodiscard]] ScrVmArithmeticError Error() const noexcept;

private:
    ScrVmArithmeticError error_;
};

// 对应 opcode 1 / 0x4D0F40 的 12 类 DWORD 运算及值表写回。
class ScrAssignInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrAssignInstructionHandler(
        ScrVmValueTable& values,
        IScrVmCommandErrorSink& errorSink, IScriptArithmeticCommandSource* source = nullptr) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable& values_;
    IScrVmCommandErrorSink& errorSink_;
    IScriptArithmeticCommandSource* source_;
};

// 对应 opcode 13 / 0x4CD640 的八类 DWORD 比较及 0/1 写回。
class ScrCompareInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrCompareInstructionHandler(
        ScrVmValueTable& values,
        IScrVmCommandErrorSink& errorSink, IScriptArithmeticCommandSource* source = nullptr) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable& values_;
    IScrVmCommandErrorSink& errorSink_;
    IScriptArithmeticCommandSource* source_;
};
}
