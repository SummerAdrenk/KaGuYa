#include <Runtime/ReadScriptUpdateRuntime.h>

#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/ReadScriptMenuInteractionRuntime.h>
#include <Runtime/ScrVmCore.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptUpdateRuntime::ReadScriptUpdateRuntime(const ScrVmCore& vm,
    ScrScriptRuntimeState& script, IScrVmYieldBoundary& yield,
    ReadScriptTextInteractionRuntime& text, ReadScriptMenuInteractionRuntime& menu,
    IReadScriptOtherUpdateBoundary& other) noexcept
    : vm_(vm), script_(script), yield_(yield), text_(text), menu_(menu), other_(other)
{
}

bool ReadScriptUpdateRuntime::RunVmYield(ReadScriptSceneState& state)
{
    // 每次以正在 Update 的 receiver 建立借用；generation 或脚本切换后不保留旧 PC。
    ScrVmYieldDispatcher dispatcher(state, yield_);
    return dispatcher.DispatchOneStep([this] { return vm_.ExecuteUntilYield(script_); });
}

bool ReadScriptUpdateRuntime::RunTextInteraction(ReadScriptSceneState& state)
{
    return text_.RunAt4B2730(state);
}

bool ReadScriptUpdateRuntime::RunUpdateEffect(ReadScriptSceneState& state)
{
    return other_.RunUpdateEffect(state);
}

bool ReadScriptUpdateRuntime::RunSuspend(ReadScriptSceneState& state)
{
    return other_.RunSuspend(state);
}

bool ReadScriptUpdateRuntime::RunMenuInteraction(ReadScriptSceneState& state)
{
    return menu_.RunAt4B11B0(state);
}

bool ReadScriptUpdateRuntime::RunWait(ReadScriptSceneState& state)
{
    return other_.RunWait(state);
}
}
