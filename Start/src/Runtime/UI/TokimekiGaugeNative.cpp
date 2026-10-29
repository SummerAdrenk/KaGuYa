#include <Runtime/TokimekiGaugeNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Logging/Logger.h>
#include <array>
#include <bit>
#include <cstdio>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
struct TokimekiGaugeStorage final
{
    std::unique_ptr<GraphicsSurfaceNativeBody> background;
    std::array<std::unique_ptr<GraphicsSurfaceNativeBody>, 50> frames;
    std::int32_t frame = 50;
};

namespace
{
using Body = TokimekiGaugeNativeBody;
using Base = LayerItemNativeBody14h;
constexpr std::int32_t FrameCount = 50;

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }

void ReleaseStorage(Body& body) noexcept
{
    if (!body.storage) return;
    const GraphicsSurfaceNativeScope graphics(body.services->surfaces);
    // 468D20/4D8E30：先退役计量帧，再退役底图，最后由共享基类注销图层。
    for (auto& image : body.storage->frames) image.reset();
    body.storage->background.reset();
    delete body.storage;
    body.storage = nullptr;
}

std::unique_ptr<GraphicsSurfaceNativeBody> Load(Body& body, const char* name)
{
    auto& s = *body.services;
    auto image = std::make_unique<GraphicsSurfaceNativeBody>(s.surfaces, s.strings);
    if (!s.resources.LoadIntoAt4800A0(s.resources.CaptureLoadReceiverAt413BE0(), *image, name))
        throw 1; // 4687DE/46894D：不能将false当成加载成功；服务抛出的异常原样传播。
    return image;
}

RECT ImageBounds(const GraphicsSurfaceNativeBody& image) noexcept
{
    return {image.originXAt10, image.originYAt14,
        Add(image.originXAt10, image.widthAt08), Add(image.originYAt14, image.heightAt0C)};
}

template<class Paint>
void PaintGauge(Body& body, Paint&& paint)
{
    const auto& layer = body.window.layer;
    const auto x = std::bit_cast<std::int32_t>(layer.valueAt04);
    const auto y = std::bit_cast<std::int32_t>(layer.valueAt08);
    const auto& storage = *body.storage;
    // 468AE0/468BF0：底图不加资源原点；计量帧才加原点。启用门控由共享父层负责。
    if (storage.background->handleAt04) paint(*storage.background, x, y);
    if (storage.frame == FrameCount) return;
    const auto& image = *storage.frames[static_cast<unsigned>(storage.frame)];
    if (image.handleAt04) paint(image, Add(x, image.originXAt10), Add(y, image.originYAt14));
}

void __fastcall Draw(Base* self, void*, const RECT* clip)
{ DrawTokimekiGaugeNative(*reinterpret_cast<Body*>(self), clip); }
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ CompositeTokimekiGaugeNative(*reinterpret_cast<Body*>(self), target, clip); }
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
{ return reinterpret_cast<Base*>(DeleteTokimekiGaugeNative(*reinterpret_cast<Body*>(self), flags)); }
RECT* __fastcall Bounds(Base* self, void*, RECT* out)
{ *out = ReadTokimekiGaugeBounds(*reinterpret_cast<Body*>(self)); return out; }
std::uint8_t __fastcall Request(Base* self, void*, std::uint8_t requested, std::int32_t mode)
{ return SetRequestedTokimekiGaugeNative(*reinterpret_cast<Body*>(self), requested, mode); }
void __fastcall Advance(Base*, void*) {} // 原+14槽4DEDF0为nullsub，不能在此擅自轮询变量。
void __fastcall Reset(Base* self, void*)
{ ResetTokimekiGaugeNative(*reinterpret_cast<Body*>(self)); }
std::int32_t __fastcall Clear(Base* self, void*)
{ return ClearTokimekiGaugeNative(*reinterpret_cast<Body*>(self)); }
const TokimekiGaugeNativeVtable methods{
    {{reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
      reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
      reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
     reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)},
    reinterpret_cast<decltype(TokimekiGaugeNativeVtable::clearAt1C)>(&Clear)};
}

const TokimekiGaugeNativeVtable& TokimekiGaugeNativeMethods() noexcept { return methods; }

Body* ConstructTokimekiGaugeNative(Body& body, LayerListNativeBody18h* parent,
    TokimekiGaugeNativeServices& services)
{
    const GraphicsSurfaceNativeScope graphics(services.surfaces);
    body.services = &services;
    body.storage = nullptr;
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.layer.vptrAt00 = methods.frame.AsBase();
    body.window.layer.enabledAt0C = 0;
    body.window.boundsAt14 = {};
    try
    {
        const auto& profile = services.profile;
        const auto requireName = [](const char* name) {
            if (!name || !*name) throw std::invalid_argument("TokimekiGauge missing resource name");
        };
        requireName(profile.background);
        for (const auto* name : profile.frames) requireName(name);
        body.storage = new TokimekiGaugeStorage;
        auto& storage = *body.storage;
        storage.background = Load(body, profile.background);
        body.window.boundsAt14 = ImageBounds(*storage.background);
        for (unsigned i = 0; i < profile.frames.size(); ++i)
        {
            storage.frames[i] = Load(body, profile.frames[i]);
            const auto bounds = ImageBounds(*storage.frames[i]);
            (void)::UnionRect(&body.window.boundsAt14, &body.window.boundsAt14, &bounds);
        }
    }
    catch (...)
    {
        ReleaseStorage(body);
        DestroyLayerWindowNativeAt464910(body.window);
        throw;
    }
    return &body;
}

