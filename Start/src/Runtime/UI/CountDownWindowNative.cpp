#include <Runtime/CountDownWindowNative.h>
#include <Runtime/CountDownWindowNativeResourceProfile.h>
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/SpriteNativeResourceState.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = CountDownWindowNativeBody44h;
using Base = LayerItemNativeBody14h;
using Slot = YtlReferenceSlotRuntime;
bool FiveFrames(const Body& body) noexcept
{ return body.window.layer.vptrAt00 == FiveFrameCountDownWindowNativeMethods().AsBase(); }
thread_local CountDownWindowNativeServices* activeServices;
CountDownWindowNativeServices& Current()
{ if (!activeServices) throw std::logic_error("CountDownWindowNativeScope not installed"); return *activeServices; }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
RECT MoveTo(RECT rect, POINT position) noexcept
{
    rect.right = Add(rect.right, Sub(position.x, rect.left));
    rect.bottom = Add(rect.bottom, Sub(position.y, rect.top));
    rect.left = position.x; rect.top = position.y;
    return rect;
}
struct Unwind final
{
    Slot& slot;
    bool armed = true;
    ~Unwind() noexcept { if (armed) ReleaseSpriteReferenceAt460260(slot); }
};
void ReleaseCaptured(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
void ClearSprites(DrawLayerReferenceArray& array) noexcept
{
    // 491360：固定传入范围，释放后才重读begin写end；保留原容量。
    ReleaseDrawLayerRangeAt452440(array.begin, array.end);
    array.end = array.begin;
}
const std::array<POINT, 6>& Positions(const CountDownWindowNativeServices& services, bool number) noexcept
{
    if (services.nativeProfile)
        return number ? services.nativeProfile->numberPositions : services.nativeProfile->suffixPositions;
    return number ? services.profile.numberPositions : services.profile.suffixPositions;
}
const char* Name(const CountDownWindowNativeServices& services, bool number, unsigned index) noexcept
{
    if (services.fiveFrameProfile) return services.fiveFrameProfile->namesCp932[index];
    if (services.nativeProfile)
        return number ? services.nativeProfile->numberNamesCp932[index].data() : services.nativeProfile->suffixNamesCp932[index].data();
    return number ? services.profile.numberNamesCp932[index].c_str() : services.profile.suffixNamesCp932[index].c_str();
}
RECT LoadSprites(DrawLayerReferenceArray& array, bool number, unsigned count,
    CountDownWindowNativeServices& services)
{
    RECT bounds{};
    ClearSprites(array);
    for (unsigned i = 0; i < count; ++i)
    {
        Slot local;
        Unwind unwind{local};
        auto* const receiver = services.resources.CaptureDrawReceiverAt413BE0();
        const char* const name = Name(services, number, i);
        auto* const sprite = NewSpriteNativeAt4813E4(services.strings);
        InstallSpriteReferenceAt48140F(local, sprite);
        if (!services.resources.LoadAt4814F0(receiver, sprite, name, 0)) throw 1;
        const auto drawable = ReadSpriteDrawableRectNativeAt4AD080(*sprite);
        const RECT rect = drawable.state == PatternSpriteRectState::Valid ? drawable.rect : RECT{};
        (void)::UnionRect(&bounds, &bounds, &rect);
        AppendDrawLayerReference(array, local, services.memory);
        // 458447/45861C处普通尾部释放已进入terminate状态；不能进入外层清表catch。
        unwind.armed = false;
        ReleaseCaptured(local.controlAt08);
    }
    return bounds;
}
Slot* Element(DrawLayerReferenceArray& array, std::uint32_t index) noexcept
{ return reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(array.begin) + index * 12U); }
Slot* Select(Body& body, Slot& output, std::int32_t mode, bool number, CountDownWindowNativeServices& services)
{
    if (FiveFrames(body))
    {
        // 10号4667C0：countdown_suffix直接作为有符号索引；不查询GlobalParams，不重定位。
        if (!number || body.numberAt3C < 0 || body.numberAt3C >= DrawLayerReferenceCount(body.numbersAt30))
            output = {};
        else
        {
            output = *Element(body.numbersAt30, static_cast<std::uint32_t>(body.numberAt3C));
            if (output.controlAt08) ++output.controlAt08->referenceCountAt04;
        }
        return &output;
    }
    if (number ? std::uint32_t(body.numberAt3C) > 4U : body.suffixAt40 == 0) { output = {}; return &output; }
    const auto size = services.state.CaptureGlobalVia417990()->wordsAt0C[6];
    const auto color = services.state.CaptureGlobalVia417990()->wordsAt0C[7];
    const auto alpha = services.state.CaptureGlobalVia417990()->wordsAt0C[8];
    if (size >= 3 || color >= 2 || alpha >= 2) { output = {}; return &output; }
    std::uint32_t index = size + 3U * (color + 2U * alpha);
    if (number) index = index * 5U + std::uint32_t(body.numberAt3C);
    auto& array = number ? body.numbersAt30 : body.suffixesAt24;
    if (std::bit_cast<std::int32_t>(index) >= DrawLayerReferenceCount(array)) { output = {}; return &output; }
    auto* const sprite = ResolveSpriteReference(*Element(array, index));
    const auto positionIndex = size + std::uint32_t(mode) * 3U;
    const auto& positions = Positions(services, number);
    sprite->xAt2C = positions[positionIndex].x;
    sprite->yAt30 = positions[positionIndex].y;
    // 47BF13/47C85D写坐标后重读vector起址，457F20构造fresh返回Ref。
    output = *Element(array, index);
    if (output.controlAt08) ++output.controlAt08->referenceCountAt04;
    return &output;
}
void Invalidate(Body& body)
{
    RECT rect;
    const auto* table = reinterpret_cast<const LayerItemNativeFrameVtable*>(body.window.layer.vptrAt00);
    auto* const output = table->boundsAt0C(&body.window.layer, &rect);
    auto* const parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, output);
}
template<class Render>
void RenderSprites(Body& body, CountDownWindowNativeServices& services, Render&& render)
{
    const bool five = FiveFrames(body);
    const auto mode = five ? 0 : std::bit_cast<std::int16_t>(services.state.CaptureGlobalInlineAt4180C0()->wordsAt0C[5]);
    if (std::uint32_t(mode) > 1U) return;
    for (bool number : {false, true})
    {
        if (five && !number) continue;
        Slot local;
        Select(body, local, mode, number, services);
        auto* const control = local.controlAt08;
        Unwind unwind{local};
        if (local.baseAt00) render(*ResolveSpriteReference(local));
        unwind.armed = false;
        ReleaseCaptured(control);
    }
}
void __fastcall Draw(Base* self, void*, const RECT* clip)
{ DrawCountDownWindowNativeAt4827A0(*reinterpret_cast<Body*>(self), clip, Current()); }
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ CompositeCountDownWindowNativeAt481830(*reinterpret_cast<Body*>(self), target, clip, Current()); }
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
{ return reinterpret_cast<Base*>(DeleteCountDownWindowNativeAt468770(*reinterpret_cast<Body*>(self), flags)); }
RECT* __fastcall Bounds(Base* self, void*, RECT* output)
{ return CountDownBoundsNativeAt479CE0(*reinterpret_cast<Body*>(self), output, Current()); }
std::uint8_t __fastcall Request(Base* self, void*, std::uint8_t value, std::int32_t)
{ return RequestCountDownWindowNativeAt486F70(*reinterpret_cast<Body*>(self), value, Current()); }
void __fastcall Advance(Base* self, void*)
{ AdvanceCountDownWindowNativeAt46B0D0(*reinterpret_cast<Body*>(self), Current()); }
void __fastcall Reset(Base* self, void*) { ResetCountDownWindowNativeAt47EC20(*reinterpret_cast<Body*>(self)); }
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 同一套Sprite/Layer入口，只有虚表身份承载策略；0D..0F、suffixAt40均不作标记。
const LayerItemNativeFrameVtable fiveFrameMethods = methods;
}
CountDownWindowNativeScope::CountDownWindowNativeScope(CountDownWindowNativeServices& services) noexcept
    : previous_(activeServices), surfaces_(services.surfaces), layers_(services.memory) { activeServices = &services; }
