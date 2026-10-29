#include <Runtime/ReadScriptEffectSurfaceRuntime.h>

#include <Runtime/DrawManagerResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptEffectSurfaceRuntime::ReadScriptEffectSurfaceRuntime(IGraphicsSurfaceApi& surfaces,
    IGraphicsScreenSizeApi& sizes, IGraphicsSurfaceTransitionApi& transition,
    const ResourceCatalog& catalog, const FileTransactionContext* transaction,
    IStartFileApi& files, const IEnvironmentKeyTableSource& key,
    IGraphicsResourceErrorDisplay& errors, ArchiveCodecServices codecs) noexcept
    : surfaces_(surfaces), sizes_(sizes), transition_(transition), catalog_(catalog),
      transaction_(transaction), files_(files), key_(key), errors_(errors), codecs_(codecs)
{
}

bool ReadScriptEffectSurfaceRuntime::LoadStaticAt480000(
    std::shared_ptr<RuntimeGraphicsSurface>& target, const std::string& nameCp932)
{
    if (!target)
        target = std::make_shared<RuntimeGraphicsSurface>(surfaces_);
    return LoadDrawManagerStaticInto(*target, nameCp932, catalog_, transaction_, files_, key_, errors_, codecs_);
}

std::shared_ptr<RuntimeGraphicsSurface> ReadScriptEffectSurfaceRuntime::CaptureScreenAt4ACDD0()
{
    // 原顺序为创建对象、读取 DLL 尺寸、创建像素缓冲、Backup；不能先读尺寸。
    auto surface = std::make_shared<RuntimeGraphicsSurface>(surfaces_);
    const SIZE size = sizes_.ReadScreenSizeAt47CC00();
    if (!surface->Create(size.cx, size.cy, false, false))
        return {};
    transition_.BackupScreenToSurfaceIfLoaded(surface->Handle());
    return surface;
}

GraphicsSurfaceReference ReadScriptEffectSurfaceRuntime::CaptureScreenNativeAt4ACDD0()
{
    GraphicsSurfaceReference result(surfaces_);
    CaptureGraphicsScreenAt4ACDD0(result.Slot(), surfaces_, sizes_, transition_);
    return result;
}
}
