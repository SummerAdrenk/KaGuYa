#include <Runtime/DrawOpacityView.h>
#include <Runtime/OpacityWindowNative.h>
#include <Runtime/DrawGuiNativeScope.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
IOpacityWindowInputSource& BoundInput(IOpacityWindowInputSource* input)
{
    if (!input) throw std::logic_error("native Draw Opacity input source was not bound");
    return *input;
}
constexpr DrawOpacityViewMethods methods{
    [](void* body, IOpacityWindowInputSource* input, const DrawOpacityControlledServices&) {
        // IDA 4BE530/4ABA60：输入末尾、Dialog重读及dirty异常前缀全部归既有原生叶。
        return UpdateOpacityWindowAt4BE530(*static_cast<OpacityWindowNativeBody30h*>(body), BoundInput(input));
    },
    [](void* body) {
        auto* const layer = &static_cast<OpacityWindowNativeBody30h*>(body)->window.layer;
        // IDA 4B1ACC/4B372C：晚读正文原虚表，不使用OpacityWindowNativeMethods固定表。
        const auto* const table = reinterpret_cast<const LayerItemNativeFrameVtable*>(layer->vptrAt00);
        table->resetAt18(layer);
    },
    [](void* body) { return static_cast<const volatile OpacityWindowNativeBody30h*>(body)->window.layer.enabledAt0C; }};
}

DrawOpacityView::DrawOpacityView(OpacityWindowNativeBody30h* body, IOpacityWindowInputSource* input) noexcept
    : body_(body), methods_(&methods), native_(input) {}
DrawOpacityView::DrawOpacityView(OpacityWindowNativeBody30h& body, IOpacityWindowInputSource& input) noexcept
    : DrawOpacityView(&body, &input) {}

bool DrawOpacityView::TickAt4BE530() const
{
    const DrawGuiNativeScope scope(gui_);
    return Methods().tick(body_, native_, controlled_);
}
void DrawOpacityView::ResetAtVslot18() const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().reset(body_);
}

const DrawOpacityViewMethods& DrawOpacityView::Methods() const
{
    if (!methods_) throw std::logic_error("Draw Opacity view methods were not bound");
    return *methods_;
}
}
