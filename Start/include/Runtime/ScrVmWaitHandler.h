#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScrVmScriptStorage.h>

#include <cstdint>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class IScriptControlCommandSource;
// 对应 opcode 5 / 0x4D11D0，完成状态写入后固定返回外层状态码 7。
class ScrWaitInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrWaitInstructionHandler(
        ScrVmValueTable& values,
        ScrVmScriptStorage& storage, IScriptControlCommandSource* source = nullptr) noexcept;

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    ScrVmValueTable& values_;
    ScrVmScriptStorage& storage_;
    IScriptControlCommandSource* source_;
};
}
