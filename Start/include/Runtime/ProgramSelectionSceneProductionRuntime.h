#pragma once

#include <Runtime/ProgramSelectionDialogProductionRuntime.h>
#include <Runtime/ProgramSelectionSceneRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 保留 CProgramSelect 的场景状态服务，只接管 0x4A11E0 中同步创建
// CharaSelect00/01Dialog 的生产所有权边界。
class ProgramSelectionSceneProductionServices final
    : public IProgramSelectionSceneServices
{
public:
    ProgramSelectionSceneProductionServices(
        IProgramSelectionSceneServices& upstream,
        ProgramSelectionDialogProductionDependencies dialogDependencies) noexcept;

    [[nodiscard]] std::int32_t ReadProgramParameter00() const override;
    [[nodiscard]] ProgramSelectionDialogRef CreateCharacterSelectionDialog(
        std::int32_t mode) override;
    void MaskScriptSlot2DwordAt18With2() override;
    void WriteScriptSlot2ByteAt0C(std::uint8_t value) override;
    [[nodiscard]] std::uint32_t ReadScriptSlot2DwordAt14() const override;
    void MaskScriptSlot2DwordAt14With2() override;
    void WriteRootParamsDwordAt60(std::int32_t value) override;
    void WriteRootSelectionDwordAt04(std::uint32_t value) override;
    void WriteRootSelectionByteAt08(std::uint8_t value) override;
    void WriteRootDialogDwordAt10(std::uint32_t value) override;
    [[nodiscard]] std::string ReadCurrentSceneNameCp932() const override;
    [[nodiscard]] bool ReadCondition47F830() const override;
    [[nodiscard]] bool ReadCondition47F650() const override;
    [[nodiscard]] bool ReadCondition47F780() const override;
    [[nodiscard]] bool ReadCondition47F6D0() const override;
    [[nodiscard]] bool ReadCondition47F530WithOne() const override;
    [[nodiscard]] ProgramSelectionTransitionRef CaptureTransitionFrame() override;
    void SubmitTransitionFrame(
        const ProgramSelectionTransitionRef& transition) override;
    void ClearCurrentSceneName() override;
    void OpenSceneInCurrentGui(const char* sceneNameCp932) override;
    void FinalizeProgramSelectionAt47E940() override;
    void AdvanceActiveSceneFromPending() override;
    void WriteScriptSlot2DwordAt04(std::uint32_t value) override;
    void OrScriptSlot2DwordAt18With1() override;
    void WriteScriptSlot2ByteAt0CForCompletion(std::uint8_t value) override;
    [[nodiscard]] bool ReadScriptSlot1DwordAt38Bit8() const override;
    void WriteScriptSlot2DwordAt14(std::uint32_t value) override;
    void WriteRootByteAt1AF(std::uint8_t value) override;
    void ApplySelectionReference(
        const ProgramSelectionReference& reference) override;
    void ExecuteQuickSaveAt485FC0(std::int32_t value) override;
    [[nodiscard]] bool ExecuteQuickLoadAt485CE0() override;
    void ClearAllScenes() override;
    void QueueScene(const char* sceneNameCp932) override;
    [[nodiscard]] bool ReadScriptSlot1ByteAt7D() const override;
    [[nodiscard]] std::int32_t RunModalCommandMode1(
        const ProgramSelectionReference& reference) override;
    void PostMainWindowCommand(std::uint32_t command) override;

    [[nodiscard]] const ProgramSelectionDialogProductionRuntime*
        DialogOwner() const noexcept;

private:
    IProgramSelectionSceneServices* upstream_ = nullptr;
    ProgramSelectionDialogProductionDependencies dialogDependencies_;
    std::unique_ptr<ProgramSelectionDialogProductionRuntime> dialogOwner_;
};

// scene_ 必须先于 services_ 析构；只有场景持有的 Dialog 引用撤销后，
// services_ 才能按 0x4F8D50 释放完整角色选择对象图。
class ProgramSelectionSceneProductionObjectGraphRuntime final
{
public:
    ProgramSelectionSceneProductionObjectGraphRuntime(
        IProgramSelectionSceneServices& sceneUpstream,
        ProgramSelectionDialogProductionDependencies dialogDependencies);

    [[nodiscard]] const std::shared_ptr<ProgramSelectionSceneRuntime>&
        Scene() const noexcept;
    [[nodiscard]] const ProgramSelectionDialogProductionRuntime&
        DialogOwner() const;

private:
    ProgramSelectionSceneProductionServices services_;
    std::shared_ptr<ProgramSelectionSceneRuntime> scene_;
};

// SceneFactory 的“程序选择处理”生产适配层。返回指针指向内部 Scene，
// 但控制块持有完整 ProgramSelection 场景与 Dialog 对象图。
class ProgramSelectionSceneProductionConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    ProgramSelectionSceneProductionConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        IProgramSelectionSceneServices& programSelectionServices,
        ProgramSelectionDialogProductionDependencies dialogDependencies) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateProgramSelectionProcessing() override;

private:
    IProgramSelectionSceneServices* programSelectionServices_ = nullptr;
    ProgramSelectionDialogProductionDependencies dialogDependencies_;
};
}
