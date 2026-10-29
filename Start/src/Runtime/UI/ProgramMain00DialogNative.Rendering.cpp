#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
void AppendLoaded(GuiControlSurfaceReferences& v, const char* name)
{
    Slot temporary;
    RefFrame frame{temporary};
    LoadFresh(temporary, name);
    auto* control = Read(temporary.controlAt08);
    if (Read(v.end) == Read(v.capacity))
    {
        GrowGuiControlSurfaceReferencesAt4540A0(v, Read(v.end), temporary);
        control = Read(temporary.controlAt08);
    }
    else AppendGuiControlSurfaceReference(v, temporary);
    frame.Captured(control);
}
}
void RenderProgramMain00DialogNativeAt5279C0(ProgramMain00DialogNativeBodyC0h& b,
    std::int32_t character, std::uint32_t stress, std::int32_t trainingLevel,
    std::int32_t weight, std::int32_t levelUp, std::int32_t effect,
    std::int32_t increase, std::int32_t decrease, std::int32_t invalidate)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    if (character >= Count(Part(b).canvasesAt00)) return;
    auto canvas = [&]() { return Resolve<GraphicsSurfaceNativeBody>(Read(Part(b).canvasesAt00.begin)[character]); };
    Clear(canvas());
    auto* destination = canvas();
    const auto originX = Read(destination->originXAt10), originY = Read(destination->originYAt14);
    auto positioned = [&](auto&& slot) {
        auto* selected = Resolve<GraphicsSurfaceNativeBody>(slot());
        const auto x = Read(selected->originXAt10), y = Read(selected->originYAt14);
        auto* target = canvas();
        auto* source = Resolve<GraphicsSurfaceNativeBody>(slot());
        Blit(target, source, Sub(x, originX), Sub(y, originY));
    };
    if (stress && stress <= 4)
        positioned([&]() -> Slot& { return Read(Part(b).stressAt0C.begin)[stress - 1]; });
    if (weight)
        positioned([&]() -> Slot& { return Read(Part(b).symbolsAt28.begin)[15]; });
    auto* digit = Resolve<GraphicsSurfaceNativeBody>(Read(Part(b).symbolsAt28.begin)[5 + weight % 10]);
    destination = canvas();
    Blit(destination, digit, Sub(Sample18::numberPoints[0].x, originX), Sub(Sample18::numberPoints[0].y, originY));
    digit = Resolve<GraphicsSurfaceNativeBody>(Read(Part(b).symbolsAt28.begin)[5 + weight / 10]);
    destination = canvas();
    Blit(destination, digit, Sub(Sample18::numberPoints[1].x, originX), Sub(Sample18::numberPoints[1].y, originY));
    if (trainingLevel < 3)
        positioned([&]() -> Slot& { return Read(Part(b).symbolsAt28.begin)[trainingLevel]; });
    if (levelUp)
    {
        positioned([&]() -> Slot& { return Part(b).levelAt68; });
        positioned([&]() -> Slot& { return Read(Part(b).symbolsAt28.begin)[levelUp + 2]; });
    }
    if (increase) positioned([&]() -> Slot& { return Part(b).increaseAt50; });
    if (decrease) positioned([&]() -> Slot& { return Part(b).decreaseAt5C; });
    if (effect) positioned([&]() -> Slot& { return Part(b).effectAt44; });
    if (invalidate)
    {
        destination = canvas();
        const auto width = Read(destination->widthAt08), height = Read(destination->heightAt0C);
        destination = canvas();
        const auto x = Read(destination->originXAt10), y = Read(destination->originYAt14);
        Methods(b).invalidateAt64(&b.common, {x, y, Add(width, x), Add(height, y)});
    }
}
void CalendarProgramMain00DialogNativeAt505920(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    GuiControlSurfaceReferences time{};
    VectorFrame timeFrame{time};
    for (const auto* name : Sample18::time) AppendLoaded(time, name);
    auto* const timeEnd = Read(time.end);
    GuiControlSurfaceReferences digits{};
    VectorFrame digitsFrame{digits};
    for (const auto* name : Sample18::digits) AppendLoaded(digits, name);
    if (Read(b.calendarAt80.baseAt00)) Clear(Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80));
    else
    {
        RECT bounds{};
        for (auto* at = Read(time.begin); at != timeEnd; ++at)
        { const auto rectangle = SurfaceRect(*at); ::UnionRect(&bounds, &bounds, &rectangle); }
        // 505CBD/505CCF读取var_68，即time.begin，不是digits.begin；原行为原样保留。
        RECT repeatedTimeBounds{};
        for (auto* at = Read(time.begin); at != timeEnd; ++at)
        { const auto rectangle = SurfaceRect(*at); ::UnionRect(&repeatedTimeBounds, &repeatedTimeBounds, &rectangle); }
        const auto width = Sub(repeatedTimeBounds.right, repeatedTimeBounds.left);
        const auto height = Sub(repeatedTimeBounds.bottom, repeatedTimeBounds.top);
        for (const auto& point : Sample18::datePoints)
        {
            const RECT rectangle{point.x, point.y, Add(point.x, width), Add(point.y, height)};
            ::UnionRect(&bounds, &bounds, &rectangle);
        }
        AdoptGraphicsSurfaceAt407A80(b.calendarAt80, NewGraphicsSurfaceAt40C2E0(leaves.services.shared.surfaces, leaves.services.shared.strings));
        if (!Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80)->CreateAt415010(leaves.services.shared.surfaces,
            Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top), 1, 1)) throw 1;
        auto* surface = Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80);
        surface->originXAt10 = bounds.left; surface->originYAt14 = bounds.top;
        AppendStaticSurface(b, b.calendarAt80, 39, bounds, true);
    }
    auto* surface = Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80);
    const auto originX = Read(surface->originXAt10), originY = Read(surface->originYAt14);
    auto* archive = BorrowEnvironment(0x505FE7);
    auto* root = Root(0x50603E, 0x506046);
    const auto month = ReadNamed(root, archive, "month", 0x506051, 0x506059, 0x506060);
    auto* source = Resolve<GraphicsSurfaceNativeBody>(Read(digits.begin)[month % 10]);
    Blit(Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80), source,
        Sub(Sample18::datePoints[0].x, originX), Sub(Sample18::datePoints[0].y, originY));
    archive = BorrowEnvironment(0x5060F9);
    root = Root(0x506150, 0x506158);
    const auto date = ReadNamed(root, archive, "date", 0x506163, 0x50616B, 0x506172);
    source = Resolve<GraphicsSurfaceNativeBody>(Read(digits.begin)[date % 10]);
    Blit(Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80), source,
        Sub(Sample18::datePoints[1].x, originX), Sub(Sample18::datePoints[1].y, originY));
    if (date / 10)
    {
        source = Resolve<GraphicsSurfaceNativeBody>(Read(digits.begin)[date / 10]);
        Blit(Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80), source,
            Sub(Sample18::datePoints[2].x, originX), Sub(Sample18::datePoints[2].y, originY));
    }
    archive = BorrowEnvironment(0x5062AA);
    root = Root(0x506301, 0x506309);
    const auto index = ReadNamed(root, archive, "time", 0x506314, 0x50631C, 0x506323);
    if (index < Count(time))
    {
        source = Resolve<GraphicsSurfaceNativeBody>(Read(time.begin)[index]);
        const auto x = Read(source->originXAt10), y = Read(source->originYAt14);
        source = Resolve<GraphicsSurfaceNativeBody>(Read(time.begin)[index]);
        Blit(Resolve<GraphicsSurfaceNativeBody>(b.calendarAt80), source, Sub(x, originX), Sub(y, originY));
    }
}
}
