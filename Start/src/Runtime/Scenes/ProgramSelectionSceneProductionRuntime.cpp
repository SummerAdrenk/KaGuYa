#include <Runtime/ProgramSelectionSceneProductionRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ProgramSelectionSceneProductionServices::
ProgramSelectionSceneProductionServices(
    IProgramSelectionSceneServices& upstream,
    ProgramSelectionDialogProductionDependencies dialogDependencies) noexcept
    : upstream_(&upstream), dialogDependencies_(dialogDependencies)
{
}

std::int32_t
ProgramSelectionSceneProductionServices::ReadProgramParameter00() const
{
    return upstream_->ReadProgramParameter00();
}

ProgramSelectionDialogRef
ProgramSelectionSceneProductionServices::CreateCharacterSelectionDialog(
    const std::int32_t mode)
{
    if (dialogOwner_)
    {
        throw std::logic_error(
            "ProgramSelectionScene attempted to create its dialog more than once");
    }

    if (mode != 0 && mode != 1)
        throw std::logic_error("ProgramSelectionScene passed an invalid dialog mode");

    // 0x4A12A5/0x4A1358：场景先确定模式，再同步进入对应 Dialog 构造。
    dialogDependencies_.mode = static_cast<ProgramSelectionDialogMode>(mode);
    dialogOwner_ = std::make_unique<ProgramSelectionDialogProductionRuntime>(
        dialogDependencies_);
    return dialogOwner_->Dialog();
}

void ProgramSelectionSceneProductionServices::MaskScriptSlot2DwordAt18With2()
{
    upstream_->MaskScriptSlot2DwordAt18With2();
}

void ProgramSelectionSceneProductionServices::WriteScriptSlot2ByteAt0C(
    const std::uint8_t value)
{
    upstream_->WriteScriptSlot2ByteAt0C(value);
}

std::uint32_t
ProgramSelectionSceneProductionServices::ReadScriptSlot2DwordAt14() const
{
    return upstream_->ReadScriptSlot2DwordAt14();
}

void ProgramSelectionSceneProductionServices::MaskScriptSlot2DwordAt14With2()
{
    upstream_->MaskScriptSlot2DwordAt14With2();
}

void ProgramSelectionSceneProductionServices::WriteRootParamsDwordAt60(
    const std::int32_t value)
{
    upstream_->WriteRootParamsDwordAt60(value);
}

void ProgramSelectionSceneProductionServices::WriteRootSelectionDwordAt04(
    const std::uint32_t value)
{
    upstream_->WriteRootSelectionDwordAt04(value);
}

void ProgramSelectionSceneProductionServices::WriteRootSelectionByteAt08(
    const std::uint8_t value)
{
    upstream_->WriteRootSelectionByteAt08(value);
}

void ProgramSelectionSceneProductionServices::WriteRootDialogDwordAt10(
    const std::uint32_t value)
{
    upstream_->WriteRootDialogDwordAt10(value);
}

std::string
ProgramSelectionSceneProductionServices::ReadCurrentSceneNameCp932() const
{
    return upstream_->ReadCurrentSceneNameCp932();
}

bool ProgramSelectionSceneProductionServices::ReadCondition47F830() const
{
    return upstream_->ReadCondition47F830();
}

bool ProgramSelectionSceneProductionServices::ReadCondition47F650() const
{
    return upstream_->ReadCondition47F650();
}

bool ProgramSelectionSceneProductionServices::ReadCondition47F780() const
{
    return upstream_->ReadCondition47F780();
}

bool ProgramSelectionSceneProductionServices::ReadCondition47F6D0() const
{
    return upstream_->ReadCondition47F6D0();
}

bool
ProgramSelectionSceneProductionServices::ReadCondition47F530WithOne() const
{
    return upstream_->ReadCondition47F530WithOne();
}

ProgramSelectionTransitionRef
ProgramSelectionSceneProductionServices::CaptureTransitionFrame()
{
    return upstream_->CaptureTransitionFrame();
}

void ProgramSelectionSceneProductionServices::SubmitTransitionFrame(
    const ProgramSelectionTransitionRef& transition)
{
    upstream_->SubmitTransitionFrame(transition);
}

void ProgramSelectionSceneProductionServices::ClearCurrentSceneName()
{
    upstream_->ClearCurrentSceneName();
}

void ProgramSelectionSceneProductionServices::OpenSceneInCurrentGui(
    const char* const sceneNameCp932)
{
    upstream_->OpenSceneInCurrentGui(sceneNameCp932);
}

void ProgramSelectionSceneProductionServices::FinalizeProgramSelectionAt47E940()
{
    upstream_->FinalizeProgramSelectionAt47E940();
}

