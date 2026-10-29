#include <Runtime/MoveEffectNative.h>
#include <Runtime/ReadScriptEffectNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/PatternLayerNativeResourceTarget.h>

#include <bit>
#include <cstdlib>
#include <new>
#include <stdexcept>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = MoveEffectNativeBody24h;
using Item = MoveEffectItemNativeBody3Ch;
using Surface = GraphicsSurfaceNativeBody;
thread_local MoveEffectNativeServices* currentServices = nullptr;

template<class T> T Read(const T& value) noexcept { return static_cast<const volatile T&>(value); }
template<class T> T* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
float Float(std::int32_t value) noexcept
{ return _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_cvtsi32_si128(value))); }
float Plus(float a, float b) noexcept
{ return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a), _mm_set_ss(b))); }
float Minus(float a, float b) noexcept
{ return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a), _mm_set_ss(b))); }
bool Equal(float a, float b) noexcept
{ return (_mm_movemask_ps(_mm_cmpeq_ss(_mm_set_ss(a), _mm_set_ss(b))) & 1) != 0; }
bool Greater(float a, float b) noexcept
{ return _mm_comigt_ss(_mm_set_ss(a), _mm_set_ss(b)) != 0; }

MoveEffectNativeServices& Services()
{
    if (!currentServices) throw std::logic_error("Move native virtual call has no bound services");
    return *currentServices;
}
std::uint8_t __fastcall Tick(Body* body, void*) { return TickMoveEffectNativeAt4BB960(*body); }
void __fastcall Draw(Body* body, void*) { DrawMoveEffectNativeAt4B5550(*body, Services()); }
std::uint8_t __fastcall Ready(Body* body, void*) { return Read(body->readyAt04); }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteMoveEffectNativeAt4A65C0(*body, flags, Services()); }
const MoveEffectNativeVtable methods{
    reinterpret_cast<decltype(MoveEffectNativeVtable::tickAt00)>(&Tick),
    reinterpret_cast<decltype(MoveEffectNativeVtable::drawAt04)>(&Draw),
    reinterpret_cast<decltype(MoveEffectNativeVtable::readyAt08)>(&Ready),
    reinterpret_cast<decltype(MoveEffectNativeVtable::deleteAt0C)>(&Delete)};

void RenderAndInvalidate(Body& body, MoveEffectNativeServices& services)
{
    // 4B5752/4B57AC（析构4A43E6/4A4440）分别取根，不复用第一次注册表。
    auto* layers = ResolveDrawLayerListAt47AE80(*services.draw.CaptureDrawManagerAfter413BE0());
    layers->vptrAt00->addDirtyAt20(layers, &body.viewportAt14);
    layers = ResolveDrawLayerListAt47AE80(*services.draw.CaptureDrawManagerAfter413BE0());
    layers->vptrAt00->drawDirtyAt18(layers);
    services.screen.InvalidatedRectIfLoaded(nullptr);
}
}

const MoveEffectNativeVtable& MoveEffectNativeMethods() noexcept { return methods; }
const MoveEffectNativeVtable& MoveEffectBaseNativeMethods() noexcept
{
    // 各原Effect共用5B4D00，不复制第二份基类表；四槽this/参数/返回的x86 ABI相同。
    return *reinterpret_cast<const MoveEffectNativeVtable*>(&ReadScriptEffectBaseNativeMethods());
}
MoveEffectNativeScope::MoveEffectNativeScope(MoveEffectNativeServices& value) noexcept
    : previous_(currentServices) { currentServices = &value; }
MoveEffectNativeScope::~MoveEffectNativeScope() noexcept { currentServices = previous_; }

std::uint8_t TickMoveEffectNativeAt4BB960(Body& body)
{
    std::uint8_t moved = 0;
    for (auto* slot = Read(body.itemsAt08.begin); slot != Read(body.itemsAt08.end); ++slot)
    {
        auto* targets = Resolve<Item>(*slot);
        auto* values = Resolve<Item>(*slot);
        const auto tx = Float(Read(targets->targetXAt08));
        if (Equal(Read(values->currentXAt00), tx) &&
            Equal(Read(values->currentYAt04), Float(Read(targets->targetYAt0C)))) continue;
        if (Read(Resolve<Item>(*slot)->directionsAt20) & 2)
        {
            auto* value = Resolve<Item>(*slot);
            auto* step = Resolve<Item>(*slot);
            value->currentXAt00 = Minus(Read(value->currentXAt00), Read(step->stepXAt18));
            auto* target = Resolve<Item>(*slot);
            value = Resolve<Item>(*slot);
            if (Greater(Float(Read(target->targetXAt08)), Read(value->currentXAt00)))
                Resolve<Item>(*slot)->currentXAt00 = Float(Read(Resolve<Item>(*slot)->targetXAt08));
        }
        else if (Read(Resolve<Item>(*slot)->directionsAt20) & 1)
        {
            auto* value = Resolve<Item>(*slot);
            auto* step = Resolve<Item>(*slot);
            value->currentXAt00 = Plus(Read(step->stepXAt18), Read(value->currentXAt00));
            auto* target = Resolve<Item>(*slot);
            value = Resolve<Item>(*slot);
            if (Greater(Read(value->currentXAt00), Float(Read(target->targetXAt08))))
                Resolve<Item>(*slot)->currentXAt00 = Float(Read(Resolve<Item>(*slot)->targetXAt08));
        }
        if (Read(Resolve<Item>(*slot)->directionsAt20) & 8)
        {
            auto* value = Resolve<Item>(*slot);
            auto* step = Resolve<Item>(*slot);
            value->currentYAt04 = Minus(Read(value->currentYAt04), Read(step->stepYAt1C));
            auto* target = Resolve<Item>(*slot);
            value = Resolve<Item>(*slot);
            if (Greater(Float(Read(target->targetYAt0C)), Read(value->currentYAt04)))
                Resolve<Item>(*slot)->currentYAt04 = Float(Read(Resolve<Item>(*slot)->targetYAt0C));
        }
        else if (Read(Resolve<Item>(*slot)->directionsAt20) & 4)
        {
            auto* value = Resolve<Item>(*slot);
            auto* step = Resolve<Item>(*slot);
            value->currentYAt04 = Plus(Read(step->stepYAt1C), Read(value->currentYAt04));
            auto* target = Resolve<Item>(*slot);
            value = Resolve<Item>(*slot);
            if (Greater(Read(value->currentYAt04), Float(Read(target->targetYAt0C))))
                Resolve<Item>(*slot)->currentYAt04 = Float(Read(Resolve<Item>(*slot)->targetYAt0C));
        }
        moved = 1;
        if (!Read(Resolve<Item>(*slot)->directionsAt20)) continue;
        // 4BBD6C仅看snapshot.base；无快照时已推进float，但不更新Pattern/后续条目。
        if (!Read(Resolve<Item>(*slot)->snapshotAt28.baseAt00)) return 0;
        values = Resolve<Item>(*slot);
        const auto x = _mm_cvtt_ss2si(_mm_set_ss(Read(values->currentXAt00)));
        const auto y = _mm_cvtt_ss2si(_mm_set_ss(Read(values->currentYAt04)));
        auto* pattern = Read(Resolve<Item>(*slot)->patternAt24);
        pattern->layer.valueAt04 = static_cast<std::uint32_t>(x);
        pattern->layer.valueAt08 = static_cast<std::uint32_t>(y);
    }
    return moved;
}

