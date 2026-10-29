#include "SettingPage01PreviewBlockNative.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage01PreviewBlockNativeDetail
{
namespace
{
struct Selected final { Surface* surface; std::int32_t x, y; };
Selected Current(Body& b, std::int32_t index)
{
    const auto& element = Read(b.previewsAt8C.begin)[index];
    const auto base = Read(element.baseAt00);
    auto* color = ResolveBase<Color>(element, base);
    auto* surface = Resolve<Surface>(color->processedAt1C);
    const auto x = Read(surface->originXAt10), y = Read(surface->originYAt14);
    color = ResolveBase<Color>(element, base); // 不把第一次processed receiver扩散成draw缓存。
    return {Resolve<Surface>(color->processedAt1C), x, y};
}
void DrawSurface(Surface* fixed, int x, int y, const RECT* clip)
{
    const auto handle = Read(fixed->handleAt04);
    if (handle) static_cast<void>(RequireSettingPage01PreviewBlockNativeServices().colors.drawing.SurfaceDrawRaw(
        handle, x, y, nullptr, nullptr, clip, 0)); // 原441980固定handle后仅一次4182F0。
}
void BlitSurface(Surface* target, Surface* source, int x, int y, const RECT* clip)
{
    const auto targetHandle = Read(target->handleAt04);
    if (!targetHandle) return;
    const auto sourceHandle = Read(source->handleAt04);
    if (sourceHandle) static_cast<void>(RequireSettingPage01PreviewBlockNativeServices().colors.drawing.SurfaceBlitRaw(
        targetHandle, sourceHandle, x, y, nullptr, nullptr, clip, 0));
}
}
void __fastcall Draw(Common* p, void*, const RECT* clip)
{
    GuiDialogBaseVtable().drawAt60(p, clip); // 直接5230B0，disabled也先执行基类。
    if (!Read(p->enabledAt38)) return;
    const auto index = ReadSettingPage01PreviewSelectionNativeAt535E60(*p);
    auto& b = B(p);
    if (index == 0 || index == 1)
    { const auto selected = Current(b, index); DrawSurface(selected.surface, selected.x, selected.y, clip); }
    if (Read(b.textAtA8.baseAt00))
    {
        const auto point = CurrentSettingTextDisplayProfile().preview.textPosition;
        DrawSurface(Resolve<Surface>(b.textAtA8), point.x, point.y, clip);
    }
}
void __fastcall Composite(Common* p, void*, Slot destination, const RECT* clip)
{
    RefLife life{destination};
    Slot argument = Copy(destination);
    GuiDialogBaseVtable().compositeAt5C(p, argument, clip); // 原基类消费独立复制的拥有Ref。
    if (Read(p->enabledAt38))
    {
        const auto index = ReadSettingPage01PreviewSelectionNativeAt535E60(*p);
        auto& b = B(p);
        if (index == 0 || index == 1)
        {
            const auto selected = Current(b, index);
            BlitSurface(Resolve<Surface>(destination), selected.surface, selected.x, selected.y, clip);
        }
        if (Read(b.textAtA8.baseAt00))
        {
            auto* source = Resolve<Surface>(b.textAtA8);
            auto* target = Resolve<Surface>(destination);
            const auto point = CurrentSettingTextDisplayProfile().preview.textPosition;
            BlitSurface(target, source, point.x, point.y, clip);
        }
    }
    life.Normal();
}
}
