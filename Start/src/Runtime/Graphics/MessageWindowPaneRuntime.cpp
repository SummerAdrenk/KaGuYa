#include <Runtime/MessageWindowPaneRuntime.h>

#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add32(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right));
}

std::int32_t Sub32(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) -
        static_cast<std::uint32_t>(right));
}

std::int32_t Mul32(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) *
        static_cast<std::uint32_t>(right));
}

void DrawSurface(IGraphicsWindowSurfaceApi& graphics, const RuntimeGraphicsSurface& source,
    const std::int32_t x, const std::int32_t y, const RECT* const clip)
{
    const auto handle = source.Handle();
    if (handle != 0)
        static_cast<void>(graphics.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, clip, 0));
}

void BlitSurface(IGraphicsWindowSurfaceApi& graphics, const GraphicsSurfaceDestination destination,
    const RuntimeGraphicsSurface& source, const std::int32_t x, const std::int32_t y,
    const RECT* const clip)
{
    // 4425C0 先判目标句柄，再读取源句柄；模块存在性由 Raw 包装判断。
    const auto targetHandle = destination.Handle();
    if (targetHandle == 0) return;
    const auto handle = source.Handle();
    if (handle != 0)
        static_cast<void>(graphics.SurfaceBlitRaw(targetHandle, handle, x, y,
            nullptr, nullptr, clip, 0));
}
}

MessageWindowNormalPaneRuntime::MessageWindowNormalPaneRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
    const ColorTransWindowDescriptor& descriptor,
    std::shared_ptr<const IMessageWindowColorConfig> config)
    // 45F381：复制成员引用；传值参数须存活到 597023/597018 成员清理之后。
    : graphics_(graphics), config_(config)
{
    window_ = std::make_unique<ColorTransWindowRuntime>(surfaces, graphics, resources, descriptor);
    if (window_->Processed() != nullptr)
    {
        if (!RefreshColorsAt4874C0()) throw 1;
        const auto* const surface = window_->Processed();
        if (!surface)
            throw std::logic_error("normalWindow processed surface disappeared after ApplyColors");
        bounds_ = RECT{surface->OriginX(), surface->OriginY(),
            Add32(surface->OriginX(), surface->Width()), Add32(surface->OriginY(), surface->Height())};
    }
}

MessageWindowNormalPaneRuntime::~MessageWindowNormalPaneRuntime()
{
    config_.reset();
    window_.reset();
}

std::int32_t MessageWindowNormalPaneRuntime::TypeAtVslot00() const { return 0; }
RECT MessageWindowNormalPaneRuntime::BoundsAtVslot04() const { return bounds_; }

SIZE MessageWindowNormalPaneRuntime::BaseTileSizeAt47DFE0() const
{
    const auto* const surface = Processed();
    if (!surface)
        throw std::logic_error("normalWindow has no processed surface for tile size");
    return SIZE{surface->Width(), surface->Height()};
}

bool MessageWindowNormalPaneRuntime::RefreshColorsAt4874C0()
{
    if (!config_) throw std::logic_error("normalWindow color config reference is missing");
    const auto colors = config_->Read();
    return window_->ApplyColorsAt46F6D0(colors.color, colors.transparency);
}

RuntimeGraphicsSurface* MessageWindowNormalPaneRuntime::Processed() noexcept
{
    return window_->Processed();
}

const RuntimeGraphicsSurface* MessageWindowNormalPaneRuntime::Processed() const noexcept
{
    return window_->Processed();
}

void MessageWindowNormalPaneRuntime::DrawAtVslot0C(const RECT& dirty)
{
    const auto* const surface = Processed();
    if (surface) DrawSurface(graphics_, *surface, surface->OriginX(), surface->OriginY(), &dirty);
}

void MessageWindowNormalPaneRuntime::CompositeAtVslot10(
    const GraphicsSurfaceDestination destination, const RECT* const clip)
{
    const auto* const surface = Processed();
    if (surface)
        BlitSurface(graphics_, destination, *surface, surface->OriginX(), surface->OriginY(), clip);
}

