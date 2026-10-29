#include <Runtime/ColorTransWindowRuntime.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Logging/Logger.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add32(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right));
}
std::int32_t Sub32(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(left) -
        static_cast<std::uint32_t>(right));
}

void UnionSurface(RECT& bounds, const RuntimeGraphicsSurface& surface)
{
    const RECT source{surface.OriginX(), surface.OriginY(),
        Add32(surface.OriginX(), surface.Width()), Add32(surface.OriginY(), surface.Height())};
    ::UnionRect(&bounds, &bounds, &source);
}

void ValidatePixelRange(const RuntimeGraphicsSurface& surface, const std::byte* pointer,
    std::int32_t pitch)
{
    // 正常引擎画布由 Graphics 创建；对损坏的布局明确失败，不裁到合法区后继续写。
    const auto rowBytes = static_cast<std::int64_t>(surface.Width()) * 4;
    const auto total = static_cast<std::int64_t>(pitch) * surface.Height();
    if (!pointer || rowBytes <= 0 || pitch < rowBytes ||
        total <= 0 || total > (std::numeric_limits<std::ptrdiff_t>::max)())
        throw std::runtime_error("ColorTransWindow has an invalid 32-bit pixel buffer");
}
}

ColorTransWindowArchiveLoader::ColorTransWindowArchiveLoader(const ResourceCatalog& catalog,
    const FileTransactionContext* transaction, IStartFileApi& files,
    const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplay& errors,
    ArchiveCodecServices codecs) noexcept
    : catalog_(catalog), transaction_(transaction), files_(files), key_(key), errors_(errors),
      codecs_(codecs) {}

bool ColorTransWindowArchiveLoader::LoadInto(RuntimeGraphicsSurface& target,
    const std::string& nameCp932)
{
    return LoadDrawManagerStaticInto(target, nameCp932, catalog_, transaction_, files_, key_, errors_, codecs_);
}

ColorTransWindowRuntime::ColorTransWindowRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& graphics, IColorTransWindowResourceLoader& resources,
    const ColorTransWindowDescriptor& descriptor)
    : graphics_(graphics), mode_(descriptor.mode)
{
    try
    {
        RECT bounds{};
        if (descriptor.firstNameCp932)
        {
            first_ = std::make_unique<RuntimeGraphicsSurface>(surfaces);
            if (!resources.LoadInto(*first_, descriptor.firstNameCp932)) throw 1;
            UnionSurface(bounds, *first_);
        }
        if (descriptor.secondNameCp932)
        {
            second_ = std::make_unique<RuntimeGraphicsSurface>(surfaces);
            if (!resources.LoadInto(*second_, descriptor.secondNameCp932)) throw 2;
            UnionSurface(bounds, *second_);
        }
        if (::IsRectEmpty(&bounds)) throw 3;
        processed_ = std::make_unique<RuntimeGraphicsSurface>(surfaces);
        if (!processed_->Create(Sub32(bounds.right, bounds.left),
            Sub32(bounds.bottom, bounds.top), true, true)) throw 3;
        processed_->SetOrigin(bounds.left, bounds.top);
        if (first_)
            first_->SetOrigin(Sub32(first_->OriginX(), bounds.left), Sub32(first_->OriginY(), bounds.top));
        if (second_)
            second_->SetOrigin(Sub32(second_->OriginX(), bounds.left), Sub32(second_->OriginY(), bounds.top));
    }
    catch (...)
    {
        // 5EA420 states 3..4 -> 45ECA7：清三份资源后正常返回空对象。
        ClearResourcesAt486A30();
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "ColorTransWindow 45ECA7: construction failed; all three resources released");
    }
}

