#include <Runtime/TextDisplayGraphicsRuntime.h>
#include <Runtime/TransitRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
TextDisplayGraphicsSurface::TextDisplayGraphicsSurface(IGraphicsSurfaceApi& api) noexcept
    : surface_(api)
{
}

RuntimeGraphicsSurface& TextDisplayGraphicsSurface::Surface() noexcept { return surface_; }

TextDisplayGraphicsRuntime::TextDisplayGraphicsRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsSurfaceCopyApi& copy, IGraphicsSurfaceCompositionApi& composition,
    IGraphicsSurfaceTransitionApi& transition, IGraphicsScreenSizeApi& sizes,
    IGraphicsScreenUpdateApi& screen, IGraphicsTextApi& text, DrawTransitionView manager) noexcept
    : surfaces_(surfaces), copy_(copy), composition_(composition), transition_(transition),
      sizes_(sizes), screen_(screen), text_(text), manager_(manager)
{
}

TextDisplayGraphicsRuntime::TextDisplayGraphicsRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsSurfaceCopyApi& copy, IGraphicsSurfaceCompositionApi& composition,
    IGraphicsSurfaceTransitionApi& transition, IGraphicsScreenSizeApi& sizes,
    IGraphicsScreenUpdateApi& screen, IGraphicsTextApi& text, const DrawManagerRuntime& manager) noexcept
    : TextDisplayGraphicsRuntime(surfaces, copy, composition, transition, sizes, screen, text,
        DrawTransitionView(manager))
{
}

RuntimeGraphicsSurface& TextDisplayGraphicsRuntime::RequireSurface(const TextDisplaySurfaceRef& resource)
{
    const auto surface = std::dynamic_pointer_cast<TextDisplayGraphicsSurface>(resource);
    if (!surface)
        throw std::invalid_argument("TextDisplay resource has no production CSurface");
    return surface->Surface();
}

SIZE TextDisplayGraphicsRuntime::ReadGraphicsTargetSize() const
{
    return sizes_.ReadScreenSizeAt47CC00();
}

TextDisplaySurfaceRef TextDisplayGraphicsRuntime::CreateTextSurface(
    const std::int32_t width, const std::int32_t height, const bool alpha, const bool clear)
{
    auto resource = std::make_shared<TextDisplayGraphicsSurface>(surfaces_);
    if (!resource->Surface().Create(width, height, alpha, clear))
        return {};
    return resource;
}

bool TextDisplayGraphicsRuntime::BlitAllText(const TextDisplaySurfaceRef& target)
{
    // 4A25BC..4A25F7：传入文字 Surface 的句柄，输出 RECT 参数为零。
    return text_.BlitAllTextToSurface(RequireSurface(target).Handle(), nullptr);
}

TextDisplaySurfaceRef TextDisplayGraphicsRuntime::CaptureCurrentFrame()
{
    const SIZE size = ReadGraphicsTargetSize();
    auto target = CreateTextSurface(size.cx, size.cy, false, false);
    // 4ACDD0：创建失败返回空引用；Backup 返回值不控制成功与否。
    if (target)
        transition_.BackupScreenToSurfaceIfLoaded(RequireSurface(target).Handle());
    return target;
}

TextDisplaySurfaceRef TextDisplayGraphicsRuntime::CaptureDisplayLayersWithAlpha(RegisteredLayersView layers)
{
    const SIZE size = ReadGraphicsTargetSize();
    auto target = CreateTextSurface(size.cx, size.cy, true, false);
    // IDA 4A8471..4A8476：LayerList+1C收到真实CSurface正文，不能冻结成句柄。
    if (target)
        layers.CompositeToSurface(
            GraphicsSurfaceDestination::FromNative(RequireSurface(target).NativeBody()), nullptr);
    return target;
}

TextDisplaySurfaceRef TextDisplayGraphicsRuntime::CreateCompositeFromBase(const TextDisplaySurfaceRef& base)
{
    auto& source = RequireSurface(base);
    auto target = CreateTextSurface(source.Width(), source.Height(), source.HasAlpha(), true);
    if (!target)
        return {};
    auto& destination = RequireSurface(target);
    // 444470(copyPixels=1)：不强行改成 alpha 画布；CopyBlt 返回值被忽略，随后复制 AP 原点。
    static_cast<void>(copy_.CopyBlitSurface(destination.Handle(), source.Handle(),
        0, 0, nullptr, nullptr, nullptr, 0));
    destination.SetOrigin(source.OriginX(), source.OriginY());
    return target;
}

void TextDisplayGraphicsRuntime::BlitTextSurfaceIntoComposite(const TextDisplaySurfaceRef& composite,
    const TextDisplaySurfaceRef& text)
{
    const auto destination = RequireSurface(composite).Handle();
    const auto source = RequireSurface(text).Handle();
    // 4425C0 的两个句柄零检查早于模块检查；调用者不使用返回值。
    if (destination && source && screen_.ModuleLoaded())
        static_cast<void>(composition_.BlitSurface(destination, source, 0, 0, nullptr, nullptr, nullptr, 0));
}

void TextDisplayGraphicsRuntime::DrawTextSurface(const TextDisplaySurfaceRef& text, const RECT& dirtyRect)
{
    const auto source = RequireSurface(text).Handle();
    // 4B5E0C -> 441980：dirtyRect 是目标 clip，不是 sourceRect，也不叠加 AP 原点。
    if (source && screen_.ModuleLoaded())
        static_cast<void>(composition_.DrawSurface(source, 0, 0, nullptr, nullptr, &dirtyRect, 0));
}

void TextDisplayGraphicsRuntime::RenderTransition(const TextDisplayTransitionOperation& operation)
{
    // CTextDisp 只构造屏幕原点、无 clip 的 selector 0/1；其他槽非零表示接线错误。
    if ((operation.selectorAt1C != 0 && operation.selectorAt1C != 1) ||
        operation.valueAt00 || operation.valueAt10 || operation.valueAt14 ||
        operation.valueAt18 || operation.valueAt20)
        throw std::invalid_argument("TextDisplay transition is outside its verified screen contract");
    if (operation.selectorAt1C == 0 && operation.progressAt24 <= 0)
        return; // 448CB0 在这一分支不访问任何 Surface。
    TransitOperation state;
    state.source = &RequireSurface(operation.compositeFrameAt04).NativeBody();
    state.previous = operation.baseFrameAt08 ? RequireSurface(operation.baseFrameAt08).Handle() : 0;
    state.mask = operation.effectMaskAt0C ? RequireSurface(operation.effectMaskAt0C).Handle() : 0;
    state.opcode = operation.selectorAt1C;
    state.current = operation.progressAt24;
    state.target = operation.targetAt28;
    state.range = operation.auxiliaryAt2C;
    static_cast<void>(manager_.RenderTransitionAt4C39E0(state));
}
}
