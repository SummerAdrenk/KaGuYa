#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/SpriteArchiveResourceLoader.h>

#include <limits>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] SpriteRuntime& RequirePatternReceiver(const PatternSpriteReceiver receiver)
{
    if (!receiver) throw std::logic_error("Pattern Sprite receiver is null (safe boundary)");
    return *reinterpret_cast<SpriteRuntime*>(receiver);
}

class PatternLoadAction final : public IPatternResourceLoadAction
{
public:
    PatternLoadAction(PatternLayerRuntime& target, std::uint32_t slot,
        PatternSpriteReceiver receiver, IPatternResourceLoadCommit& commit) noexcept
        : target_(target), slot_(slot), receiver_(receiver), commit_(commit) {}

    void CaptureCgDirectOwnerAfterRead() override {}
    bool ResolveDirectTargetAfterInitialization() override
    {
        // 4CCBE3..4CCC0D：registry 回调之后才做 Direct gate 和解析；其他路径不调用。
        if (slot_ >= target_.SpriteSlotCount()) return false;
        receiver_ = target_.CaptureSpriteReceiver(slot_);
        return true;
    }
    bool LoadFromReader(const ArchiveReaderReference& reader, std::uint8_t initialization,
        const PatternResourceLoadRequest& request, const ArchiveCodecServices& codecs) override
    {
        return LoadPatternSpriteFromReaderModern(
            RequirePatternReceiver(receiver_), reader, initialization, request, codecs);
    }
    void CommitWhileFactoryAlive() override { commit_.Commit(); }

private:
    PatternLayerRuntime& target_;
    std::uint32_t slot_;
    PatternSpriteReceiver receiver_;
    IPatternResourceLoadCommit& commit_;
};

[[nodiscard]] std::int32_t CheckedCoordinateAdd(
    const std::int32_t first, const std::int32_t second)
{
    const auto sum = static_cast<std::int64_t>(first) + second;
    if (sum < (std::numeric_limits<std::int32_t>::min)() ||
        sum > (std::numeric_limits<std::int32_t>::max)())
    {
        throw std::overflow_error("Pattern layer coordinate overflow");
    }
    return static_cast<std::int32_t>(sum);
}

[[nodiscard]] RECT CheckedOffsetRect(
    const RECT& rect, const std::int32_t x, const std::int32_t y)
{
    return RECT{
        CheckedCoordinateAdd(rect.left, x),
        CheckedCoordinateAdd(rect.top, y),
        CheckedCoordinateAdd(rect.right, x),
        CheckedCoordinateAdd(rect.bottom, y)};
}

[[nodiscard]] LONG CheckedMirrorCoordinate(const LONG bound, const LONG value)
{
    const auto result = static_cast<std::int64_t>(bound) - value;
    if (result < (std::numeric_limits<LONG>::min)() ||
        result > (std::numeric_limits<LONG>::max)())
        throw std::overflow_error("Pattern mirrored rectangle coordinate overflow");
    return static_cast<LONG>(result);
}

void ValidateDirtyExtent(const DirtyRectCollection& dirtyRects, const RECT& candidate)
{
    RECT extent = candidate;
    for (const RECT& rect : dirtyRects.Rects())
    {
        RECT combined{};
        ::UnionRect(&combined, &extent, &rect);
        extent = combined;
    }
    if (::IsRectEmpty(&extent))
        return;
    const auto width = static_cast<std::int64_t>(extent.right) - extent.left;
    const auto height = static_cast<std::int64_t>(extent.bottom) - extent.top;
    constexpr auto maximum = (std::numeric_limits<std::int32_t>::max)();
    // 现代安全边界：先拒绝不能由现有 dirty 集合的 32 位面积运算表示的范围。
    if (width > maximum || height > maximum || width * height > maximum)
        throw std::overflow_error("Pattern dirty rectangle extent overflow");
}

void AddDirtyRectChecked(DirtyRectCollection& dirtyRects, const RECT& rect)
{
    if (::IsRectEmpty(&rect))
        return;
    ValidateDirtyExtent(dirtyRects, rect);
    dirtyRects.Add(rect);
}

