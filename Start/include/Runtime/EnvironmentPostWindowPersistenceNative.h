#pragma once

#include <Runtime/NativeSerialize18.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class FileManagerRootRuntime;
struct PreWindowLifecycleState;

// 借与selectedRoot同源的Process服务，不保存归档/路径/key快照，不增持Ref或另建Environment。
struct EnvironmentPostWindowPersistenceNativeServices final
{
    EnvironmentStartupRuntime& environment;
    FileManagerRootRuntime& keys;
    PreWindowLifecycleState& rootState;
    IStartFileApi& files;
    const FileTransactionContext* transaction;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
};

// 48A16A..48A1DC及完整481220读取/清零/正常-EH尾；异常外传，正常清理完成后才写+1AD。
void LoadSecretEnvironmentPostWindowNativeAt48A16A(EnvironmentPostWindowPersistenceNativeServices&);
// 48A1E8..48A345；470630扫描仍由caller在前执行。读失败保留默认值，正常尾才写+1B3。
void LoadGlobalEnvironmentPostWindowNativeAt48A1E8(EnvironmentPostWindowPersistenceNativeServices&);
// 4777A8/4777B6的原生写回入口；bool失败由caller忽略，普通异常不吞掉。
bool WriteSecretEnvironmentNativeAt489250(EnvironmentPostWindowPersistenceNativeServices&);
bool WriteGlobalEnvironmentNativeAt488EE0(EnvironmentPostWindowPersistenceNativeServices&);
}
