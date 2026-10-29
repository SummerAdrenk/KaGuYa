#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

#include <memory>
#include <string>

namespace KaGuYa::Start::Runtime
{
class ResourceCatalog;
struct FileTransactionContext;
class IStartFileApi;
class IEnvironmentKeyTableSource;
class IGraphicsResourceErrorDisplay;

class IReadScriptEffectSurfaceServices
{
public:
    virtual ~IReadScriptEffectSurfaceServices() = default;
    // 480000 会先提交新对象；后续装载失败不清空该引用。
    [[nodiscard]] virtual bool LoadStaticAt480000(
        std::shared_ptr<RuntimeGraphicsSurface>& target, const std::string& nameCp932) = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface> CaptureScreenAt4ACDD0() = 0;
};

class ReadScriptEffectSurfaceRuntime final : public IReadScriptEffectSurfaceServices,
    public INativeScreenCaptureServices
{
public:
    ReadScriptEffectSurfaceRuntime(IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes,
        IGraphicsSurfaceTransitionApi& transition, const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplay& errors,
        ArchiveCodecServices codecs) noexcept;
    bool LoadStaticAt480000(std::shared_ptr<RuntimeGraphicsSurface>& target,
        const std::string& nameCp932) override;
    std::shared_ptr<RuntimeGraphicsSurface> CaptureScreenAt4ACDD0() override;
    GraphicsSurfaceReference CaptureScreenNativeAt4ACDD0() override;

private:
    IGraphicsSurfaceApi& surfaces_;
    IGraphicsScreenSizeApi& sizes_;
    IGraphicsSurfaceTransitionApi& transition_;
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsResourceErrorDisplay& errors_;
    ArchiveCodecServices codecs_;
};
}
