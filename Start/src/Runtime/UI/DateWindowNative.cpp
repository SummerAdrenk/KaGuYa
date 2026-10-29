#include <Runtime/DateWindowNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Logging/Logger.h>
#include <bit>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct DateWindowStorage final
{
    std::vector<std::unique_ptr<GraphicsSurfaceNativeBody>> images;
    unsigned month = 0, weekday = 0, time = 0;
    DateWindowValues values;
};

namespace
{
using Body = DateWindowNativeBody;
using Base = LayerItemNativeBody14h;
using Strategy = DateWindowStrategy;
constexpr unsigned Day = 1;

void ValidateProfile(const DateWindowResourceProfile& p)
{
    const auto require = [](bool valid) {
        if (!valid) throw std::invalid_argument("DateWindow unsupported resource profile");
    };
    const auto names = [&](std::span<const char* const> group) {
        for (const auto* name : group) require(name && *name);
    };
    require(p.background && *p.background);
    switch (p.strategy)
    {
    case Strategy::EroZemiDigits:
        require(p.digits.size() == 10 && p.months.size() == 12 && p.weekdays.size() == 7
            && p.times.size() == 3 && p.dayPositions.size() == 3 && p.wholeDays.empty());
        names(p.digits);
        break;
    case Strategy::ShigokareWholeDate:
        require(p.digits.empty() && p.months.size() == 13 && p.weekdays.size() == 7
            && p.times.size() == 2 && p.dayPositions.empty() && p.wholeDays.size() == 32);
        require(p.wholeDays[0] == nullptr);
        names(p.wholeDays.subspan(1));
        break;
    case Strategy::LoveHolicDays:
        require(p.digits.empty() && p.months.empty() && p.weekdays.empty()
            && p.times.size() == 2 && p.dayPositions.empty() && p.wholeDays.size() == 60);
        names(p.wholeDays);
        break;
    default:
        require(false);
    }
    // 原PE只提供七/八/九月图片；null是保留的月份槽，不代表加载失败。
    const unsigned firstMonth = p.strategy == Strategy::EroZemiDigits ? 6 : 7;
    for (unsigned i = 0; i < p.months.size(); ++i)
        require(i >= firstMonth && i < firstMonth + 3 ? p.months[i] && *p.months[i] : !p.months[i]);
    names(p.weekdays);
    names(p.times);
}

std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }

void Invalidate(Body& body)
{
    auto* parent = body.window.layer.parentAt10;
    parent->vptrAt00->addDirtyAt20(parent, &body.window.boundsAt14);
}

void ClearStorage(Body& body) noexcept
{
    if (!body.storage) return;
    const GraphicsSurfaceNativeScope scope(body.services->surfaces);
    // 467830逆序退役五组；现代存储仍保证所有已加载Surface在父图层注销前释放。
    for (auto it = body.storage->images.rbegin(); it != body.storage->images.rend(); ++it) it->reset();
    delete body.storage;
    body.storage = nullptr;
}

RECT ResourceBounds(const GraphicsSurfaceNativeBody& surface, bool useOrigin) noexcept
{
    const auto x = useOrigin ? surface.originXAt10 : 0;
    const auto y = useOrigin ? surface.originYAt14 : 0;
    return {x, y, Add(x, surface.widthAt08), Add(y, surface.heightAt0C)};
}

void Load(Body& body, unsigned index, const char* name, bool optional, RECT& bounds, bool useOrigin)
{
    if (!name)
    {
        if (optional) return;
        throw std::invalid_argument("DateWindow required resource name is null");
    }
    auto& services = *body.services;
    auto image = std::make_unique<GraphicsSurfaceNativeBody>(services.surfaces, services.strings);
    if (!services.resources.LoadIntoAt4800A0(services.resources.CaptureLoadReceiverAt413BE0(), *image, name))
        throw 1;
    const auto rect = ResourceBounds(*image, useOrigin);
    (void)::UnionRect(&bounds, &bounds, &rect);
    body.storage->images[index] = std::move(image);
}

