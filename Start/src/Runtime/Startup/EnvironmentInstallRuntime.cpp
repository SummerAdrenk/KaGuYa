#include <Runtime/EnvironmentInstallRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void ResolveSystemDataInstallFilesAt47E520(const ISystemDataInstallSource& source,
    IEnvironmentPathApi& pathApi, IEnvironmentPathErrorSink& errors)
{
    for (std::uint32_t index = 0;; ++index)
    {
        // 47E64C..47E658读count，47E7A6..47E7C7再取元素；不能固定一个span跨API持有。
        const auto count = static_cast<std::uint32_t>(source.CaptureSystemDataInstallFilesAt4180C0().size());
        if (std::bit_cast<std::int32_t>(index) >= std::bit_cast<std::int32_t>(count)) break;
        const auto* file = source.CaptureSystemDataInstallFilesAt4180C0().ResolveAt(index);
        auto& paths = source.CaptureInstallPathsAt4180C0();
        // 46A06D先检查ready；空元素只有实际取字段时才失败，不能提前取字段。
        if (!paths.readyAt00) throw 2;
        if (!file) throw std::logic_error("47E8F1: install element has no payload");
        if (!paths.ResolveInstallFile(*file, pathApi, errors)) throw 2;
    }
}
}
