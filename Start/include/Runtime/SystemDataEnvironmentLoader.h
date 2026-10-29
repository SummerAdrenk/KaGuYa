#pragma once

#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
// 4984A0..498587：receiver固定为498280入口选中的根；正文内部另按495EBC捕获来源根。
// 只把实际68h正文及唯一control登记到+30，不建立Loader/shared_ptr旁路持有。
void RegisterSystemDataArchiveAt4984A0(EnvironmentRootStorage& receiver,
    EnvironmentRootOwner& source, ParamsRuntimeRootOwner& params, IStartFileApi& files,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 现代装配层只持有原12-byte Ref；正文和0x18 control分别分配，不合并make_shared。
class SystemDataEnvironmentLoader final
{
public:
    SystemDataEnvironmentLoader(RuntimeStringBorrow currentDirectoryCp932,
        ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget = {},
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    ~SystemDataEnvironmentLoader() noexcept;
    SystemDataEnvironmentLoader(const SystemDataEnvironmentLoader&) = delete;
    SystemDataEnvironmentLoader& operator=(const SystemDataEnvironmentLoader&) = delete;

    [[nodiscard]] bool Ready() const noexcept;
    [[nodiscard]] const SystemDataEnvironmentArchive& State() const noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& Reference() const noexcept { return state_; }
    [[nodiscard]] const SystemDataEnvironmentArchive& ReadyArchive() const;
    [[nodiscard]] const SystemDataInstalledFiles& InstallFiles() const noexcept;
    void Serialize(SerializeBuffer&) noexcept;

private:
    YtlReferenceSlotRuntime state_;
};
static_assert(sizeof(SystemDataEnvironmentLoader) == 12);
}
