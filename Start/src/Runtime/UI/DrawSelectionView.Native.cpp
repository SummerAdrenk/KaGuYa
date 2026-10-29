#include <Runtime/DrawSelectionView.h>
#include <Runtime/SelectWindowNativeWorkflow.h>
#include <Runtime/DrawGuiNativeScope.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
SelectWindowNativeWorkflowServices& BoundServices(SelectWindowNativeWorkflowServices* services)
{
    if (!services) throw std::logic_error("native Draw Selection workflow services were not bound");
    return *services;
}

// IDA 4369E0/4BBFD0：Ref释放、逐次根查询、dirty和catch均委托已验原生两叶。
constexpr DrawSelectionViewMethods methods{
    [](void* body, SelectWindowNativeWorkflowServices* services, const DrawSelectionControlledServices&) {
        RebuildSelectWindowNativeAt4369E0(*static_cast<SelectWindowNativeBody6Ch*>(body), BoundServices(services));
    },
    [](void* body, std::uint8_t blocked, SelectWindowNativeWorkflowServices* services,
        const DrawSelectionControlledServices&) {
        return TickSelectWindowNativeAt4BBFD0(*static_cast<SelectWindowNativeBody6Ch*>(body), blocked, BoundServices(services));
    },
    [](void* body) { return static_cast<const volatile SelectWindowNativeBody6Ch*>(body)->window.layer.enabledAt0C; }};
}

DrawSelectionView::DrawSelectionView(SelectWindowNativeBody6Ch* body,
    SelectWindowNativeWorkflowServices* services) noexcept
    : body_(body), methods_(&methods), native_(services) {}
DrawSelectionView::DrawSelectionView(SelectWindowNativeBody6Ch& body,
    SelectWindowNativeWorkflowServices& services) noexcept : DrawSelectionView(&body, &services) {}

void DrawSelectionView::RebuildAt4369E0() const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().rebuild(body_, native_, controlled_);
}
std::int32_t DrawSelectionView::TickAt4BBFD0(std::uint8_t blocked) const
{
    const DrawGuiNativeScope scope(gui_);
    return Methods().tick(body_, blocked, native_, controlled_);
}

const DrawSelectionViewMethods& DrawSelectionView::Methods() const
{
    if (!methods_) throw std::logic_error("Draw Selection view methods were not bound");
    return *methods_;
}
}
