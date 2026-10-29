#include <Runtime/DrawToolView.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/DrawGuiNativeScope.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ToolWindowNativeBody40h;
using Services = ToolWindowNativeInteractionServices;

Services& BoundServices(void* services)
{
    if (!services) throw std::logic_error("native Draw Tool interaction services were not bound");
    return *static_cast<Services*>(services);
}

// IDA 434FB0/4BE200/4C01F0/4BF4E0/4C2300/415910/4AB910：
// scope、逐次417B50、Ref退役与失败前缀全部归已验native函数，不在view里重写。
constexpr DrawToolViewMethods methods{
    [](void* body, void* services, std::int32_t mode) {
        SetToolModeNativeAt434FB0(*static_cast<Body*>(body), mode, BoundServices(services));
    },
    [](void* body, void* services) {
        return TickToolWindowNativeAt4BE200(*static_cast<Body*>(body), BoundServices(services));
    },
    [](void* body, void* services) {
        CaptureToolWindowNativeAt4C01F0(*static_cast<Body*>(body), BoundServices(services));
    },
    [](void* body, void* services) {
        RestoreToolWindowNativeAt4BF4E0(*static_cast<Body*>(body), BoundServices(services));
    },
    [](void* body, void* services, std::uint8_t value) {
        SetToolCaptureModeNativeAt4C2300(*static_cast<Body*>(body), value, BoundServices(services));
    },
    [](void* body, void* services) {
        PropagateToolDirtyNativeAt415910(*static_cast<Body*>(body), BoundServices(services));
    },
    [](void* body, void* services, std::uint8_t value) {
        ResetToolSkipLatchNativeAt4AB910(*static_cast<Body*>(body), value, BoundServices(services));
    },
    [](void* body) { return static_cast<const volatile Body*>(body)->valueAt34; },
    [](void* body, std::uint8_t value) { static_cast<Body*>(body)->valueAt38 = value; },
    [](void* body) { return static_cast<const volatile Body*>(body)->window.layer.enabledAt0C; }};
}

DrawToolView::DrawToolView(Body* body, Services* services) noexcept
    : body_(body), methods_(&methods), services_(services) {}
DrawToolView::DrawToolView(Body& body, Services& services) noexcept : DrawToolView(&body, &services) {}

void DrawToolView::SetModeAt434FB0(std::int32_t mode) const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().setMode(body_, services_, mode);
}
std::uint32_t DrawToolView::TickAt4BE200() const
{
    const DrawGuiNativeScope scope(gui_);
    return Methods().tick(body_, services_);
}
void DrawToolView::CaptureAt4C01F0() const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().capture(body_, services_);
}
void DrawToolView::RestoreAt4BF4E0() const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().restore(body_, services_);
}
void DrawToolView::SetCaptureModeAt4C2300(std::uint8_t value) const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().setCaptureMode(body_, services_, value);
}
void DrawToolView::PropagateDirtyRectsAt415910() const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().propagateDirty(body_, services_);
}
void DrawToolView::ResetSkipLatchAt4AB910(std::uint8_t value) const
{
    const DrawGuiNativeScope scope(gui_);
    Methods().resetSkipLatch(body_, services_, value);
}

const DrawToolViewMethods& DrawToolView::Methods() const
{
    // 默认view没有分派协议，属于接口未绑定；显式native null仍交原叶/原字段。
    if (!methods_) throw std::logic_error("Draw Tool view methods were not bound");
    return *methods_;
}
}
