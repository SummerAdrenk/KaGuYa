#include <Runtime/ProgramSelectionSceneRuntime.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kSaveScreenCp932[] =
    "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
constexpr char kLoadScreenCp932[] =
    "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
constexpr char kQuickSaveCp932[] =
    "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75";
constexpr char kQuickLoadCp932[] =
    "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68";
constexpr char kSettingsScreenCp932[] =
    "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char kTitleCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6";
constexpr char kAffectionScreenCp932[] =
    "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
constexpr char kLoadAfterSelectionCp932[] =
    "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
constexpr char kUnsupportedDialogTypeCp932[] =
    "\x83\x5F\x83\x43\x83\x41\x83\x8D\x83\x4F\x82\xCC\x8E\xED\x97\xDE"
    "\x82\xAA\x91\xCE\x89\x9E\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB9"
    "\x82\xF1\x81\x49";

bool Equals(const std::string& value, const char* const expected) noexcept
{
    return std::strcmp(value.c_str(), expected) == 0;
}
}

ProgramSelectionSceneRuntime::ProgramSelectionSceneRuntime(
    IProgramSelectionSceneServices& services)
    : services_(&services)
{
    dialogMode_ = services_->ReadProgramParameter00();
    if (dialogMode_ != 0 && dialogMode_ != 1)
        throw kUnsupportedDialogTypeCp932;

    dialog_ = services_->CreateCharacterSelectionDialog(dialogMode_);
    if (!dialog_)
        throw std::runtime_error("ProgramSelection dialog construction returned null");

    services_->MaskScriptSlot2DwordAt18With2();
    services_->WriteScriptSlot2ByteAt0C(0u);
    savedScriptSlot2DwordAt14_ = services_->ReadScriptSlot2DwordAt14();
    services_->MaskScriptSlot2DwordAt14With2();
    services_->WriteRootParamsDwordAt60(-1);
    services_->WriteRootSelectionDwordAt04(0u);
    services_->WriteRootSelectionByteAt08(0u);
    phase_ = ProgramSelectionPhase::InitializeDialog;
}

void ProgramSelectionSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    // 5B4E3C -> 4B5E20：Draw 是原空函数。
}

ProgramSelectionPhase ProgramSelectionSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t ProgramSelectionSceneRuntime::SavedScriptSlot2DwordAt14() const noexcept
{
    return savedScriptSlot2DwordAt14_;
}

std::int32_t ProgramSelectionSceneRuntime::DialogMode() const noexcept
{
    return dialogMode_;
}