[[nodiscard]] RECT SpriteRectAt4423C0(const SpriteRuntime& sprite)
{
    // 原函数对空逻辑名或非 0/1/2 mode 留下零矩形，不以 Active 过滤。
    if (sprite.LogicalName().empty() ||
        static_cast<std::uint32_t>(sprite.LoadMode()) > 2u)
    {
        return RECT{};
    }
    return CheckedOffsetRect(
        RECT{0, 0, sprite.Width(), sprite.Height()}, sprite.X(), sprite.Y());
}

[[nodiscard]] const RuntimeGraphicsSurface& SpriteSurfaceAt443580(
    const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount())
        return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        const std::size_t count = sprite.AuxiliarySurfaceCount(group);
        if (index < count)
            return sprite.AuxiliarySurfaceAt(group, index);
        index -= count;
    }
    // 原 443580 返回空对象后会被调用方解引用；现代边界明确拒绝。
    throw std::out_of_range("Pattern sprite surface reference is missing");
}
}

PatternLayerRuntime::PatternLayerRuntime(
    LayerList* const internalLayerList,
    const PatternLayerScreenSize screenSize,
    const bool surfaceHasAlpha,
    const std::uint8_t spriteSlotCount,
    IGraphicsSurfaceApi& graphicsApi,
    IGraphicsSurfaceCompositionApi& composition,
    IGraphicsWindowSurfaceApi& drawing,
    IPatternLayerResourceLoader& resourceLoader,
    IGraphicsPatternFilterApi& filters,
    IPatternFilterResourceLoader& filterResources)
    : ILayerWindowPatternLayer(internalLayerList),
      screenSize_(screenSize),
      surfaceHasAlpha_(surfaceHasAlpha),
      surface_(graphicsApi),
      composition_(&composition),
      drawing_(drawing),
      resourceLoader_(&resourceLoader),
      graphicsApi_(&graphicsApi), filters_(&filters), filterResources_(&filterResources)
{
    // 45B120 忽略 415010 的布尔返回；SurfaceCreate 失败时对象仍会完成构造。
    static_cast<void>(surface_.Create(
        screenSize.width, screenSize.height, surfaceHasAlpha, true));

    sprites_.reserve(spriteSlotCount);
    for (std::size_t index = 0; index < spriteSlotCount; ++index)
        sprites_.emplace_back(graphicsApi);
}

void PatternLayerRuntime::Draw(const RECT& dirtyRect)
{
    // 482C60 与 482C00 的源选择一致，前者固定使用屏幕原点。
    DrawAtOffsetAt482C00(0, 0, dirtyRect);
}

void PatternLayerRuntime::DrawAtOffsetAt482C00(
    const std::int32_t x, const std::int32_t y, const RECT& dirtyRect)
{
    // 482C60 按 processed 引用存在与否选源，不看 filter mode 或 Handle。
    // 提交缓存，不在 Draw 内隐式刷新。
    const RuntimeGraphicsSurface& source =
        processedSurface_ ? *processedSurface_ : surface_;
    if (source.Handle() != 0)
    {
        // 4419A9 检查源句柄，4419CC 仅检查模块；不得追加 initialized 门槛。
        static_cast<void>(drawing_.SurfaceDrawRaw(
            source.Handle(), x, y, nullptr, nullptr, &dirtyRect, 0));
    }
}

void PatternLayerRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip)
{
    // 481BD0 对正数且非 10 的 mode 必选 processed，不能回退 main。
    // 4425C0 先检查目标 handle；原空 processed 的解引用在现代边界明确拒绝。
    const auto targetHandle = destination.Handle();
    if (targetHandle == 0)
        return;
    const RuntimeGraphicsSurface* const source = filterMode_ > 0 && filterMode_ != 10
        ? processedSurface_.get() : &surface_;
    if (source == nullptr)
        throw std::logic_error("Pattern processed surface reference is missing");
    if (source->Handle() != 0)
    {
        static_cast<void>(composition_->BlitSurface(targetHandle, source->Handle(),
            0, 0, nullptr, nullptr, destinationClip, 0));
    }
}

