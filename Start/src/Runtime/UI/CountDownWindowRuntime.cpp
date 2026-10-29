#include <Runtime/CountDownWindowRuntime.h>

#include <bit>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Sub(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
RECT MoveTo(const RECT& rect, const POINT position) noexcept
{
    return {position.x, position.y, Add(position.x, Sub(rect.right, rect.left)),
        Add(position.y, Sub(rect.bottom, rect.top))};
}
RECT SpriteRect(const SpriteRuntime& sprite) noexcept
{
    if (sprite.LogicalName().empty() || static_cast<std::uint32_t>(sprite.LoadMode()) > 2u)
        return {};
    return MoveTo({0, 0, sprite.Width(), sprite.Height()},
        {std::bit_cast<std::int32_t>(sprite.OpaqueValueAt2C()),
         std::bit_cast<std::int32_t>(sprite.OpaqueValueAt30())});
}
const RuntimeGraphicsSurface& SurfaceAt(const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount()) return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        const auto count = sprite.AuxiliarySurfaceCount(group);
        if (index < count) return sprite.AuxiliarySurfaceAt(group, index);
        index -= count;
    }
    throw std::out_of_range("CountDown Sprite surface reference is missing");
}
}

CountDownWindowRuntime::CountDownWindowRuntime(LayerList* const parent,
    CountDownWindowDependencies dependencies, CountDownWindowResourceProfile resourceProfile)
    : FrameLayerItem(parent), dependencies_(dependencies), profile_(std::move(resourceProfile))
{
    Initialize(nullptr);
}

CountDownWindowRuntime::CountDownWindowRuntime(LayerList* const parent,
    CountDownWindowDependencies dependencies, const CountDownWindowFiveFrameResourceProfile& resourceProfile)
    : FrameLayerItem(parent), dependencies_(dependencies), profile_{}, fiveFrames_(true)
{
    Initialize(&resourceProfile);
}

void CountDownWindowRuntime::Initialize(const CountDownWindowFiveFrameResourceProfile* fiveFrames)
{
    SetEnabled(false);
    try
    {
        const auto load = [this](const auto& names, auto& sprites) {
            RECT result{};
            for (const auto& name : names)
            {
                auto sprite = std::make_shared<SpriteRuntime>(dependencies_.surfaces);
                if (!dependencies_.resources.LoadSpriteAt4814F0(*sprite, name, false)) throw 1;
                const RECT rect = SpriteRect(*sprite);
                RECT combined{};
                ::UnionRect(&combined, &result, &rect);
                result = combined;
                sprites.push_back(std::move(sprite));
            }
            return result;
        };
        if (fiveFrames)
        {
            const RECT bounds = load(fiveFrames->namesCp932, numbers_);
            const auto origin = fiveFrames->origin;
            // 10号466711..466780：各Sprite保留相对偏移；五帧不构造任何后缀。
            for (auto& sprite : numbers_)
                sprite->SetOpaqueValuesAt2CAnd30(
                    static_cast<std::uint32_t>(Add(std::bit_cast<std::int32_t>(sprite->OpaqueValueAt2C()), Sub(origin.x, bounds.left))),
                    static_cast<std::uint32_t>(Add(std::bit_cast<std::int32_t>(sprite->OpaqueValueAt30()), Sub(origin.y, bounds.top))));
            bounds_ = MoveTo(bounds, origin);
        }
        else
        {
            const RECT numberRect = load(profile_.numberNamesCp932, numbers_);
            const RECT suffixRect = load(profile_.suffixNamesCp932, suffixes_);
            const RECT positionedNumbers = MoveTo(numberRect, profile_.numberPositions[2]);
            const RECT positionedSuffixes = MoveTo(suffixRect, profile_.suffixPositions[2]);
            RECT combined{};
            ::UnionRect(&combined, &positionedSuffixes, &positionedNumbers);
            bounds_ = {0, 0, Sub(combined.right, combined.left), Sub(combined.bottom, combined.top)};
        }
    }
    catch (...)
    {
        // 5EB518 的唯一 HandlerType 为 catch(...)；先 suffix 后 number。
        bounds_ = {};
        ReleaseForward(suffixes_);
        ReleaseForward(numbers_);
    }
    numberIndex_ = -1;
    suffixVisible_ = 0;
}

