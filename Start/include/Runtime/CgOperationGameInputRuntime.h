#pragma once

#include <Runtime/CgOperationControlsRuntime.h>

namespace KaGuYa::Start::Runtime
{
class GameGuiInputStateRuntime;

// 根 Dialog 的 +70..+84 转发到真实 GameGuiInputState 的 +04..+18。
// 借用对象必须比本适配器长寿；逐次读取输入，不复制帧状态，也不持有 active。
class CgOperationGameInputRuntime final : public ICgOperationSliderInputSource
{
public:
    CgOperationGameInputRuntime(
        const GameGuiInputStateRuntime& input,
        IGraphicsSurfacePixelApi& pixels) noexcept;

    [[nodiscard]] bool ReadInputAtVslot70(std::int32_t eventValue) override;
    [[nodiscard]] POINT ReadPointerAtVslot74() override;
    [[nodiscard]] std::int32_t ReadStepDirectionAtVslot78() override;
    [[nodiscard]] bool ReadInputAtVslot7C() override;
    [[nodiscard]] bool ReadPressAtVslot80() override;
    [[nodiscard]] bool ReadReleaseAtVslot84() override;
    [[nodiscard]] std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) override;

private:
    const GameGuiInputStateRuntime& input_;
    IGraphicsSurfacePixelApi& pixels_;
};
}