void PatternLayerRuntime::SetMainClearColor(const std::uint32_t color) noexcept
{
    surface_.SetClearColor(color);
}

void PatternLayerRuntime::RefreshMainRectAt444230(const RECT& dirtyRect)
{
    if (surface_.Handle() != 0)
        composition_->ClearSurface(surface_.Handle(), surface_.ClearColor(), &dirtyRect);

    for (SpriteRuntime& sprite : sprites_)
    {
        if (sprite.LogicalName().empty())
            continue;

        const std::int32_t savedX = sprite.X();
        const std::int32_t savedY = sprite.Y();
        const std::int32_t drawX = CheckedCoordinateAdd(savedX, x_);
        const std::int32_t drawY = CheckedCoordinateAdd(savedY, y_);
        sprite.SetOrigin(drawX, drawY);
        // 444230 无 EH 恢复：仅正常返回才还原坐标。绘制抛出时必须保留偏移。
        const auto blit = [&](const RuntimeGraphicsSurface& source,
                              const std::int32_t x, const std::int32_t y) {
            // 4425C0 对已有对象的零 Handle 返回 false，调用者不检查返回值。
            if (surface_.Handle() != 0 && source.Handle() != 0)
            {
                static_cast<void>(composition_->BlitSurface(surface_.Handle(),
                    source.Handle(), x, y, nullptr, nullptr, &dirtyRect, 0));
            }
        };
        switch (sprite.LoadMode())
        {
        case SpriteLoadMode::StaticWithoutAlpha:
        case SpriteLoadMode::StaticWithAlpha:
            blit(SpriteSurfaceAt443580(sprite, 0u), drawX, drawY);
            break;
        case SpriteLoadMode::Animation:
            for (const std::int32_t mapping : sprite.CurrentFrameMappings())
            {
                if (mapping < 0)
                    continue;
                const RuntimeGraphicsSurface& source =
                    SpriteSurfaceAt443580(sprite, static_cast<std::size_t>(mapping));
                const std::int32_t frameX = CheckedCoordinateAdd(drawX, source.OriginX());
                const std::int32_t frameY = CheckedCoordinateAdd(drawY, source.OriginY());
                blit(source, frameX, frameY);
            }
            break;
        default:
            break;
        }
        sprite.SetOrigin(savedX, savedY);
    }
}

void PatternLayerRuntime::CreateProcessedSurfaceAt444470(const bool copyPixels)
{
    auto created = std::make_unique<RuntimeGraphicsSurface>(*graphicsApi_);
    // 444470 使用主 CSurface 的实际尺寸/alpha；失败不能伪造有效空画布。
    if (!created->Create(surface_.Width(), surface_.Height(), surface_.HasAlpha(), true))
        return;
    if (copyPixels)
    {
        filters_->SurfaceCopyBlitRaw(created->Handle(), surface_.Handle(),
            0, 0, nullptr, nullptr, nullptr, 0);
        created->SetOrigin(surface_.OriginX(), surface_.OriginY());
    }
    // 原临时 ref 在复制正常返回后才交给 +64；复制抛出时不能留下半提交 owner。
    processedSurface_ = std::move(created);
}

