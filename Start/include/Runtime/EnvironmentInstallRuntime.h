#pragma once

#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/SystemDataInstalledFiles.h>

namespace KaGuYa::Start::Runtime
{
class ISystemDataInstallSource
{
public:
    virtual ~ISystemDataInstallSource() = default;
    // 47E520和47E65E分别捕获；返回借用，不增持数组或SysData。
    virtual const SystemDataInstalledFiles& CaptureSystemDataInstallFilesAt4180C0() const = 0;
    // 47E7C9在取得元素地址之后，单独捕获当前Paths。
    virtual EnvironmentPathRuntime& CaptureInstallPathsAt4180C0() const = 0;
};

void ResolveSystemDataInstallFilesAt47E520(const ISystemDataInstallSource& source,
    IEnvironmentPathApi& pathApi, IEnvironmentPathErrorSink& errors);
}