CountDownWindowNativeScope::~CountDownWindowNativeScope() noexcept { activeServices = previous_; }
const LayerItemNativeFrameVtable& CountDownWindowNativeMethods() noexcept { return methods; }
const LayerItemNativeFrameVtable& FiveFrameCountDownWindowNativeMethods() noexcept { return fiveFrameMethods; }
Body* ConstructCountDownWindowNativeAt458210(Body& body, LayerListNativeBody18h* parent,
    CountDownWindowNativeServices& services)
{
    if (services.fiveFrameProfile && services.nativeProfile)
        throw std::invalid_argument("CountDown five-frame and modern native profiles are mutually exclusive");
    const CountDownWindowNativeScope scope(services);
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.boundsAt14 = {};
    body.window.layer.vptrAt00 = services.fiveFrameProfile ? fiveFrameMethods.AsBase() : methods.AsBase();
    body.suffixesAt24 = {}; body.numbersAt30 = {};
    body.window.layer.enabledAt0C = 0;
    try
    {
        if (services.fiveFrameProfile)
        {
            const RECT bounds = LoadSprites(body.numbersAt30, true, 5, services);
            const POINT origin = services.fiveFrameProfile->origin;
            // 10号4666E0..466780：保留五帧各自偏移，统一平移union，不把每帧强塞到同一点。
            for (auto* entry = body.numbersAt30.begin; entry != body.numbersAt30.end; ++entry)
            {
                auto* sprite = ResolveSpriteReference(*entry);
                sprite->xAt2C = Add(sprite->xAt2C, Sub(origin.x, bounds.left));
                sprite->yAt30 = Add(sprite->yAt30, Sub(origin.y, bounds.top));
            }
            body.window.boundsAt14 = MoveTo(bounds, origin);
        }
        else
        {
            const RECT numbers = LoadSprites(body.numbersAt30, true, 60, services);
            const RECT suffixes = LoadSprites(body.suffixesAt24, false, 12, services);
            const RECT positionedNumbers = MoveTo(numbers, Positions(services, true)[2]);
            const RECT positionedSuffixes = MoveTo(suffixes, Positions(services, false)[2]);
            RECT combined{};
            (void)::UnionRect(&combined, &positionedSuffixes, &positionedNumbers);
            body.window.boundsAt14 = {0, 0, Sub(combined.right, combined.left), Sub(combined.bottom, combined.top)};
        }
    }
    catch (...)
    {
        body.window.boundsAt14 = {};
        ClearSprites(body.suffixesAt24);
        ClearSprites(body.numbersAt30);
    }
    body.numberAt3C = -1; body.suffixAt40 = 0;
    return &body;
}
Body* DeleteCountDownWindowNativeAt468770(Body& body, std::uint32_t flags) noexcept
{
    DestroyDrawLayerReferencesAt461750(body.numbersAt30);
    DestroyDrawLayerReferencesAt461750(body.suffixesAt24);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1U) ::operator delete(&body, std::size_t{0x44});
    return &body;
}
RECT* CountDownBoundsNativeAt479CE0(Body& body, RECT* output, CountDownWindowNativeServices& services)
{
    if (FiveFrames(body)) { *output = body.window.boundsAt14; return output; }
    *output = {};
    const auto mode = std::bit_cast<std::int16_t>(services.state.CaptureGlobalInlineAt4180C0()->wordsAt0C[5]);
    if (std::uint32_t(mode) <= 1U) *output = MoveTo(body.window.boundsAt14, Positions(services, false)[std::uint32_t(mode) * 3U + 2U]);
    return output;
}
Slot* CountDownNumberNativeAt47BE60(Body& body, Slot& output, std::int32_t mode, CountDownWindowNativeServices& services)
{ return Select(body, output, mode, true, services); }
Slot* CountDownSuffixNativeAt47C7B0(Body& body, Slot& output, std::int32_t mode, CountDownWindowNativeServices& services)
{ return Select(body, output, mode, false, services); }
std::uint8_t RequestCountDownWindowNativeAt486F70(Body& body, std::uint8_t requested, CountDownWindowNativeServices& services)
{
    if (requested)
    {
        const auto value = services.state.ReadSuffixAfterRootAt486F8C();
        body.numberAt3C = value;
        body.suffixAt40 = FiveFrames(body) ? 0 : static_cast<std::int32_t>(~std::uint32_t(value) >> 31);
        if (body.window.layer.enabledAt0C) return 0;
        body.window.layer.enabledAt0C = 1;
    }
    else
    {
        if (!body.window.layer.enabledAt0C) return 0;
        body.window.layer.enabledAt0C = 0;
    }
    Invalidate(body);
    return 1;
}
void AdvanceCountDownWindowNativeAt46B0D0(Body& body, CountDownWindowNativeServices& services)
{
    if (!body.window.layer.enabledAt0C) return;
    const auto mode = FiveFrames(body) ? 0 : std::bit_cast<std::int16_t>(services.state.CaptureGlobalVia417990()->wordsAt0C[5]);
    if (std::uint32_t(mode) > 1U) return;
    Slot local;
    CountDownNumberNativeAt47BE60(body, local, mode, services);
    Unwind unwind{local};
    if (local.baseAt00 && AdvanceSpriteNativeAt46B1D0(*ResolveSpriteReference(local)) == 0) Invalidate(body);
}
void DrawCountDownWindowNativeAt4827A0(Body& body, const RECT* clip, CountDownWindowNativeServices& services)
{ RenderSprites(body, services, [&](SpriteNativeBodyA0h& sprite) { (void)DrawSpriteNativeAt4768D0(sprite, clip, services.drawing); }); }
void CompositeCountDownWindowNativeAt481830(Body& body, GraphicsSurfaceNativeBody* target,
    const RECT* clip, CountDownWindowNativeServices& services)
{ RenderSprites(body, services, [&](SpriteNativeBodyA0h& sprite) { (void)CompositeSpriteNativeAt46B390(sprite, target, clip, services.drawing); }); }
void ResetCountDownWindowNativeAt47EC20(Body& body) noexcept
{ body.window.layer.enabledAt0C = 0; body.numberAt3C = -1; body.suffixAt40 = 0; }
void ClearCountDownNumberNativeAt46DC00(Body& body) noexcept { body.numberAt3C = -1; }
}