RECT PatternLayerRuntime::RefreshProcessedRectAt443B00(const RECT& dirtyRect)
{
    RECT affected = dirtyRect;
    if (!processedSurface_)
    {
        const std::int32_t initialMode = filterMode_;
        CreateProcessedSurfaceAt444470(initialMode != 11);
        if (initialMode == 11)
            affected = RECT{0, 0, screenSize_.width, screenSize_.height};
    }
    // 原 clone 失败会在后续解引用空 ref；不照搬访问违规，也不回退主画布。
    if (!processedSurface_)
        throw std::runtime_error("Pattern processed surface creation failed");

    switch (filterMode_)
    {
    case 3:
        static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
            filterArguments_, *processedSurface_, &surface_,
            temporaryFilterSurface_.get(), &dirtyRect));
        break;
    case 6:
    {
        filters_->SurfaceCopyBlitRaw(processedSurface_->Handle(), surface_.Handle(),
            0, 0, nullptr, nullptr, &dirtyRect, 0);
        // 443E80保存原参数地址；443F31..50解Pattern后，443F52才读首DWORD。
        const std::span<const std::uint32_t> operands(filterArguments_);
        const auto root = filterResources_->CaptureParamsForNames();
        const auto names = root.CapturePatternNames();
        if (operands.empty())
            throw std::out_of_range("Pattern resource filter requires a resource ID");
        const char* const rawName = names.FindNameAt4AD1B0(std::bit_cast<std::int32_t>(operands.front()));
        const std::string name(rawName ? rawName : ""); // 443FBA复制，早于临时图创建/加载。
        if (!temporaryFilterSurface_)
            temporaryFilterSurface_ = std::make_shared<RuntimeGraphicsSurface>(*graphicsApi_);
        if (filterResources_->LoadInto(*temporaryFilterSurface_, name))
            static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
                filterArguments_, *processedSurface_, nullptr,
                temporaryFilterSurface_.get(), &dirtyRect));
        // 4800A0 正常 false/true 均释放；Show 抛出时原成员仍存活，不能用局部 RAII 抹去。
        temporaryFilterSurface_.reset();
        break;
    }
    case 11:
    {
        if (filterArguments_.empty())
            throw std::out_of_range("Pattern flip filter requires flags");
        RECT mirrored = affected;
        const std::uint32_t flags = filterArguments_.front();
        if ((flags & 1u) != 0)
        {
            mirrored.left = CheckedMirrorCoordinate(screenSize_.width, affected.right);
            mirrored.right = CheckedMirrorCoordinate(screenSize_.width, affected.left);
        }
        if ((flags & 2u) != 0)
        {
            mirrored.top = CheckedMirrorCoordinate(screenSize_.height, affected.bottom);
            mirrored.bottom = CheckedMirrorCoordinate(screenSize_.height, affected.top);
        }
        filters_->SurfaceCopyBlitRaw(processedSurface_->Handle(), surface_.Handle(),
            mirrored.left, mirrored.top, nullptr, &affected, &mirrored, 0);
        static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
            filterArguments_, *processedSurface_, nullptr,
            temporaryFilterSurface_.get(), &mirrored));
        affected = mirrored;
        break;
    }
    default:
        filters_->SurfaceCopyBlitRaw(processedSurface_->Handle(), surface_.Handle(),
            0, 0, nullptr, nullptr, &dirtyRect, 0);
        static_cast<void>(ApplyPatternPixelFilterAt442900(*filters_, filterMode_,
            filterArguments_, *processedSurface_, nullptr,
            temporaryFilterSurface_.get(), &dirtyRect));
        break;
    }
    return affected;
}

void PatternLayerRuntime::RefreshMainAndPropagateAt443B00()
{
    ValidateDirtyExtent(dirtyRects_, RECT{});
    dirtyRects_.Coalesce();
    if (dirtyRects_.Count() != 0u)
    {
        if (filterRefreshPending_)
        {
            processedSurface_.reset();
            filterRefreshPending_ = false;
        }
        const RECT bounds{0, 0, screenSize_.width, screenSize_.height};
        for (const RECT& dirtyRect : dirtyRects_.Rects())
        {
            RECT affected = dirtyRect;
            if (Enabled())
            {
                RefreshMainRectAt444230(dirtyRect);
                if (filterMode_ > 0 && filterMode_ != 10)
                    affected = RefreshProcessedRectAt443B00(dirtyRect);
            }
            RECT clipped{};
            ::IntersectRect(&clipped, &affected, &bounds);
            LayerList* const parent = RegisteredLayerList();
            if (!parent)
                throw std::logic_error("Pattern dirty propagation requires a parent layer list");
            AddDirtyRectChecked(parent->DirtyRects(), clipped);
        }
        dirtyRects_.Clear();
    }
    // 即使无 dirty 也释放非像素滤镜的 processed；pending 仅在非空分支消费。
    if (filterMode_ <= 0 || filterMode_ == 10)
        processedSurface_.reset();
}

