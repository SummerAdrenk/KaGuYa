#include <Runtime/SpriteRuntime.h>
#include <Runtime/AnimationResourceGroupRuntime.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveReaderNative.h>

#include <mbstring.h>

#include <bit>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
class SpriteRuntime::ResourceInput final
{
public:
    explicit ResourceInput(const ArchiveReaderReference& reader) noexcept : reader_(&reader) {}
    ResourceInput(const ArchiveReaderReference& reader, const ArchiveCodecServices& codecs) noexcept
        : reader_(&reader), codecs_(&codecs) {}
    ResourceInput(const std::string& name, std::span<const std::byte> bytes) noexcept
        : modernName_(&name), modernBytes_(bytes) {}

    const char* Name() const
    {
        return reader_ ? CurrentReader().NameAt4179F0() : modernName_->c_str();
    }
    std::span<const std::byte> Bytes() const
    {
        if (!reader_) return modernBytes_;
        // 现代公共view允许空outerRef；原Sprite此处必调虚槽，显式拒绝而不伪装格式失败。
        static_cast<void>(CurrentReader());
        return ArchiveReaderPayloadView(*reader_);
    }
    bool LoadStatic(RuntimeGraphicsSurface& surface, std::uint32_t flags) const
    {
        // 419D70独立Ready/Bytes/Name，不能把AN/PL探测时的span交给静态回落。
        return reader_ ? surface.LoadArchiveReaderAt419D70(*reader_, flags)
                       : surface.LoadStatic(modernBytes_, flags);
    }
    ParsedPictureLayerResource ParsePictureLayer(std::span<const std::byte> bytes) const
    {
        if (!reader_) return ParsePictureLayerResourceForTools(bytes);
        // 生产reader分支没有local codec兜底；只有辅助成员构造口携带显式services。
        if (!codecs_) throw std::logic_error("Native PL input requires archive codec services");
        return ParsePictureLayerResource(bytes, *codecs_);
    }
    AnimationParseState ParseAnimation(std::span<const std::byte> bytes,
        ParsedAnimationResource& output, bool fileGroup = false) const
    {
        // 只有显式名称/span工具构造口可以调用工具算法；生产reader绝不降级。
        if (!reader_) return fileGroup ? ParseFileGroupAnimationResourceForTools(bytes, output) :
            ParseAnimationResourceForTools(bytes, output);
        if (!codecs_) throw std::logic_error("Native AN input requires archive codec services");
        return fileGroup ? ParseFileGroupAnimationResource(bytes, output, *codecs_) :
            ParseAnimationResource(bytes, output, *codecs_);
    }
    bool IsNative() const noexcept { return reader_ != nullptr; }
private:
    ArchiveStreamReaderRuntime& CurrentReader() const
    {
        auto* const body = reader_->Get();
        if (!body) throw std::logic_error("Sprite archive reader body is null (safe boundary)");
        return *body;
    }
    const ArchiveReaderReference* reader_ = nullptr;
    const ArchiveCodecServices* codecs_ = nullptr;
    const std::string* modernName_ = nullptr;
    std::span<const std::byte> modernBytes_{};
};

namespace
{
[[nodiscard]] std::int32_t AddWrapping(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) +
        std::bit_cast<std::uint32_t>(right));
}

[[nodiscard]] std::int32_t SubWrapping(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) -
        std::bit_cast<std::uint32_t>(right));
}

[[nodiscard]] RECT MakeRect(
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t width,
    const std::int32_t height) noexcept
{
    return RECT{x, y, AddWrapping(x, width), AddWrapping(y, height)};
}
}

void SpriteAnimationRuntimeState::Reset() noexcept
{
    stateFlags = 0;
    waitTicks = 0;
    commandIndex = 0;
    resultCode = -1;
    repeatCommands = false;
    frameMappingSlotCount = 0;
    currentFrameMappings.clear();
    finalFrameMappings.clear();
    branchRepeatCounters.clear();
    commands.clear();
    branches.clear();
}

