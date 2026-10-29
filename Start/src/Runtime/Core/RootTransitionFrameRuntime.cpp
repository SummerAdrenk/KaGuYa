#include <Runtime/RootTransitionFrameRuntime.h>
#include <Runtime/RootRuntimeOwner.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
DrawLayersView RootTransitionFrameSource::CaptureDrawLayersAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    const auto manager = root_.RequirePublished().PostWindow().DrawLayersAt18();
    if (!manager) throw std::logic_error("Quick capture dereferenced a null root DrawManager");
    return manager;
}
YtlReferenceSlotRuntime& RootTransitionFrameSource::CaptureFrameSlotAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    auto& current = root_.RequirePublished();
    current.BindGraphicsSurfaceApi(surfaces_);
    return current.TransitionFrameSlotAt1F0();
}

DrawLayersView RootGuiModalSource::CaptureDrawLayersAt413BE0()
{ return transition_.CaptureDrawLayersAt413BE0(); }

YtlReferenceSlotRuntime& RootGuiModalSource::CaptureFrameSlotAt413BE0()
{ return transition_.CaptureFrameSlotAt413BE0(); }

GraphicsSurfaceReference RootGuiModalSource::CaptureScreenAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    // 50CF8F..50CFB4仍解析+18，但4ACDD0不读取ECX；空manager不得提前拒绝。
    // 当前+18发布指针是已登记的现代边界，不声称模拟了原Ref12的损坏指针故障。
    static_cast<void>(root_.RequirePublished().PostWindow().DrawLayersAt18());
    return capture_.CaptureScreenNativeAt4ACDD0();
}

std::uint32_t& RootGuiModalSource::ModalGateAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().ModalGateAt1B8();
}

void CaptureRootQuickOperationFrame(IRootTransitionFrameSource& source,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes)
{
    auto layers = source.CaptureDrawLayersAt413BE0();
    GraphicsSurfaceReference returned(surfaces);
    CompositeGraphicsFrameAt4A8340(returned.Slot(), surfaces, sizes, layers, false);
    // 第二次Ensure失败仍由caller释放返回临时；进入setter之前才转交按值参数责任。
    auto& current = source.CaptureFrameSlotAt413BE0();
    auto argument = returned.Detach();
    GraphicsSurfaceNativeScope scope(surfaces);
    AssignGraphicsSurfaceReferenceAt4C1C60(current, argument);
}
}