template<class Paint>
void PaintDate(Body& body, Paint&& paint)
{
    const auto& values = body.storage->values;
    if (values.display < 0) return;
    const auto draw = [&](unsigned index, const POINT* overridePosition = nullptr) {
        const auto* image = body.storage->images[index].get();
        if (!image || !image->handleAt04) return;
        const POINT position = overridePosition ? *overridePosition : POINT{image->originXAt10, image->originYAt14};
        paint(*image, position);
    };
    const auto& profile = body.services->profile;
    const auto& storage = *body.storage;
    // 05 467490/467670; 07 467D30/468020; 08 467700/4678E0.
    draw(0);
    if (profile.strategy == Strategy::LoveHolicDays)
    {
        draw(Day + std::uint32_t(values.days));
    }
    else
    {
        const bool digits = profile.strategy == Strategy::EroZemiDigits;
        draw(storage.month + std::uint32_t(values.month - (digits ? 1 : 0)));
        if (!digits) draw(Day + std::uint32_t(values.date));
        else if (values.date >= 10)
        {
            draw(Day + std::uint32_t(values.date % 10), &profile.dayPositions[1]);
            draw(Day + std::uint32_t(values.date / 10), &profile.dayPositions[2]);
        }
        else draw(Day + std::uint32_t(values.date), &profile.dayPositions[0]);
        draw(storage.weekday + std::uint32_t(values.weekday));
    }
    draw(storage.time + std::uint32_t(values.time));
}

void __fastcall Draw(Base* self, void*, const RECT* clip)
{ DrawDateWindowNative(*reinterpret_cast<Body*>(self), clip); }
void __fastcall Composite(Base* self, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{ CompositeDateWindowNative(*reinterpret_cast<Body*>(self), target, clip); }
Base* __fastcall Delete(Base* self, void*, std::uint32_t flags)
{ return reinterpret_cast<Base*>(DeleteDateWindowNative(*reinterpret_cast<Body*>(self), flags)); }
RECT* __fastcall Bounds(Base* self, void*, RECT* out)
{ *out = reinterpret_cast<Body*>(self)->window.boundsAt14; return out; }
std::uint8_t __fastcall Request(Base* self, void*, std::uint8_t requested, std::int32_t mode)
{ return RequestDateWindowNative(*reinterpret_cast<Body*>(self), requested, mode); }
void __fastcall Advance(Base*, void*) {} // 原vtable指向4AE170 nullsub_1。
void __fastcall Reset(Base* self, void*)
{ ResetDateWindowNative(*reinterpret_cast<Body*>(self)); }
const LayerItemNativeFrameVtable methods{
    {reinterpret_cast<decltype(LayerItemNativeVtable::drawAt00)>(&Draw),
     reinterpret_cast<decltype(LayerItemNativeVtable::compositeAt04)>(&Composite),
     reinterpret_cast<decltype(LayerItemNativeVtable::deletingDestructorAt08)>(&Delete)},
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::boundsAt0C)>(&Bounds),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::requestAt10)>(&Request),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::advanceAt14)>(&Advance),
    reinterpret_cast<decltype(LayerItemNativeFrameVtable::resetAt18)>(&Reset)};
}

Body* ConstructDateWindowNative(Body& body, LayerListNativeBody18h* parent, DateWindowNativeServices& services)
{
    const GraphicsSurfaceNativeScope graphics(services.surfaces);
    body.services = &services;
    body.storage = nullptr;
    ConstructLayerItemNativeAt441270(body.window.layer, parent);
    body.window.layer.vptrAt00 = methods.AsBase();
    body.window.layer.enabledAt0C = 0;
    body.window.boundsAt14 = {};
    try
    {
        const auto& profile = services.profile;
        ValidateProfile(profile);
        body.storage = new DateWindowStorage;
        auto& storage = *body.storage;
        storage.month = Day + static_cast<unsigned>(profile.digits.size() + profile.wholeDays.size());
        storage.weekday = storage.month + static_cast<unsigned>(profile.months.size());
        storage.time = storage.weekday + static_cast<unsigned>(profile.weekdays.size());
        storage.images.resize(storage.time + profile.times.size());
        auto& bounds = body.window.boundsAt14;
        Load(body, 0, profile.background, false, bounds, true);
        RECT digits{};
        for (unsigned i = 0; i < profile.digits.size(); ++i) Load(body, Day + i, profile.digits[i], false, digits, false);
        for (const auto position : profile.dayPositions)
        {
            RECT rect{Add(digits.left, position.x), Add(digits.top, position.y),
                Add(digits.right, position.x), Add(digits.bottom, position.y)};
            (void)::UnionRect(&bounds, &bounds, &rect);
        }
        RECT days{};
        for (unsigned i = 0; i < profile.wholeDays.size(); ++i)
            Load(body, Day + i, profile.wholeDays[i],
                profile.strategy == Strategy::ShigokareWholeDate && i == 0, days,
                profile.strategy == Strategy::ShigokareWholeDate);
        // 07的467185计算整日矩形，但467208..467211清空后未合入总范围。
        // 08的467059/4670B0只合入零原点尺寸；实际绘图仍使用资源原点。
        if (profile.strategy == Strategy::LoveHolicDays) (void)::UnionRect(&bounds, &bounds, &days);
        for (unsigned i = 0; i < profile.months.size(); ++i) Load(body, storage.month + i, profile.months[i], true, bounds, true);
        for (unsigned i = 0; i < profile.weekdays.size(); ++i) Load(body, storage.weekday + i, profile.weekdays[i], false, bounds, true);
        for (unsigned i = 0; i < profile.times.size(); ++i) Load(body, storage.time + i, profile.times[i], false, bounds, true);
    }
    catch (...)
    {
        ClearStorage(body);
        DestroyLayerWindowNativeAt464910(body.window);
        throw;
    }
    return &body;
}