void SpriteAnimationRuntimeState::Initialize(
    const ParsedAnimationResource& resource,
    const bool playFromStart)
{
    Reset();
    repeatCommands = resource.controlFlag;
    frameMappingSlotCount = resource.frameMappingSlotCount;
    commands = resource.commands;
    branches = resource.branches;
    currentFrameMappings.assign(
        static_cast<std::size_t>(frameMappingSlotCount), -1);
    finalFrameMappings.assign(
        static_cast<std::size_t>(frameMappingSlotCount), -1);
    for (const AnimationCommand& command : commands)
    {
        if (command.opcode == AnimationCommandOpcode::SetFrameMapping &&
            command.first >= 0 && command.first < frameMappingSlotCount)
        {
            finalFrameMappings[static_cast<std::size_t>(command.first)] =
                command.second;
        }
    }
    branchRepeatCounters.assign(branches.size(), 0);
    Restart(playFromStart);
}

void SpriteAnimationRuntimeState::Restart(const bool playFromStart)
{
    currentFrameMappings.assign(
        static_cast<std::size_t>(frameMappingSlotCount), -1);
    branchRepeatCounters.assign(branches.size(), 0);
    stateFlags = 1;
    waitTicks = 0;
    commandIndex = 0;
    if (Advance() < 0)
    {
        stateFlags = 0;
        waitTicks = 0;
        commandIndex = 0;
        return;
    }

    stateFlags = 1;
    if (!playFromStart)
    {
        stateFlags = 3;
        currentFrameMappings = finalFrameMappings;
        waitTicks = 0;
        commandIndex = static_cast<std::int32_t>(commands.size()) - 1;
    }
}

void SpriteAnimationRuntimeState::ApplyPatternFilterCommand(
    const std::int32_t commandCount)
{
    if (commands.empty())
        return;

    if (commandCount < 0 &&
        commands.back().opcode ==
            AnimationCommandOpcode::DecrementBranchAndJumpIfPositive)
    {
        // 4CAF10 -> 4C1D20/4C23A0：推进到末尾分支前，暂停并复制最终帧映射。
        AdvanceToFinalCommandAt4C1D20();
        stateFlags |= 2;
        currentFrameMappings = finalFrameMappings;
        return;
    }

    // 4D5790：从命令 0 开始临时关闭循环，按等待段数量推进；负值表示推进至结束。
    const std::int32_t savedStateFlags = stateFlags;
    const bool savedRepeatCommands = repeatCommands;
    stateFlags = 1;
    waitTicks = 0;
    commandIndex = 0;
    repeatCommands = false;

    std::int32_t completedSegments = 0;
    while (stateFlags != 0)
    {
        waitTicks = 0;
        if (Advance() < 0)
            break;
        if (commandCount >= 0 && ++completedSegments >= commandCount)
            break;
    }

    repeatCommands = savedRepeatCommands;
    if (stateFlags != 0)
        stateFlags = savedStateFlags;
}

void SpriteAnimationRuntimeState::AdvanceToFinalCommandAt4C1D20()
{
    const std::int32_t savedStateFlags = stateFlags;
    stateFlags = 1;
    const std::int32_t finalCommandIndex =
        static_cast<std::int32_t>(commands.size()) - 1;
    while (commandIndex < finalCommandIndex)
    {
        waitTicks = 0;
        if (Advance() < 0)
            break;
    }
    if (stateFlags != 0)
        stateFlags = savedStateFlags;
}

void SpriteAnimationRuntimeState::CopyFinalFrameMappingsAt4C23A0()
{
    // 原非法 Ref 会访问无效内存；这里显式拒绝，不扩容、不补 -1 映射。
    if (frameMappingSlotCount < 0 ||
        static_cast<std::size_t>(frameMappingSlotCount) > currentFrameMappings.size() ||
        static_cast<std::size_t>(frameMappingSlotCount) > finalFrameMappings.size())
        throw std::length_error("Sprite final mapping storage is shorter than its fixed slot count");
    if (frameMappingSlotCount != 0)
        std::memmove(currentFrameMappings.data(), finalFrameMappings.data(),
            static_cast<std::size_t>(frameMappingSlotCount) * sizeof(std::int32_t));
}

