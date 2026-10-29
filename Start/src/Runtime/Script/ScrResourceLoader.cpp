#include <Runtime/ScrResourceLoader.h>

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
ScrResourceLoadException::ScrResourceLoadException(
    const ScrResourceLoadError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

ScrResourceLoadError ScrResourceLoadException::Error() const noexcept
{
    return error_;
}

static std::uint32_t LoadScrFactory(
    ArchiveReaderFactoryRuntime& factory,
    const std::string& resourceName,
    const IEnvironmentKeyTableSource& environmentKeyBytes)
{
    try
    {
        return factory.LoadOrThrow(resourceName, environmentKeyBytes);
    }
    catch (const ArchiveEntryDecodeException&)
    {
        throw ScrResourceLoadException(ScrResourceLoadError::PayloadMalformed,
            "SCR archive payload transform failed");
    }
    catch (const int error)
    {
        const ScrResourceLoadError mapped = error == 0 ? ScrResourceLoadError::ResourceNotFound
            : error == 4 ? ScrResourceLoadError::KeyUnavailable
            : error >= 1 && error <= 3
            ? static_cast<ScrResourceLoadError>(error)
            : ScrResourceLoadError::FileReadFailed;
        throw ScrResourceLoadException(mapped, "SCR archive range could not be read");
    }
}

LoadedScrResource LoadScrResource(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    const std::int32_t scriptId,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes)
{
    // 4D3D64..4D3E31：局部工厂须覆盖 parser 消费；流则在 Load 内已经关闭。
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog, transactionContext, api, memory, ToolArchiveCodecsTag{});
    const auto archiveVersion = LoadScrFactory(factory, resourceName, environmentKeyBytes);
    // 本值接口返回独立脚本容器；深导出是显式工具拥有，不移走生产reader的唯一byte Ref。
    auto reader = ExportArchiveReader(factory.Reader());

    return LoadedScrResource{
        std::move(reader.resourceName),
        archiveVersion,
        ParseScrContainer(scriptId, std::move(reader.bytes))};
}
}
