#include <Runtime/FreeScrollNativeDrawing.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/SpriteAnimationInitializationSource.h>
#include <Runtime/SpriteAnimationNativeExecution.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Entry = FreeScrollEntryNativeBody24h;
using Surface = GraphicsSurfaceNativeBody;
using Sprite = SpriteNativeBodyA0h;
using Pattern = PatternLayerNativeBody80h;
using Services = FreeScrollNativeDrawServices;

template<class T> T Read(const T& field) noexcept
{ return static_cast<const volatile T&>(field); }
std::int32_t Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return Signed(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return Signed(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
std::uint32_t Count(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    return control ? Read(control->countAt10) : 0;
}
template<class T> T* Resolve(const YtlReferenceSlotRuntime& slot, std::uint32_t index = 0) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    const auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(slot.biasAt04) + index) * stride);
}
bool Named(const Sprite& sprite) noexcept
{
    const auto chars = *reinterpret_cast<const std::uintptr_t volatile*>(&sprite.logicalNameAt0C);
    return *reinterpret_cast<const std::int32_t volatile*>(chars - 0x0Cu) != 0;
}
RECT Bounds(const Sprite& sprite)
{
    RECT result{};
    if (Named(sprite) && Read(sprite.modeAt04) <= 2u)
    {
        result.right = Read(sprite.widthAt24);
        result.bottom = Read(sprite.heightAt28);
        const auto y = Read(sprite.yAt30);
        const auto x = Read(sprite.xAt2C);
        ::OffsetRect(&result, x, y);
    }
    return result;
}
void AddLocalDirty(Entry& entry, GuiDirtyRectNativeBody80h& dirty, RECT& rect)
{
    const auto y = Sub(0, Read(entry.unionOriginAt04.y));
    const auto x = Sub(0, Read(entry.unionOriginAt04.x));
    ::OffsetRect(&rect, x, y);
    dirty.vptrAt00->addRectAt08(&dirty, &rect);
}
void Blit(Surface* destination, Surface* source, std::int32_t x, std::int32_t y,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    const auto target = Read(destination->handleAt04);
    if (!target) return;
    const auto handle = Read(source->handleAt04);
    if (handle) (void)drawing.SurfaceBlitRaw(target, handle, x, y, nullptr, nullptr, clip, 0);
}
enum class SpriteDrawPath { Margin, Renderer };
void DrawSprites(Entry& entry, const RECT* clip, IGraphicsWindowSurfaceApi& drawing, SpriteDrawPath path)
{
    std::uint32_t index = 0;
    for (;; ++index)
    {
        auto* pattern = Read(entry.patternAt00);
        if (index >= Count(pattern->spritesAt34)) break;
        if (!Named(*Resolve<Sprite>(pattern->spritesAt34, index))) continue;
        auto* sprite = Resolve<Sprite>(pattern->spritesAt34, index);
        const auto oldX = Read(sprite->xAt2C);
        const auto oldY = Read(sprite->yAt30);
        std::int32_t x, y;
        if (path == SpriteDrawPath::Margin)
        {
            x = Sub(oldX, Read(entry.unionOriginAt04.x));
            y = Sub(oldY, Read(entry.unionOriginAt04.y));
        }
        else
        {
            y = Sub(oldY, Read(entry.unionOriginAt04.y));
            x = Sub(oldX, Read(entry.unionOriginAt04.x));
        }
        sprite->xAt2C = x;
        sprite->yAt30 = y;
        auto* destination = Resolve<Surface>(entry.mainAt0C);
        if (Named(*sprite))
        {
            const auto mode = Signed(Read(sprite->modeAt04));
            if (mode >= 0 && mode <= 1)
            {
                const auto drawY = Read(sprite->yAt30);
                const auto drawX = Read(sprite->xAt2C);
                auto* source = ResolveSpriteSurfaceNativeAt443580(*sprite, 0);
                Blit(destination, source, drawX, drawY, clip, drawing);
            }
            else if (mode == 2)
            {
                for (std::int32_t i = 0; i < Signed(Count(sprite->animationAt34.currentAt18)); i = Add(i, 1))
                {
                    const auto mapping = Read(*Resolve<std::int32_t>(sprite->animationAt34.currentAt18,
                        static_cast<std::uint32_t>(i)));
                    if (mapping < 0) continue;
                    auto* origin = ResolveSpriteSurfaceNativeAt443580(*sprite, mapping);
                    // 4C44F7 先读 y/x 再读原点；4AF5E2 先完成 y，再读 x。
                    auto drawY = Read(sprite->yAt30);
                    std::int32_t drawX;
                    if (path == SpriteDrawPath::Margin)
                    {
                        drawX = Read(sprite->xAt2C);
                        drawY = Add(drawY, Read(origin->originYAt14));
                    }
                    else
                    {
                        drawY = Add(drawY, Read(origin->originYAt14));
                        drawX = Read(sprite->xAt2C);
                    }
                    drawX = Add(drawX, Read(origin->originXAt10));
                    auto* source = ResolveSpriteSurfaceNativeAt443580(*sprite, mapping);
                    Blit(destination, source, drawX, drawY, clip, drawing);
                }
            }
        }
        // 4C454B/4AF816 仅在正常路径恢复固定的 Sprite 正文。
        sprite->xAt2C = oldX;
        sprite->yAt30 = oldY;
    }
}
union TemporaryReference
{
    TemporaryReference() noexcept {}
    ~TemporaryReference() noexcept {}
    YtlReferenceSlotRuntime slot;
};
struct ReferenceUnwind final
{
    YtlReferenceSlotRuntime& slot;
    ~ReferenceUnwind() noexcept { ReleaseYtlReference(slot); }
};
void ReleaseTemporary(YtlReferenceSlotRuntime& slot) noexcept
{
    // PE 状态 4C43D0:1 / 4AF200:4 / 4ADF70:14 直接终止，不重试清理。
    ReleaseYtlReference(slot);
}
bool CreateInline(Surface& destination, std::int32_t width, std::int32_t height,
    std::uint8_t alpha, IGraphicsSurfaceApi& surfaces)
{
    destination.ResetAt432670(surfaces);
    const auto handle = surfaces.CreateSurface(width, height, alpha, 1);
    destination.handleAt04 = handle;
    if (!handle) return false;
    destination.widthAt08 = width;
    destination.heightAt0C = height;
    destination.alphaAt18 = alpha;
    return true;
}
void CloneEmpty(YtlReferenceSlotRuntime& out, const Surface& source, Services& services)
{
    // 444470(copyPixels=0)：直接解原 Ref，不采用现代 capture 的空正文异常转换。
    auto* created = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
    ConstructGraphicsSurfaceReferenceAt451500(out, created, 1);
    try
    {
        const auto width = Read(source.widthAt08);
        const auto height = Read(source.heightAt0C);
        const auto alpha = Read(source.alphaAt18);
        if (!CreateInline(*Resolve<Surface>(out), width, height, alpha, services.surfaces))
            ReleaseYtlReference(out);
    }
    catch (...)
    {
        ReferenceUnwind unwind{out};
        throw;
    }
}
void ProcessFilter(Entry& entry, const RECT* clip, Services& services)
{
    auto* pattern = Read(entry.patternAt00);
    const auto initialMode = Signed(Read(pattern->filterAt40.modeAt04));
    if (initialMode <= 0 || initialMode == 10) return;
    if (!Read(entry.processedAt18.baseAt00))
    {
        TemporaryReference temporary;
        CloneEmpty(temporary.slot, *Resolve<Surface>(entry.mainAt0C), services);
        try { CopyAssignGraphicsSurfaceReferenceAt465870(entry.processedAt18, temporary.slot); }
        catch (...)
        {
            ReferenceUnwind unwind{temporary.slot};
            throw;
        }
        ReleaseTemporary(temporary.slot);
        pattern = Read(entry.patternAt00);
    }
    // clone 回调后重读 mode，但不重复最外层资格判断。
    const auto mode = Read(pattern->filterAt40.modeAt04);
    if (mode == 6u) return;
    if (mode == 3u)
    {
        auto* source = Resolve<Surface>(entry.mainAt0C);
        auto* destination = Resolve<Surface>(entry.processedAt18);
        (void)ApplyPatternPixelFilterNativeAt442900(services.filters, pattern->filterAt40,
            destination, source, clip);
    }
    else
    {
        const auto source = Read(Resolve<Surface>(entry.mainAt0C)->handleAt04);
        const auto destination = Read(Resolve<Surface>(entry.processedAt18)->handleAt04);
        services.filters.SurfaceCopyBlitRaw(destination, source, 0, 0, nullptr, nullptr, clip, 0);
        auto* processed = Resolve<Surface>(entry.processedAt18);
        pattern = Read(entry.patternAt00);
        (void)ApplyPatternPixelFilterNativeAt442900(services.filters, pattern->filterAt40,
            processed, nullptr, clip);
    }
}
POINT Projection(YtlReferenceSlotRuntime& slot, POINT delta)
{
    auto* pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto fixed = Read(pattern->resultAt7D) & 1u;
    pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto x = Signed(Read(pattern->layer.valueAt04));
    const auto y = Signed(Read(pattern->layer.valueAt08));
    POINT result{fixed ? x : Add(x, delta.x), fixed ? y : Add(y, delta.y)};
    auto* entry = Resolve<Entry>(slot);
    result.x = Add(result.x, Read(entry->unionOriginAt04.x));
    result.y = Add(result.y, Read(entry->unionOriginAt04.y));
    return result;
}
void DrawBand(YtlReferenceSlotRuntime& slot, RECT& rect, Services& services)
{
    if (!::IsRectEmpty(&rect)) RedrawFreeScrollEntryAt4C43D0(*Resolve<Entry>(slot), &rect, services);
}
}

