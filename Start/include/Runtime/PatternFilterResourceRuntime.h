#pragma once

#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/ScrResourceNameSource.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IPatternFilterResourceLoader
{
public:
    virtual ~IPatternFilterResourceLoader() = default;
    [[nodiscard]] virtual ScrResourceNameRootReceiver CaptureParamsForNames() = 0;
    [[nodiscard]] virtual bool LoadInto(
        RuntimeGraphicsSurface& target, const std::string& nameCp932) = 0;
};

class PatternFilterArchiveResourceLoader final : public IPatternFilterResourceLoader
{
public:
    PatternFilterArchiveResourceLoader(
        const IScrResourceNameRootSource& names,
        const ResourceCatalog& catalog,
        const FileTransactionContext* transaction,
        IStartFileApi& files,
        const IEnvironmentKeyTableSource& key,
        IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs) noexcept;

    [[nodiscard]] ScrResourceNameRootReceiver CaptureParamsForNames() override;
    [[nodiscard]] bool LoadInto(
        RuntimeGraphicsSurface& target, const std::string& nameCp932) override;

private:
    const IScrResourceNameRootSource& names_;
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsResourceErrorDisplay& errors_;
    ArchiveCodecServices codecs_;
};
}