MessageWindowVariablePaneRuntime::MessageWindowVariablePaneRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
    const MessageWindowVariablePaneDescriptor& descriptor,
    std::shared_ptr<const IMessageWindowColorConfig> config)
    : graphics_(graphics)
{
    try
    {
        if (!descriptor.center)
            throw std::logic_error("variableWindow center descriptor is missing");
        center_ = std::make_unique<MessageWindowNormalPaneRuntime>(
            surfaces, graphics, resources, *descriptor.center, config);
        repeat.rows = 1;
        repeat.columns = 1;
        repeat.maximumColumns = descriptor.maximumColumns;
        repeat.maximumRows = descriptor.maximumRows;
        const RECT centerBounds = center_->BoundsAtVslot04();
        const auto rightAtMaximum = Add32(centerBounds.left,
            Mul32(Sub32(centerBounds.right, centerBounds.left), repeat.maximumColumns));
        const auto bottomAtMaximum = Add32(centerBounds.top,
            Mul32(Sub32(centerBounds.bottom, centerBounds.top), repeat.maximumRows));
        for (std::uint32_t index = 0; index < descriptor.edges.size(); ++index)
        {
            if (!descriptor.edges[index]) continue;
            Edge edge{index, std::make_unique<MessageWindowNormalPaneRuntime>(
                surfaces, graphics, resources, *descriptor.edges[index], config)};
            RECT bounds = edge.pane->BoundsAtVslot04();
            if (index == 1)
            {
                bounds.right = Add32(rightAtMaximum, Sub32(bounds.right, bounds.left));
                bounds.left = rightAtMaximum;
            }
            else if (index == 3)
            {
                bounds.bottom = Add32(bottomAtMaximum, Sub32(bounds.bottom, bounds.top));
                bounds.top = bottomAtMaximum;
            }
            // 45FA88：只合并边缘；中心范围没有加入 variable bounds。
            ::UnionRect(&bounds_, &bounds_, &bounds);
            edges_.push_back(std::move(edge));
        }
    }
    catch (...)
    {
        Release();
        throw;
    }
}

MessageWindowVariablePaneRuntime::~MessageWindowVariablePaneRuntime() { Release(); }

void MessageWindowVariablePaneRuntime::Release() noexcept
{
    center_.reset();
    for (auto& edge : edges_) edge.pane.reset();
    edges_.clear();
}

std::int32_t MessageWindowVariablePaneRuntime::TypeAtVslot00() const { return 1; }
RECT MessageWindowVariablePaneRuntime::BoundsAtVslot04() const { return bounds_; }
SIZE MessageWindowVariablePaneRuntime::BaseTileSizeAt47DFE0() const
{
    return center_->BaseTileSizeAt47DFE0();
}
const MessageWindowNormalPaneRuntime& MessageWindowVariablePaneRuntime::Center() const noexcept
{
    return *center_;
}
std::size_t MessageWindowVariablePaneRuntime::EdgeCount() const noexcept { return edges_.size(); }

void MessageWindowVariablePaneRuntime::RefreshColorsAt487540()
{
    static_cast<void>(center_->RefreshColorsAt4874C0());
    for (auto& edge : edges_) static_cast<void>(edge.pane->RefreshColorsAt4874C0());
}

void MessageWindowVariablePaneRuntime::DrawAtVslot0C(const RECT& dirty)
{
    Render(0, &dirty, false);
}

void MessageWindowVariablePaneRuntime::CompositeAtVslot10(
    const GraphicsSurfaceDestination destination, const RECT* const clip)
{
    Render(destination, clip, true);
}

void MessageWindowVariablePaneRuntime::Render(const GraphicsSurfaceDestination destination,
    const RECT* const clip, const bool composite)
{
    const auto* const initial = center_->Processed();
    if (!initial) return;
    const auto originX = initial->OriginX();
    const auto originY = initial->OriginY();
    const auto width = initial->Width();
    const auto height = initial->Height();
    auto y = originY;
    for (std::int32_t row = 0; row < repeat.rows; row = Add32(row, 1))
    {
        auto x = originX;
        for (std::int32_t column = 0; column < repeat.columns; column = Add32(column, 1))
        {
            const auto* const surface = center_->Processed();
            if (!surface)
                throw std::logic_error("variableWindow center surface disappeared during tiling");
            if (composite) BlitSurface(graphics_, destination, *surface, x, y, clip);
            else DrawSurface(graphics_, *surface, x, y, clip);
            x = Add32(x, width);
        }
        y = Add32(y, height);
    }
    for (auto& edge : edges_)
    {
        if (edge.index == 0 || edge.index == 2)
        {
            if (composite) edge.pane->CompositeAtVslot10(destination, clip);
            else edge.pane->DrawAtVslot0C(*clip);
            continue;
        }
        const auto* const surface = edge.pane->Processed();
        if (!surface) continue;
        const auto x = edge.index == 1 ? Add32(originX, Mul32(width, repeat.columns)) :
            surface->OriginX();
        const auto edgeY = edge.index == 3 ? Add32(originY, Mul32(height, repeat.rows)) :
            surface->OriginY();
        if (composite) BlitSurface(graphics_, destination, *surface, x, edgeY, clip);
        else DrawSurface(graphics_, *surface, x, edgeY, clip);
    }
}
}
