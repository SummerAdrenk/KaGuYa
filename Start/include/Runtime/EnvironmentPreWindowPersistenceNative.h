#pragma once
#include <Runtime/PersistenceFileAccess.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;
// 480230/488740共用原生归档与CString路径；不持Environment或归档快照。
bool ReadPreWindowEnvironmentAt480230(EnvironmentRegistryRuntime&, IStartFileApi&, const FileTransactionContext*);
bool WritePreWindowEnvironmentAt488740(EnvironmentRegistryRuntime&, IStartFileApi&, const FileTransactionContext*);
}
