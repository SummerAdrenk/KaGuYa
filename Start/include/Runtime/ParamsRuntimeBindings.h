#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/InstallTableCatalogBuilder.h>
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ScrScriptRuntime.h>

#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ParamsRuntimeBindings final
{
    ParamsRuntimeBindings(
        std::vector<InstallTableArchive> installedArchives,
        ScrResourceNameTable installedResourceNames,
        EnvironmentKeyByteTable installedEnvironmentKeyBytes);

    std::vector<InstallTableArchive> archives;
    ScrResourceNameTable resourceNames;
    EnvironmentKeyByteTable environmentKeyBytes;
};

// 495E10/489C90/47A020：只按 params 中已有顺序建立运行时对象，不扫描目录补项。
[[nodiscard]] ParamsRuntimeBindings BuildParamsRuntimeBindings(
    const ParamsRuntimeProfile& profile);
}
