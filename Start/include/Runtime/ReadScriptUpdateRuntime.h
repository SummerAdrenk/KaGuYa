#pragma once

#include <Runtime/ReadScriptSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ScrVmCore;
class ScrScriptRuntimeState;
class ReadScriptTextInteractionRuntime;
class ReadScriptMenuInteractionRuntime;

// 其余三个真实处理器由同一 owner 装配；不能把未接通的场景分支当作 false 等待。
class IReadScriptOtherUpdateBoundary
{
public:
    virtual ~IReadScriptOtherUpdateBoundary() = default;
    [[nodiscard]] virtual bool RunUpdateEffect(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunSuspend(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunWait(ReadScriptSceneState& state) = 0;
};

// 4BA530 -> 4BFFD0/4B2730：执行真实 VM，不缓存 yield result，也不复制 scene state。
class ReadScriptUpdateRuntime final : public IReadScriptUpdateBoundary
{
public:
    ReadScriptUpdateRuntime(const ScrVmCore& vm, ScrScriptRuntimeState& script,
        IScrVmYieldBoundary& yield, ReadScriptTextInteractionRuntime& text,
        ReadScriptMenuInteractionRuntime& menu,
        IReadScriptOtherUpdateBoundary& other) noexcept;

    [[nodiscard]] bool RunVmYield(ReadScriptSceneState& state) override;
    [[nodiscard]] bool RunTextInteraction(ReadScriptSceneState& state) override;
    [[nodiscard]] bool RunUpdateEffect(ReadScriptSceneState& state) override;
    [[nodiscard]] bool RunSuspend(ReadScriptSceneState& state) override;
    [[nodiscard]] bool RunMenuInteraction(ReadScriptSceneState& state) override;
    [[nodiscard]] bool RunWait(ReadScriptSceneState& state) override;

private:
    const ScrVmCore& vm_;
    ScrScriptRuntimeState& script_;
    IScrVmYieldBoundary& yield_;
    ReadScriptTextInteractionRuntime& text_;
    ReadScriptMenuInteractionRuntime& menu_;
    IReadScriptOtherUpdateBoundary& other_;
};
}