CountDownWindowRuntime::~CountDownWindowRuntime()
{
    // 468770 与构造 catch 的向量顺序不同；每个向量内都正向释放 Ref。
    ReleaseForward(numbers_);
    ReleaseForward(suffixes_);
}

void CountDownWindowRuntime::ReleaseForward(std::vector<std::shared_ptr<SpriteRuntime>>& sprites) noexcept
{
    for (auto& sprite : sprites) sprite.reset();
    sprites.clear();
}

std::int32_t CountDownWindowRuntime::DisplayMode() const noexcept
{
    return std::bit_cast<std::int16_t>(dependencies_.globalParams.wordsAt0C[5]);
}

RECT CountDownWindowRuntime::BoundsAt479CE0() const
{
    if (fiveFrames_) return bounds_;
    const auto mode = DisplayMode();
    if (static_cast<std::uint32_t>(mode) > 1u) return {};
    return MoveTo(bounds_, profile_.suffixPositions[static_cast<std::size_t>(mode) * 3 + 2]);
}

std::uint8_t CountDownWindowRuntime::SetRequestedEnabledAtVslot10(
    const std::uint8_t requested, const std::int32_t)
{
    if (requested != 0)
    {
        const auto value = dependencies_.values.ReadSigned(
            dependencies_.executableProfile.ResolveNamedValue("countdown_suffix"));
        numberIndex_ = value;
        suffixVisible_ = fiveFrames_ ? 0 : static_cast<std::int32_t>(~static_cast<std::uint32_t>(value) >> 31);
    }
    if (Enabled() == (requested != 0)) return 0;
    SetEnabled(requested != 0);
    InvalidateBounds();
    return 1;
}

void CountDownWindowRuntime::InvalidateBounds()
{
    const RECT bounds = BoundsAt479CE0();
    auto* const parent = RegisteredLayerList();
    if (!parent) throw std::logic_error("CountDown invalidation requires parent LayerList");
    parent->DirtyRects().Add(bounds);
}

void CountDownWindowRuntime::ResetAt47EC20() noexcept
{
    SetEnabled(false);
    numberIndex_ = -1;
    suffixVisible_ = 0;
}

std::int32_t CountDownWindowRuntime::SuffixVisibleAt47C7A0() const { return suffixVisible_.value(); }
std::int32_t CountDownWindowRuntime::NumberIndexAt3C() const { return numberIndex_.value(); }
void CountDownWindowRuntime::ClearNumberAt46DC00() noexcept { numberIndex_ = -1; }
const SpriteRuntime& CountDownWindowRuntime::NumberSpriteAt(const std::size_t index) const
    { return *numbers_.at(index); }
const SpriteRuntime& CountDownWindowRuntime::SuffixSpriteAt(const std::size_t index) const
    { return *suffixes_.at(index); }

std::shared_ptr<SpriteRuntime> CountDownWindowRuntime::NumberAt47BE60(const std::int32_t mode)
{
    if (fiveFrames_)
    {
        const auto index = NumberIndexAt3C();
        return index < 0 || static_cast<std::size_t>(index) >= numbers_.size() ? nullptr : numbers_[static_cast<std::size_t>(index)];
    }
    if (static_cast<std::uint32_t>(NumberIndexAt3C()) > 4u) return {};
    const auto size = dependencies_.globalParams.wordsAt0C[6];
    const auto color = dependencies_.globalParams.wordsAt0C[7];
    const auto alpha = dependencies_.globalParams.wordsAt0C[8];
    if (size >= 3 || color >= 2 || alpha >= 2) return {};
    const auto index = static_cast<std::size_t>((size + 3 * (color + 2 * alpha)) * 5 + NumberIndexAt3C());
    if (index >= numbers_.size()) return {};
    // 原直接 getter 不验证 mode；可达调用者只传 0/1。非法现代调用明确抛错。
    if (static_cast<std::uint32_t>(mode) > 1u) throw std::out_of_range("CountDown number position mode");
    const POINT point = profile_.numberPositions[static_cast<std::size_t>(mode) * 3 + size];
    numbers_[index]->SetOpaqueValuesAt2CAnd30(static_cast<std::uint32_t>(point.x), static_cast<std::uint32_t>(point.y));
    return numbers_[index];
}