void CollectFreeScrollDirtyAt4A7A30(Entry& entry, GuiDirtyRectNativeBody80h& dirty)
{
    for (std::uint32_t index = 0;; ++index)
    {
        auto* pattern = Read(entry.patternAt00);
        if (index >= Count(pattern->spritesAt34)) break;
        auto before = Bounds(*Resolve<Sprite>(pattern->spritesAt34, index));
        // Bounds 内的 OffsetRect 是原调用边界，之后重新取 Pattern。
        pattern = Read(entry.patternAt00);
        auto* sprite = Resolve<Sprite>(pattern->spritesAt34, index);
        if (!Named(*sprite) || Read(sprite->modeAt04) != 2u) continue;
        if (AdvanceSpriteAnimationNativeAt443740(sprite->animationAt34) != 0) continue;
        AddLocalDirty(entry, dirty, before);
        pattern = Read(entry.patternAt00);
        auto after = Bounds(*Resolve<Sprite>(pattern->spritesAt34, index));
        AddLocalDirty(entry, dirty, after);
    }
}

void RedrawFreeScrollEntryAt4C43D0(Entry& entry, const RECT* clip, Services& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    DrawSprites(entry, clip, services.drawing, SpriteDrawPath::Margin);
    ProcessFilter(entry, clip, services);
}

