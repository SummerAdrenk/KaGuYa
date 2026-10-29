#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/ArchiveCodecServices.h>

#include <memory>
#include <optional>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentKeyTableSource;
class ResourceCatalog;
class IStartFileApi;
class IGraphicsResourceErrorDisplay;
struct FileTransactionContext;

struct ColorTransWindowDescriptor final
{
    const char* firstNameCp932 = nullptr;
    const char* secondNameCp932 = nullptr;
    std::int32_t mode = 0;
};

struct ColorTransWindowAppliedColors final
{
    std::uint32_t color = 0;
    std::int32_t transparency = 0;
};

class IColorTransWindowResourceLoader
{
public:
    virtual ~IColorTransWindowResourceLoader() = default;
    [[nodiscard]] virtual bool LoadInto(RuntimeGraphicsSurface& target,
        const std::string& nameCp932) = 0;
};

class ColorTransWindowArchiveLoader final : public IColorTransWindowResourceLoader
{
public:
    ColorTransWindowArchiveLoader(const ResourceCatalog& catalog,
        const FileTransactionContext* transaction, IStartFileApi& files,
        const IEnvironmentKeyTableSource& key, IGraphicsResourceErrorDisplay& errors,
        ArchiveCodecServices codecs) noexcept;
    bool LoadInto(RuntimeGraphicsSurface& target, const std::string& nameCp932) override;
private:
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transaction_;
    IStartFileApi& files_;
    const IEnvironmentKeyTableSource& key_;
    IGraphicsResourceErrorDisplay& errors_;
    ArchiveCodecServices codecs_;
};

// 45E970/46F6D0：两个静态资源与单独的已处理画布，不是 PatternLayer。
class ColorTransWindowRuntime final
{
public:
    ColorTransWindowRuntime(IGraphicsSurfaceApi& surfaces, IGraphicsWindowSurfaceApi& graphics,
        IColorTransWindowResourceLoader& resources, const ColorTransWindowDescriptor& descriptor);
    ~ColorTransWindowRuntime();
    ColorTransWindowRuntime(const ColorTransWindowRuntime&) = delete;
    ColorTransWindowRuntime& operator=(const ColorTransWindowRuntime&) = delete;

    [[nodiscard]] RuntimeGraphicsSurface* Processed() noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface* Processed() const noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface* FirstSource() const noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface* SecondSource() const noexcept;
    [[nodiscard]] std::optional<ColorTransWindowAppliedColors> AppliedColors() const noexcept;
    [[nodiscard]] bool ApplyColorsAt46F6D0(std::uint32_t color, std::int32_t transparency);
    void ClearResourcesAt486A30() noexcept;

private:
    IGraphicsWindowSurfaceApi& graphics_;
    std::unique_ptr<RuntimeGraphicsSurface> first_;
    std::unique_ptr<RuntimeGraphicsSurface> second_;
    std::unique_ptr<RuntimeGraphicsSurface> processed_;
    std::int32_t mode_;
    // 原 +30/+34 只在成功变色后初始化，不能提前把零当作已应用的配置。
    std::optional<ColorTransWindowAppliedColors> applied_;
};
}
