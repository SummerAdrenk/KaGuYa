#include <Runtime/DrawOpacityView.h>
#include <Runtime/OpacityWindowRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
OpacityWindowRuntime& Opacity(void* body)
{
    // 保留受控slot6的类型拒绝；构造借用view不提前查询，也不强转成Native。
    auto* const opacity = dynamic_cast<OpacityWindowRuntime*>(static_cast<FrameLayerItem*>(body));
    if (!opacity) throw std::logic_error("controlled Draw slot6 is not COpacityWin");
    return *opacity;
}
constexpr DrawOpacityViewMethods methods{
    [](void* body, IOpacityWindowInputSource*, const DrawOpacityControlledServices& services) {
        auto& opacity = Opacity(body);
        if (!services.mouse || !services.keyboard || !services.update)
            throw std::logic_error("controlled Draw Opacity mouse/keyboard/update services were not bound");
        // IDA 4BE530：不复制状态、不重做输入门控，不在失败后改走Native。
        return opacity.TickAt4BE530(services.mouse, services.keyboard, *services.update);
    },
    [](void* body) { Opacity(body).ResetAtVslot18(); },
    [](void* body) { return Opacity(body).EnabledByteAt0C(); }};
}

DrawOpacityView::DrawOpacityView(FrameLayerItem* body, DrawOpacityControlledServices services) noexcept
    : body_(body), methods_(&methods), controlled_(services) {}
DrawOpacityView::DrawOpacityView(OpacityWindowRuntime& body, DrawOpacityControlledServices services) noexcept
    : DrawOpacityView(static_cast<FrameLayerItem*>(&body), services) {}
DrawOpacityView::DrawOpacityView(OpacityWindowRuntime& body, Platform::MouseView mouse,
    Platform::KeyboardView keyboard, IOpacityDialogUpdateServices& update) noexcept
    : DrawOpacityView(body, {mouse, keyboard, &update}) {}
}