std::int32_t SpriteAnimationRuntimeState::Advance() noexcept
{
    if ((stateFlags & 1) == 0)
        return 1;
    if ((stateFlags & 2) != 0)
        return 2;
    if (waitTicks != 0)
    {
        --waitTicks;
        return 2;
    }

    bool encounteredWait = false;
    while (true)
    {
        if (commandIndex < 0 ||
            commandIndex >= static_cast<std::int32_t>(commands.size()))
        {
            return -1;
        }

        const AnimationCommand& command =
            commands[static_cast<std::size_t>(commandIndex++)];
        switch (command.opcode)
        {
        case AnimationCommandOpcode::NoOperation:
            break;
        case AnimationCommandOpcode::SetFrameMapping:
            if (command.first < 0 || command.first >= frameMappingSlotCount)
                return -1;
            currentFrameMappings[static_cast<std::size_t>(command.first)] =
                command.second;
            break;
        case AnimationCommandOpcode::WaitTicks:
            waitTicks = command.first;
            encounteredWait = true;
            break;
        case AnimationCommandOpcode::LoadBranchRepeatCounter:
            if (command.first < 0 ||
                command.first >= static_cast<std::int32_t>(branches.size()))
            {
                return -1;
            }
            branchRepeatCounters[static_cast<std::size_t>(command.first)] =
                branches[static_cast<std::size_t>(command.first)].repeatCount;
            break;
        case AnimationCommandOpcode::DecrementBranchAndJumpIfPositive:
            if (command.first < 0 ||
                command.first >= static_cast<std::int32_t>(branches.size()))
            {
                return -1;
            }
            {
                const std::size_t branchIndex =
                    static_cast<std::size_t>(command.first);
                if (branches[branchIndex].repeatCount > 0)
                {
                    // 443882 / 4C1E96：有限计数耗尽时保留取指后的 index，不跳回。
                    if (branchRepeatCounters[branchIndex] == 0)
                        break;
                    branchRepeatCounters[branchIndex] = AddWrapping(branchRepeatCounters[branchIndex], -1);
                }
                commandIndex = branches[branchIndex].jumpCommandIndex;
            }
            break;
        case AnimationCommandOpcode::SetAnimationResultCode:
            resultCode = command.first;
            break;
        default:
            return -1;
        }

        if (commandIndex >= static_cast<std::int32_t>(commands.size()))
        {
            commandIndex = 0;
            if (!repeatCommands)
            {
                stateFlags = 0;
                return 0;
            }
        }
        if (encounteredWait)
            return 0;
    }
}

SpriteRuntime::SpriteRuntime(IGraphicsSurfaceApi& graphicsApi) noexcept
    : graphicsApi_(&graphicsApi)
{
    animation_.Reset();
}

void SpriteRuntime::Clear()
{
    // 顺序对应 43F140；Surface vector 的 clear 会释放句柄但保留容量。
    loadMode_ = SpriteLoadMode::StaticWithoutAlpha;
    active_ = false;
    width_ = 0;
    height_ = 0;
    x_ = 0;
    y_ = 0;
    logicalNameCp932_.clear();
    primaryMemberNameCp932_.clear();
    auxiliaryMemberNamesCp932_.clear();
    loadArgument_ = 0;
    primarySurfaces_.clear();
    auxiliarySurfaces_.clear();
    animation_.Reset();
}

bool SpriteRuntime::LoadDirectStatic(
    const std::string& resourceNameCp932,
    const std::uintptr_t loadArgument,
    const std::span<const std::byte> bytes)
{
    return LoadDirectInput(ResourceInput(resourceNameCp932, bytes), false, loadArgument, false);
}

bool SpriteRuntime::LoadDirectResource(
    const std::string& resourceNameCp932,
    const bool playAnimationFromStart,
    const std::uintptr_t loadArgument,
    const std::span<const std::byte> bytes)
{
    return LoadDirectInput(ResourceInput(resourceNameCp932, bytes),
        playAnimationFromStart, loadArgument, true);
}

