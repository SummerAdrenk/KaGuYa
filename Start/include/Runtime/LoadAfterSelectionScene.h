#pragma once

#include <Runtime/RestoredScriptTransaction.h>
#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

#include <cstdint>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// bg_black.bgd 与屏幕备份在原对象中均以强引用持有。具体 Sprite/Surface 类型由
// 后续生产适配器提供，这里只表达 CLoadAfter 所需要的共同生命周期语义。
class ILoadAfterTransitionSource
{
public:
    virtual ~ILoadAfterTransitionSource() = default;
};

using LoadAfterTransitionSourceRef =
    std::shared_ptr<ILoadAfterTransitionSource>;

// 对应 CLoadAfter+0x24 的 0x30 字节操作块。sourceAt04 使用语义强引用代替原 x86
// 指针槽；其余槽保持原 offset，以便 renderer 精确翻译 selector 57/58。
struct LoadAfterTransitionOperation final
{
    std::int32_t valueAt00 = 0;
    LoadAfterTransitionSourceRef sourceAt04;
    std::int32_t valueAt08 = 0;
    std::int32_t valueAt0C = 0;
    std::int32_t valueAt10 = 0;
    std::int32_t valueAt14 = 0;
    std::int32_t valueAt18 = 0;
    std::int32_t selectorAt1C = -1;
    std::int32_t valueAt20 = 0;
    std::int32_t progressAt24 = 0;
    std::int32_t maximumAt28 = 255;
    std::int32_t valueAt2C = 0;

    void Reset() noexcept;
};

enum class LoadAfterSelectionConstructionError : std::uint8_t
{
    BlackTransitionResourceLoadFailed,
};

class LoadAfterSelectionConstructionException final : public std::runtime_error
{
public:
    LoadAfterSelectionConstructionException(
        LoadAfterSelectionConstructionError error,
        const char* message);

    [[nodiscard]] LoadAfterSelectionConstructionError Error() const noexcept;

private:
    LoadAfterSelectionConstructionError error_;
};

// CLoadAfter 的外部边界按原调用顺序拆分。不得在实现类中把这些接口重新合并成
// 事务或自动回滚操作，否则会掩盖原函数的部分提交行为。
class ILoadAfterSelectionServices
{
public:
    virtual ~ILoadAfterSelectionServices() = default;

    [[nodiscard]] virtual LoadAfterTransitionSourceRef LoadTransitionSource(
        const char* resourceNameCp932) = 0;
    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteRootByteAt218(std::uint8_t value) = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;

    [[nodiscard]] virtual std::uint32_t ReadTransitionSpeedIndexAt70() const = 0;
    [[nodiscard]] virtual bool ReadTransitionAccelerationGateAt6F() const = 0;
    [[nodiscard]] virtual bool PollTransitionAccelerationAtSlot5() = 0;
    virtual void RenderTransition(LoadAfterTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrame(std::int32_t argument) = 0;

    virtual void WriteRootServiceDwordAt10(std::uint32_t value) = 0;
    virtual void StopSoundManagerEntryNamedAt10() = 0;
    virtual void ClearSoundManagerEntries() = 0;
    virtual void StopRootSoundEntryNamedByRegistrySlot2At30() = 0;

    virtual void WriteSceneMarkerAt38(std::int32_t value) = 0;
    virtual void SynchronizeRestoredGroup0BeforeGroup3() = 0;
    virtual void RestoreEnvironmentGroup3(std::int32_t argument) = 0;
    virtual void SynchronizeRestoredGroup0AfterGroup3() = 0;
    virtual void SynchronizeRestoredRootOwners() = 0;
    [[nodiscard]] virtual LoadAfterTransitionSourceRef CaptureScreenBackup() = 0;
    virtual void ApplyPostRestoreScreenState(std::int32_t argument) = 0;
    virtual void ClearCurrentSceneName() = 0;

    virtual void ClearScenes() noexcept = 0;
    virtual void PushActiveScene(const char* sceneNameCp932) = 0;
    virtual void QueueScene(const char* sceneNameCp932) = 0;
    virtual void ResetScriptForDefaultRoute() = 0;
    virtual void ResetOwnerGroup3ForDefaultRoute() = 0;
};

// SaveLoadScene::CLoadAfter（0x4B6380）。Run 每帧严格执行 Update 后调用空 Draw；
// 状态范围为 0..4，route 9/13 会在状态 2 直接重建 active 并结束本帧。
class LoadAfterSelectionScene final : public ISceneTask
{
public:
    LoadAfterSelectionScene(
        ScrVmScriptCommandState& scriptCommandState,
        RestoredScriptTransaction& restoredScript,
        ILoadAfterSelectionServices& services);

    void Run(std::int32_t argument) override;

    [[nodiscard]] std::int32_t State() const noexcept;
    [[nodiscard]] const LoadAfterTransitionOperation& Transition() const noexcept;

private:
    static std::int32_t ResolveTransitionStep(std::uint32_t speedIndex) noexcept;

    void Update();
    void Draw() noexcept;
    void InitializeTransition(
        std::int32_t selector,
        const LoadAfterTransitionSourceRef& source);
    void AdvanceTransition();
    void RestoreNormalRoute(std::int32_t route);
    void RestoreImmediateRoute(const char* secondSceneNameCp932);
    void RestoreDefaultRoute();

    ScrVmScriptCommandState* scriptCommandState_ = nullptr;
    RestoredScriptTransaction* restoredScript_ = nullptr;
    ILoadAfterSelectionServices* services_ = nullptr;
    LoadAfterTransitionSourceRef blackTransitionSource_;
    LoadAfterTransitionSourceRef screenBackupSource_;
    std::uint32_t savedRegistrySlot2DwordAt04_ = 0;
    std::int32_t state_ = 0;
    LoadAfterTransitionOperation transition_;
    std::int32_t transitionStep_ = 0;
};

// SceneFactory 的“读取后处理”分支。脚本命令状态、恢复事务与场景服务均为
// 根对象中的同一份实例，禁止在构造适配层中复制恢复状态。
class LoadAfterSelectionSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    LoadAfterSelectionSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        ScrVmScriptCommandState& scriptCommandState,
        RestoredScriptTransaction& restoredScript,
        ILoadAfterSelectionServices& services) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateLoadAfterSelection() override;

private:
    ScrVmScriptCommandState* scriptCommandState_ = nullptr;
    RestoredScriptTransaction* restoredScript_ = nullptr;
    ILoadAfterSelectionServices* services_ = nullptr;
};
}
