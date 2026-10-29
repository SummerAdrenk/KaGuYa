#include <Runtime/FreeScrollRuntime.h>

#include <Runtime/SaveDataEnvironmentArchive.h>

#include <algorithm>
#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireRepresentableArea(const RECT& rect)
{
    constexpr auto maximum = (std::numeric_limits<std::int32_t>::max)();
    if (static_cast<std::int64_t>(rect.right) - rect.left > maximum ||
        static_cast<std::int64_t>(rect.bottom) - rect.top > maximum)
        throw std::overflow_error("FreeScroll dirty extent exceeds signed LONG");
}

void RequireRepresentablePairAreas(const std::span<const RECT> rects, const RECT* extra = nullptr)
{
    RECT extent = extra ? *extra : rects.front();
    for (const auto& rect : rects)
    {
        extent.left = (std::min)(extent.left, rect.left);
        extent.top = (std::min)(extent.top, rect.top);
        extent.right = (std::max)(extent.right, rect.right);
        extent.bottom = (std::max)(extent.bottom, rect.bottom);
    }
    // 满容量 Add 会检查所有 pair；Coalesce 的合并不缩小范围，极值所在 pair
    // 或其合并后继也必被检查。此时全范围超限才代表实际 RectArea 路径会溢出。
    RequireRepresentableArea(extent);
}
}

std::int32_t FreeScrollRuntime::Add(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

std::int32_t FreeScrollRuntime::Sub(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}

RECT FreeScrollRuntime::Offset(const RECT& rect, const std::int32_t x, const std::int32_t y) noexcept
{
    return {Add(rect.left, x), Add(rect.top, y), Add(rect.right, x), Add(rect.bottom, y)};
}

void FreeScrollRuntime::AddDirty(DirtyRectCollection& dirty, const RECT& rect)
{
    if (::IsRectEmpty(&rect)) return;
    // 未满时共享 Add 只求新矩形面积，不能提前拒绝远处的 resident。
    RequireRepresentableArea(rect);
    if (dirty.Count() == DirtyRectCollection::MaximumResidentRectCount)
        RequireRepresentablePairAreas(dirty.Rects(), &rect);
    dirty.Add(rect);
}

void FreeScrollRuntime::CoalesceDirty(DirtyRectCollection& dirty)
{
    if (dirty.Count() >= 2) RequireRepresentablePairAreas(dirty.Rects());
    dirty.Coalesce();
}

RECT FreeScrollRuntime::SpriteRect(const SpriteRuntime& sprite) noexcept
{
    if (sprite.LogicalName().empty() || static_cast<std::uint32_t>(sprite.LoadMode()) > 2u)
        return {};
    return Offset({0, 0, sprite.Width(), sprite.Height()}, sprite.X(), sprite.Y());
}

RECT FreeScrollRuntime::LocalUnionAt4ADDE0(const PatternLayerRuntime& pattern)
{
    RECT result{};
    for (std::size_t i = 0; i < pattern.SpriteSlotCount(); ++i)
    {
        const RECT sprite = SpriteRect(pattern.SpriteAt(i));
        RECT combined{};
        ::UnionRect(&combined, &result, &sprite);
        result = combined;
    }
    return result;
}

const RuntimeGraphicsSurface& FreeScrollRuntime::SpriteSurface(
    const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount()) return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        const auto count = sprite.AuxiliarySurfaceCount(group);
        if (index < count) return sprite.AuxiliarySurfaceAt(group, index);
        index -= count;
    }
    throw std::out_of_range("FreeScroll Sprite surface reference is missing");
}