void PatternLayerRuntime::AdvanceFrameAt4423C0()
{
    if (!Enabled())
        return;
    for (SpriteRuntime& sprite : sprites_)
    {
        const RECT oldRect = SpriteRectAt4423C0(sprite);
        if (!sprite.LogicalName().empty() &&
            sprite.LoadMode() == SpriteLoadMode::Animation &&
            sprite.AdvanceAnimation() == 0)
        {
            AddDirtyRectChecked(dirtyRects_, CheckedOffsetRect(oldRect, x_, y_));
            const RECT newRect = SpriteRectAt4423C0(sprite);
            AddDirtyRectChecked(dirtyRects_, CheckedOffsetRect(newRect, x_, y_));
        }
    }
    RefreshMainAndPropagateAt443B00();
}

const PatternLayerScreenSize& PatternLayerRuntime::ScreenSize() const noexcept
{
    return screenSize_;
}

bool PatternLayerRuntime::SurfaceHasAlpha() const noexcept
{
    return surfaceHasAlpha_;
}

GraphicsSurfaceHandle PatternLayerRuntime::SurfaceHandle() const noexcept
{
    return surface_.Handle();
}

SpriteRuntime& PatternLayerRuntime::SpriteAt(const std::size_t index)
{
    return sprites_.at(index);
}

const SpriteRuntime& PatternLayerRuntime::SpriteAt(const std::size_t index) const
{
    return sprites_.at(index);
}

DirtyRectCollection& PatternLayerRuntime::DirtyRects() noexcept
{
    return dirtyRects_;
}

const DirtyRectCollection& PatternLayerRuntime::DirtyRects() const noexcept
{
    return dirtyRects_;
}

void PatternLayerRuntime::SetLayerPosition(
    const std::int32_t x,
    const std::int32_t y) noexcept
{
    x_ = x;
    y_ = y;
}

bool PatternLayerRuntime::FilterRefreshPending() const noexcept
{
    return filterRefreshPending_;
}

std::uint8_t PatternLayerRuntime::ResultValueAt10() const noexcept
{
    return resultValueAt10_;
}

bool PatternLayerRuntime::HasPatternItem() const noexcept
{
    return hasPatternItem_;
}

std::size_t PatternLayerRuntime::SpriteSlotCount() const noexcept
{
    return sprites_.size();
}

std::int32_t PatternLayerRuntime::LayerX() const noexcept { return x_; }
std::int32_t PatternLayerRuntime::LayerY() const noexcept { return y_; }

const char* PatternLayerRuntime::LogicalNameCharacters(
    const std::size_t spriteSlotIndex) const
{
    return sprites_.at(spriteSlotIndex).LogicalName().c_str();
}

std::int32_t PatternLayerRuntime::LogicalNameLength(const std::size_t spriteSlotIndex) const
{
    // 4CB9E9/4CB9EC的旧显示资格独立读取长度，不以首字符或strlen替代。
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(sprites_.at(spriteSlotIndex).LogicalName().size()));
}

PatternSpriteRectResult PatternLayerRuntime::LocalDrawableRect(
    const std::size_t spriteSlotIndex) const
{
    return sprites_.at(spriteSlotIndex).LocalDrawableRect();
}

void PatternLayerRuntime::ClearSlot(const std::size_t spriteSlotIndex)
{
    sprites_.at(spriteSlotIndex).Clear();
}

PatternSpriteReceiver PatternLayerRuntime::CaptureSpriteReceiver(const std::uint32_t slotIndex) const
{
    // Exc/Convert 可先得到空 receiver，安全拒绝留到 factory/registry 后的首次使用点。
    if (slotIndex >= sprites_.size()) return 0;
    return reinterpret_cast<PatternSpriteReceiver>(&sprites_[slotIndex]);
}

void PatternLayerRuntime::LoadResource(const std::uint32_t slotIndex,
    const PatternSpriteReceiver capturedReceiver, const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadCommit& commit,
    IRuntimeStringManagerSource& strings)
{
    PatternLoadAction action(*this, slotIndex, capturedReceiver, commit);
    resourceLoader_->ExecuteLoad(request, failureName, action, strings);
}

