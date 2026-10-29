#pragma once

#include <Runtime/ScriptInvalidCommandNative.h>
#include <Runtime/ScrVmParamsGeneration.h>

namespace KaGuYa::Start::Runtime
{
class ScrVmValueTable;
class ScrScriptRuntimeState;

struct ScriptArithmeticRootReceiver final
{
    void* object;
    std::uint32_t (*readPcAt47BDB0)(void*);
    ScrVmParamsGeneration& (*paramsAt47BDB0)(void*);
};
class IScriptArithmeticCommandSource
{
public:
    virtual ~IScriptArithmeticCommandSource() = default;
    virtual ScriptArithmeticRootReceiver CaptureAfterEnsureAt413BE0() = 0;
};
class RootScriptArithmeticCommandSource final : public IScriptArithmeticCommandSource
{
public:
    explicit RootScriptArithmeticCommandSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    ScriptArithmeticRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
};
class BoundScriptArithmeticCommandSource final : public IScriptArithmeticCommandSource
{
public:
    BoundScriptArithmeticCommandSource(ScrScriptRuntimeState& runtime, ScrVmValueTable& values) noexcept
        : runtime_(runtime), values_(values) {}
    ScriptArithmeticRootReceiver CaptureAfterEnsureAt413BE0() override;
private:
    ScrScriptRuntimeState& runtime_;
    ScrVmValueTable& values_;
};

// Params getter由调用者先执行。-1走原MessageBox/throw int1，其余越界保留明确安全拒绝。
std::int32_t ReadScriptValueAt4C73A0(const ScrVmParamsGeneration& params, std::int32_t index,
    IScrVmCommandErrorSink& errors);
void WriteScriptValueAt4C8220(ScrVmParamsGeneration& params, std::int32_t index,
    std::int32_t value, IScrVmCommandErrorSink& errors);
void ReportScriptOperationError(IScrVmCommandErrorSink& errors, bool assign);
std::int32_t ExecuteScriptAssignAt4D0F40(ScrContainer& parser, IScriptArithmeticCommandSource& source,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory);
std::int32_t ExecuteScriptCompareAt4CD640(ScrContainer& parser, IScriptArithmeticCommandSource& source,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory);
}
