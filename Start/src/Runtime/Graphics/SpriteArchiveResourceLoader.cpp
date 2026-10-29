#include <Runtime/SpriteArchiveResourceLoader.h>

#include <Runtime/ArchiveReaderFactoryRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
SpriteArchiveResourceLoader::SpriteArchiveResourceLoader(
    const ResourceCatalog& catalog,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi,
    const IEnvironmentKeyTableSource& environmentKeyBytes, ArchiveCodecServices codecs) noexcept
    : catalog_(catalog),
      transactionContext_(transactionContext),
      fileApi_(fileApi),
      environmentKeyBytes_(environmentKeyBytes), codecs_(codecs)
{
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadDirectStatic(
    SpriteRuntime& sprite,
    const std::string& resourceNameCp932,
    const std::uintptr_t loadArgument) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(resourceNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    return sprite.LoadDirectStatic(factory.Reader(), loadArgument)
        ? SpriteArchiveLoadResult::Success
        : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadDirect(
    SpriteRuntime& sprite,
    const std::string& resourceNameCp932,
    const bool playAnimationFromStart,
    const std::uintptr_t loadArgument) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(resourceNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    return sprite.LoadArchiveReaderAt480650(factory.Reader(),
        static_cast<std::uint8_t>(playAnimationFromStart), loadArgument, codecs_)
        ? SpriteArchiveLoadResult::Success
        : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadDirectFromEnvironment(
    SpriteRuntime& sprite,
    const std::string& resourceNameCp932,
    const ISpriteAnimationInitializationSource& environment,
    const std::uintptr_t loadArgument) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(resourceNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    // 4CC4BA -> 4CC61E -> 480650：失败不读取配置，成功后才捕获当前字节。
    const auto initialization = environment.ReadRegistrySlot1ByteAt6D();
    return sprite.LoadArchiveReaderAt480650(factory.Reader(), initialization, loadArgument, codecs_)
        ? SpriteArchiveLoadResult::Success : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadFileGroupMemberFromEnvironment(
    SpriteRuntime& sprite,
    const std::size_t memberIndex,
    const std::string& resourceNameCp932,
    const ISpriteAnimationInitializationSource& environment) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(resourceNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    // 4A8958..4A8976：每个变化的成员均查询一次，包括不消费该字节的辅助图。
    const auto initialization = environment.ReadRegistrySlot1ByteAt6D();
    const bool loaded = sprite.LoadArchiveReaderMemberAt4B0840(memberIndex, factory.Reader(), initialization, codecs_);
    return loaded ? SpriteArchiveLoadResult::Success : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadFileGroupPrimaryStatic(
    SpriteRuntime& sprite,
    const std::string& memberNameCp932,
    const std::uint32_t bitmapFlags) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(memberNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    return sprite.LoadFileGroupPrimaryStatic(factory.Reader(), bitmapFlags)
        ? SpriteArchiveLoadResult::Success
        : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadFileGroupPrimary(
    SpriteRuntime& sprite,
    const std::string& memberNameCp932,
    const std::uint8_t animationInitialization) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(memberNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    return sprite.LoadArchiveReaderMemberAt4B0840(0, factory.Reader(), animationInitialization, codecs_)
        ? SpriteArchiveLoadResult::Success
        : SpriteArchiveLoadResult::ResourceReadFailed;
}

SpriteArchiveLoadResult SpriteArchiveResourceLoader::LoadFileGroupAuxiliary(
    SpriteRuntime& sprite,
    const std::size_t memberIndex,
    const std::string& memberNameCp932) const
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog_, transactionContext_, fileApi_, memory, codecs_);
    if (!factory.Load(memberNameCp932, environmentKeyBytes_))
        return SpriteArchiveLoadResult::ResourceReadFailed;
    return (memberIndex != 0 && sprite.LoadArchiveReaderMemberAt4B0840(memberIndex, factory.Reader(), 0, codecs_))
        ? SpriteArchiveLoadResult::Success
        : SpriteArchiveLoadResult::ResourceReadFailed;
}

PatternLayerArchiveResourceException::PatternLayerArchiveResourceException(
    std::string resourceNameCp932,
    const std::size_t memberIndex)
    : std::runtime_error("Pattern layer archive resource load failed"),
      resourceNameCp932_(std::move(resourceNameCp932)),
      memberIndex_(memberIndex)
{
}

const std::string&
PatternLayerArchiveResourceException::ResourceNameCp932() const noexcept
{
    return resourceNameCp932_;
}

std::size_t PatternLayerArchiveResourceException::MemberIndex() const noexcept
{
    return memberIndex_;
}

bool LoadPatternSpriteFromReaderModern(SpriteRuntime& sprite,
    const ArchiveReaderReference& reader, const std::uint8_t initialization,
    const PatternResourceLoadRequest& request, const ArchiveCodecServices& codecs)
{
    // 不在adapter解析正文；480650/4B0840各自的getter分支决定解析时点。
    switch (request.path)
    {
    case PatternResourceLoadPath::NormalGroup:
    case PatternResourceLoadPath::CgGroup:
        // 4B0840 的 44h 临时 Ref/EH 仍是现代正文边界，不能以这里的 vector 冒充。
        return sprite.LoadArchiveReaderMemberAt4B0840(request.memberIndex, reader, initialization, codecs);
    case PatternResourceLoadPath::NormalDirect:
    case PatternResourceLoadPath::NormalExc:
    case PatternResourceLoadPath::NormalFileConvert:
    case PatternResourceLoadPath::CgDirect:
    case PatternResourceLoadPath::CgExc:
        return sprite.LoadArchiveReaderAt480650(reader, initialization, request.loadArgument, codecs);
    default:
        throw std::invalid_argument("Unknown Pattern resource load path");
    }
}

PatternLayerArchiveResourceLoader::PatternLayerArchiveResourceLoader(
    const PatternResourceLoadServices& services) noexcept
    : services_(services)
{
}

void PatternLayerArchiveResourceLoader::ExecuteLoad(const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
    IRuntimeStringManagerSource& strings)
{
    // 不提前 CaptureCatalog；本次字符串 source 沿 caller 传入，依赖均为实际借用。
    const PatternResourceLoadServices current{services_.source, services_.transaction,
        services_.files, services_.memory, services_.key, services_.initialization, services_.codecs, strings};
    ExecutePatternResourceLoad(request, failureName, action, current);
}
}
