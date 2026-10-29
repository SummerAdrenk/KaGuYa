#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/EnvironmentPersistenceLifecycle.h>
#include <Runtime/FileManagerRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
void EnvironmentPostWindowStartupRuntime::PersistEnvironmentState()
{
    // 4777A8..4777C4：只传稳定根；归档和路径在各自原读点重新查询，不在此预取Paths。
    EnvironmentPersistenceLifecycle::WriteCurrentOnShutdownNativeAt4777A8(
        environment_, rootState_, fileManager_, transaction_, files_);
}
}
