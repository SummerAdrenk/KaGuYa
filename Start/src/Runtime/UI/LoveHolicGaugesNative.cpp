#include <Runtime/LoveHolicGaugesNative.h>
#include <Runtime/LayerListNative.h>
#include <algorithm>
#include <bit>
#include <memory>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class LoveHolicGaugeKind { Percentage, Maso };
struct LoveHolicGaugeStorage final
{
    LoveHolicGaugeKind kind;
    std::int32_t point = 0; // 原467E60/468C70构造均置0，局部reset则不同。
    std::vector<std::unique_ptr<GraphicsSurfaceNativeBody>> images;
};

namespace
{
using Body = LoveHolicGaugeNativeBody;
using Base = LayerItemNativeBody14h;
using Kind = LoveHolicGaugeKind;
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
bool Percentage(const Body& b) noexcept { return b.storage->kind == Kind::Percentage; }

void Release(Body& body) noexcept
{
    if (!body.storage) return;
    const GraphicsSurfaceNativeScope scope(body.services->surfaces);
    for (auto i = body.storage->images.rbegin(); i != body.storage->images.rend(); ++i) i->reset();
    delete body.storage;
    body.storage = nullptr;
}

void Load(Body& body, const char* name)
{
    if (!name || !*name) throw std::invalid_argument("LoveHolic Gauge resource name is empty");
    auto& services = *body.services;
    auto image = std::make_unique<GraphicsSurfaceNativeBody>(services.surfaces, services.strings);
    // 原4310D0和Maso直调462EC0最终使用同一静态Surface loader。
    if (!services.resources.LoadIntoAt4800A0(services.resources.CaptureLoadReceiverAt413BE0(), *image, name))
        throw 1;
    body.storage->images.push_back(std::move(image));
}

template<class Paint>
void PaintPercentage(Body& body, Paint&& paint)
{
    const auto& images = body.storage->images;
    const auto x = std::bit_cast<std::int32_t>(body.window.layer.valueAt04);
    const auto y = std::bit_cast<std::int32_t>(body.window.layer.valueAt08);
    const auto origin = [&](const auto& image, const RECT* source = nullptr) {
        paint(image, POINT{Add(x,image.originXAt10),Add(y,image.originYAt14)}, source);
    };
    // 4681A0/4684A0：reset=-1仍画框，只有0..100画条和数字。
    origin(*images[0]);
    const auto point = body.storage->point;
    if (std::uint32_t(point) > 100u) return;
    if (point != 0)
    {
        const auto& bar = *images[1];
        // 原imul保留低32位后有符号除100；明确保留该算术，不使用浮点舍入。
        const auto product = std::bit_cast<std::int32_t>(std::uint32_t(bar.widthAt08) * std::uint32_t(point));
        const RECT source{0,0,product / 100,bar.heightAt0C};
        origin(bar, &source);
    }
    auto position = body.services->profile.percentage.unitsPosition;
    const auto digit = [&](int value) {
        const auto& image = *images[2 + value];
        paint(image, POINT{Add(x,position.x),Add(y,position.y)}, nullptr);
        position.x = Sub(position.x,image.widthAt08);
    };
    // 不是等宽字体：下一位减前一张图片的宽，所有数字忽略资源origin。
    digit(point % 10);
    if (point >= 10) digit(point / 10 % 10);
    if (point == 100) digit(1);
}

template<class Paint>
void PaintMaso(Body& body, Paint&& paint)
{
    const auto& images = body.storage->images;
    const auto& background = *images[0];
    paint(background, POINT{background.originXAt10,background.originYAt14}, nullptr);
    // 468F80/469250：固定屏幕坐标，无图层偏移；on相对表坐标(-1,-21)。
    for (unsigned i = 0; i < 5; ++i)
    {
        const bool on = i < std::uint32_t(body.storage->point);
        auto position = body.services->profile.maso.positions[i];
        if (on) { position.x = Sub(position.x,1); position.y = Sub(position.y,21); }
        paint(*images[on ? 2 : 1], position, nullptr);
    }
}

void __fastcall Draw(Base* self, void*, const RECT* clip)
{ DrawLoveHolicGaugeNative(*reinterpret_cast<Body*>(self), clip); }
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ CompositeLoveHolicGaugeNative(*reinterpret_cast<Body*>(self), target, clip); }
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
{ return reinterpret_cast<Base*>(DeleteLoveHolicGaugeNative(*reinterpret_cast<Body*>(self), flags)); }
RECT* __fastcall Bounds(Base* self, void*, RECT* out)
{ *out = ReadLoveHolicGaugeBounds(*reinterpret_cast<Body*>(self)); return out; }
std::uint8_t __fastcall Request(Base* self, void*, std::uint8_t requested, std::int32_t mode)
{ return RequestLoveHolicGaugeNative(*reinterpret_cast<Body*>(self), requested, mode); }
void __fastcall Advance(Base*, void*) {} // 两个原+14均为5069F0 nullsub_1，不逐帧读脚本。
void __fastcall Reset(Base* self, void*)
{ ResetLoveHolicGaugeNative(*reinterpret_cast<Body*>(self)); }
void __fastcall Clear(Base* self, void*)
{ ClearLoveHolicGaugeValueNative(*reinterpret_cast<Body*>(self)); }
LoveHolicGaugeNativeVtable MakeMethods()
{
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
    return {{{reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
               reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
               reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
              reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
              reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
              reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
              reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)},
             reinterpret_cast<decltype(LoveHolicGaugeNativeVtable::clearValueAt1C)>(&Clear)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
const LoveHolicGaugeNativeVtable percentageMethods = MakeMethods(), masoMethods = MakeMethods();

Body* Construct(Body& body, LayerListNativeBody18h* parent, LoveHolicGaugesNativeServices& services, Kind kind)
{
    const GraphicsSurfaceNativeScope scope(services.surfaces);
    body.services = &services;
    body.storage = nullptr;
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.layer.vptrAt00 = (kind == Kind::Percentage ? percentageMethods : masoMethods).frame.AsBase();
    body.window.layer.enabledAt0C = 0;
    body.window.boundsAt14 = {};
    try
    {
        body.storage = new LoveHolicGaugeStorage{kind,0,{}};
        body.storage->images.reserve(kind == Kind::Percentage ? 12 : 3);
        Load(body,kind == Kind::Percentage ? services.profile.percentage.background : services.profile.maso.background);
        // 原两ctor都仅取背景边界，不将条、数字或五枚图标Union进去。
        const auto& image = *body.storage->images[0];
        body.window.boundsAt14 = {image.originXAt10,image.originYAt14,
            Add(image.originXAt10,image.widthAt08),Add(image.originYAt14,image.heightAt0C)};
        if (kind == Kind::Percentage)
        {
            const auto& profile = services.profile.percentage;
            Load(body,profile.bar);
            for (auto* name : profile.digits) Load(body,name);
        }
        else
        {
            const auto& profile = services.profile.maso;
            if (!profile.valueName || !*profile.valueName || !profile.displayName || !*profile.displayName)
                throw std::invalid_argument("LoveHolic Maso script name is empty");
            Load(body,profile.off);
            Load(body,profile.on);
        }
    }
    catch (...)
    {
        Release(body);
        DestroyLayerWindowNativeAt464910(body.window);
        throw;
    }
    return &body;
}
}

Body* ConstructLoveHolicPercentageGaugeNative(Body& b, LayerListNativeBody18h* p, LoveHolicGaugesNativeServices& s)
{ return Construct(b,p,s,Kind::Percentage); }
Body* ConstructLoveHolicMasoGaugeNative(Body& b, LayerListNativeBody18h* p, LoveHolicGaugesNativeServices& s)
{ return Construct(b,p,s,Kind::Maso); }
Body* DeleteLoveHolicGaugeNative(Body& body, std::uint32_t flags) noexcept
{
    Release(body);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1u) ::operator delete(&body,sizeof(Body));
    return &body;
}
RECT ReadLoveHolicGaugeBounds(const Body& body) noexcept
{
    auto result = body.window.boundsAt14;
    if (Percentage(body))
        ::OffsetRect(&result,std::bit_cast<std::int32_t>(body.window.layer.valueAt04),
            std::bit_cast<std::int32_t>(body.window.layer.valueAt08));
    return result;
}
std::int32_t ReadLoveHolicGaugePoint(const Body& body) noexcept { return body.storage->point; }
void InvalidateLoveHolicGaugeNative(Body& body)
{
    const auto bounds = ReadLoveHolicGaugeBounds(body);
    auto* parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent,&bounds);
}
void UpdateLoveHolicGaugeNative(Body& body)
{
    auto& source = body.services->state;
    auto& point = body.storage->point;
    if (Percentage(body))
    {
        const auto x = source.ReadGaugeValue("gague_x");
        const auto y = source.ReadGaugeValue("gague_y");
        const auto bounds = ReadLoveHolicGaugeBounds(body);
        // 4689FF比较的是偏移后矩形左上角，不是缓存的x/y；背景top=70时重复dirty属原行为。
        if (bounds.left != x || bounds.top != y)
        {
            InvalidateLoveHolicGaugeNative(body);
            body.window.layer.valueAt04 = std::uint32_t(x);
            body.window.layer.valueAt08 = std::uint32_t(y);
            InvalidateLoveHolicGaugeNative(body);
        }
        const auto value = std::clamp(source.ReadGaugeValue("gague_point"),0,100);
        if (value == point) return;
        point = value;
    }
    else
    {
        const auto value = source.ReadGaugeValue(body.services->profile.maso.valueName);
        // 46967D的无符号jbe：负数和>5均清为0，每次都dirty，不饱和到5。
        if (std::uint32_t(value) > 5u) ClearLoveHolicGaugeValueNative(body);
        else
        {
            if (value == point) return;
            point = value;
        }
    }
    InvalidateLoveHolicGaugeNative(body);
}
std::uint8_t RequestLoveHolicGaugeNative(Body& body, std::uint8_t requested, std::int32_t)
{
    // 468B10/4696C0分支等价，第二参数都未使用；hide不是无条件隐藏。
    if (requested) UpdateLoveHolicGaugeNative(body);
    auto& enabled = body.window.layer.enabledAt0C;
    if ((enabled != 0) == (requested != 0)) return 0;
    const auto* name = Percentage(body) ? "gague_disp" : body.services->profile.maso.displayName;
    const bool show = body.services->state.ReadGaugeValue(name) != 0;
    if (show != (requested != 0)) return 0;
    enabled = show ? 1 : 0;
    InvalidateLoveHolicGaugeNative(body);
    return 1;
}
void DrawLoveHolicGaugeNative(Body& body, const RECT* clip)
{
    const auto paint = [&](const auto& image, POINT position, const RECT* source) {
        if (image.handleAt04)
            (void)body.services->drawing.SurfaceDrawRaw(image.handleAt04,position.x,position.y,nullptr,source,clip,0);
    };
    if (Percentage(body)) PaintPercentage(body,paint); else PaintMaso(body,paint);
}
void CompositeLoveHolicGaugeNative(Body& body, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    if (!target || !target->handleAt04) return;
    const auto paint = [&](const auto& image, POINT position, const RECT* source) {
        if (image.handleAt04)
            (void)body.services->drawing.SurfaceBlitRaw(target->handleAt04,image.handleAt04,
                position.x,position.y,nullptr,source,clip,0);
    };
    if (Percentage(body)) PaintPercentage(body,paint); else PaintMaso(body,paint);
}
void ClearLoveHolicGaugeValueNative(Body& body) noexcept
{ body.storage->point = Percentage(body) ? -1 : 0; }
void ResetLoveHolicGaugeNative(Body& body) noexcept
{
    // 468F70(+18)调用局部+1C后仅隐藏；不清位置、不dirty、不重载资源。
    ClearLoveHolicGaugeValueNative(body);
    body.window.layer.enabledAt0C = 0;
}
}