bool SpriteRuntime::LoadArchiveReaderAt480650(const ArchiveReaderReference& reader,
    std::uint8_t restart, std::uintptr_t loadArgument,
    const ArchiveCodecServices& codecs, bool* cacheHit)
{
    return LoadDirectInput(ResourceInput(reader, codecs), restart != 0, loadArgument, true, cacheHit);
}

bool SpriteRuntime::LoadDirectStatic(const ArchiveReaderReference& reader, std::uintptr_t loadArgument)
{
    return LoadDirectInput(ResourceInput(reader), false, loadArgument, false);
}

bool SpriteRuntime::LoadDirectInput(const ResourceInput& input,
    bool playAnimationFromStart, std::uintptr_t loadArgument, bool parseAnimation, bool* cacheHit)
{
    const char* const cacheName = input.Name(); // 48069C：只用于本次比较，不供末尾命名。
    if (!cacheName) throw ATL::CAtlException(E_FAIL); // 480821
    const bool cached = DirectCacheMatches(cacheName, loadArgument);
    if (cacheHit) *cacheHit = cached; // 现代观察口，不让CountDown为缓存判断提前再调一次getter。
    if (cached) return true;

    Clear();
    ParsedAnimationResource animationResource;
    AnimationParseState parseState = AnimationParseState::NotAnimation;
    std::span<const std::byte> animationBytes;
    if (parseAnimation)
    {
        // getter在448010的解析catch之外；getter抛出的异常不能误折叠成格式失败。
        animationBytes = input.Bytes(); // 4806EC，Clear释放回调之后重新解析outerRef。
        try { parseState = input.ParseAnimation(animationBytes, animationResource); }
        catch (const AnimationResourceException&) { return false; }
        catch (int code)
        {
            // 448220原int catch返回原值；47EF61..6D把3映射为静态，其他非零失败。
            if (!input.IsNative()) throw;
            if (code != 0 && code != 3) return false;
            parseState = code == 3 ? AnimationParseState::NotAnimation : AnimationParseState::Success;
        }
        catch (...)
        {
            // 44823D catch-all返回-1；不捕获前面的Bytes虚getter异常。
            if (!input.IsNative()) throw;
            return false;
        }
    }

    if (parseState == AnimationParseState::NotAnimation)
    {
        primarySurfaces_.emplace_back(*graphicsApi_);
        if (!input.LoadStatic(primarySurfaces_.front(), static_cast<std::uint32_t>(loadArgument)))
        {
            // 480795保留新Surface，但不提交逻辑名、active与+10；不再次Clear/比较缓存。
            return false;
        }
        const auto& surface = primarySurfaces_.front();
        loadMode_ = surface.HasAlpha() ? SpriteLoadMode::StaticWithAlpha : SpriteLoadMode::StaticWithoutAlpha;
        width_ = surface.Width();
        height_ = surface.Height();
        x_ = surface.OriginX();
        y_ = surface.OriginY();
    }
    else
    {
        std::vector<RuntimeGraphicsSurface> frames;
        frames.reserve(animationResource.frames.size());
        for (const AnimationFrameDescriptor& frame : animationResource.frames)
        {
            const auto pixels = frame.Pixels(animationBytes);
            frames.emplace_back(*graphicsApi_);
            if (!frames.back().LoadAnimationFrame(frame.originX, frame.originY,
                    frame.width, frame.height, frame.channels, pixels.data()))
                return false;
        }
        loadMode_ = SpriteLoadMode::Animation;
        width_ = animationResource.canvasWidth;
        height_ = animationResource.canvasHeight;
        x_ = animationResource.canvasX;
        y_ = animationResource.canvasY;
        primarySurfaces_ = std::move(frames);
        animation_.Initialize(animationResource, playAnimationFromStart);
    }

    const char* const finalName = input.Name(); // 4807DF，允许与缓存比较使用的名称不同。
    logicalNameCp932_ = finalName ? finalName : "";
    active_ = true;
    loadArgument_ = loadArgument;
    return true;
}

