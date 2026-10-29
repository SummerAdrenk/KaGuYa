#include <Runtime/ReadScriptSceneContextRuntime.h>

#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
ScrVmRuntimeStateBundle& CheckedBundle(const ReadScriptSceneContextDependencies& dependencies)
{
    // 现代装配前置检查不读取/发布业务字段；拒绝scene与VM借到两个CScript正文。
    dependencies.bundle.RequireScriptView(dependencies.script);
    return dependencies.bundle;
}
}
ReadScriptSceneContextRuntime::ControlledGraph::ControlledGraph(
    ReadScriptSceneContextDependencies dependencies)
    : workflow(dependencies.effectFactory, CheckedBundle(dependencies), dependencies.profile,
          dependencies.save, dependencies.unsave, dependencies.mouse, dependencies.keyboard,
          dependencies.manager, dependencies.sound, dependencies.graphics, dependencies.rootByteAt1B0),
      startupConfig(dependencies.params),
      scroll(FreeScrollDependencies{dependencies.manager, startupConfig, dependencies.save,
          dependencies.mouse, dependencies.keyboard, dependencies.cursor, dependencies.window,
          dependencies.windowApi, dependencies.graphics, dependencies.graphics, dependencies.graphics,
          dependencies.graphics, dependencies.graphics}),
      layers(dependencies.manager, scroll, dependencies.mouse, dependencies.keyboard, dependencies.opacity),
      text(dependencies.interactionHost, dependencies.tools, layers, dependencies.bundle.values,
          dependencies.save, dependencies.unsave, dependencies.text),
      menu(dependencies.interactionHost, dependencies.tools, layers, dependencies.menu,
          dependencies.save, dependencies.unsave, dependencies.text),
      update(dependencies.vm, dependencies.script, dependencies.yield, text, menu, workflow)
{
    // FreeScroll 构造已按 4A14D0 读取真实 SIZE；不缓存 scene state，也不提前做恢复业务。
}

ReadScriptSceneContextRuntime::NativeResources::NativeResources(ReadScriptEffectFactoryNativeServices& value)
    : services(value)
{
    InitializeReadScriptEffectSlotsAt4A14D0(effects);
    ReadScriptEffectNativeCallScope scope(services);
    try { ConstructFreeScrollNativeAt4A1559(scroll, services.sizes); }
    catch (...)
    {
        // FreeScroll已展开自己的entries，随后按59CCD9..59CCB8反序清已成立四Ref。
        UnwindReadScriptEffectSlotsAt4A14D0(effects);
        throw;
    }
}
ReadScriptSceneContextRuntime::NativeResources::~NativeResources() noexcept
{
    ReadScriptEffectNativeCallScope scope(services);
    // 正常owner已先4C0990，再销毁scene字符串；这里只做成员尾，不补End或Finish。
    DestroyFreeScrollNativeAt4A3B60(scroll);
    DestroyReadScriptEffectSlotMembersAt4A4920(effects);
}
ReadScriptSceneContextRuntime::NativeGraph::NativeGraph(ReadScriptSceneContextNativeDependencies d)
    : resources(d.effects), owner(resources.effects),
      actions(owner, d.effects, d.masks, d.effectState, d.flashShake),
      workflow(owner, actions, d.waitState, d.layers, d.sound, d.screen, d.effects.pattern, d.effects),
      scroll(resources.scroll, d.effects.draw, d.startupConfig, d.scrollDrawing, d.effects.pattern,
          d.scrollInput, d.animation, d.screen),
      layers(d.layers, scroll, d.opacity),
      text(d.interactionHost, d.tools, layers, d.interactionState, d.text),
      menu(d.interactionHost, d.tools, layers, d.menu, d.interactionState, d.text),
      update(d.vm, d.script, d.yield, text, menu, workflow) {}

ReadScriptSceneContextRuntime::ReadScriptSceneContextRuntime(ReadScriptSceneContextDependencies d)
    : graph_(std::in_place_type<ControlledGraph>, d) {}
ReadScriptSceneContextRuntime::ReadScriptSceneContextRuntime(ReadScriptSceneContextNativeDependencies d)
    : graph_(std::in_place_type<NativeGraph>, d) {}

// 4A4920 没有 Scroll End；纯资源析构不补 Finish，供半构造失败及外层显式释放后使用。
ReadScriptSceneContextRuntime::~ReadScriptSceneContextRuntime() noexcept = default;

ReadScriptUpdateRuntime& ReadScriptSceneContextRuntime::Update() noexcept
{ return std::visit([](auto& graph) -> ReadScriptUpdateRuntime& { return graph.update; }, graph_); }

ReadScriptTransitionWorkflowRuntime& ReadScriptSceneContextRuntime::Transition() noexcept
{
    return std::visit([](auto& graph) -> ReadScriptTransitionWorkflowRuntime& { return graph.workflow; }, graph_);
}

FreeScrollRuntime& ReadScriptSceneContextRuntime::Scroll()
{
    auto* graph = std::get_if<ControlledGraph>(&graph_);
    if (!graph) throw std::logic_error("Native scene context has no modern FreeScroll");
    return graph->scroll;
}
IFreeScrollOperations& ReadScriptSceneContextRuntime::ScrollOperations() noexcept
{ return std::visit([](auto& graph) -> IFreeScrollOperations& { return graph.scroll; }, graph_); }
FreeScrollNativeBody50h& ReadScriptSceneContextRuntime::NativeScrollBody()
{
    auto* graph = std::get_if<NativeGraph>(&graph_);
    if (!graph) throw std::logic_error("Controlled scene context has no native FreeScroll body");
    return graph->resources.scroll;
}
ReadScriptEffectSlotsNative30h& ReadScriptSceneContextRuntime::NativeEffectSlots()
{
    auto* graph = std::get_if<NativeGraph>(&graph_);
    if (!graph) throw std::logic_error("Controlled scene context has no native Effect slots");
    return graph->resources.effects;
}

void ReadScriptSceneContextRuntime::ReleaseResourcesAt4C0990()
{
    Transition().ReleaseResourcesAt4C0990();
}
}
