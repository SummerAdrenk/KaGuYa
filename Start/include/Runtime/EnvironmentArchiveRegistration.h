#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentArchiveRuntime;
class IUnSaveDataGameSystemSource;
class ISecretScCountSource;
class ISaveDataConstructionSource;
class ISoundParamsSettingsSource;
class ISettingTagErrorSource;

// 417990：只解本次receiver；越界/base空返回null，control空时stride=0。
// 不Capture、不增持、不检查control.count或vptr，偏移使用原DWORD回绕。
[[nodiscard]] IEnvironmentArchiveRuntime* ResolveEnvironmentArchiveAt417990(
    const EnvironmentRootStorage& root, std::uint32_t index) noexcept;

// 46A4E5/4984E3：只初始化空Ref；分配失败保留base，不回收调用者已构造的正文。
void InitializeEnvironmentArchiveReferenceAt46A4E5(
    YtlReferenceSlotRuntime& reference, IEnvironmentArchiveRuntime* archive);

// 46A4A0：调用者已经捕获本次Environment，不在分配/回调后重新捕获另一个根。
// null正文仍追加空Ref；失败保留此前归档和原版规定的部分发布状态。
void RegisterEnvironmentArchiveAt46A4A0(EnvironmentRootStorage& root,
    IEnvironmentArchiveRuntime* archive,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 47E2D8..47E30D：完成90h构造后才捕获登记根；捕获或登记失败不补正文回滚。
void RegisterSaveDataArchiveAt47E2D8(EnvironmentRootOwner& owner,
    ISaveDataConstructionSource& source, ISoundParamsSettingsSource& sounds,
    ISettingTagErrorSource& errors,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 47E3B7..47E43C：先构造ClickInfo，之后才捕获本次根并登记；捕获失败不补回收正文。
void RegisterClickInfoArchiveAt47E3B7(EnvironmentRootOwner& owner,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 47E4A8..47E500：最后一个正文无构造EH资格，完成后另取当前登记根。
void RegisterGlobalParamsArchiveAt47E4A8(EnvironmentRootOwner& owner,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 47E312：只有正文构造期间持有raw分配块清理资格，捕获根之前撤去。
void RegisterUnSaveDataArchiveAt47E312(EnvironmentRootOwner& owner,
    IUnSaveDataGameSystemSource& source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 47E380：构造catch正常返回ready=0仍登记；Capture之前撤销raw正文清理资格。
void RegisterSecretScArchiveAt47E380(EnvironmentRootOwner& owner,
    ISecretScCountSource& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}