bool SpriteRuntime::LoadFileGroupPrimaryStatic(
    const std::string& memberNameCp932,
    const std::uint32_t bitmapFlags,
    const std::span<const std::byte> bytes)
{
    return LoadPrimaryInput(ResourceInput(memberNameCp932, bytes), 0, bitmapFlags, false);
}

bool SpriteRuntime::LoadFileGroupPrimaryResource(
    const std::string& memberNameCp932,
    const std::uint8_t animationInitialization,
    const std::span<const std::byte> bytes)
{
    return LoadPrimaryInput(ResourceInput(memberNameCp932, bytes), animationInitialization, 0, true);
}

bool SpriteRuntime::LoadFileGroupPrimaryStatic(const ArchiveReaderReference& reader, std::uint32_t bitmapFlags)
{
    return LoadPrimaryInput(ResourceInput(reader), 0, bitmapFlags, false);
}

bool SpriteRuntime::LoadArchiveReaderMemberAt4B0840(std::size_t memberIndex,
    const ArchiveReaderReference& reader, std::uint8_t animationInitialization,
    const ArchiveCodecServices& codecs)
{
    return memberIndex == 0 ? LoadPrimaryInput(ResourceInput(reader, codecs), animationInitialization, 0, true)
                            : LoadAuxiliaryInput(memberIndex, ResourceInput(reader, codecs));
}

bool SpriteRuntime::LoadPrimaryInput(const ResourceInput& input,
    std::uint8_t initialization, std::uint32_t bitmapFlags, bool parseAnimation)
{
    ParsedAnimationResource animationResource;
    AnimationParseState parseState = AnimationParseState::NotAnimation;
    std::span<const std::byte> animationBytes;
    if (parseAnimation)
    {
        animationBytes = input.Bytes(); // 4B0905：本分支没有入口Name比较。
        try { parseState = input.ParseAnimation(animationBytes, animationResource, true); }
        catch (const AnimationResourceException&) { return false; }
        catch (int code)
        {
            // 4B0AB6同样消费448010返回码；只有非AN的3允许创建静态主成员。
            if (!input.IsNative()) throw;
            if (code != 0 && code != 3) return false;
            parseState = code == 3 ? AnimationParseState::NotAnimation : AnimationParseState::Success;
        }
        catch (...)
        {
            if (!input.IsNative()) throw;
            return false;
        }
    }

    if (parseState == AnimationParseState::NotAnimation)
    {
        // 4B0AEA替换+88后才进入419D70；旧Surface释放可以改写outerRef。
        primarySurfaces_.clear();
        primarySurfaces_.emplace_back(*graphicsApi_);
        if (!input.LoadStatic(primarySurfaces_.front(), bitmapFlags)) return false;
        const auto& surface = primarySurfaces_.front();
        loadMode_ = surface.HasAlpha() ? SpriteLoadMode::StaticWithAlpha : SpriteLoadMode::StaticWithoutAlpha;
        width_ = surface.Width();
        height_ = surface.Height();
        x_ = surface.OriginX();
        y_ = surface.OriginY();
    }
    else
    {
        std::vector<RuntimeGraphicsSurface> frames;
        frames.reserve(animationResource.frames.size());
        for (const AnimationFrameDescriptor& frame : animationResource.frames)
        {
            const auto pixels = frame.Pixels(animationBytes);
            frames.emplace_back(*graphicsApi_);
            if (!frames.back().LoadAnimationFrame(frame.originX, frame.originY,
                    frame.width, frame.height, frame.channels, pixels.data()))
                return false;
        }
        loadMode_ = SpriteLoadMode::Animation;
        width_ = animationResource.canvasWidth;
        height_ = animationResource.canvasHeight;
        x_ = animationResource.canvasX;
        y_ = animationResource.canvasY;
        primarySurfaces_ = std::move(frames);
        animation_.Initialize(animationResource, initialization != 0);
    }

    const char* const finalName = input.Name(); // 4B0B71，成功之后重取而非复用入口名。
    primaryMemberNameCp932_ = finalName ? finalName : "";
    return true;
}

