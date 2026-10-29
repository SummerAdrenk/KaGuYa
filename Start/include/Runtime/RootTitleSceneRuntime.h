#pragma once
#include <Runtime/SceneManagerView.h>

#include <Runtime/TitleSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class RootRuntimeOwner;
class RootRuntimePayload;
class SceneManagerRuntime;

// 4BADA0真实终端：每个原413BE0点取当前root，不缓存SceneManager或输入正文。
class RootTitleSceneRuntime final : public ITitleSceneRuntimeServices, public ITitleSceneState
{
public:
    RootTitleSceneRuntime(EnvironmentStartupRuntime& environment, RootRuntimeOwner& root) noexcept;
    void ClearDescriptorTargetDwordAt10() override;
    std::uint32_t GameInputStateModeAt04() const override;
    RuntimeCStringResult CloneCurrentSceneNameAt414540() const override;
    bool EvaluateInputPredicate47F650() override;
    void ClearCurrentSceneName() override;
    void SetRootByteAt1AF(std::uint8_t value) override;
    void PushActiveScene(const char* name) override;
private:
    RootRuntimePayload& CaptureRoot() const;
    SceneManagerView CaptureScenes() const;
    EnvironmentStartupRuntime& environment_;
    RootRuntimeOwner& root_;
};
}
