#pragma once

#include <Runtime/SaveLoadDialogProductionRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>
#include <Runtime/SaveLoadSceneRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 保留 CSaveLoadScene 的场景宿主，只接管 0x4A17B0 中通向
// SaveLoadDialog(0x4EB950) 的生产构造边界。
class SaveLoadSceneProductionServices final : public ISaveLoadSceneServices
{
public:
    SaveLoadSceneProductionServices(
        ISaveLoadSceneServices& upstream,
        SaveLoadDialogProductionDependencies dialogDependencies) noexcept;

    [[nodiscard]] std::uint8_t ReadRootByteAt1AF() const override;
    [[nodiscard]] SaveLoadDialogRef CreateSaveLoadDialog(
        SaveLoadSceneMode mode,
        std::uint8_t rootByteAt1AF) override;
    [[nodiscard]] std::uint32_t ReadRegistrySlot2DwordAt04() const override;
    void WriteRegistrySlot2DwordAt04(std::uint32_t value) override;
    [[nodiscard]] std::uint8_t ReadRootByteAt219() const override;
    void WriteRootByteAt219(std::uint8_t value) override;
    void WriteRootSubobjectAt4AD230DwordAt10(std::uint32_t value) override;
    void AdvanceActiveSceneFromPendingAt4C1760() override;
    void ClearActiveSceneAt4C1460() override;
    void QueueLoadAfterSelectionSceneAt47F920() override;

    [[nodiscard]] const SaveLoadDialogProductionRuntime*
        DialogOwner() const noexcept;

private:
    ISaveLoadSceneServices* upstream_ = nullptr;
    SaveLoadDialogProductionDependencies dialogDependencies_;
    std::unique_ptr<SaveLoadDialogProductionRuntime> dialogOwner_;
};

// 成员声明顺序保证 scene 先析构；其持有的 dialog 引用消失后，
// services 才按 0x4F9030 的顺序销毁完整 SaveLoadDialog 对象图。
class SaveLoadSceneProductionObjectGraphRuntime final
{
public:
    SaveLoadSceneProductionObjectGraphRuntime(
        ISaveLoadSceneServices& sceneUpstream,
        SaveLoadDialogProductionDependencies dialogDependencies,
        SaveLoadSceneMode mode);

    [[nodiscard]] const std::shared_ptr<SaveLoadSceneRuntime>&
        Scene() const noexcept;
    [[nodiscard]] const SaveLoadDialogProductionRuntime&
        DialogOwner() const;

private:
    SaveLoadSceneProductionServices services_;
    std::shared_ptr<SaveLoadSceneRuntime> scene_;
};

// SceneFactory 的 Save/Load 两个分支适配层。其余十五个分支严格委托给
// 已有生产服务；Save/Load 返回值的控制块直接持有完整场景对象图。
class SaveLoadSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    SaveLoadSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        ISaveLoadSceneServices& saveLoadServices,
        SaveLoadDialogProductionDependencies dialogDependencies) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask> CreateSaveScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateLoadScreen() override;

private:
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateSaveLoadScene(
        SaveLoadSceneMode mode);

    ISaveLoadSceneServices* saveLoadServices_ = nullptr;
    SaveLoadDialogProductionDependencies dialogDependencies_;
};
}