Body* DeleteDateWindowNative(Body& body, std::uint32_t flags) noexcept
{
    ClearStorage(body);
    DestroyLayerWindowNativeAt464910(body.window);
    if (flags & 1u) ::operator delete(&body, sizeof(Body));
    return &body;
}

const DateWindowValues& ReadDateWindowValues(const Body& body) noexcept { return body.storage->values; }

void UpdateDateWindowNative(Body& body)
{
    auto& current = body.storage->values;
    auto& source = body.services->state;
    const auto clear = [&] { current.display = -1; Invalidate(body); };
    const auto strategy = body.services->profile.strategy;
    std::int32_t month = -1, date = -1, weekday = -1, days = -1;
    // 05的4679B0与07的468450逐字段验证/早退；08的467BD0先读取days。
    if (strategy == Strategy::LoveHolicDays) days = source.ReadDateValue("days");
    else
    {
        month = source.ReadDateValue("month");
        if (std::uint32_t(month) - 1u > 11u) { clear(); return; }
        date = source.ReadDateValue("date");
        if (std::uint32_t(date) - 1u > 30u) { clear(); return; }
        weekday = source.ReadDateValue("day");
        if (std::uint32_t(weekday) > 6u) { clear(); return; }
    }
    const auto time = source.ReadDateValue("time");
    if (std::uint32_t(time) >= (strategy == Strategy::EroZemiDigits ? 3u : 2u)) { clear(); return; }
    const auto display = source.ReadDateValue("date_win");
    if (display < 0)
    {
        if (current.display >= 0) clear();
        return;
    }
    if (current.display == display && current.month == month && current.date == date
        && current.weekday == weekday && current.time == time && current.days == days) return;
    if (display != 0) { clear(); return; }
    if (strategy == Strategy::LoveHolicDays && std::uint32_t(days) >= 60u)
    {
        // 原467BD0未检查范围，随后4677BF直接索引；原另一合成入口
        // 4F74C0..4F74DC明确限定0..59，不对days减一。
        char message[160];
        std::snprintf(message, sizeof(message),
            "DateWindow LoveHolic rejected unsafe days=%d time=%d date_win=%d (expected days 0..59)\n",
            days, time, display);
        ::OutputDebugStringA(message);
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error, message);
        clear();
        throw std::out_of_range(message);
    }
    current = {display, month, date, weekday, time, days};
    Invalidate(body);
}

std::uint8_t RequestDateWindowNative(Body& body, std::uint8_t requested, std::int32_t mode)
{
    // 467BD0：关闭mode=1时，非-1的date_win保留原显示；其它非零mode直接保留。
    if (!requested && mode != 0)
    {
        if (mode != 1 || body.services->state.ReadDateValue("date_win") != -1) return 0;
    }
    if (requested) UpdateDateWindowNative(body);
    const auto enabled = static_cast<std::uint8_t>(requested != 0);
    if (body.window.layer.enabledAt0C == enabled) return 0;
    body.window.layer.enabledAt0C = enabled;
    Invalidate(body);
    return 1;
}

void DrawDateWindowNative(Body& body, const RECT* clip)
{
    PaintDate(body, [&](const auto& image, POINT position) {
        (void)body.services->drawing.SurfaceDrawRaw(image.handleAt04, position.x, position.y, nullptr, nullptr, clip, 0);
    });
}
void CompositeDateWindowNative(Body& body, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    if (!target || !target->handleAt04) return;
    PaintDate(body, [&](const auto& image, POINT position) {
        (void)body.services->drawing.SurfaceBlitRaw(target->handleAt04, image.handleAt04,
            position.x, position.y, nullptr, nullptr, clip, 0);
    });
}
void ResetDateWindowNative(Body& body) noexcept
{
    body.storage->values = {};
    body.window.layer.enabledAt0C = 0;
}
}
