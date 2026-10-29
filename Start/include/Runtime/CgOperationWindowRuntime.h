#pragma once

#include <Runtime/CgDisplaySceneRuntime.h>
#include <Runtime/CgOperationDialogRuntime.h>
#include <Runtime/LayerRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class ICgOperationWindowLayer
{
public:
    virtual ~ICgOperationWindowLayer() = default;

    virtual void SubmitRectAtVslot20(const RECT& rect) = 0;
};

using CgOperationWindowLayerRef =
    std::shared_ptr<ICgOperationWindowLayer>;

class ICgOperationWindowServices
{
public:
    virtual ~ICgOperationWindowServices() = default;

    // 0x4BD590 在 Dialog 之前处理的全局模式命令。返回空值才进入
    // Dialog +0x48/+0x50 分支；返回值已经是原函数的最终命令号。
    [[nodiscard]] virtual std::optional<std::int32_t>
        ResolveGlobalCommandAt4BD590() = 0;
    virtual void UpdateLayerItemAt4AB980() = 0;
    [[nodiscard]] virtual bool PollDialogCompletionAt4C2690(
        CgOperationDialogRuntime& dialog) = 0;
};

// CGOpeWin::CCGOpeWin 的生产状态边界。0x499C10 证明它自身持有
// +0x0C active byte、+0x14 RECT 和 +0x24 的唯一 CGOperationDialog。
class CgOperationWindowRuntime final
    : public LayerItem,
      public ICgOperationWindow
{
public:
    CgOperationWindowRuntime(
        LayerList* registrationLayer,
        ICgOperationWindowServices& services,
        CgOperationWindowLayerRef layer,
        std::shared_ptr<CgOperationDialogRuntime> dialog,
        IGraphicsSurfaceCompositionApi& graphics);

    void CompositeToSurface(GraphicsSurfaceDestination destination,
        const RECT* destinationClip) override;
    void CompositeAt4B5240(const GraphicsSurfaceNativeBody* destination,
        const RECT* destinationClip);

    [[nodiscard]] std::int32_t PollCommandAt4BD590() override;
    void BeginSelectionAt502110() override;
    void AppendAvailabilityAt4FEA00(bool available) override;
    void SubmitWindowVisibilityAt4C22B0(std::uint8_t value) override;
    [[nodiscard]] std::int32_t DialogSelectionValueAt7C() const override;
    [[nodiscard]] bool PollDialogCompletionAt4C2690() override;
    void SetActiveAt0CWithoutLayerSubmission(bool active) noexcept override;
    [[nodiscard]] bool ActiveAt0C() const noexcept override;
    void ReactivateWindowAndLayerAt4B7C24() override;

    [[nodiscard]] const RECT& RectAt14() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationDialogRuntime>&
        DialogAt24() const noexcept;

private:
    void SubmitCurrentRectToLayer();

    ICgOperationWindowServices* services_ = nullptr;
    CgOperationWindowLayerRef layer_;
    std::shared_ptr<CgOperationDialogRuntime> dialog_;
    // 与 CGDisplay 同为宿主服务借用；graphics 必须比 Window 存活更久。
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    RECT rectAt14_{};
};
}
