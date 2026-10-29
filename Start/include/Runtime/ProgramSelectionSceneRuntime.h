#pragma once

#include <Runtime/SceneFactoryRuntime.h>

#include <cstdint>
#include <memory>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IProgramSelectionReference
{
public:
    virtual ~IProgramSelectionReference() = default;
};

using ProgramSelectionReference = std::shared_ptr<IProgramSelectionReference>;

class IProgramSelectionDialog
{
public:
    virtual ~IProgramSelectionDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual ProgramSelectionReference ReadReferenceAtVslot9C() = 0;
    [[nodiscard]] virtual bool ReadBusyAtVslot48() const = 0;
    virtual void CloseAtVslot4C() = 0;
    virtual void ReopenAtVslotA4() = 0;
};

using ProgramSelectionDialogRef = std::shared_ptr<IProgramSelectionDialog>;

class IProgramSelectionTransitionResource
{
public:
    virtual ~IProgramSelectionTransitionResource() = default;
};

using ProgramSelectionTransitionRef =
    std::shared_ptr<IProgramSelectionTransitionResource>;

enum class ProgramSelectionPhase : std::uint32_t
{
    InitializeDialog = 0,
    PollDialog = 1,
    Complete = 2,
    OpenSaveScreen = 4,
    OpenLoadScreen = 5,
    ApplySelection = 6,
    QuickSave = 8,
    ModalCommand = 10,
    ReopenDialog = 12,
};

class IProgramSelectionSceneServices
{
public:
    virtual ~IProgramSelectionSceneServices() = default;

    [[nodiscard]] virtual std::int32_t ReadProgramParameter00() const = 0;
    [[nodiscard]] virtual ProgramSelectionDialogRef CreateCharacterSelectionDialog(
        std::int32_t mode) = 0;

    virtual void MaskScriptSlot2DwordAt18With2() = 0;
    virtual void WriteScriptSlot2ByteAt0C(std::uint8_t value) = 0;
    [[nodiscard]] virtual std::uint32_t ReadScriptSlot2DwordAt14() const = 0;
    virtual void MaskScriptSlot2DwordAt14With2() = 0;
    virtual void WriteRootParamsDwordAt60(std::int32_t value) = 0;
    virtual void WriteRootSelectionDwordAt04(std::uint32_t value) = 0;
    virtual void WriteRootSelectionByteAt08(std::uint8_t value) = 0;

    virtual void WriteRootDialogDwordAt10(std::uint32_t value) = 0;
    [[nodiscard]] virtual std::string ReadCurrentSceneNameCp932() const = 0;
    [[nodiscard]] virtual bool ReadCondition47F830() const = 0;
    [[nodiscard]] virtual bool ReadCondition47F650() const = 0;
    [[nodiscard]] virtual bool ReadCondition47F780() const = 0;
    [[nodiscard]] virtual bool ReadCondition47F6D0() const = 0;
    [[nodiscard]] virtual bool ReadCondition47F530WithOne() const = 0;

    [[nodiscard]] virtual ProgramSelectionTransitionRef CaptureTransitionFrame() = 0;
    virtual void SubmitTransitionFrame(
        const ProgramSelectionTransitionRef& transition) = 0;
    virtual void ClearCurrentSceneName() = 0;
    virtual void OpenSceneInCurrentGui(const char* sceneNameCp932) = 0;

    virtual void FinalizeProgramSelectionAt47E940() = 0;
    virtual void AdvanceActiveSceneFromPending() = 0;
    virtual void WriteScriptSlot2DwordAt04(std::uint32_t value) = 0;
    virtual void OrScriptSlot2DwordAt18With1() = 0;
    virtual void WriteScriptSlot2ByteAt0CForCompletion(std::uint8_t value) = 0;
    [[nodiscard]] virtual bool ReadScriptSlot1DwordAt38Bit8() const = 0;
    virtual void WriteScriptSlot2DwordAt14(std::uint32_t value) = 0;

    virtual void WriteRootByteAt1AF(std::uint8_t value) = 0;
    virtual void ApplySelectionReference(
        const ProgramSelectionReference& reference) = 0;
    virtual void ExecuteQuickSaveAt485FC0(std::int32_t value) = 0;
    [[nodiscard]] virtual bool ExecuteQuickLoadAt485CE0() = 0;
    virtual void ClearAllScenes() = 0;
    virtual void QueueScene(const char* sceneNameCp932) = 0;

    [[nodiscard]] virtual bool ReadScriptSlot1ByteAt7D() const = 0;
    [[nodiscard]] virtual std::int32_t RunModalCommandMode1(
        const ProgramSelectionReference& reference) = 0;
    virtual void PostMainWindowCommand(std::uint32_t command) = 0;
};

// ProgramSelectScene::CProgramSelect（4A11E0/4B9710）的生产状态机。
class ProgramSelectionSceneRuntime final : public ISceneTask
{
public:
    explicit ProgramSelectionSceneRuntime(IProgramSelectionSceneServices& services);

    void Run(std::int32_t argument) override;

    [[nodiscard]] ProgramSelectionPhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedScriptSlot2DwordAt14() const noexcept;
    [[nodiscard]] std::int32_t DialogMode() const noexcept;

private:
    void Update();
    void PollDialog();
    void PrepareTransitionAndClearCurrentScene();

    IProgramSelectionSceneServices* services_ = nullptr;
    ProgramSelectionPhase phase_ = ProgramSelectionPhase::InitializeDialog;
    ProgramSelectionDialogRef dialog_;
    std::uint32_t savedScriptSlot2DwordAt14_ = 0;
    std::int32_t dialogMode_ = 0;
};

// SceneFactory 的“程序选择处理”分支。具体 CharaSelect00/01 对话框必须由
// 同一根 services 按 prog_param00 构造；本层只接通工厂所有权，不复制全局状态。
class ProgramSelectionSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    ProgramSelectionSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        IProgramSelectionSceneServices& services) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateProgramSelectionProcessing() override;

private:
    IProgramSelectionSceneServices* services_ = nullptr;
};
}
