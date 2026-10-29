#include <Runtime/CgOperationWindowRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
CgOperationWindowRuntime::CgOperationWindowRuntime(
    LayerList* const registrationLayer,
    ICgOperationWindowServices& services,
    CgOperationWindowLayerRef layer,
    std::shared_ptr<CgOperationDialogRuntime> dialog,
    IGraphicsSurfaceCompositionApi& graphics)
    : LayerItem(registrationLayer),
      services_(&services),
      layer_(std::move(layer)),
      dialog_(std::move(dialog)),
      graphics_(&graphics)
{
    if (!layer_)
        throw std::invalid_argument("CGOpeWin parent layer is null");
    if (!dialog_)
        throw std::invalid_argument("CGOpeWin Dialog at +0x24 is null");

    // 0x499D98..0x499DCE：构造末尾从唯一 Dialog+0x68 复制矩形。
    rectAt14_ = dialog_->DialogRectAt68();
}

void CgOperationWindowRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip)
{
    if (!destination.Body())
        throw std::invalid_argument("CGOpeWin composite requires a native destination body");
    CompositeAt4B5240(destination.Body(), destinationClip);
}

void CgOperationWindowRuntime::CompositeAt4B5240(
    const GraphicsSurfaceNativeBody* const destination, const RECT* const destinationClip)
{
    // 4B5243..4B527B：分配前捕获 Dialog 正文，不增持；无 Window enabled 门控。
    auto* const dialog = dialog_.get();
    YtlReferenceSlotRuntime argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument,
        const_cast<GraphicsSurfaceNativeBody*>(destination), 0);
    // 451500 失败时参数尚未构造完成；成功后由被调用者消费，不在此重复析构。
    dialog->CompositeAt523480(*graphics_, argument, destinationClip);
}

std::int32_t CgOperationWindowRuntime::PollCommandAt4BD590()
{
    std::int32_t result = -1;
    if (const auto globalResult = services_->ResolveGlobalCommandAt4BD590())
    {
        result = *globalResult;
    }
    else
    {
        (void)dialog_->UpdateAtVslot48();
        const std::int32_t dialogResult = dialog_->ResultAtVslot50();
        if (dialogResult != -1)
        {
            switch (dialogResult)
            {
                case 0:
                    result = 4;
                    break;
                case 1:
                    result = 3;
                    break;
                case 2:
                    result = 1;
                    break;
                case 3:
                    result = 5;
                    break;
                default:
                    result = 6;
                    break;
            }
        }
    }

    // 0x4BD833 的公共 LayerItem 更新对全局命令和 Dialog 命令都执行。
    services_->UpdateLayerItemAt4AB980();
    return result;
}

void CgOperationWindowRuntime::BeginSelectionAt502110()
{
    dialog_->BeginSelectionAt502110();
}

void CgOperationWindowRuntime::AppendAvailabilityAt4FEA00(
    const bool available)
{
    dialog_->AppendAvailabilityAt4FEA00(available);
}

void CgOperationWindowRuntime::SubmitWindowVisibilityAt4C22B0(
    const std::uint8_t value)
{
    // 原函数先写 +0x0C；Layer 提交抛出时该写入不回滚。
    SetEnabledByteAt0C(value);
    SubmitCurrentRectToLayer();
}

std::int32_t CgOperationWindowRuntime::DialogSelectionValueAt7C() const
{
    return dialog_->SelectionAt7C();
}

bool CgOperationWindowRuntime::PollDialogCompletionAt4C2690()
{
    return services_->PollDialogCompletionAt4C2690(*dialog_);
}

void CgOperationWindowRuntime::ReactivateWindowAndLayerAt4B7C24()
{
    SetEnabled(true);
    SubmitCurrentRectToLayer();
}

void CgOperationWindowRuntime::SetActiveAt0CWithoutLayerSubmission(
    const bool active) noexcept
{
    SetEnabled(active);
}

bool CgOperationWindowRuntime::ActiveAt0C() const noexcept
{
    return Enabled();
}

const RECT& CgOperationWindowRuntime::RectAt14() const noexcept
{
    return rectAt14_;
}

const std::shared_ptr<CgOperationDialogRuntime>&
CgOperationWindowRuntime::DialogAt24() const noexcept
{
    return dialog_;
}

void CgOperationWindowRuntime::SubmitCurrentRectToLayer()
{
    layer_->SubmitRectAtVslot20(rectAt14_);
}
}