void FreeScrollRuntime::CreateCanvasAt4AE574(Entry& entry)
{
    auto created = std::make_unique<RuntimeGraphicsSurface>(dependencies_.surfaces);
    entry.main.reset();
    entry.main = std::move(created);
    const RECT local = LocalUnionAt4ADDE0(entry.pattern);
    const auto width = Sub(local.right, local.left);
    const auto height = Sub(local.bottom, local.top);
    const bool alpha = entry.pattern.MainSurfaceAt18().HasAlpha();
    static_cast<void>(entry.main->Create(width, height, alpha, true));
    entry.unionOrigin = {local.left, local.top};
    const RECT global = Offset(local, entry.pattern.X(), entry.pattern.Y());
    RECT clipped{};
    ::IntersectRect(&clipped, &state_.viewportAt1C, &global);
    const auto dx = Sub(clipped.left, global.left);
    const auto dy = Sub(clipped.top, global.top);
    dependencies_.filters.SurfaceCopyBlitRaw(entry.main->Handle(),
        entry.pattern.MainSurfaceAt18().Handle(), dx, dy, nullptr, &clipped, nullptr, 0);

    if (entry.pattern.FilterMode() > 0 && entry.pattern.FilterMode() != 10)
    {
        auto processed = std::make_unique<RuntimeGraphicsSurface>(dependencies_.surfaces);
        if (!processed->Create(entry.main->Width(), entry.main->Height(), entry.main->HasAlpha(), true))
            processed.reset();
        entry.processed.reset();
        entry.processed = std::move(processed);
        const auto* source = entry.pattern.ProcessedSurfaceAt64();
        if (!source) throw std::logic_error("FreeScroll Begin requires Pattern processed surface");
        if (!entry.processed) throw std::runtime_error("FreeScroll processed surface creation failed");
        dependencies_.filters.SurfaceCopyBlitRaw(entry.processed->Handle(), source->Handle(),
            dx, dy, nullptr, &clipped, nullptr, 0);
    }

    // 四带尺寸来自 union；原 IsRectEmpty 门控：4AF01A/4AF08E/4AF0E9/4AF161。
    const auto margin = [this, &entry](const RECT& rect) {
        if (!::IsRectEmpty(&rect)) RedrawMarginAt4C43D0(entry, rect);
    };
    if (dx > 0) margin({0, 0, dx, height});
    const auto right = Add(dx, Sub(clipped.right, clipped.left));
    if (right < width) margin({right, 0, width, height});
    if (dy > 0) margin({0, 0, width, dy});
    const auto bottom = Add(dy, Sub(clipped.bottom, clipped.top));
    if (bottom < height) margin({0, bottom, width, height});
}

void FreeScrollRuntime::DrawSpritesInRect(Entry& entry, const RECT& rect)
{
    for (std::size_t i = 0; i < entry.pattern.SpriteSlotCount(); ++i)
    {
        auto& sprite = entry.pattern.SpriteAt(i);
        if (sprite.LogicalName().empty()) continue;
        const auto savedX = sprite.X();
        const auto savedY = sprite.Y();
        sprite.SetOrigin(Sub(savedX, entry.unionOrigin.x), Sub(savedY, entry.unionOrigin.y));
        if (!entry.main) throw std::logic_error("FreeScroll main surface reference is missing");
        const auto blit = [this, &entry, &rect](const RuntimeGraphicsSurface& source,
                                              const std::int32_t x, const std::int32_t y) {
            if (entry.main->Handle() != 0 && source.Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceBlitRaw(entry.main->Handle(),
                    source.Handle(), x, y, nullptr, nullptr, &rect, 0));
        };
        switch (sprite.LoadMode())
        {
        case SpriteLoadMode::StaticWithoutAlpha:
        case SpriteLoadMode::StaticWithAlpha:
            // 4C452D/4C4530：静态图只用 Sprite 坐标，不加 Surface origin。
            blit(SpriteSurface(sprite, 0), sprite.X(), sprite.Y());
            break;
        case SpriteLoadMode::Animation:
            for (std::size_t j = 0; j < sprite.CurrentFrameMappings().size(); ++j)
            {
                const auto mapping = sprite.CurrentFrameMappings()[j];
                if (mapping < 0) continue;
                const auto& source = SpriteSurface(sprite, static_cast<std::size_t>(mapping));
                blit(source, Add(sprite.X(), source.OriginX()), Add(sprite.Y(), source.OriginY()));
            }
            break;
        default:
            break;
        }
        // 4C43D0/4AF200 均无坐标恢复的 EH；异常时保留暂写后的 Sprite 坐标。
        sprite.SetOrigin(savedX, savedY);
    }
}

void FreeScrollRuntime::RefreshFilterInRect(Entry& entry, const RECT& rect)
{
    auto& pattern = entry.pattern;
    if (pattern.FilterMode() <= 0 || pattern.FilterMode() == 10) return;
    if (!entry.processed)
    {
        if (!entry.main) throw std::logic_error("FreeScroll filter requires main surface");
        auto processed = std::make_unique<RuntimeGraphicsSurface>(dependencies_.surfaces);
        if (processed->Create(entry.main->Width(), entry.main->Height(), entry.main->HasAlpha(), true))
            entry.processed = std::move(processed);
    }
    // mode6 原本也会尝试创建 processed，但不访问创建结果、不复制或装载资源。
    if (pattern.FilterMode() == 6) return;
    if (!entry.processed) throw std::runtime_error("FreeScroll filter surface creation failed");
    if (!entry.main) throw std::logic_error("FreeScroll filter requires main surface");
    if (pattern.FilterMode() == 3)
        static_cast<void>(ApplyPatternPixelFilterAt442900(dependencies_.filters,
            pattern.FilterMode(), pattern.FilterArguments(), *entry.processed, entry.main.get(), nullptr, &rect));
    else
    {
        dependencies_.filters.SurfaceCopyBlitRaw(entry.processed->Handle(), entry.main->Handle(),
            0, 0, nullptr, nullptr, &rect, 0);
        static_cast<void>(ApplyPatternPixelFilterAt442900(dependencies_.filters,
            pattern.FilterMode(), pattern.FilterArguments(), *entry.processed, nullptr, nullptr, &rect));
    }
}