bool SpriteRuntime::LoadFileGroupAuxiliaryResource(
    const std::size_t memberIndex,
    const std::string& memberNameCp932,
    const std::span<const std::byte> bytes)
{
    return LoadAuxiliaryInput(memberIndex, ResourceInput(memberNameCp932, bytes));
}

bool SpriteRuntime::LoadAuxiliaryInput(std::size_t memberIndex, const ResourceInput& input)
{
    if (memberIndex == 0u || loadMode_ != SpriteLoadMode::Animation)
        return false;

    std::vector<RuntimeGraphicsSurface> newGroup;
    RECT memberRect{};
    bool pictureLayer = true;
    ParsedPictureLayerResource parsed;
    const auto bytes = input.Bytes(); // 4B0C25在解析catch之前；拒绝非Animation时不取getter。
    try
    {
        parsed = input.ParsePictureLayer(bytes);
    }
    catch (const PictureLayerResourceException& error)
    {
        // 生产非PL由原int3表达；服务回调抛出的现代异常不能误认为非PL。
        if (input.IsNative() || error.Error() != PictureLayerResourceError::UnsupportedMagic)
            return false;
        pictureLayer = false;
    }
    catch (int code)
    {
        // 4482A0的int catch原值返回；4B0E50只有3允许静态回落。
        if (code != 3) return false;
        pictureLayer = false;
    }
    catch (...)
    {
        // 4483B6的catch-all返回-1，4B0EA8结束本次辅助成员装载。
        return false;
    }

    if (pictureLayer)
    {
        newGroup.reserve(parsed.frames.size());
        for (const PictureLayerFrame& frame : parsed.frames)
        {
            newGroup.emplace_back(*graphicsApi_);
            if (!newGroup.back().LoadRawFrame(
                    frame.surfaceOriginX, frame.surfaceOriginY,
                    frame.width, frame.height, frame.channels,
                    frame.createWithAlpha, frame.pixels.data()))
            {
                return false;
            }
        }
        memberRect = MakeRect(parsed.globalRect.x, parsed.globalRect.y,
            parsed.globalRect.width, parsed.globalRect.height);
    }
    else
    {
        newGroup.emplace_back(*graphicsApi_);
        if (!input.LoadStatic(newGroup.front(), 0u))
            return false;
        const RuntimeGraphicsSurface& surface = newGroup.front();
        memberRect = MakeRect(surface.OriginX(), surface.OriginY(),
            surface.Width(), surface.Height());
    }

    const std::int32_t previousX = x_;
    const std::int32_t previousY = y_;
    const RECT previousRect = MakeRect(x_, y_, width_, height_);
    RECT unionRect{};
    UnionRect(&unionRect, &previousRect, &memberRect);

    if (unionRect.left != previousX || unionRect.top != previousY)
    {
        const std::int32_t deltaX = SubWrapping(previousX, unionRect.left);
        const std::int32_t deltaY = SubWrapping(previousY, unionRect.top);
        for (std::vector<RuntimeGraphicsSurface>& group : auxiliarySurfaces_)
        {
            for (RuntimeGraphicsSurface& surface : group)
            {
                surface.SetOrigin(
                    AddWrapping(surface.OriginX(), deltaX),
                    AddWrapping(surface.OriginY(), deltaY));
            }
        }
    }

    if (pictureLayer)
    {
        for (std::size_t index = 0u; index < newGroup.size(); ++index)
        {
            const PictureLayerFrame& frame = parsed.frames[index];
            newGroup[index].SetOrigin(
                SubWrapping(AddWrapping(
                    frame.surfaceOriginX, parsed.globalRect.x), unionRect.left),
                SubWrapping(AddWrapping(
                    frame.surfaceOriginY, parsed.globalRect.y), unionRect.top));
        }
    }
    else
    {
        newGroup.front().SetOrigin(
            SubWrapping(memberRect.left, unionRect.left),
            SubWrapping(memberRect.top, unionRect.top));
    }

    x_ = unionRect.left;
    y_ = unionRect.top;
    width_ = SubWrapping(unionRect.right, unionRect.left);
    height_ = SubWrapping(unionRect.bottom, unionRect.top);

    const std::size_t auxiliaryIndex = memberIndex - 1u;
    if (auxiliaryIndex >= auxiliarySurfaces_.size())
        auxiliarySurfaces_.push_back(std::move(newGroup));
    else
        auxiliarySurfaces_[auxiliaryIndex] = std::move(newGroup);

    const char* const finalName = input.Name(); // 4B105B/4B1071，Surface表发布后才取名。
    if (auxiliaryIndex >= auxiliaryMemberNamesCp932_.size())
        auxiliaryMemberNamesCp932_.emplace_back(finalName ? finalName : "");
    else
        auxiliaryMemberNamesCp932_[auxiliaryIndex] = finalName ? finalName : "";
    return true;
}

