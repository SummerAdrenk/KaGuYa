#pragma once
#include <Runtime/CgDisplayButtonGroupFactoryRuntime.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
// 默认根来源每个原getter独立Ensure/借用；显式固定receiver入口不创建另一份状态。
class SceneManagerPublicControlsSourceRuntime final:public ICgDisplayPublicControlsSourceAt499DC0
{
public:
    explicit SceneManagerPublicControlsSourceRuntime(RootRuntimeOwner& root) noexcept:root_(&root){}
    explicit SceneManagerPublicControlsSourceRuntime(SceneManagerView receiver) noexcept:receiver_(receiver){}
    CgDisplayPublicSelectorRuntime CaptureSelectorAt478A30() override;
    CgDisplayPublicModeButtonsRuntime CaptureModesAt47B7B0() override;
private:
    SceneManagerView Capture();
    RootRuntimeOwner* root_=nullptr;
    SceneManagerView receiver_{static_cast<SceneManagerRuntime*>(nullptr)};
};
}