void ProgramSelectionSceneRuntime::Update()
{
    switch (phase_)
    {
    case ProgramSelectionPhase::InitializeDialog:
        services_->WriteRootDialogDwordAt10(0u);
        dialog_->BeginAtVslot44();
        phase_ = ProgramSelectionPhase::PollDialog;
        // 4B9837 直接落入 case 1。
        PollDialog();
        return;

    case ProgramSelectionPhase::PollDialog:
        PollDialog();
        return;

    case ProgramSelectionPhase::Complete:
        services_->FinalizeProgramSelectionAt47E940();
        services_->AdvanceActiveSceneFromPending();
        services_->WriteScriptSlot2DwordAt04(1u);
        services_->OrScriptSlot2DwordAt18With1();
        services_->WriteScriptSlot2ByteAt0CForCompletion(1u);
        services_->WriteScriptSlot2DwordAt14(
            services_->ReadScriptSlot1DwordAt38Bit8() ?
                savedScriptSlot2DwordAt14_ : 1u);
        return;

    case ProgramSelectionPhase::OpenSaveScreen:
        services_->WriteRootByteAt1AF(1u);
        services_->OpenSceneInCurrentGui(kSaveScreenCp932);
        phase_ = ProgramSelectionPhase::PollDialog;
        return;

    case ProgramSelectionPhase::OpenLoadScreen:
        services_->WriteRootByteAt1AF(1u);
        services_->OpenSceneInCurrentGui(kLoadScreenCp932);
        phase_ = ProgramSelectionPhase::PollDialog;
        return;

    case ProgramSelectionPhase::ApplySelection:
    {
        const ProgramSelectionReference reference = dialog_->ReadReferenceAtVslot9C();
        services_->ApplySelectionReference(reference);
        services_->ExecuteQuickSaveAt485FC0(0);
        phase_ = ProgramSelectionPhase::PollDialog;
        return;
    }

    case ProgramSelectionPhase::QuickSave:
    {
        const ProgramSelectionReference reference = dialog_->ReadReferenceAtVslot9C();
        services_->ApplySelectionReference(reference);
        if (services_->ExecuteQuickLoadAt485CE0())
        {
            services_->ClearAllScenes();
            services_->QueueScene(kLoadAfterSelectionCp932);
            services_->WriteScriptSlot2DwordAt04(0u);
        }
        else
        {
            phase_ = ProgramSelectionPhase::PollDialog;
        }
        return;
    }

    case ProgramSelectionPhase::ModalCommand:
        if (services_->ReadScriptSlot1ByteAt7D())
        {
            const ProgramSelectionReference reference = dialog_->ReadReferenceAtVslot9C();
            if (services_->RunModalCommandMode1(reference) != 6)
            {
                phase_ = ProgramSelectionPhase::PollDialog;
                return;
            }
        }
        services_->PostMainWindowCommand(0x8015u);
        services_->WriteScriptSlot2DwordAt04(0u);
        return;

    case ProgramSelectionPhase::ReopenDialog:
        services_->WriteScriptSlot2DwordAt14(
            services_->ReadScriptSlot1DwordAt38Bit8() ?
                (savedScriptSlot2DwordAt14_ & 2u) : 0u);
        dialog_->ReopenAtVslotA4();
        phase_ = ProgramSelectionPhase::PollDialog;
        // 4B97C7 直接落入 case 1。
        PollDialog();
        return;
    }
}

void ProgramSelectionSceneRuntime::PollDialog()
{
    std::string sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kSaveScreenCp932) || services_->ReadCondition47F830())
    {
        PrepareTransitionAndClearCurrentScene();
        phase_ = ProgramSelectionPhase::OpenSaveScreen;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kLoadScreenCp932) || services_->ReadCondition47F650())
    {
        PrepareTransitionAndClearCurrentScene();
        phase_ = ProgramSelectionPhase::OpenLoadScreen;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kQuickSaveCp932) || services_->ReadCondition47F780())
    {
        PrepareTransitionAndClearCurrentScene();
        phase_ = ProgramSelectionPhase::ApplySelection;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kQuickLoadCp932) || services_->ReadCondition47F6D0())
    {
        services_->ClearCurrentSceneName();
        phase_ = ProgramSelectionPhase::QuickSave;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kSettingsScreenCp932))
    {
        services_->ClearCurrentSceneName();
        services_->OpenSceneInCurrentGui(kSettingsScreenCp932);
        phase_ = ProgramSelectionPhase::ReopenDialog;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kTitleCp932))
    {
        services_->ClearCurrentSceneName();
        phase_ = ProgramSelectionPhase::ModalCommand;
        return;
    }

    sceneName = services_->ReadCurrentSceneNameCp932();
    if (Equals(sceneName, kAffectionScreenCp932) ||
        services_->ReadCondition47F530WithOne())
    {
        services_->ClearCurrentSceneName();
        services_->OpenSceneInCurrentGui(kAffectionScreenCp932);
        return;
    }

    if (!dialog_->ReadBusyAtVslot48())
    {
        dialog_->CloseAtVslot4C();
        phase_ = ProgramSelectionPhase::Complete;
    }
}

void ProgramSelectionSceneRuntime::PrepareTransitionAndClearCurrentScene()
{
    const ProgramSelectionTransitionRef transition = services_->CaptureTransitionFrame();
    services_->SubmitTransitionFrame(transition);
    services_->ClearCurrentSceneName();
}

ProgramSelectionSceneConstructionServicesRuntime::
ProgramSelectionSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    IProgramSelectionSceneServices& services) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      services_(&services)
{
}

std::shared_ptr<ISceneTask>
ProgramSelectionSceneConstructionServicesRuntime::
CreateProgramSelectionProcessing()
{
    return std::make_shared<ProgramSelectionSceneRuntime>(*services_);
}
}