const char* PatternLayerRuntime::ReceiverPrimaryNameCharacters(const PatternSpriteReceiver receiver) const
{
    return RequirePatternReceiver(receiver).PrimaryMemberName().c_str();
}

PatternResourceNamesBorrow PatternLayerRuntime::ReceiverAuxiliaryNames(
    const PatternSpriteReceiver receiver) const
{
    return PatternResourceNamesBorrow(RequirePatternReceiver(receiver).AuxiliaryMemberNames());
}

void PatternLayerRuntime::PrepareReceiverFileGroup(
    const PatternSpriteReceiver receiver, const FileGroupChange changes)
{
    RequirePatternReceiver(receiver).PrepareFileGroup(changes);
}

void PatternLayerRuntime::SetReceiverLogicalName(
    const PatternSpriteReceiver receiver, const RuntimeCString& source)
{
    RequirePatternReceiver(receiver).SetLogicalNameModernFromCString(source);
}

void PatternLayerRuntime::SetReceiverOrigin(
    const PatternSpriteReceiver receiver, const std::int32_t x, const std::int32_t y)
{
    RequirePatternReceiver(receiver).SetOrigin(x, y);
}

void PatternLayerRuntime::SetReceiverActive(const PatternSpriteReceiver receiver, const bool active)
{
    RequirePatternReceiver(receiver).SetActive(active);
}

PatternSpriteRectResult PatternLayerRuntime::ReceiverDrawableRect(const PatternSpriteReceiver receiver) const
{
    return RequirePatternReceiver(receiver).LocalDrawableRect();
}

std::int32_t PatternLayerRuntime::CurrentPatternItemIndex() const noexcept
{
    return currentPatternItemIndex_;
}

bool PatternLayerRuntime::DrawingEnabled() const noexcept
{
    return Enabled();
}

RECT PatternLayerRuntime::ComputePatternUnionRect() const
{
    RECT result{};
    bool hasRect = false;
    for (std::size_t index = 0; index < sprites_.size(); ++index)
    {
        const PatternSpriteRectResult drawable = sprites_[index].LocalDrawableRect();
        if (drawable.state == PatternSpriteRectState::Empty)
            continue;
        if (drawable.state == PatternSpriteRectState::InvalidLoadMode)
        {
            throw PatternResourceExecutionException(
                PatternResourceExecutionError::InvalidSpriteState,
                index, sprites_[index].LogicalName());
        }

        if (!hasRect)
        {
            result = drawable.rect;
            hasRect = true;
        }
        else
        {
            RECT combined{};
            ::UnionRect(&combined, &result, &drawable.rect);
            result = combined;
        }
    }

    if (hasRect)
        ::OffsetRect(&result, x_, y_);
    return result;
}

void PatternLayerRuntime::ClearAllPatternItems()
{
    for (SpriteRuntime& sprite : sprites_)
        sprite.Clear();
    filterMode_ = 0;
    filterArguments_.clear();
}

void PatternLayerRuntime::ResetForRootAt47EC71(IGraphicsWindowSurfaceApi& drawing)
{
    x_ = 0;
    y_ = 0;
    currentPatternItemIndex_ = -1;
    hasPatternItem_ = false;
    resultValueAt10_ = 0;
    // 442700(-1) 只清 Sprite；不能调用还会提前清滤镜的 ClearAllPatternItems。
    for (auto& sprite : sprites_) sprite.Clear();
    if (surface_.Handle()) drawing.SurfaceClearRaw(surface_.Handle(), surface_.ClearColor(), nullptr);
    dirtyRects_.Clear();
    filterMode_ = 0;
    filterArguments_.clear();
    filterRefreshPending_ = false;
    processedSurface_.reset();
    // 原47ED3E仅在以上全部完成后写enabled=0；+54临时滤镜资源保留。
    SetEnabledByteAt0C(0);
}

void PatternLayerRuntime::SetCurrentPatternItemIndex(
    const std::int32_t index) noexcept
{
    currentPatternItemIndex_ = index;
}

