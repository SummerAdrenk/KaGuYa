#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>
#include <Runtime/ScriptControlCommandNative.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class ScrVmCallStackError : std::uint8_t
{
    Empty,
    TooLong,
};

class ScrVmCallStackException final : public std::runtime_error
{
public:
    ScrVmCallStackException(ScrVmCallStackError error, const char* message);
    [[nodiscard]] ScrVmCallStackError Error() const noexcept;

private:
    ScrVmCallStackError error_;
};

// 对应 CParams +0x24/+0x28/+0x2C 的 int32 动态数组。
class ScrVmCallStack final
{
public:
    static constexpr std::size_t MaxElementCount = 0x3FFFFFFFu;

    // 默认构造保留独立栈；显式绑定只持有 slot，不缓存其当前 generation。
    ScrVmCallStack() = default;
    explicit ScrVmCallStack(ScrVmParamsGenerationBinding generationSlot);
    // handler借用本对象地址；禁止移出后丢失绑定或把复制误当作栈快照。
    ScrVmCallStack(const ScrVmCallStack&) = delete;
    ScrVmCallStack& operator=(const ScrVmCallStack&) = delete;
    ScrVmCallStack(ScrVmCallStack&&) = delete;
    ScrVmCallStack& operator=(ScrVmCallStack&&) = delete;
    [[nodiscard]] bool UsesParamsGenerationSlot(
        const ScrVmParamsGenerationBinding& generationSlot) const noexcept;

    void Push(std::uint32_t programCounter);
    [[nodiscard]] std::uint32_t Pop();
    void Clear();
    [[nodiscard]] std::size_t Size() const;

private:
    [[nodiscard]] ScriptLocalCallStorage& Entries();
    [[nodiscard]] const ScriptLocalCallStorage& Entries() const;

    // root+1C0借用不得跨payload生存期；稳定view每次取Current，不缓存正文或伪造shared所有权。
    ScrVmParamsGenerationBinding generationSlot_;
    // 仅默认独立入口使用；绑定入口的实际 vector 位于当前 CParams+24。
    ScriptLocalCallStorage entries_;
};

// 对应 opcode 18 / 0x4CEC40。
class ScrCallInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrCallInstructionHandler(ScrVmCallStack& callStack,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmCallStack& callStack_;
    IScriptControlCommandSource* source_;
};

// 对应 opcode 19 / 0x4D0A90。
class ScrReturnInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrReturnInstructionHandler(ScrVmCallStack& callStack,
        IScriptControlCommandSource* source = nullptr) noexcept;
    std::int32_t ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmCallStack& callStack_;
    IScriptControlCommandSource* source_;
};
}
