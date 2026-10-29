#include <Runtime/DrawToolView.h>
#include <Runtime/ToolWindowInteractionRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
ToolWindowLayerRuntime& Tool(void* body)
{
    // 与现有slot2消费者相同的明确类型拒绝；构造view不提前触发此查询。
    auto* tool = dynamic_cast<ToolWindowLayerRuntime*>(static_cast<FrameLayerItem*>(body));
    if (!tool) throw std::logic_error("controlled Draw slot2 is not CToolWin");
    return *tool;
}
ToolWindowInteractionRuntime Interaction(void* body, void* services)
{
    auto& tool = Tool(body);
    if (!services) throw std::logic_error("controlled Draw Tool interaction services were not bound");
    // 临时外观只借用既有现代Tool与服务，不另持interaction或Dialog owner。
    return ToolWindowInteractionRuntime(tool, *static_cast<IToolWindowInteractionServices*>(services));
}
constexpr DrawToolViewMethods methods{
    [](void* body, void* services, std::int32_t mode) { Interaction(body, services).SetModeAt434FB0(mode); },
    [](void* body, void* services) { return Interaction(body, services).TickAt4BE200(); },
    [](void* body, void* services) { Interaction(body, services).CaptureAt4C01F0(); },
    [](void* body, void* services) { Interaction(body, services).RestoreAt4BF4E0(); },
    [](void* body, void* services, std::uint8_t value) { Interaction(body, services).SetCaptureModeAt4C2300(value); },
    [](void* body, void* services) { Interaction(body, services).PropagateDirtyRectsAt415910(); },
    [](void* body, void* services, std::uint8_t value) { Interaction(body, services).ResetSkipLatchAt4AB910(value); },
    // ReadScriptToolServices::ToolEventAt34既有.value()拒绝不能改成value_or(-1)。
    [](void* body) { return Tool(body).State().valueAt34.value(); },
    [](void* body, std::uint8_t value) { Tool(body).State().valueAt38 = value; },
    [](void* body) { return Tool(body).EnabledByteAt0C(); }};

ToolWindowInteractionRuntime& BorrowedInteraction(void* body, void* interaction)
{
    (void)Tool(body);
    if (!interaction) throw std::logic_error("controlled Draw Tool interaction was not bound");
    return *static_cast<ToolWindowInteractionRuntime*>(interaction);
}
constexpr DrawToolViewMethods borrowedMethods{
    [](void* body, void* interaction, std::int32_t mode) { BorrowedInteraction(body, interaction).SetModeAt434FB0(mode); },
    [](void* body, void* interaction) { return BorrowedInteraction(body, interaction).TickAt4BE200(); },
    [](void* body, void* interaction) { BorrowedInteraction(body, interaction).CaptureAt4C01F0(); },
    [](void* body, void* interaction) { BorrowedInteraction(body, interaction).RestoreAt4BF4E0(); },
    [](void* body, void* interaction, std::uint8_t value) { BorrowedInteraction(body, interaction).SetCaptureModeAt4C2300(value); },
    [](void* body, void* interaction) { BorrowedInteraction(body, interaction).PropagateDirtyRectsAt415910(); },
    [](void* body, void* interaction, std::uint8_t value) { BorrowedInteraction(body, interaction).ResetSkipLatchAt4AB910(value); },
    methods.readEvent, methods.writeLatch, methods.enabled};
}

DrawToolView::DrawToolView(FrameLayerItem* body, IToolWindowInteractionServices* services) noexcept
    : body_(body), methods_(&methods), services_(services) {}
DrawToolView::DrawToolView(ToolWindowLayerRuntime& body, IToolWindowInteractionServices& services) noexcept
    : DrawToolView(static_cast<FrameLayerItem*>(&body), &services) {}
DrawToolView::DrawToolView(ToolWindowLayerRuntime& body, ToolWindowInteractionRuntime& interaction) noexcept
    : body_(static_cast<FrameLayerItem*>(&body)), methods_(&borrowedMethods), services_(&interaction) {}
}