void ProgramSelectionSceneProductionServices::AdvanceActiveSceneFromPending()
{
    upstream_->AdvanceActiveSceneFromPending();
}

void ProgramSelectionSceneProductionServices::WriteScriptSlot2DwordAt04(
    const std::uint32_t value)
{
    upstream_->WriteScriptSlot2DwordAt04(value);
}

void ProgramSelectionSceneProductionServices::OrScriptSlot2DwordAt18With1()
{
    upstream_->OrScriptSlot2DwordAt18With1();
}

void ProgramSelectionSceneProductionServices::
WriteScriptSlot2ByteAt0CForCompletion(const std::uint8_t value)
{
    upstream_->WriteScriptSlot2ByteAt0CForCompletion(value);
}

bool
ProgramSelectionSceneProductionServices::ReadScriptSlot1DwordAt38Bit8() const
{
    return upstream_->ReadScriptSlot1DwordAt38Bit8();
}

void ProgramSelectionSceneProductionServices::WriteScriptSlot2DwordAt14(
    const std::uint32_t value)
{
    upstream_->WriteScriptSlot2DwordAt14(value);
}

void ProgramSelectionSceneProductionServices::WriteRootByteAt1AF(
    const std::uint8_t value)
{
    upstream_->WriteRootByteAt1AF(value);
}

void ProgramSelectionSceneProductionServices::ApplySelectionReference(
    const ProgramSelectionReference& reference)
{
    upstream_->ApplySelectionReference(reference);
}

void ProgramSelectionSceneProductionServices::ExecuteQuickSaveAt485FC0(
    const std::int32_t value)
{
    upstream_->ExecuteQuickSaveAt485FC0(value);
}

bool ProgramSelectionSceneProductionServices::ExecuteQuickLoadAt485CE0()
{
    return upstream_->ExecuteQuickLoadAt485CE0();
}

void ProgramSelectionSceneProductionServices::ClearAllScenes()
{
    upstream_->ClearAllScenes();
}

void ProgramSelectionSceneProductionServices::QueueScene(
    const char* const sceneNameCp932)
{
    upstream_->QueueScene(sceneNameCp932);
}

bool ProgramSelectionSceneProductionServices::ReadScriptSlot1ByteAt7D() const
{
    return upstream_->ReadScriptSlot1ByteAt7D();
}

std::int32_t ProgramSelectionSceneProductionServices::RunModalCommandMode1(
    const ProgramSelectionReference& reference)
{
    return upstream_->RunModalCommandMode1(reference);
}

void ProgramSelectionSceneProductionServices::PostMainWindowCommand(
    const std::uint32_t command)
{
    upstream_->PostMainWindowCommand(command);
}

const ProgramSelectionDialogProductionRuntime*
ProgramSelectionSceneProductionServices::DialogOwner() const noexcept
{
    return dialogOwner_.get();
}

ProgramSelectionSceneProductionObjectGraphRuntime::
ProgramSelectionSceneProductionObjectGraphRuntime(
    IProgramSelectionSceneServices& sceneUpstream,
    ProgramSelectionDialogProductionDependencies dialogDependencies)
    : services_(sceneUpstream, dialogDependencies),
      scene_(std::make_shared<ProgramSelectionSceneRuntime>(services_))
{
}

const std::shared_ptr<ProgramSelectionSceneRuntime>&
ProgramSelectionSceneProductionObjectGraphRuntime::Scene() const noexcept
{
    return scene_;
}

const ProgramSelectionDialogProductionRuntime&
ProgramSelectionSceneProductionObjectGraphRuntime::DialogOwner() const
{
    const auto* const owner = services_.DialogOwner();
    if (owner == nullptr)
        throw std::logic_error("ProgramSelectionScene has no production dialog owner");
    return *owner;
}

ProgramSelectionSceneProductionConstructionServicesRuntime::
ProgramSelectionSceneProductionConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    IProgramSelectionSceneServices& programSelectionServices,
    ProgramSelectionDialogProductionDependencies dialogDependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      programSelectionServices_(&programSelectionServices),
      dialogDependencies_(dialogDependencies)
{
}

std::shared_ptr<ISceneTask>
ProgramSelectionSceneProductionConstructionServicesRuntime::
CreateProgramSelectionProcessing()
{
    auto owner =
        std::make_shared<ProgramSelectionSceneProductionObjectGraphRuntime>(
            *programSelectionServices_, dialogDependencies_);
    ProgramSelectionSceneRuntime* const scene = owner->Scene().get();
    if (scene == nullptr)
        throw std::logic_error("ProgramSelection production owner has no scene");

    return std::shared_ptr<ISceneTask>(std::move(owner), scene);
}
}
