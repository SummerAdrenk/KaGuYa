#include <Runtime/PostWindowNativeLayers.h>
#include <Runtime/GameBuildProfile.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Body, class Construct>
Body* CreateAndAppend(IDrawManagerNativeRootSource& roots, IEnvironmentRootMemory& memory, Construct&& construct)
{
    // 489C90 state0D/0F/11/12/14：分配先于第一个Ensure；仅构造阶段保有raw guard。
    void* raw = memory.AllocateBody(sizeof(Body));
    struct RawUnwind final
    {
        IEnvironmentRootMemory& memory;
        void* raw;
        bool active = true;
        ~RawUnwind() noexcept { if (active) memory.DeleteBody(raw, sizeof(Body)); }
    } unwind{memory, raw};
    Body* created = nullptr;
    if (raw)
    {
        auto* manager = roots.CaptureDrawManagerAfter413BE0();
        auto* parent = ResolveDrawLayerListAt47AE80(*manager);
        created = construct(*::new(raw) Body, parent);
    }
    unwind.active = false;
    // 构造回调可以替换root+18：此处必须重新捕获。捕获/control失败不补删完成正文。
    auto* manager = roots.CaptureDrawManagerAfter413BE0();
    AppendOwnedDrawLayerAt46A880(manager->ownedLayersAt14,
        reinterpret_cast<LayerItemNativeBody14h*>(created), memory);
    return created;
}
}
LayerWindowNativeBody4Ch* CreateAndAppendLayerWindowNativeAt48A6FF(IDrawManagerNativeRootSource& roots,
    ILayerWindowConstructionSource& source, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    GraphicsSurfaceNativeScope graphics(surfaces);
    LayerListNativeScope registration(memory);
    return CreateAndAppend<LayerWindowNativeBody4Ch>(roots, memory, [&](auto& body, auto* parent) {
        return ConstructLayerWindowNativeAt459650(body, parent, source, surfaces, strings);
    });
}
ToolWindowNativeBody40h* CreateAndAppendToolWindowNativeAt48A7F7(IDrawManagerNativeRootSource& roots,
    ToolWindowNativeServices& services)
{
    ToolWindowNativeScope scope(services);
    return CreateAndAppend<ToolWindowNativeBody40h>(roots, services.memory, [&](auto& body, auto* parent) {
        return ConstructToolWindowNativeAt45A4B0(body, parent, services);
    });
}
WaitMarkNativeBody34h* CreateAndAppendWaitMarkNativeAt48A8F2(IDrawManagerNativeRootSource& roots,
    WaitMarkNativeServices& services)
{
    WaitMarkNativeScope scope(services);
    return CreateAndAppend<WaitMarkNativeBody34h>(roots, services.memory, [&](auto& body, auto* parent) {
        return ConstructWaitMarkNativeAt45E450(body, parent, services);
    });
}
SelectWindowNativeBody6Ch* CreateAndAppendSelectWindowNativeAt48A96E(IDrawManagerNativeRootSource& roots,
    SelectWindowNativeServices& services)
{
    SelectWindowNativeScope scope(services);
    return CreateAndAppend<SelectWindowNativeBody6Ch>(roots, services.memory, [&](auto& body, auto* parent) {
        return ConstructSelectWindowNativeAt45CC30(body, parent, services);
    });
}
DateWindowNativeBody* CreateAndAppendDateWindowNative(IDrawManagerNativeRootSource& roots,
    DateWindowNativeServices* services, IEnvironmentRootMemory& memory)
{
    if (!CompiledGameProfile().dateWindow) return nullptr;
    if (!services) throw std::logic_error("Configured DateWindow services are absent");
    const LayerListNativeScope layers(memory);
    return CreateAndAppend<DateWindowNativeBody>(roots, memory, [&](auto& body, auto* parent) {
        return ConstructDateWindowNative(body, parent, *services);
    });
}
void CreateAndAppendPrimaryGaugeNative(IDrawManagerNativeRootSource& roots, TokimekiGaugeNativeServices* tokimeki,
    LoveHolicGaugesNativeServices* loveHolic, IEnvironmentRootMemory& memory)
{
    if (!HasPrimaryGauge()) return;
    if (CompiledGameProfile().tokimekiGauge && CompiledGameProfile().loveHolicGauges)
        throw std::logic_error("Primary gauge profiles are mutually exclusive");
    const LayerListNativeScope layers(memory);
    if (CompiledGameProfile().tokimekiGauge)
    {
        if (!tokimeki) throw std::logic_error("Tokimeki services are absent");
        (void)CreateAndAppend<TokimekiGaugeNativeBody>(roots, memory, [&](auto& body, auto* parent) {
            return ConstructTokimekiGaugeNative(body, parent, *tokimeki);
        });
    }
    else
    {
        if (!loveHolic) throw std::logic_error("Percentage gauge services are absent");
        (void)CreateAndAppend<LoveHolicGaugeNativeBody>(roots, memory, [&](auto& body, auto* parent) {
            return ConstructLoveHolicPercentageGaugeNative(body, parent, *loveHolic);
        });
    }
}
void CreateAndAppendMasoGaugeNative(IDrawManagerNativeRootSource& roots, LoveHolicGaugesNativeServices* services,
    IEnvironmentRootMemory& memory)
{
    if (!CompiledGameProfile().loveHolicGauges) return;
    if (!services) throw std::logic_error("Maso gauge services are absent");
    const LayerListNativeScope layers(memory);
    (void)CreateAndAppend<LoveHolicGaugeNativeBody>(roots, memory, [&](auto& body, auto* parent) {
        return ConstructLoveHolicMasoGaugeNative(body, parent, *services);
    });
}
CommandSelectionNativeBody* CreateAndAppendCommandSelectionNative(IDrawManagerNativeRootSource& roots,
    CommandSelectionNativeServices* services, IEnvironmentRootMemory& memory)
{
    if (!CompiledGameProfile().commandSelection) return nullptr;
    if (!services) throw std::logic_error("Configured command selection services are absent");
    const LayerListNativeScope layers(memory);
    return CreateAndAppend<CommandSelectionNativeBody>(roots, memory, [&](auto& body, auto* parent) {
        return ConstructCommandSelectionNative(body, parent, *services);
    });
}
CountDownWindowNativeBody44h* CreateAndAppendCountDownWindowNativeAt48AA66(IDrawManagerNativeRootSource& roots,
    CountDownWindowNativeServices& services)
{
    // Baku 44B810只追加0..6；以已审计游戏能力选择，不靠缺资源异常跳过。
    if (!CompiledGameProfile().countdownEnabled) return nullptr;
    CountDownWindowNativeScope scope(services);
    return CreateAndAppend<CountDownWindowNativeBody44h>(roots, services.memory, [&](auto& body, auto* parent) {
        return ConstructCountDownWindowNativeAt458210(body, parent, services);
    });
}
void CreatePostWindowNativeLayersAt48A6FF(IDrawManagerNativeRootSource& roots, PostWindowNativeLayerServices& services)
{
    CreateAndAppendLayerWindowNativeAt48A6FF(roots, services.layer, services.surfaces, services.strings, services.memory);
    CreateAndAppendMessageWindowLayerNative(roots, services.message);
    CreateAndAppendToolWindowNativeAt48A7F7(roots, services.tool);
    CreateAndAppendTextWindowLayerNativeAt48A873(roots, services.text);
    CreateAndAppendWaitMarkNativeAt48A8F2(roots, services.wait);
    CreateAndAppendDateWindowNative(roots, services.date, services.memory);
    CreateAndAppendSelectWindowNativeAt48A96E(roots, services.select);
    CreateAndAppendOpacityWindowAt48A9EC(roots, services.opacityResources, services.surfaces, services.strings, services.memory,
        services.opacityProfile);
    CreateAndAppendPrimaryGaugeNative(roots, services.tokimeki, services.loveHolic, services.memory);
    CreateAndAppendCommandSelectionNative(roots, services.command, services.memory);
    CreateAndAppendMasoGaugeNative(roots, services.loveHolic, services.memory);
    CreateAndAppendCountDownWindowNativeAt48AA66(roots, services.countdown);
}
}