ColorTransWindowRuntime::~ColorTransWindowRuntime() { ClearResourcesAt486A30(); }
void ColorTransWindowRuntime::ClearResourcesAt486A30() noexcept
{
    first_.reset();
    second_.reset();
    processed_.reset();
}
RuntimeGraphicsSurface* ColorTransWindowRuntime::Processed() noexcept { return processed_.get(); }
const RuntimeGraphicsSurface* ColorTransWindowRuntime::Processed() const noexcept { return processed_.get(); }
const RuntimeGraphicsSurface* ColorTransWindowRuntime::FirstSource() const noexcept { return first_.get(); }
const RuntimeGraphicsSurface* ColorTransWindowRuntime::SecondSource() const noexcept { return second_.get(); }
std::optional<ColorTransWindowAppliedColors> ColorTransWindowRuntime::AppliedColors() const noexcept { return applied_; }

bool ColorTransWindowRuntime::ApplyColorsAt46F6D0(std::uint32_t color, std::int32_t transparency)
{
    try
    {
        if (!processed_) throw 1;
        const auto clamped = (std::clamp)(transparency, 0, 255);
        if (processed_->Handle())
            graphics_.SurfaceClearRaw(processed_->Handle(), processed_->ClearColor(), nullptr);
        if (second_)
        {
            // 46F7AF..46F82F：依次读取源 ptr/pitch、目标 ptr/pitch，不合并或提前查询。
            const auto* const source = graphics_.GetSurfacePointerRaw(second_->Handle());
            const auto sourcePitch = graphics_.GetSurfacePitchRaw(second_->Handle());
            auto* const target = graphics_.GetSurfacePointerRaw(processed_->Handle());
            const auto targetPitch = graphics_.GetSurfacePitchRaw(processed_->Handle());
            if (second_->Height() > 0 && second_->Width() > 0)
            {
                ValidatePixelRange(*second_, source, sourcePitch);
                ValidatePixelRange(*processed_, target, targetPitch);
                const auto x = second_->OriginX(), y = second_->OriginY();
                if (x < 0 || y < 0 || static_cast<std::int64_t>(x) + second_->Width() > processed_->Width() ||
                    static_cast<std::int64_t>(y) + second_->Height() > processed_->Height())
                    throw std::runtime_error("ColorTransWindow rebased input exceeds its processed canvas");
                const std::uint32_t replacement = ((color & 0xFFu) << 16) |
                    (color & 0xFF00u) | ((color >> 16) & 0xFFu);
                for (std::int32_t row = 0; row < second_->Height(); ++row)
                {
                    const auto* input = source + static_cast<std::ptrdiff_t>(row) * sourcePitch;
                    auto* output = target + static_cast<std::ptrdiff_t>(y + row) * targetPitch + x * 4;
                    for (std::int32_t column = 0; column < second_->Width(); ++column)
                    {
                        std::uint32_t pixel;
                        std::memcpy(&pixel, input + column * 4, sizeof pixel);
                        if ((pixel & 0xFF000000u) == 0) continue;
                        const auto alpha = (std::max)(0, static_cast<std::int32_t>(pixel >> 24) - clamped);
                        const std::uint32_t rgb = mode_ == 0 ? (pixel & 0xFFFFFFu) : replacement;
                        const std::uint32_t result = rgb | (static_cast<std::uint32_t>(alpha) << 24);
                        std::memcpy(output + column * 4, &result, sizeof result);
                    }
                }
            }
        }
        // 4425C0 自身先检查两个 handle；返回假不阻止成功缓存的更新。
        if (first_ && processed_->Handle() && first_->Handle())
            static_cast<void>(graphics_.SurfaceBlitRaw(processed_->Handle(), first_->Handle(),
                first_->OriginX(), first_->OriginY(), nullptr, nullptr, nullptr, 0));
        applied_ = ColorTransWindowAppliedColors{color, clamped};
        return true;
    }
    catch (...)
    {
        // 5EA3D8 -> 46F9FD/46FA03：返回 false，保留之前的像素和缓存副作用。
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "ColorTransWindow 46FA03: color application failed; prior side effects retained");
        return false;
    }
}
}
