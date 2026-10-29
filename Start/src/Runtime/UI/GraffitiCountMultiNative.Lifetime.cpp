#include "GraffitiCountMultiNative.Internal.h"
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiCountMultiDetail;
namespace
{
void Load(std::vector<GraphicsSurfaceReference>& out, std::span<const char* const> names)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    for (const char* name : names)
    {
        GraphicsSurfaceReference surface{s.surfaces};
        surface.ConstructAt40C2E0();
        if (!s.loader.LoadIntoAt4800A0(s.loader.CaptureLoadReceiverAt413BE0(), *surface, name)) throw 1;
        out.push_back(std::move(surface));
    }
}
RECT Bounds(const Multi& body)
{
    LONG width = 0, height = 0;
    for (const auto& surface : body.gauges)
    {
        width = (std::max)(width, static_cast<LONG>(surface->widthAt08));
        height = (std::max)(height, static_cast<LONG>(surface->heightAt0C));
    }
    RECT combined{};
    // 16:4F2370始终按三组计量条/图标位置确定画布大小。
    const auto first = body.data->layoutOffsets[3];
    for (std::size_t i = first; i != first + 3; ++i)
    {
        const auto& row = body.data->layouts[i];
        RECT r{row.gauge.x, row.gauge.y, Add(row.gauge.x, width), Add(row.gauge.y, height)};
        ::UnionRect(&combined, &combined, &r);
        const POINT positions[]{row.up, row.maximum};
        for (std::size_t icon = 0; icon != 2; ++icon)
        {
            const auto p = positions[icon];
            r = {p.x, p.y, Add(p.x, body.icons[icon]->widthAt08), Add(p.y, body.icons[icon]->heightAt0C)};
            ::UnionRect(&combined, &combined, &r);
        }
    }
    return combined;
}
}
const GuiDialogNativeVtable35& GraffitiCountMultiNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = GraffitiCountDialogBaseVtable();
#define BIND(slot, fn) t.slot = reinterpret_cast<decltype(t.slot)>(&GraffitiCountMultiDetail::fn)
        BIND(deletingDestructorAt04, Delete);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(endAt4C, End);
        BIND(processInputAt54, Input);
#undef BIND
        return t;
    }();
    return table;
}
GuiDialogNativeBody5Ch* NewGraffitiCountMultiNative(GraffitiCountDialogNativeServices& s)
{
    if (!s.resources) throw std::logic_error("Count: game profile is not bound");
    ValidateCountResourceProfile(*s.resources);
    if (s.resources->kind != CountResourceKind::MultiGauge)
        throw std::invalid_argument("Count: multi-gauge constructor requires its own profile");
    GraffitiCountDialogNativeScope scope{s};
    auto body = std::make_unique<Multi>();
    ConstructCountPrefix(body->common, false);
    try
    {
        body->common.dialog.vptrAt00 = &GraffitiCountMultiNativeVtable();
        body->data = s.resources->multi;
        Load(body->gauges, body->data->gauges);
        Load(body->icons, body->data->icons);
        Load(body->digits, body->data->digits);
        auto bounds = Bounds(*body);
        body->layoutOrigin = {bounds.left, bounds.top};
        body->canvasSize = {Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top)};
        ConstructCountWidgets(body->common, bounds);
        body->common.rectangleAt5C = bounds;
        AdoptGuiInputAt536EA0(body->common.dialog.inputAt18, NewGameGuiInputAt4EAAED());
    }
    catch (...)
    {
        DestroyGraffitiCountDialogBaseAt4F7A40(body->common);
        throw;
    }
    return &body.release()->common.dialog;
}
namespace GraffitiCountMultiDetail
{
Dialog* __fastcall Delete(Dialog* dialog, void*, std::uint32_t flags) noexcept
{
    auto* body = reinterpret_cast<Multi*>(dialog);
    DestroyGraffitiCountDialogBaseAt4F7A40(body->common);
    body->~Multi();
    if (flags & 1u) ::operator delete(body, sizeof(Multi));
    return dialog;
}
}
}
