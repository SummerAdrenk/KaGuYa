#include <Runtime/PatternFilterResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
PatternFilterArchiveResourceLoader::PatternFilterArchiveResourceLoader(
    const IScrResourceNameRootSource& names,
    const ResourceCatalog& catalog,
    const FileTransactionContext* transaction,
    IStartFileApi& files,
    const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs) noexcept
    : names_(names),
      catalog_(catalog),
      transaction_(transaction),
      files_(files),
      key_(key),
      errors_(errors),
      codecs_(codecs)
{
}

ScrResourceNameRootReceiver PatternFilterArchiveResourceLoader::CaptureParamsForNames()
{
    return names_.CaptureParamsForNames();
}

bool PatternFilterArchiveResourceLoader::LoadInto(
    RuntimeGraphicsSurface& target, const std::string& nameCp932)
{
    return LoadDrawManagerStaticInto(
        target, nameCp932, catalog_, transaction_, files_, key_, errors_, codecs_);
}
}
