#pragma once

#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/ParamsDemoLookupRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ILogoDemoSequenceRuntime
{
public:
    virtual ~ILogoDemoSequenceRuntime() = default;

    // CSceneFactory::Create(0x4AAFEF..0x4AB059)：查询“ロゴ”并写入 CDemoSeq+0x98。
    virtual void BindDemoDataAt4C2220(const YtlReferenceSlotRuntime& source) = 0;

    // CDemoSeq::Run(0x4BDAE0)：参数来自 CLogo+0x14，而不是帧参数。
    [[nodiscard]] virtual std::int32_t Run(
        std::uint8_t completionRequested) = 0;

    // 0x4C38A0：状态 1 的过渡推进；返回非零后才离开该状态。
    [[nodiscard]] virtual bool AdvanceTransition() = 0;
    // 4A7690 -> 4C3D50：更新可见精灵的运动/动画，再重绘 dirty rect。
    virtual void UpdateSpritesAndRedraw() = 0;
};

class ILogoSceneRuntimeServices
{
public:
    virtual ~ILogoSceneRuntimeServices() = default;

    // 0x4B5FC0 的三个短路条件由输入与全局状态对象共同计算。
    [[nodiscard]] virtual bool ShouldRequestCompletion() = 0;

    // 构造末尾把全局 12 字节描述项数组的第 3 项目标字段清零。
    virtual void ClearGlobalDescriptorTarget(std::uint32_t descriptorIndex) = 0;

    // 4B5EF0以nullptr调用413270：停止音效但保留环境映射，再替换active场景。
    virtual void ClearRootObjectAt24Binding() = 0;
    virtual void QueueScene(const char* sceneNameCp932) = 0;
};

// DemoScene::CLogo（factory 内联构造区与 0x4B5FC0）的可证明行为边界。
class LogoSceneRuntime final : public ISceneTask
{
public:
    LogoSceneRuntime(
        ILogoDemoSequenceRuntime& sequence,
        ILogoSceneRuntimeServices& services, IDemoParamsSource& params);

    // ITask 包装 0x4C3630 忽略传入参数，并固定执行 Update + 空 Draw。
    void Run(std::int32_t argument) override;

    void SetInputCompletionCheckEnabled(bool enabled) noexcept;

    [[nodiscard]] std::uint32_t State() const noexcept;
    [[nodiscard]] bool CompletionRequested() const noexcept;
    [[nodiscard]] bool InputCompletionCheckEnabled() const noexcept;

private:
    void Update();
    void Finish();

    ILogoDemoSequenceRuntime* sequence_ = nullptr;
    ILogoSceneRuntimeServices* services_ = nullptr;
    std::uint32_t state_ = 0u;
    bool completionRequested_ = false;
    bool inputCompletionCheckEnabled_ = true;
};
}
