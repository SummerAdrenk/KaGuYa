#include <Runtime/SaveLoadCaptureCoordinatorRuntime.h>
#include <Runtime/RootSavePreviewRuntime.h>
#include <Runtime/ToolWindowDialogRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SaveLoadCaptureCoordinatorRuntime::SaveLoadCaptureCoordinatorRuntime(
    DrawManagerLayerRuntime& layers, RootSavePreviewRuntime& preview) noexcept
    : layers_(layers), preview_(preview) {}

void SaveLoadCaptureCoordinatorRuntime::CaptureBeforeSaveAt4B01A0()
{
    {
        auto* tool = dynamic_cast<ToolWindowLayerRuntime*>(layers_.LayerAt418C10(2));
        if (!tool) throw std::logic_error("4B01A0 requires actual ToolWindow owning slot2");
        auto selected = tool->SelectedDialogAt417B50();
        auto* dialog = dynamic_cast<ToolWindowDialogRuntime*>(selected.get());
        if (!dialog) throw std::logic_error("4B01A0 requires actual selected ToolWindowDialog");
        dialog->ResetChildrenForCaptureAt5325A0();
    } // 4B026E..4B02A8：局部SelectedDialog强Ref先释放，再进入根截图。
    preview_.CaptureForSaveAt48DBE0();
}
}