void FreeScrollRuntime::RedrawMarginAt4C43D0(Entry& entry, const RECT& rect)
{
    DrawSpritesInRect(entry, rect);
    RefreshFilterInRect(entry, rect);
}

void FreeScrollRuntime::CollectAnimationDirtyAt4A7A30(Entry& entry, DirtyRectCollection& dirty)
{
    for (std::size_t i = 0; i < entry.pattern.SpriteSlotCount(); ++i)
    {
        auto& sprite = entry.pattern.SpriteAt(i);
        const RECT before = SpriteRect(sprite);
        if (!sprite.LogicalName().empty() && sprite.LoadMode() == SpriteLoadMode::Animation &&
            sprite.AdvanceAnimation() == 0)
        {
            AddDirty(dirty, Offset(before, Sub(0, entry.unionOrigin.x), Sub(0, entry.unionOrigin.y)));
            AddDirty(dirty, Offset(SpriteRect(sprite), Sub(0, entry.unionOrigin.x), Sub(0, entry.unionOrigin.y)));
        }
    }
}

POINT FreeScrollRuntime::ScreenOffset(const Entry& entry, const POINT delta) noexcept
{
    const bool fixed = (entry.pattern.ResultValueAt10() & 1u) != 0;
    return {Add(Add(entry.pattern.X(), fixed ? 0 : delta.x), entry.unionOrigin.x),
        Add(Add(entry.pattern.Y(), fixed ? 0 : delta.y), entry.unionOrigin.y)};
}

void FreeScrollRuntime::RenderAt4AF200(const bool changed)
{
    DirtyRectCollection dirty;
    if (changed) AddDirty(dirty, state_.viewportAt1C);
    if (entries_.empty()) throw std::logic_error("FreeScroll active rendering requires a first entry");
    const auto& first = entries_.front()->pattern;
    const POINT delta{Sub(Sub(0, state_.xAt14), first.X()), Sub(Sub(0, state_.yAt18), first.Y())};
    if (dependencies_.save.bytesAt6D[0] != 0)
    {
        for (std::size_t i = 0; i < entries_.size(); ++i)
        {
            auto& entry = *entries_[i];
            DirtyRectCollection local;
            CollectAnimationDirtyAt4A7A30(entry, local);
            CoalesceDirty(local);
            // 先清完该项的全部 dirty，再逐 rect 重绘，不能交错 Clear/Draw。
            for (const RECT& rect : local.Rects())
            {
                if (!entry.main) throw std::logic_error("FreeScroll animated main surface is missing");
                if (entry.main->Handle() != 0)
                    dependencies_.drawing.SurfaceClearRaw(entry.main->Handle(), entry.main->ClearColor(), &rect);
            }
            for (const RECT& rect : local.Rects())
            {
                DrawSpritesInRect(entry, rect);
                RefreshFilterInRect(entry, rect);
                const POINT offset = ScreenOffset(entry, delta);
                const RECT screenRect = Offset(rect, offset.x, offset.y);
                RECT clipped{};
                ::IntersectRect(&clipped, &screenRect, &state_.viewportAt1C);
                AddDirty(dirty, clipped);
            }
        }
    }
    CoalesceDirty(dirty);
    for (const RECT& rect : dirty.Rects())
    {
        for (std::size_t i = 0; i < entries_.size(); ++i)
        {
            const auto& entry = *entries_[i];
            const POINT offset = ScreenOffset(entry, delta);
            const auto* source = entry.processed ? entry.processed.get() : entry.main.get();
            if (!source) throw std::logic_error("FreeScroll screen source reference is missing");
            if (source->Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceDrawRaw(source->Handle(),
                    offset.x, offset.y, nullptr, nullptr, &rect, 0));
        }
        dependencies_.screen.InvalidatedRectIfLoaded(&rect);
    }
}
}
