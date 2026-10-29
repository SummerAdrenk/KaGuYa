#pragma once

#include <Runtime/ArchiveCodecServices.h>

#include <cstdint>
#include <memory>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentKeyTableSource;
struct FileTransactionContext;
class IGraphicsResourceErrorDisplay;
class IGraphicsSurfaceApi;
class IGraphicsTransitApi;
class IStartFileApi;
class ResourceCatalog;
class RuntimeGraphicsSurface;

class IReadScriptTransitionMaskLoader
{
public:
    virtual ~IReadScriptTransitionMaskLoader() = default;
    [[nodiscard]] virtual bool LoadMaskAt480B30(
        std::shared_ptr<RuntimeGraphicsSurface>& output,
        const char* nameCp932) = 0;
};

// 480B30 同时被 ReadScript 4C3A20 与 TextDisplay 4A272A 使用。
// 所有依赖均借用；尺寸是 Program 运行时 +18/+1C，不能传 ParamsGameSystem 或临时副本。
class ReadScriptTransitionMaskLoaderRuntime final : public IReadScriptTransitionMaskLoader
{
public:
    ReadScriptTransitionMaskLoaderRuntime(const std::uint32_t& programWidthAt18,
        const std::uint32_t& programHeightAt1C,
        const ResourceCatalog& catalog, const FileTransactionContext* transaction,
        IStartFileApi& files, const IEnvironmentKeyTableSource& key,
        IGraphicsSurfaceApi& surfaces, IGraphicsTransitApi& transit,
        IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs) noexcept;

    [[nodiscard]] bool LoadMaskAt480B30(
        std::shared_ptr<RuntimeGraphicsSurface>& output,
        const char* nameCp932) override;

private:
    const std::uint32_t& programWidthAt18_;
    const std::uint32_t& programHeightAt1C_;
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsTransitApi& transit_;
    IGraphicsResourceErrorDisplay& errors_;
    ArchiveCodecServices codecs_;
};
}
