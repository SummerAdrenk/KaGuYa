#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/CgUiButtonRuntime.h>
#include <Runtime/SceneManagerPublicDialogRuntime.h>

#include <array>
#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
// 样本 18 CModeSelectButton(0x45AC40) 的四按钮生产对象。
// 固定表来自原 Start.exe 的 0x601658，共 3 个模式、每模式 4 条 40-byte 记录。
class SceneManagerModeSelectButtonRuntime final
    : public ISceneManagerModeSelectButton
{
public:
    SceneManagerModeSelectButtonRuntime(
        std::int32_t mode,
        ICgUiButtonRuntimeServices& buttonServices,
        IGraphicsSurfaceCompositionApi& compositionApi,
        ICgOperationSurfaceFactory& surfaceFactory);

    void ResetButtonsAt487470() override;
    [[nodiscard]] std::vector<std::shared_ptr<CgUiButtonRuntime>>
        AcquireButtonsAt47B7B0() override;

    [[nodiscard]] const std::array<std::shared_ptr<CgUiButtonRuntime>, 4>&
        ButtonsAt47B7B0() const noexcept;

private:
    std::array<std::shared_ptr<CgUiButtonRuntime>, 4> buttons_;
};
}
