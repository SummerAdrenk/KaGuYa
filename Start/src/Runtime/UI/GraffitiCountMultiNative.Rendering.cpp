#include "GraffitiCountMultiNative.Internal.h"
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace GraffitiCountMultiDetail;
void RefreshGraffitiCountMultiNative(Multi& body)
{
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.common.canvasAt6C);
    auto& graphics = RequireGuiControlGraphicsSource().CompositionAt4182F0();
    if (canvas->handleAt04) graphics.ClearSurface(canvas->handleAt04, canvas->clearColorAt20, nullptr);
    const auto blit = [&](const GraphicsSurfaceReference& source, POINT p, const RECT* clip = nullptr)
    {
        if (canvas->handleAt04 && source->handleAt04)
            static_cast<void>(graphics.BlitSurface(canvas->handleAt04, source->handleAt04,
                Sub(p.x, body.layoutOrigin.x), Sub(p.y, body.layoutOrigin.y), nullptr, nullptr, clip, 0));
    };
    const auto digit = [&](std::int32_t index, POINT p)
    {
        if (index < 0 || static_cast<std::size_t>(index) >= body.digits.size())
            throw std::out_of_range("Count: native negative digit index is unsafe (no lower clamp)");
        blit(body.digits[index], p);
    };
    const auto offset = body.data->layoutOffsets[body.rows.size()];
    for (std::size_t i = 0; i != body.rows.size(); ++i)
    {
        const auto& row = body.rows[i];
        const auto& layout = body.data->layouts[offset + i];
        const auto frame = body.data->variables[row.selector].frameIndex;
        blit(body.gauges[frame], layout.gauge);
        const auto& fill = body.gauges[frame + 1];
        const auto pixels = Add(std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(row.current) * static_cast<std::uint32_t>(row.pixelsPerValue)), row.pixelProgress);
        const POINT fillAt{Add(layout.gauge.x, 2), Add(layout.gauge.y, 2)};
        if (pixels == fill->heightAt0C) blit(fill, fillAt);
        else if (pixels != 0)
        {
            // 16:4C2000传目标裁剪框，不缩放、不裁源图。
            const auto x = Sub(fillAt.x, body.layoutOrigin.x);
            const auto y = Add(Sub(fillAt.y, body.layoutOrigin.y), Sub(fill->heightAt0C, pixels));
            RECT clip{x, y, Add(x, fill->widthAt08), Add(y, fill->heightAt0C)};
            blit(fill, fillAt, &clip);
        }
        if (row.current < 10) digit(row.current % 10, layout.singleDigit);
        else
        {
            digit(row.current % 10, layout.ones);
            digit((row.current / 10) % 10, layout.tens);
        }
        if (row.current == row.upperLimit) blit(body.icons[1], layout.maximum);
        else if (row.current != row.target) blit(body.icons[0], layout.up);
    }
}
}
