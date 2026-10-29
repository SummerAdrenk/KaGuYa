#pragma once

#include <Runtime/ScrVmCore.h>
#include <Runtime/ScriptTitleCommandNative.h>

namespace KaGuYa::Start::Runtime
{
// 对应 opcode 21 / 0x4D25A0：覆盖当前 Environment 视图的标题字符串。
class ScrTitleInstructionHandler final : public IScrVmInstructionHandler
{
public:
    explicit ScrTitleInstructionHandler(IScriptTitleCommandSource* source = nullptr) noexcept : source_(source) {}
    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;
    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;
private:
    IScriptTitleCommandSource* source_;
};
}
