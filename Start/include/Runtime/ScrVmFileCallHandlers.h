#pragma once

#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmCallHandlers.h>
#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>
#include <Runtime/ScriptFileCommandsNative.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class ScrVmFileCallStackError : std::uint8_t
{
    Empty,
};

class ScrVmFileCallStackException final : public std::runtime_error
{
public:
    ScrVmFileCallStackException(ScrVmFileCallStackError error, const char* message);
    [[nodiscard]] ScrVmFileCallStackError Error() const noexcept;

private:
    ScrVmFileCallStackError error_;
};

// 对应 CParams+0x18/+0x1C/+0x20 的 12 字节引用元素栈；业务帧保存脚本 ID 与返回 PC。
class ScrVmFileCallStack final
{
public:
    static constexpr std::size_t ElementSize = 12u;
    static constexpr std::size_t MaxElementCount = 0x15555555u;

    // 默认构造保留独立栈；显式绑定只持有 slot，不缓存其当前 generation。
    ScrVmFileCallStack() = default;
    explicit ScrVmFileCallStack(ScrVmParamsGenerationBinding generationSlot);
    // handler借用本对象地址；绑定不能因移动静默退回独立栈。
    ScrVmFileCallStack(const ScrVmFileCallStack&) = delete;
    ScrVmFileCallStack& operator=(const ScrVmFileCallStack&) = delete;
    ScrVmFileCallStack(ScrVmFileCallStack&&) = delete;
    ScrVmFileCallStack& operator=(ScrVmFileCallStack&&) = delete;
    [[nodiscard]] bool UsesParamsGenerationSlot(
        const ScrVmParamsGenerationBinding& generationSlot) const noexcept;

    void Push(ScrVmFileCallFrame frame);
    [[nodiscard]] ScrVmFileCallFrame Pop();
    void Clear();
    [[nodiscard]] std::size_t Size() const;

private:
    [[nodiscard]] ScriptFileCallStorage& Frames();
    [[nodiscard]] const ScriptFileCallStorage& Frames() const;

    // root+1C0借用不得跨payload生存期；稳定view每次取Current，不缓存正文或伪造shared所有权。
    ScrVmParamsGenerationBinding generationSlot_;
    // 仅默认独立入口使用；绑定入口的实际 vector 位于当前 CParams+18。
    ScriptFileCallStorage frames_;
};

class IScrVmScriptSwitchBoundary
{
public:
    virtual ~IScrVmScriptSwitchBoundary() = default;
    [[nodiscard]] virtual ScrScriptSwitchResult Switch(
        ScrScriptRuntimeState& runtime,
        std::int32_t scriptId) = 0;
};

// 把 VM handler 接到 0x4D3C90 对应的正式资源读取链。
class ScrVmRuntimeScriptSwitchBoundary final : public IScrVmScriptSwitchBoundary
{
public:
    ScrVmRuntimeScriptSwitchBoundary(
        const IScrResourceNameSource& resourceNames,
        const ResourceCatalog& catalog,
        const FileTransactionContext* transactionContext,
        IStartFileApi& api,
        const IEnvironmentKeyTableSource& environmentKeyBytes,
        ArchiveCodecServices codecs) noexcept;

    [[nodiscard]] ScrScriptSwitchResult Switch(
        ScrScriptRuntimeState& runtime,
        std::int32_t scriptId) override;

private:
    const IScrResourceNameSource& resourceNames_;
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transactionContext_;
    IStartFileApi& api_;
    const IEnvironmentKeyTableSource& environmentKeyBytes_;
    ArchiveCodecServices codecs_;
};

// opcode 16 / 4CD240，直接从当前Params和parser执行，不预读快照操作数。
class ScrFileCallInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrFileCallInstructionHandler(IScriptFileCommandSource& source,
        IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime, const ScrInstructionView& instruction) override;
private:
    IScriptFileCommandSource& source_;
    IScrVmScriptSwitchBoundary& scriptSwitch_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};

// opcode 17 / 4D08A0，直接从当前Params和parser执行，不预读快照操作数。
class ScrFileReturnInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrFileReturnInstructionHandler(IScriptFileCommandSource& source,
        IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime, const ScrInstructionView& instruction) override;
private:
    IScriptFileCommandSource& source_;
    IScrVmScriptSwitchBoundary& scriptSwitch_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};

// opcode 12 / 4CDD90，直接从当前Params和parser执行，不预读快照操作数。
class ScrFileJumpInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrFileJumpInstructionHandler(IScriptFileCommandSource& source,
        IScrVmScriptSwitchBoundary& scriptSwitch, IScrVmCommandErrorSink& errors,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime, const ScrInstructionView& instruction) override;
private:
    IScriptFileCommandSource& source_;
    IScrVmScriptSwitchBoundary& scriptSwitch_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};

}