void DrawMoveEffectNativeAt4B5550(Body& body, MoveEffectNativeServices& services)
{
    PatternLayerNativeGraphicsScope graphics(services.pattern.drawing);
    LayerListNativeScope layers(DefaultEnvironmentRootMemory(), &services.screen);
    for (auto* slot = Read(body.itemsAt08.begin); slot != Read(body.itemsAt08.end); ++slot)
    {
        auto* pattern = Read(Resolve<Item>(*slot)->patternAt24);
        const auto x = std::bit_cast<std::int32_t>(Read(pattern->layer.valueAt04));
        const auto y = std::bit_cast<std::int32_t>(Read(pattern->layer.valueAt08));
        if (!Read(Resolve<Item>(*slot)->directionsAt20))
        {
            pattern = Read(Resolve<Item>(*slot)->patternAt24);
            pattern->layer.vptrAt00->drawAt00(&pattern->layer, nullptr);
        }
        else
        {
            auto* item = Resolve<Item>(*slot);
            const auto dx = Add(x, Read(item->offsetXAt34));
            const auto dy = Add(y, Read(item->offsetYAt38));
            auto* snapshot = Resolve<Surface>(Resolve<Item>(*slot)->snapshotAt28);
            const auto handle = Read(snapshot->handleAt04);
            if (handle) (void)services.pattern.drawing.SurfaceDrawRaw(handle, dx, dy,
                nullptr, nullptr, nullptr, 0);
        }
    }
    RenderAndInvalidate(body, services);
}

void DestroyMoveEffectNativeAt4A4280(Body& body, MoveEffectNativeServices& services) noexcept
{
    const auto ready = Read(body.readyAt04);
    body.vptrAt00 = &methods;
    GraphicsSurfaceNativeScope surfaces(services.pattern.surfaces);
    PatternLayerNativeGraphicsScope graphics(services.pattern.drawing);
    LayerListNativeScope layers(DefaultEnvironmentRootMemory(), &services.screen);
    if (ready)
    {
        for (auto* slot = Read(body.itemsAt08.begin); slot != Read(body.itemsAt08.end); ++slot)
        {
            if (!Read(Resolve<Item>(*slot)->directionsAt20)) continue;
            auto* item = Resolve<Item>(*slot);
            auto* pattern = Read(Resolve<Item>(*slot)->patternAt24);
            const auto x = Read(item->finalXAt10);
            const auto y = Read(item->finalYAt14);
            pattern->layer.valueAt04 = static_cast<std::uint32_t>(x);
            pattern->layer.valueAt08 = static_cast<std::uint32_t>(y);
            InvalidatePatternLayerNativeAt443650(*Read(Resolve<Item>(*slot)->patternAt24), nullptr);
            RefreshPatternProcessedNativeAt443B00(*Read(Resolve<Item>(*slot)->patternAt24), services.pattern);
        }
        auto* manager = services.draw.CaptureDrawManagerAfter413BE0();
        auto* window = Resolve<LayerWindowNativeBody4Ch>(*manager->ownedLayersAt14.begin);
        window->window.layer.enabledAt0C = 1;
        RenderAndInvalidate(body, services);
    }
    // 4A38A0/498AF0同已核Ref12算法；末尾恢复5B4D00基类表，不清ready。
    DestroyDrawLayerReferencesAt461750(body.itemsAt08);
    body.vptrAt00 = &MoveEffectBaseNativeMethods();
}

MoveEffectNativeBody24h* DeleteMoveEffectNativeAt4A65C0(Body& body,
    std::uint32_t flags, MoveEffectNativeServices& services) noexcept
{
    DestroyMoveEffectNativeAt4A4280(body, services);
    if (flags & 1) ::operator delete(&body, 0x24);
    return &body;
}
}