void SpriteRuntime::PrepareFileGroup(const FileGroupChange changes)
{
    const std::uint8_t mask = static_cast<std::uint8_t>(changes);

    // 本分支无条件撤销显示与逻辑组名；失败时原函数不会恢复。
    active_ = false;
    logicalNameCp932_.clear();

    if ((mask & static_cast<std::uint8_t>(
            FileGroupChange::PrimaryResource)) != 0)
    {
        loadMode_ = SpriteLoadMode::StaticWithoutAlpha;
        primaryMemberNameCp932_.clear();
        primarySurfaces_.clear();
        animation_.Reset();
    }

    if ((mask & (static_cast<std::uint8_t>(FileGroupChange::AuxiliaryCount) |
                 static_cast<std::uint8_t>(FileGroupChange::AuxiliaryResource))) != 0)
    {
        width_ = 0;
        height_ = 0;
        x_ = 0;
        y_ = 0;
    }

    if ((mask & static_cast<std::uint8_t>(
            FileGroupChange::AuxiliaryCount)) != 0)
    {
        auxiliaryMemberNamesCp932_.clear();
        auxiliarySurfaces_.clear();
    }
}

PatternSpriteRectResult SpriteRuntime::LocalDrawableRect() const noexcept
{
    if (logicalNameCp932_.empty())
        return {PatternSpriteRectState::Empty, {}};
    if (static_cast<std::uint32_t>(loadMode_) > 2u)
        return {PatternSpriteRectState::InvalidLoadMode, {}};
    // 4AD0A9..4AD0D1按DWORD矩形再OffsetRect，不引入C++有符号溢出。
    return {PatternSpriteRectState::Valid, MakeRect(x_, y_, width_, height_)};
}

SpriteLoadMode SpriteRuntime::LoadMode() const noexcept { return loadMode_; }
bool SpriteRuntime::Active() const noexcept { return active_ != 0; }
std::uint8_t SpriteRuntime::ActiveByte() const noexcept { return active_; }
const std::string& SpriteRuntime::LogicalName() const noexcept { return logicalNameCp932_; }
std::uintptr_t SpriteRuntime::LoadArgument() const noexcept { return loadArgument_; }
const std::string& SpriteRuntime::PrimaryMemberName() const noexcept { return primaryMemberNameCp932_; }
const std::vector<std::string>& SpriteRuntime::AuxiliaryMemberNames() const noexcept { return auxiliaryMemberNamesCp932_; }
std::int32_t SpriteRuntime::Width() const noexcept { return width_; }
std::int32_t SpriteRuntime::Height() const noexcept { return height_; }
std::int32_t SpriteRuntime::X() const noexcept { return x_; }
std::int32_t SpriteRuntime::Y() const noexcept { return y_; }
std::uint32_t SpriteRuntime::OpaqueValueAt2C() const noexcept
{
    return std::bit_cast<std::uint32_t>(x_);
}
std::uint32_t SpriteRuntime::OpaqueValueAt30() const noexcept
{
    return std::bit_cast<std::uint32_t>(y_);
}
std::size_t SpriteRuntime::PrimarySurfaceCount() const noexcept { return primarySurfaces_.size(); }
const RuntimeGraphicsSurface& SpriteRuntime::PrimarySurfaceAt(
    const std::size_t surfaceIndex) const
{
    return primarySurfaces_.at(surfaceIndex);
}
std::size_t SpriteRuntime::AuxiliarySurfaceGroupCount() const noexcept
{
    return auxiliarySurfaces_.size();
}
std::size_t SpriteRuntime::AuxiliarySurfaceCount(
    const std::size_t groupIndex) const
{
    return auxiliarySurfaces_.at(groupIndex).size();
}
const RuntimeGraphicsSurface& SpriteRuntime::AuxiliarySurfaceAt(
    const std::size_t groupIndex,
    const std::size_t surfaceIndex) const
{
    return auxiliarySurfaces_.at(groupIndex).at(surfaceIndex);
}
const SpriteAnimationRuntimeState& SpriteRuntime::AnimationState() const noexcept
{
    return animation_;
}
const std::vector<std::int32_t>& SpriteRuntime::CurrentFrameMappings() const noexcept
{
    return animation_.currentFrameMappings;
}

