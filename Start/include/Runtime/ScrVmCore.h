#pragma once

#include <Runtime/ScrInstructionDecoder.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScriptDispatchNative.h>
#include <Runtime/ScriptInvalidCommandNative.h>

#include <array>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IScrVmInstructionHandler
{
public:
    virtual ~IScrVmInstructionHandler() = default;

    // 未迁移的业务入口保留显式Decode适配；已核入口覆盖此方法，不能在Core统一预读。
    [[nodiscard]] virtual std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory);

    [[nodiscard]] virtual std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) = 0;
};

// 对应 handler 0 / 0x4CDC00：报告失败并返回外层状态码 2。
class ScrInvalidOpcodeHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrInvalidOpcodeHandler(IScrVmCommandErrorSink& errorSink,
        IScriptExecutionRootSource* root = nullptr,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    IScrVmCommandErrorSink& errorSink_;
    IScriptExecutionRootSource* root_;
    IRuntimeStringManagerSource& strings_;
};

// 对应 opcode 28 / 0x4CE150。
class ScrNopInstructionHandler final : public IScrVmInstructionHandler
{
public:
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;
};

class ScrVmNativeEntryContextRuntime;
class ScrVmCore final
{
public:
    static constexpr std::size_t HandlerCount = 29u;
    using HandlerTable = std::array<IScrVmInstructionHandler*, HandlerCount>;

    // 现代服务适配表不进入CScript正文。以下无body入口仅供独立工具/夹具派发。
    // 无root参数的入口显式使用runtime绑定槽，供独立执行/测试；生产装配应传真实root source。
    explicit ScrVmCore(HandlerTable handlers,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    ScrVmCore(HandlerTable handlers, IScriptExecutionRootSource& root,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    // 生产入口晚读同一正文的+2C函数表；所有handler视图由composition绑定到该body。
    ScrVmCore(HandlerTable handlers, ScriptNativeBody& body, ScriptNativePublication& publication,
        IScriptExecutionRootSource* root = nullptr,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

    // 对应 0x4D3FF0：handler 返回 0 时继续，非零时提交 staged PC 并原样返回。
    [[nodiscard]] std::int32_t ExecuteUntilYield(ScrScriptRuntimeState& runtime) const;
    [[nodiscard]] ScrVmNativeEntryContextRuntime BorrowNativeEntryContext(ScrScriptRuntimeState& runtime) const;

private:
    friend class ScrVmNativeEntryContextRuntime;
    void RequireNativeRuntime(const ScrScriptRuntimeState&) const;
    HandlerTable handlers_{};
    IScriptExecutionRootSource* root_ = nullptr;
    IEnvironmentRootMemory& memory_;
    ScriptNativeBody* nativeBody_ = nullptr;
    ScriptNativePublication* nativePublication_ = nullptr;
};
// 只借core及同一CCh视图；不复制handler表、Script/Params状态或新增拥有控制块。
class ScrVmNativeEntryContextRuntime final:public IScriptNativeEntryContext
{
public:
    std::int32_t Invoke(ScriptNativeBody&,unsigned) override;
private:
    friend class ScrVmCore;
    ScrVmNativeEntryContextRuntime(const ScrVmCore& core,ScrScriptRuntimeState& runtime) noexcept
        :core_(core),runtime_(runtime){}
    const ScrVmCore& core_;
    ScrScriptRuntimeState& runtime_;
};
}