void RenderFreeScrollAt4AF200(FreeScrollNativeBody50h& body, std::uint32_t changed,
    Services& services, const ISpriteAnimationInitializationSource& registry, IGraphicsScreenCalls& screen)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    GuiDirtyRectNativeBody80h total;
    if (changed)
    {
        const auto bottom = Read(body.viewportAt1C.bottom);
        const auto right = Read(body.viewportAt1C.right);
        const auto top = Read(body.viewportAt1C.top);
        const auto left = Read(body.viewportAt1C.left);
        total.AddAt441680(left, top, right, bottom);
    }
    const auto scrollX = Sub(0, Read(body.xAt14));
    const auto scrollY = Sub(0, Read(body.yAt18));
    auto* first = Read(Resolve<Entry>(*Read(body.entriesAt00.begin))->patternAt00);
    const POINT delta{Sub(scrollX, Signed(Read(first->layer.valueAt04))),
        Sub(scrollY, Signed(Read(first->layer.valueAt08)))};
    if (registry.ReadRegistrySlot1ByteAt6D())
    {
        for (auto* slot = Read(body.entriesAt00.begin); slot != Read(body.entriesAt00.end); ++slot)
        {
            GuiDirtyRectNativeBody80h local;
            CollectFreeScrollDirtyAt4A7A30(*Resolve<Entry>(*slot), local);
            local.CoalesceAt442060();
            const auto count = Signed(Read(local.countAt7C));
            for (std::int32_t i = 0; i < count; ++i)
            {
                auto* main = Resolve<Surface>(Resolve<Entry>(*slot)->mainAt0C);
                const auto handle = Read(main->handleAt04);
                if (handle)
                {
                    const auto color = Read(main->clearColorAt20);
                    services.drawing.SurfaceClearRaw(handle, color, &local.rectsAt04[i]);
                }
            }
            for (std::int32_t i = 0; i < count; ++i)
            {
                auto* entry = Resolve<Entry>(*slot);
                auto* clip = &local.rectsAt04[i];
                DrawSprites(*entry, clip, services.drawing, SpriteDrawPath::Renderer);
                ProcessFilter(*entry, clip, services);
                RECT projected;
                ::CopyRect(&projected, clip);
                const auto position = Projection(*slot, delta);
                ::OffsetRect(&projected, position.x, position.y);
                ::IntersectRect(&projected, &projected, &body.viewportAt1C);
                total.vptrAt00->addLTRBAt0C(&total, projected.left, projected.top,
                    projected.right, projected.bottom);
            }
        }
    }
    total.CoalesceAt442060();
    const auto count = Signed(Read(total.countAt7C));
    for (std::int32_t i = 0; i < count; ++i)
    {
        for (auto* slot = Read(body.entriesAt00.begin); slot != Read(body.entriesAt00.end); ++slot)
        {
            const auto position = Projection(*slot, delta);
            const auto processed = Read(Resolve<Entry>(*slot)->processedAt18.baseAt00);
            auto* entry = Resolve<Entry>(*slot);
            auto* source = Resolve<Surface>(processed ? entry->processedAt18 : entry->mainAt0C);
            const auto handle = Read(source->handleAt04);
            if (handle) (void)services.drawing.SurfaceDrawRaw(handle, position.x, position.y,
                nullptr, nullptr, &total.rectsAt04[i], 0);
        }
        screen.InvalidatedRectIfLoaded(&total.rectsAt04[i]);
    }
}

