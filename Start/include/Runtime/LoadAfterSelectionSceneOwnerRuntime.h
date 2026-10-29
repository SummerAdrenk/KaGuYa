#pragma once

#include <Runtime/DrawManagerOwnerResetRuntime.h>
#include <Runtime/LoadAfterSelectionServicesRuntime.h>
#include <Runtime/RestoredScriptServicesRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ScrVmRuntimeStateBundle;

struct LoadAfterSelectionSceneOwnerDependencies final
{
    LoadAfterSelectionDependencies scene;
    ScrVmRuntimeStateBundle& bundle;
    ScrScriptRuntimeState& script;
    // 构造及 RestoreDependencies 不捕获 receiver，不固定某一代 Pattern。
    const IScrResourceNameSource& resourceNames;
    const ScrVmExecutableProfile& profile;
    const ResourceCatalog& catalog;
    const FileTransactionContext* transaction;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    const ScrVmCore& vm;
    IGraphicsResourceErrorDisplay& errors;
};

// 4B6380/4D4CC0/47E180 共用根状态；现代 owner 只拥有适配器和场景，不复制根字段。
// 同步、恢复和绘制从同一 manager/Save/UnSave/values 构造，避免各自借到不同对象。
class LoadAfterSelectionSceneOwnerRuntime final
{
public:
    explicit LoadAfterSelectionSceneOwnerRuntime(LoadAfterSelectionSceneOwnerDependencies dependencies);
    LoadAfterSelectionSceneOwnerRuntime(const LoadAfterSelectionSceneOwnerRuntime&) = delete;
    LoadAfterSelectionSceneOwnerRuntime& operator=(const LoadAfterSelectionSceneOwnerRuntime&) = delete;
    [[nodiscard]] LoadAfterSelectionScene& Scene() noexcept { return scene_; }

private:
    DrawManagerOwnerResetRuntime resetOwners_;
    RestoredScriptServicesRuntime restored_;
    RestoredScriptTransaction transaction_;
    LoadAfterSelectionServicesRuntime services_;
    // 先析构场景的 Surface 引用，再释放其借用的服务；最后一个 alias Ref 控制整个寿命。
    LoadAfterSelectionScene scene_;
};

class LoadAfterSelectionOwnerConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    LoadAfterSelectionOwnerConstructionServicesRuntime(ISceneConstructionServices& upstream,
        LoadAfterSelectionSceneOwnerDependencies dependencies) noexcept;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateLoadAfterSelection() override;

private:
    LoadAfterSelectionSceneOwnerDependencies dependencies_;
};
}