std::int32_t PatternLayerRuntime::FilterMode() const noexcept
{
    return filterMode_;
}

std::span<const std::uint32_t> PatternLayerRuntime::FilterArguments() const noexcept
{
    return filterArguments_;
}

void PatternLayerRuntime::ResetFilterBeforePatternLoad()
{
    filterMode_ = 0;
    filterArguments_.clear();
}

void PatternLayerRuntime::SetFilterParameters(
    const std::int32_t mode,
    const std::span<const std::uint32_t> arguments)
{
    filterMode_ = mode;
    filterArguments_.assign(arguments.begin(), arguments.end());
}

void PatternLayerRuntime::MarkFilterRefreshPending() noexcept
{
    filterRefreshPending_ = true;
}

void PatternLayerRuntime::ApplyFilterCommand()
{
    if (filterMode_ != 10)
        return;
    if ((filterArguments_.size() & 1u) != 0u)
        throw std::runtime_error("Pattern animation filter argument pair is truncated");

    for (std::size_t argumentIndex = 0;
         argumentIndex < filterArguments_.size();
         argumentIndex += 2u)
    {
        const std::size_t spriteIndex = filterArguments_[argumentIndex];
        if (spriteIndex >= sprites_.size())
            throw std::out_of_range("Pattern animation filter sprite index is out of range");

        SpriteRuntime& sprite = sprites_[spriteIndex];
        const std::int32_t command = static_cast<std::int32_t>(
            filterArguments_[argumentIndex + 1u]);
        if (command == 0)
        {
            if (sprite.LoadMode() == SpriteLoadMode::Animation)
                sprite.RestartAnimation(true);
            continue;
        }

        // 4CAF10 对非零命令要求逻辑名非空且 Sprite 为动画，否则抛出异常码 10。
        if (sprite.LogicalName().empty() ||
            sprite.LoadMode() != SpriteLoadMode::Animation)
        {
            throw std::runtime_error("Pattern animation filter requires an animation sprite");
        }
        sprite.ApplyPatternFilterAnimationCommand(command);
    }
}

std::int32_t PatternLayerRuntime::X() const noexcept { return x_; }
std::int32_t PatternLayerRuntime::Y() const noexcept { return y_; }

void PatternLayerRuntime::SetPosition(
    const std::int32_t x,
    const std::int32_t y) noexcept
{
    x_ = x;
    y_ = y;
}

void PatternLayerRuntime::SetResultValueAt10(const std::uint8_t value) noexcept
{
    resultValueAt10_ = value;
}

void PatternLayerRuntime::SetHasPatternItem(const bool hasPatternItem) noexcept
{
    hasPatternItem_ = hasPatternItem;
}

void PatternLayerRuntime::Invalidate(const RECT& rect)
{
    AddDirtyRectChecked(dirtyRects_, rect);
}

PatternLayerRuntimeFactory::PatternLayerRuntimeFactory(
    IGraphicsSurfaceApi& graphicsApi,
    IGraphicsSurfaceCompositionApi& composition,
    IGraphicsWindowSurfaceApi& drawing,
    IPatternLayerResourceLoader& resourceLoader,
    IGraphicsPatternFilterApi& filters,
    IPatternFilterResourceLoader& filterResources) noexcept
    : graphicsApi_(&graphicsApi), composition_(&composition), drawing_(drawing), resourceLoader_(&resourceLoader),
      filters_(&filters), filterResources_(&filterResources)
{
}

std::unique_ptr<ILayerWindowPatternLayer> PatternLayerRuntimeFactory::Create(
    const std::size_t,
    LayerList& internalLayerList,
    const PatternLayerScreenSize screenSize,
    const bool nonZeroIndexFlag,
    const std::uint8_t spriteSlotCount)
{
    return std::make_unique<PatternLayerRuntime>(&internalLayerList,
        screenSize, nonZeroIndexFlag, spriteSlotCount,
        *graphicsApi_, *composition_, drawing_, *resourceLoader_, *filters_, *filterResources_);
}
}
