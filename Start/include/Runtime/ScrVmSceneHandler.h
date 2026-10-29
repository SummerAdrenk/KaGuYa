#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ScrVmCore.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class IScriptControlCommandSource;

class IScrVmSceneReplaySource
{
public:
    virtual ~IScrVmSceneReplaySource() = default;
    [[nodiscard]] virtual std::int32_t ReadReplayValue() = 0;
};

class IScrVmSceneSource
{
public:
    virtual ~IScrVmSceneSource() = default;
    [[nodiscard]] virtual std::size_t CurrentSceneLabelCount() = 0;
    // 4D0ECD 借用 Name 裸指针，可为 null；不复制字符串，也不保活所属标签。
    [[nodiscard]] virtual const char* CurrentSceneLabelName(std::size_t index) = 0;
    [[nodiscard]] virtual const ParamsGameSystem& CurrentGameSystem() = 0;
    // 4179E6 可返回 null；4D6068 仅保存 receiver，首匹配 4D6268 才解引用。
    [[nodiscard]] virtual SecretScArchive* CurrentSecretSc() = 0;
};

class IScrVmSceneCommandErrorSink
{
public:
    virtual ~IScrVmSceneCommandErrorSink() = default;
    virtual void ReportSceneLabelIndexOutOfRange(std::uint16_t index) = 0;
};

// 原生命令与离线解码共用业务段，不在此重复读取replay或命令正文。
[[nodiscard]] std::int32_t ApplyScriptSceneLabelAt4D0E39(std::uint16_t index,
    IScrVmSceneSource& source, IScrVmSceneCommandErrorSink& errorSink);

// 对应 opcode 22 / 0x4D0CA0。
class ScrSceneInstructionHandler final : public IScrVmInstructionHandler
{
public:
    ScrSceneInstructionHandler(
        IScrVmSceneReplaySource& replay,
        IScrVmSceneSource& source,
        IScrVmSceneCommandErrorSink& errorSink,
        IScriptControlCommandSource* pcSource = nullptr);

    [[nodiscard]] std::int32_t ExecuteRuntimeEntry(
        ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory) override;

    [[nodiscard]] std::int32_t Execute(
        ScrScriptRuntimeState& runtime,
        const ScrInstructionView& instruction) override;

private:
    IScrVmSceneReplaySource& replay_;
    IScrVmSceneSource& source_;
    IScrVmSceneCommandErrorSink& errorSink_;
    IScriptControlCommandSource* pcSource_;
};
}
