#include <Runtime/ParamsRuntimeBindings.h>

#include <Runtime/ParamsRawBlobInstaller.h>

#include <cstddef>
#include <cstdint>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ParamsRuntimeBindings::ParamsRuntimeBindings(
    std::vector<InstallTableArchive> installedArchives,
    ScrResourceNameTable installedResourceNames,
    EnvironmentKeyByteTable installedEnvironmentKeyBytes)
    : archives(std::move(installedArchives)),
      resourceNames(std::move(installedResourceNames)),
      environmentKeyBytes(std::move(installedEnvironmentKeyBytes))
{
}

ParamsRuntimeBindings BuildParamsRuntimeBindings(const ParamsRuntimeProfile& profile)
{
    std::vector<InstallTableArchive> archives;
    archives.reserve(profile.gameSystem.installTable.size());
    for (std::uint32_t index = 0; index < profile.gameSystem.installTable.size(); ++index)
    {
        const auto& entry = profile.gameSystem.installTable.At(index);
        archives.push_back({std::string(RuntimeStringView(entry.file)), std::string(RuntimeStringView(entry.media))});
    }

    std::vector<std::string> names;
    names.reserve(profile.pattern.items.size());
    for (const ParamsPatternItem& item : profile.pattern.items)
        names.push_back(item.name.cp932);

    std::vector<std::uint8_t> rawBlob;
    rawBlob.reserve(profile.gameSystem.rawBlob.size());
    // 工具绑定显式导出深快照；不会清理实际GS源或代替原生key安装编排。
    for (const std::byte value : profile.gameSystem.rawBlob.SnapshotBytes())
        rawBlob.push_back(std::to_integer<std::uint8_t>(value));

    return ParamsRuntimeBindings(
        std::move(archives),
        ScrResourceNameTable(std::move(names)),
        EnvironmentKeyByteTable(ParamsRawBlobInstaller::Install(rawBlob)));
}
}
