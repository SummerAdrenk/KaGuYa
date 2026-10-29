#pragma once
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class ParamsRuntimeRootOwner;
class RootRuntimeOwner;
struct ParamsGameSystem;

class ISavePreviewRuntimeSource
{
public:
    virtual ~ISavePreviewRuntimeSource() = default;
    [[nodiscard]] virtual DrawLayersView CaptureDrawLayersAt413BE0() const = 0;
    [[nodiscard]] virtual const ParamsGameSystem* CaptureGameSystemAt418480() const = 0;
};

// 原调用点每次取当前根；只借正文，不把跨回调的旧管理器或GameSystem保活。
class RootSavePreviewRuntimeSource final : public ISavePreviewRuntimeSource
{
public:
    RootSavePreviewRuntimeSource(RootRuntimeOwner& root, ParamsRuntimeRootOwner& params) noexcept;
    [[nodiscard]] DrawLayersView CaptureDrawLayersAt413BE0() const override;
    [[nodiscard]] const ParamsGameSystem* CaptureGameSystemAt418480() const override;
private:
    RootRuntimeOwner& root_;
    ParamsRuntimeRootOwner& params_;
};

// 仅用于显式固定图的工具和夹具；正式懒根使用上面的生产来源。
class FixedSavePreviewRuntimeSource final : public ISavePreviewRuntimeSource
{
public:
    FixedSavePreviewRuntimeSource(DrawLayersView layers, const ParamsGameSystem& game) noexcept
        : layers_(layers), game_(game) {}
    [[nodiscard]] DrawLayersView CaptureDrawLayersAt413BE0() const override { return layers_; }
    [[nodiscard]] const ParamsGameSystem* CaptureGameSystemAt418480() const override { return &game_; }
private:
    DrawLayersView layers_;
    const ParamsGameSystem& game_;
};
}
