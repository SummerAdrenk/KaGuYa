#include <Runtime/LayerWindowNative.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/LayerListNative.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerWindowNativeBody4Ch;
using Base = LayerItemNativeBody14h;
thread_local LayerWindowNativeServices* activeServices = nullptr;
LayerWindowNativeServices& Services()
{
    if (!activeServices) throw std::logic_error("Native LayerWindow requires an external services scope");
    return *activeServices;
}
Body& Derived(Base* base) noexcept { return *reinterpret_cast<Body*>(base); }
void __fastcall Draw(Base* base, void*, const RECT* clip)
    { DrawLayerWindowNativeAt482900(Derived(base), clip, Services().drawState); }
void __fastcall Composite(Base* base, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
    { CompositeLayerWindowNativeAt481990(Derived(base), target, clip); }
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags)
    { DeleteLayerWindowNativeAt468C20(Derived(base), flags); return base; }
RECT* __fastcall Bounds(Base* base, void*, RECT* out)
    { *out = Derived(base).window.boundsAt14; return out; }
std::uint8_t __fastcall Request(Base*, void*, std::uint8_t, std::int32_t) { return 0; }
void __fastcall Advance(Base* base, void*)
{
    if (base->enabledAt0C)
        AdvanceLayerWindowNativeAt46B180(Derived(base), Services().pattern);
}
void __fastcall Reset(Base* base, void*)
    { ResetLayerWindowNativeAt47EC40(Derived(base), Services().pattern.drawing); }
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};

struct RawPatternUnwind final
{
    void* raw;
    ~RawPatternUnwind() noexcept { if (raw) ::operator delete(raw, std::size_t{0x80}); }
};
struct PatternRefUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool armed = true;
    ~PatternRefUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
};
void ReleaseLocalAt459A43(YtlReferenceSlotRuntime& slot) noexcept
{
    // 正常出口固定ESI；payload回调后不能重取局部slot.control。
    auto* const control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}
}

const LayerItemNativeFrameVtable& LayerWindowNativeMethods() noexcept { return methods; }
Body* ConstructLayerWindowNativeAt459650(Body& body, LayerListNativeBody18h* parent,
    ILayerWindowConstructionSource& source, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope graphics(surfaces);
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = methods.AsBase();
    body.internalAt24 = {};
    body.patternsAt30 = {};
    body.effectAt40 = {};
    try
    {
        body.window.layer.enabledAt0C = 1;
        const auto screen = source.ReadScreenSizeAt47CC00();
        body.selectorAt3C = -1;
        body.window.boundsAt14 = {0, 0, screen.width, screen.height};
        auto& memory = DefaultEnvironmentRootMemory();
        auto* const list = CreateLayerListNativeAt440E30(memory);
        InstallLayerListReferenceAt4516C0(body.internalAt24, list, memory);
        for (std::int32_t index = 0; index < source.ReadStartupConfigCountAt45983C(); ++index)
        {
            PatternLayerNativeBody80h* pattern = nullptr;
            {
                void* const raw = ::operator new(std::size_t{0x80});
                RawPatternUnwind unwind{raw};
                if (raw)
                {
                    pattern = ::new(raw) PatternLayerNativeBody80h;
                    // 459853申请在先；45986D固定parent之后，45988E才重取当前startup byte。
                    auto* const internal = ResolveLayerListReference(body.internalAt24);
                    const auto count = source.ReadStartupConfigByteAt45998B(static_cast<std::size_t>(index));
                    ConstructPatternLayerNativeAt45B120(*pattern, internal, {screen.width, screen.height},
                        index != 0, count, surfaces, strings);
                }
                unwind.raw = nullptr;
            }
            YtlReferenceSlotRuntime local;
            ConstructPatternLayerReferenceAt4599B5(local, pattern);
            PatternRefUnwind unwind{local};
            AppendDrawLayerReference(body.patternsAt30, local, memory);
            unwind.armed = false;
            ReleaseLocalAt459A43(local);
        }
    }
    catch (...)
    {
        // 459A80原catch只清RECT；已经安装的list、Pattern前缀和注册关系均保留。
        body.window.boundsAt14 = {};
    }
    return &body;
}
Body* DeleteLayerWindowNativeAt468C20(Body& body, std::uint32_t flags) noexcept
{
    ReleaseYtlReference(body.effectAt40);
    DestroyDrawLayerReferencesAt461750(body.patternsAt30);
    ReleaseYtlReference(body.internalAt24);
    DestroyLayerWindowNativeAt464910(body.window);
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x4C});
    return result;
}
LayerWindowNativeScope::LayerWindowNativeScope(LayerWindowNativeServices& services) noexcept
    : previous_(activeServices), patternDrawing_(services.pattern.drawing) { activeServices = &services; }
LayerWindowNativeScope::~LayerWindowNativeScope() noexcept { activeServices = previous_; }
}
