#pragma once

#include <Runtime/ReadScriptEffectRuntime.h>
#include <Runtime/ReadScriptEffectSurfaceRuntime.h>

#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ReadScriptFlashEffect final : public IReadScriptEffectRuntime
{
public:
    ReadScriptFlashEffect(IReadScriptEffectSurfaceServices& resources,
        IGraphicsWindowSurfaceApi& drawing, IGraphicsScreenUpdateApi& screen,
        const std::string& nameCp932, std::int32_t count);
    bool TickAtVslot00() override;
    void DrawAtVslot04() override;
    std::uint8_t ReadyAtVslot08() const override;
    void Finish() noexcept override;

private:
    IGraphicsWindowSurfaceApi& drawing_;
    IGraphicsScreenUpdateApi& screen_;
    std::uint8_t ready_ = 0;
    // 4A4080：间隔数组、旧画面、闪光资源依次释放。
    std::shared_ptr<RuntimeGraphicsSurface> flash_;
    std::shared_ptr<RuntimeGraphicsSurface> previous_;
    std::vector<std::uint8_t> intervals_;
    std::optional<std::int32_t> remaining_;
    std::optional<std::int32_t> index_;
};

class ReadScriptShakeEffect final : public IReadScriptEffectRuntime
{
public:
    ReadScriptShakeEffect(IReadScriptEffectSurfaceServices& resources,
        IGraphicsWindowSurfaceApi& drawing, IGraphicsScreenUpdateApi& screen,
        std::int32_t mode, std::int32_t scale, std::int32_t interval);
    bool TickAtVslot00() override;
    void DrawAtVslot04() override;
    std::uint8_t ReadyAtVslot08() const override;
    void Finish() noexcept override;

private:
    IGraphicsWindowSurfaceApi& drawing_;
    IGraphicsScreenUpdateApi& screen_;
    std::uint8_t ready_ = 0;
    std::shared_ptr<RuntimeGraphicsSurface> previous_;
    std::int32_t interval_;
    std::int32_t mode_;
    std::int32_t counter_ = 0;
    std::int32_t index_ = 0;
    std::int32_t scale_;
};
}
