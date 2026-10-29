#pragma once
#include <Platform/InputViews.h>

#include <Runtime/ReadScriptUpdateEffectRuntime.h>
#include <Runtime/ReadScriptUpdateRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ReadScriptEffectOwnerNative;
struct ReadScriptEffectFactoryNativeServices;
// 一个 ReadScript 实例拥有一个此对象；main1/main2/main5 共享四 Ref 和同一 scalar state。
// 上层图形/声音/VM/环境依赖均借用，不创建第二份全局 manager 或输入状态。
class ReadScriptTransitionWorkflowRuntime final : public IReadScriptOtherUpdateBoundary
{
public:
    ReadScriptTransitionWorkflowRuntime(IReadScriptEffectFactory& factory,
        ScrVmRuntimeStateBundle& bundle, const ScrVmExecutableProfile& profile,
        const SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
        Platform::MouseView mouse, Platform::KeyboardView keyboard,
        DrawManagerLayerRuntime& manager, IReadScriptSoundWaitSource& sound,
        IGraphicsScreenUpdateApi& screen, std::uint8_t& rootByteAt1B0) noexcept;
    // 原生正文由CReadScript唯一owner持有；不构造现代Effect或备用Surface槽。
    ReadScriptTransitionWorkflowRuntime(ReadScriptEffectOwnerNative& owner,
        IReadScriptUpdateEffectActions& actions, IReadScriptWaitStateSource& state,
        IDrawLayersViewSource& draw, IReadScriptSoundWaitSource& sound,
        IGraphicsScreenUpdateApi& screen, PatternLayerNativeRenderServices& patterns,
        ReadScriptEffectFactoryNativeServices& nativeServices) noexcept;
    ReadScriptTransitionWorkflowRuntime(const ReadScriptTransitionWorkflowRuntime&) = delete;
    ReadScriptTransitionWorkflowRuntime& operator=(const ReadScriptTransitionWorkflowRuntime&) = delete;

    bool RunUpdateEffect(ReadScriptSceneState& state) override;
    bool RunSuspend(ReadScriptSceneState& state) override;
    bool RunWait(ReadScriptSceneState& state) override;
    // 场景退出也必须显式经过此可抛边界；不能依靠 noexcept shared_ptr 析构绘制结束帧。
    void ReleaseResourcesAt4C0990();
    [[nodiscard]] ReadScriptEffectOwnerRuntime& Resources();
    [[nodiscard]] IReadScriptSuspendEffectOwner& EffectOwner() noexcept { return *resourcesView_; }

private:
    std::optional<ReadScriptEffectOwnerRuntime> resources_;
    IReadScriptSuspendEffectOwner* resourcesView_;
    ReadScriptEffectFactoryNativeServices* nativeServices_ = nullptr;
    ReadScriptUpdateEffectRuntime updateEffect_;
    ReadScriptSuspendWaitRuntime suspendWait_;
};
}