void CreateFreeScrollCanvasAt4AE574(FreeScrollNativeBody50h& body,
    YtlReferenceSlotRuntime& slot, Services& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    auto* created = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
    AdoptGraphicsSurfaceAt407A80(Resolve<Entry>(slot)->mainAt0C, created);
    auto* pattern = Read(Resolve<Entry>(slot)->patternAt00);
    RECT bounds = ReadFreeScrollLocalUnionAt4ADDE0(*pattern);
    pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto alpha = Read(Resolve<Surface>(pattern->mainAt18)->alphaAt18);
    auto* main = Resolve<Surface>(Resolve<Entry>(slot)->mainAt0C);
    const auto height = Sub(bounds.bottom, bounds.top);
    const auto width = Sub(bounds.right, bounds.left);
    (void)CreateInline(*main, width, height, alpha, services.surfaces);
    Resolve<Entry>(slot)->unionOriginAt04.x = bounds.left;
    Resolve<Entry>(slot)->unionOriginAt04.y = bounds.top;
    pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto y = Signed(Read(pattern->layer.valueAt08));
    const auto x = Signed(Read(pattern->layer.valueAt04));
    ::OffsetRect(&bounds, x, y);
    RECT intersection{};
    ::IntersectRect(&intersection, &body.viewportAt1C, &bounds);
    const auto dx = Sub(intersection.left, bounds.left);
    const auto dy = Sub(intersection.top, bounds.top);
    pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto source = Read(Resolve<Surface>(pattern->mainAt18)->handleAt04);
    const auto destination = Read(Resolve<Surface>(Resolve<Entry>(slot)->mainAt0C)->handleAt04);
    services.filters.SurfaceCopyBlitRaw(destination, source, dx, dy, nullptr, &intersection, nullptr, 0);
    pattern = Read(Resolve<Entry>(slot)->patternAt00);
    const auto mode = Signed(Read(pattern->filterAt40.modeAt04));
    if (mode > 0 && mode != 10)
    {
        main = Resolve<Surface>(Resolve<Entry>(slot)->mainAt0C);
        TemporaryReference temporary;
        auto* processed = NewGraphicsSurfaceAt40C2E0(services.surfaces, services.strings);
        ConstructGraphicsSurfaceReferenceAt451500(temporary.slot, processed, 1);
        try
        {
            const auto processedAlpha = Read(main->alphaAt18);
            auto* target = Resolve<Surface>(temporary.slot);
            const auto processedHeight = Read(main->heightAt0C);
            const auto processedWidth = Read(main->widthAt08);
            if (!target->CreateAt415010(services.surfaces, processedWidth, processedHeight, processedAlpha, 1))
                ReleaseYtlReference(temporary.slot);
            CopyAssignGraphicsSurfaceReferenceAt465870(Resolve<Entry>(slot)->processedAt18, temporary.slot);
        }
        catch (...)
        {
            ReferenceUnwind unwind{temporary.slot};
            throw;
        }
        ReleaseTemporary(temporary.slot);
        pattern = Read(Resolve<Entry>(slot)->patternAt00);
        const auto processedSource = Read(Resolve<Surface>(pattern->processedAt64)->handleAt04);
        const auto processedTarget = Read(Resolve<Surface>(Resolve<Entry>(slot)->processedAt18)->handleAt04);
        services.processedCopy.CopyProcessedAt4AEFED(processedTarget, processedSource, dx, dy, &intersection);
    }
    // 四带各自占满高度或宽度，四个角的重复绘制属于原行为。
    if (dx > 0)
    {
        RECT band{0, 0, dx, Sub(bounds.bottom, bounds.top)};
        DrawBand(slot, band, services);
    }
    const auto right = Add(dx, Sub(intersection.right, intersection.left));
    if (right < Sub(bounds.right, bounds.left))
    {
        RECT band{right, 0, Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top)};
        DrawBand(slot, band, services);
    }
    if (dy > 0)
    {
        RECT band{0, 0, Sub(bounds.right, bounds.left), dy};
        DrawBand(slot, band, services);
    }
    const auto bottom = Add(dy, Sub(intersection.bottom, intersection.top));
    if (bottom < Sub(bounds.bottom, bounds.top))
    {
        RECT band{0, bottom, Sub(bounds.right, bounds.left), Sub(bounds.bottom, bounds.top)};
        DrawBand(slot, band, services);
    }
}

}
