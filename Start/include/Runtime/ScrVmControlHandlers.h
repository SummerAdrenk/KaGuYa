#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScriptControlCommandNative.h>

#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class ScrVmOperandError : std::uint8_t
{
    OperandTruncated,
};

class ScrVmOperandException final : public std::runtime_error
{
public:
    ScrVmOperandException(
        ScrVmOperandError error,
        std::uint16_t opcode,
        const char* message);

    [[nodiscard]] ScrVmOperandError Error() const noexcept;
    [[nodiscard]] std::uint16_t Opcode() const noexcept;

private:
    ScrVmOperandError error_;
    std::uint16_t opcode_;
};

// 对应 0x4D6E90，供 jump 与 call handler 共享。
void ApplyScrVmProgramCounterTarget(
    ScrScriptRuntimeState& runtime,
    std::int32_t target);

// 对应 opcode 2 / 0x4CE2A0：把 uint16 索引对应值写为 1。
class ScrSetFlagInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrSetFlagInstructionHandler(ScrVmValueTable& values,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable& values_;
    IScriptControlCommandSource* source_;
};

// 对应 opcode 3 / 0x4CE160：把 uint16 索引对应值写为 0。
class ScrClearFlagInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrClearFlagInstructionHandler(ScrVmValueTable& values,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable& values_;
    IScriptControlCommandSource* source_;
};

// 对应 opcode 11 / 0x4CE7B0。
class ScrJumpInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrJumpInstructionHandler(IScriptControlCommandSource* source = nullptr) noexcept : source_(source) {}
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;
private:
    IScriptControlCommandSource* source_;
};

// 对应 opcode 14 / 0x4CEA90：条件值非零时设置 PC。
class ScrIfTrueInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrIfTrueInstructionHandler(const ScrVmValueTable& values,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    const ScrVmValueTable& values_;
    IScriptControlCommandSource* source_;
};

// 对应 opcode 15 / 0x4CE8E0：条件值为零时设置 PC。
class ScrIfFalseInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrIfFalseInstructionHandler(const ScrVmValueTable& values,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    const ScrVmValueTable& values_;
    IScriptControlCommandSource* source_;
};
}
