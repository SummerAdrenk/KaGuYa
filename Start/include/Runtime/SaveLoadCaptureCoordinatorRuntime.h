#pragma once

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class RootSavePreviewRuntime;

// 4B01A0保存前协调；仅借用同一根manager/preview，不复制任何host状态。
class SaveLoadCaptureCoordinatorRuntime final
{
public:
    SaveLoadCaptureCoordinatorRuntime(DrawManagerLayerRuntime& layers,
        RootSavePreviewRuntime& preview) noexcept;
    void CaptureBeforeSaveAt4B01A0();

private:
    DrawManagerLayerRuntime& layers_;
    RootSavePreviewRuntime& preview_;
};
}
