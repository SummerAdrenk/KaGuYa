#include <Runtime/WaitMarkRuntime.h>

#include <cstddef>
#include <array>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct WaitMarkResourceDescriptor final
{
    const char* nameCp932 = nullptr;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

// Start.exe:601450，两条 16 字节描述符均指定 loadFlag=1、explicitPosition=1。
constexpr std::array<WaitMarkResourceDescriptor, 2> ResourceDescriptors{{
    {"\x83\x7D\x81\x5B\x83\x4E.vrm", 1100, 650},
    {"\x83\x49\x81\x5B\x83\x67\x83\x7D\x81\x5B\x83\x4E.vrm", 1095, 650},
}};
}

WaitMarkArchiveResourceLoader::WaitMarkArchiveResourceLoader(
    SpriteArchiveResourceLoader& loader) noexcept
    : loader_(&loader)
{
}

bool WaitMarkArchiveResourceLoader::Load(
    SpriteRuntime& sprite,
    const std::string& resourceNameCp932,
    const bool playAnimationFromStart)
{
    return loader_->LoadDirect(
        sprite, resourceNameCp932, playAnimationFromStart, 0u) ==
        SpriteArchiveLoadResult::Success;
}

WaitMarkRuntime::WaitMarkRuntime(
    LayerList* const layerList,
    IGraphicsSurfaceApi& graphicsApi,
    IWaitMarkResourceLoader& resourceLoader,
    IWaitMarkStateSource& stateSource,
    IWaitMarkRenderer& renderer)
    : FrameLayerItem(layerList),
      stateSource_(&stateSource),
      renderer_(&renderer)
{
    // 45E450 在基类注册之后立即把 +0x0C 清零。
    SetEnabled(false);

    try
    {
        for (const WaitMarkResourceDescriptor& descriptor : ResourceDescriptors)
        {
            auto sprite = std::make_unique<SpriteRuntime>(graphicsApi);
            if (!resourceLoader.Load(*sprite, descriptor.nameCp932, true))
                throw 1;

            sprite->SetOrigin(descriptor.x, descriptor.y);
            const PatternSpriteRectResult drawable = sprite->LocalDrawableRect();
            RECT rect{};
            if (drawable.state == PatternSpriteRectState::Valid)
                rect = drawable.rect;
            RECT combined{};
            ::UnionRect(&combined, &bounds_, &rect);
            bounds_ = combined;
            sprites_.push_back(std::move(sprite));
        }
        completionFieldAt30_ = 0;
    }
    catch (...)
    {
        // 5EB130 的 catch-all -> 45E712：只清 bounds，保留已提交引用。
        bounds_ = {};
    }
}

WaitMarkRuntime::~WaitMarkRuntime()
{
    // 469680 -> 4615F0 -> 452320：正序释放图像，然后由基类注销图层。
    for (auto& sprite : sprites_)
        sprite.reset();
}

const RECT& WaitMarkRuntime::Bounds() const noexcept
{
    return bounds_;
}

std::size_t WaitMarkRuntime::SpriteCount() const noexcept
{
    return sprites_.size();
}

const std::optional<std::uint32_t>& WaitMarkRuntime::CompletionFieldAt30() const noexcept
{
    return completionFieldAt30_;
}

const SpriteRuntime& WaitMarkRuntime::NormalSprite() const
{
    return *sprites_.at(0);
}

const SpriteRuntime& WaitMarkRuntime::AutomaticSprite() const
{
    return *sprites_.at(1);
}

const SpriteRuntime& WaitMarkRuntime::SelectedSprite() const
{
    return *sprites_.at(stateSource_->AutomaticMarkSelected() ? 1u : 0u);
}

std::uint8_t WaitMarkRuntime::SetRequestedEnabled(
    const bool requestedEnabled,
    const std::int32_t)
{
    if (requestedEnabled)
    {
        if (stateSource_->AnyRuntimeTripletSecondByteSet() ||
            stateSource_->SlotTwoField20BitOneSet())
        {
            return 0;
        }

        if (!Enabled())
        {
            SetEnabled(true);
            InvalidateBounds();
        }
        return 0;
    }

    if (Enabled())
    {
        SetEnabled(false);
        InvalidateBounds();
    }
    return 0;
}

std::int32_t WaitMarkRuntime::DrawSelectedToSurface(
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    // 481D30 -> 46B390 -> 4425C0：第二参数原样传至 SurfaceBlt 的 clip。
    return renderer_->DrawToSurface(SelectedSprite(), destination, destinationClip);
}

void WaitMarkRuntime::Reset() noexcept
{
    // 47EE90 只清除 +0x0C，不提交旧矩形。
    SetEnabled(false);
}

void WaitMarkRuntime::Draw(const RECT& dirtyRect)
{
    static_cast<void>(renderer_->DrawToScreen(SelectedSprite(), dirtyRect));
}

void WaitMarkRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip)
{
    static_cast<void>(DrawSelectedToSurface(destination, destinationClip));
}

void WaitMarkRuntime::AdvanceOwnedFrame()
{
    AdvanceFrameAt46B1F0();
}

void WaitMarkRuntime::AdvanceFrameAt46B1F0()
{
    if (!Enabled())
        return;

    SpriteRuntime& sprite = *sprites_.at(stateSource_->AutomaticMarkSelected() ? 1u : 0u);
    if (sprite.LogicalName().empty() ||
        sprite.LoadMode() != SpriteLoadMode::Animation)
    {
        return;
    }

    // 46B1F0 只在动画解释器返回 0 时提交 WaitMark 的总包围矩形。
    if (sprite.AdvanceAnimation() == 0)
        InvalidateBounds();
}

void WaitMarkRuntime::InvalidateBounds()
{
    LayerList* const list = RegisteredLayerList();
    if (!list)
        throw std::logic_error("WaitMark invalidation requires its parent LayerList");
    list->DirtyRects().Add(bounds_);
}
}