void SpriteRuntime::SetLogicalName(std::string value) { logicalNameCp932_ = std::move(value); }
void SpriteRuntime::SetLogicalNameModernFromCString(const RuntimeCString& source)
{
    // 只在原命名提交点写现有唯一字段，保留显式长度；目的 CString 的 ABI/EH 尚未迁移。
    logicalNameCp932_.assign(RuntimeStringView(source));
}
void SpriteRuntime::SetActive(const bool value) noexcept { active_ = value; }
void SpriteRuntime::SetActiveByte(const std::uint8_t value) noexcept { active_ = value; }
void SpriteRuntime::SetOrigin(const std::int32_t x, const std::int32_t y) noexcept
{
    x_ = x;
    y_ = y;
}
void SpriteRuntime::SetOpaqueValuesAt2CAnd30(
    const std::uint32_t valueAt2C,
    const std::uint32_t valueAt30) noexcept
{
    x_ = std::bit_cast<std::int32_t>(valueAt2C);
    y_ = std::bit_cast<std::int32_t>(valueAt30);
}

void SpriteRuntime::RestartAnimation(const bool playFromStart)
{
    animation_.Restart(playFromStart);
}

void SpriteRuntime::ApplyPatternFilterAnimationCommand(
    const std::int32_t commandCount)
{
    animation_.ApplyPatternFilterCommand(commandCount);
}

void SpriteRuntime::FinishNonRepeatingAnimationAt4C4FC0()
{
    if (logicalNameCp932_.empty() || loadMode_ != SpriteLoadMode::Animation ||
        animation_.repeatCommands)
        return;
    animation_.commandIndex = 0;
    animation_.stateFlags = 0;
    animation_.CopyFinalFrameMappingsAt4C23A0();
}

void SpriteRuntime::RestoreAnimationAt4C14E0()
{
    if (logicalNameCp932_.empty() || loadMode_ != SpriteLoadMode::Animation ||
        animation_.repeatCommands)
        return;
    if (!animation_.commands.empty() && animation_.commands.back().opcode ==
        AnimationCommandOpcode::DecrementBranchAndJumpIfPositive)
    {
        animation_.AdvanceToFinalCommandAt4C1D20();
        return;
    }
    FinishNonRepeatingAnimationAt4C4FC0();
}

std::int32_t SpriteRuntime::AdvanceAnimation() noexcept
{
    return animation_.Advance();
}

bool SpriteRuntime::DirectCacheMatches(
    const char* resourceNameCp932,
    const std::uintptr_t loadArgument) const noexcept
{
    return _mbsicmp(
               reinterpret_cast<const unsigned char*>(logicalNameCp932_.c_str()),
               reinterpret_cast<const unsigned char*>(resourceNameCp932)) == 0 &&
           loadArgument_ == loadArgument;
}
}