std::shared_ptr<SpriteRuntime> CountDownWindowRuntime::SuffixAt47C7B0(const std::int32_t mode)
{
    if (fiveFrames_) return {};
    if (SuffixVisibleAt47C7A0() == 0) return {};
    const auto size = dependencies_.globalParams.wordsAt0C[6];
    const auto color = dependencies_.globalParams.wordsAt0C[7];
    const auto alpha = dependencies_.globalParams.wordsAt0C[8];
    if (size >= 3 || color >= 2 || alpha >= 2) return {};
    const auto index = static_cast<std::size_t>(size + 3 * (color + 2 * alpha));
    if (index >= suffixes_.size()) return {};
    if (static_cast<std::uint32_t>(mode) > 1u) throw std::out_of_range("CountDown suffix position mode");
    const POINT point = profile_.suffixPositions[static_cast<std::size_t>(mode) * 3 + size];
    suffixes_[index]->SetOpaqueValuesAt2CAnd30(static_cast<std::uint32_t>(point.x), static_cast<std::uint32_t>(point.y));
    return suffixes_[index];
}

void CountDownWindowRuntime::AdvanceOwnedFrame()
{
    if (!Enabled()) return;
    const auto mode = fiveFrames_ ? 0 : DisplayMode();
    if (static_cast<std::uint32_t>(mode) > 1u) return;
    auto number = NumberAt47BE60(mode);
    if (number && !number->LogicalName().empty() && number->LoadMode() == SpriteLoadMode::Animation &&
        number->AdvanceAnimation() == 0)
        InvalidateBounds();
}

void CountDownWindowRuntime::Draw(const RECT& dirtyRect)
{
    const auto mode = fiveFrames_ ? 0 : DisplayMode();
    if (static_cast<std::uint32_t>(mode) > 1u) return;
    if (auto suffix = SuffixAt47C7B0(mode)) DrawSprite(*suffix, &dirtyRect, std::nullopt);
    // suffix 调用完成并释放临时 Ref 后重新查询 number，不预取两项。
    if (auto number = NumberAt47BE60(mode)) DrawSprite(*number, &dirtyRect, std::nullopt);
}

void CountDownWindowRuntime::CompositeToSurface(const GraphicsSurfaceDestination destination, const RECT* const clip)
{
    const auto mode = fiveFrames_ ? 0 : DisplayMode();
    if (static_cast<std::uint32_t>(mode) > 1u) return;
    if (auto suffix = SuffixAt47C7B0(mode)) DrawSprite(*suffix, clip, destination);
    if (auto number = NumberAt47BE60(mode)) DrawSprite(*number, clip, destination);
}

void CountDownWindowRuntime::DrawSprite(const SpriteRuntime& sprite, const RECT* const clip,
    const std::optional<GraphicsSurfaceDestination>& destination)
{
    if (sprite.LogicalName().empty()) return;
    const auto draw = [this, clip, &destination](const RuntimeGraphicsSurface& source,
                                               const std::int32_t x, const std::int32_t y) {
        if (destination)
        {
            const auto targetHandle = destination->Handle();
            if (targetHandle != 0 && source.Handle() != 0)
                static_cast<void>(dependencies_.drawing.SurfaceBlitRaw(targetHandle,
                    source.Handle(), x, y, nullptr, nullptr, clip, 0));
        }
        else if (source.Handle() != 0)
            static_cast<void>(dependencies_.drawing.SurfaceDrawRaw(source.Handle(),
                x, y, nullptr, nullptr, clip, 0));
    };
    switch (sprite.LoadMode())
    {
    case SpriteLoadMode::StaticWithoutAlpha:
    case SpriteLoadMode::StaticWithAlpha:
        draw(SurfaceAt(sprite, 0), std::bit_cast<std::int32_t>(sprite.OpaqueValueAt2C()),
            std::bit_cast<std::int32_t>(sprite.OpaqueValueAt30()));
        break;
    case SpriteLoadMode::Animation:
        for (std::size_t i = 0; i < sprite.CurrentFrameMappings().size(); ++i)
        {
            const auto mapping = sprite.CurrentFrameMappings()[i];
            if (mapping < 0) continue;
            const auto& surface = SurfaceAt(sprite, static_cast<std::size_t>(mapping));
            draw(surface, Add(std::bit_cast<std::int32_t>(sprite.OpaqueValueAt2C()), surface.OriginX()),
                Add(std::bit_cast<std::int32_t>(sprite.OpaqueValueAt30()), surface.OriginY()));
        }
        break;
    default:
        break;
    }
}
}