Body* DeleteTokimekiGaugeNative(Body& body, std::uint32_t flags) noexcept
{
    ReleaseStorage(body);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1u) ::operator delete(&body, sizeof(Body));
    return &body;
}

RECT ReadTokimekiGaugeBounds(const Body& body) noexcept
{
    auto bounds = body.window.boundsAt14;
    (void)::OffsetRect(&bounds, std::bit_cast<std::int32_t>(body.window.layer.valueAt04),
        std::bit_cast<std::int32_t>(body.window.layer.valueAt08));
    return bounds;
}
std::int32_t ReadTokimekiGaugeFrame(const Body& body) noexcept { return body.storage->frame; }

void InvalidateTokimekiGaugeNative(Body& body)
{
    // 原4C33A0；Program18在request之后无条件调用，不等同于request的changed返回值。
    const auto bounds = ReadTokimekiGaugeBounds(body);
    auto* parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &bounds);
}

void UpdateTokimekiGaugeNative(Body& body)
{
    auto& source = body.services->state;
    const auto x = source.ReadGaugeValue("gague_x");
    const auto y = source.ReadGaugeValue("gague_y");
    const auto old = ReadTokimekiGaugeBounds(body);
    if (old.left != x || old.top != y)
    {
        // 468F59..468F8D：旧区域先标脏，再提交坐标并标新区域；之后才读point。
        InvalidateTokimekiGaugeNative(body);
        body.window.layer.valueAt04 = std::uint32_t(x);
        body.window.layer.valueAt08 = std::uint32_t(y);
        InvalidateTokimekiGaugeNative(body);
    }
    const auto point = source.ReadGaugeValue("gague_point");
    const auto frame = point / 2; // 469017..469036：有符号除法，向零截断；-1仍是第0帧。
    if (frame < 0)
    {
        // 原负索引直接越界；明确拒绝且清帧，不钳位、不复制内存破坏。
        char message[160];
        std::snprintf(message, sizeof(message),
            "TokimekiGauge rejected unsafe gague_point=%d (frame=%d)\n", point, frame);
        ::OutputDebugStringA(message);
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error, message);
        ClearTokimekiGaugeNative(body);
        InvalidateTokimekiGaugeNative(body);
        throw std::out_of_range(message);
    }
    if (frame >= FrameCount) ClearTokimekiGaugeNative(body);
    else
    {
        if (body.storage->frame == frame) return;
        body.storage->frame = frame;
    }
    // 超过末帧时每次都标脏；只清前景，底图和enabled不变。
    InvalidateTokimekiGaugeNative(body);
}

std::uint8_t SetRequestedTokimekiGaugeNative(Body& body, std::uint8_t requested, std::int32_t)
{
    // 469080：mode未读；show先Update，已启用时不再读disp。hide仅在disp==0时成立。
    auto& enabled = body.window.layer.enabledAt0C;
    if (requested)
    {
        UpdateTokimekiGaugeNative(body);
        if (enabled || body.services->state.ReadGaugeValue("gague_disp") == 0) return 0;
        enabled = 1;
    }
    else
    {
        if (!enabled || body.services->state.ReadGaugeValue("gague_disp") != 0) return 0;
        enabled = 0;
    }
    InvalidateTokimekiGaugeNative(body);
    return 1;
}

void DrawTokimekiGaugeNative(Body& body, const RECT* clip)
{
    PaintGauge(body, [&](const auto& image, std::int32_t x, std::int32_t y) {
        (void)body.services->drawing.SurfaceDrawRaw(image.handleAt04, x, y, nullptr, nullptr, clip, 0);
    });
}
void CompositeTokimekiGaugeNative(Body& body, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    if (!target || !target->handleAt04) return;
    PaintGauge(body, [&](const auto& image, std::int32_t x, std::int32_t y) {
        (void)body.services->drawing.SurfaceBlitRaw(target->handleAt04, image.handleAt04,
            x, y, nullptr, nullptr, clip, 0);
    });
}
std::int32_t ClearTokimekiGaugeNative(Body& body) noexcept
{ return body.storage->frame = FrameCount; }
void ResetTokimekiGaugeNative(Body& body) noexcept
{
    ClearTokimekiGaugeNative(body);
    body.window.layer.enabledAt0C = 0; // 45BE40：不清位置、不标脏、不释放资源。
}
}
