#include <Runtime/MusicAppreciationProductionRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
MusicAppreciationProductionButtonServices::
MusicAppreciationProductionButtonServices(
    IMusicAppreciationButtonServices& upstream,
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    IMusicAppreciationModeSoundDialogInheritedServices& dialogServices) noexcept
    : upstream_(&upstream),
      surfaceFactory_(&surfaceFactory),
      graphics_(&graphics),
      dialogServices_(&dialogServices)
{
}

std::unique_ptr<IMusicAppreciationButtonDirtyBatch>
MusicAppreciationProductionButtonServices::CreateDirtyBatchAt4A2E60()
{
    return upstream_->CreateDirtyBatchAt4A2E60();
}

IMusicAppreciationButtonNavigationState&
MusicAppreciationProductionButtonServices::
NavigationStateAt4A2E60() noexcept
{
    return upstream_->NavigationStateAt4A2E60();
}

std::shared_ptr<IMusicAppreciationModeSoundDialog>
MusicAppreciationProductionButtonServices::CreateModeSoundDialogAt4E3900(
    const MusicAppreciationButtonStaticData& staticData)
{
    return std::make_shared<
        MusicAppreciationModeSoundDialogObjectGraphRuntime>(
            *surfaceFactory_, *graphics_, *dialogServices_, staticData);
}

std::shared_ptr<IMusicAppreciationGuiChildren>
MusicAppreciationProductionButtonServices::AcquireGuiChildrenAt47B7B0()
{
    return upstream_->AcquireGuiChildrenAt47B7B0();
}

RECT MusicAppreciationProductionButtonServices::
ReadGraphicsTargetRectAt4180C0() const
{
    return upstream_->ReadGraphicsTargetRectAt4180C0();
}

std::int32_t MusicAppreciationProductionButtonServices::
PollBaseSelectionAt4AA200()
{
    return upstream_->PollBaseSelectionAt4AA200();
}

void MusicAppreciationProductionButtonServices::ClearSoundMappingAt437680()
{
    upstream_->ClearSoundMappingAt437680();
}

void MusicAppreciationProductionButtonServices::StopCurrentSoundAt4383B0()
{
    upstream_->StopCurrentSoundAt4383B0();
}

void MusicAppreciationProductionButtonServices::BindAndPlayAt4BCCC0(
    const std::string_view resourceName)
{
    upstream_->BindAndPlayAt4BCCC0(resourceName);
}

void MusicAppreciationProductionButtonServices::EmitExitSoundAt4BCD70()
{
    upstream_->EmitExitSoundAt4BCD70();
}

std::string_view MusicAppreciationProductionButtonServices::
ReadExtensionAt45DA40(const std::string_view resourceName) const
{
    return upstream_->ReadExtensionAt45DA40(resourceName);
}

void MusicAppreciationProductionButtonServices::SubmitInvalidatedRect(
    const RECT& rect)
{
    upstream_->SubmitInvalidatedRect(rect);
}

MusicAppreciationProductionSceneServices::
MusicAppreciationProductionSceneServices(
    IMusicAppreciationSceneServices& upstream,
    IMusicAppreciationButtonServices& buttonServices,
    const MusicAppreciationButtonStaticData& staticData) noexcept
    : upstream_(&upstream),
      buttonServices_(&buttonServices),
      staticData_(&staticData)
{
}

MusicAppreciationTransitionSourceRef
MusicAppreciationProductionSceneServices::LoadTransitionSource(
    const char* const resourceNameCp932)
{
    return upstream_->LoadTransitionSource(resourceNameCp932);
}

void MusicAppreciationProductionSceneServices::PrepareSceneControllerAt4703D0(
    const std::int32_t mode)
{
    upstream_->PrepareSceneControllerAt4703D0(mode);
}

MusicAppreciationButtonRef
MusicAppreciationProductionSceneServices::CreateMusicAppreciationButton()
{
    return std::make_shared<MusicAppreciationButtonRuntime>(
        *buttonServices_, *staticData_);
}

MusicAppreciationTransitionSourceRef
MusicAppreciationProductionSceneServices::CapturePrimarySceneResourceAt4ACDD0()
{
    return upstream_->CapturePrimarySceneResourceAt4ACDD0();
}

void MusicAppreciationProductionSceneServices::StopMusicEntryAt4AA890(
    const std::int32_t index)
{
    upstream_->StopMusicEntryAt4AA890(index);
}

void MusicAppreciationProductionSceneServices::ResetMusicEntriesAt4AA780()
{
    upstream_->ResetMusicEntriesAt4AA780();
}

void MusicAppreciationProductionSceneServices::ClearCurrentSceneName()
{
    upstream_->ClearCurrentSceneName();
}

void MusicAppreciationProductionSceneServices::WriteRegistrySlot2DwordAt04(
    const std::uint32_t value)
{
    upstream_->WriteRegistrySlot2DwordAt04(value);
}

void MusicAppreciationProductionSceneServices::WriteRegistrySlot2ByteAt09(
    const std::uint8_t value)
{
    upstream_->WriteRegistrySlot2ByteAt09(value);
}

void MusicAppreciationProductionSceneServices::WriteRegistrySlot2ByteAt24(
    const std::uint8_t value)
{
    upstream_->WriteRegistrySlot2ByteAt24(value);
}

void MusicAppreciationProductionSceneServices::WriteRegistrySlot2ByteAt25(
    const std::uint8_t value)
{
    upstream_->WriteRegistrySlot2ByteAt25(value);
}

void MusicAppreciationProductionSceneServices::WriteRegistrySlot2ByteAt26(
    const std::uint8_t value)
{
    upstream_->WriteRegistrySlot2ByteAt26(value);
}

void MusicAppreciationProductionSceneServices::ClearRegistrySlot2StringAt2C()
{
    upstream_->ClearRegistrySlot2StringAt2C();
}

void MusicAppreciationProductionSceneServices::InvokeSoundProcedureAt4383B0()
{
    upstream_->InvokeSoundProcedureAt4383B0();
}

void MusicAppreciationProductionSceneServices::WriteRootByteAt218(
    const std::uint8_t value)
{
    upstream_->WriteRootByteAt218(value);
}

void MusicAppreciationProductionSceneServices::WriteRootByteAt219(
    const std::uint8_t value)
{
    upstream_->WriteRootByteAt219(value);
}

bool MusicAppreciationProductionSceneServices::
ShouldAccelerateTransitionAt4AA970()
{
    return upstream_->ShouldAccelerateTransitionAt4AA970();
}

void MusicAppreciationProductionSceneServices::RenderTransition(
    MusicAppreciationTransitionOperation& operation)
{
    upstream_->RenderTransition(operation);
}

void MusicAppreciationProductionSceneServices::CommitTransitionFrameAt4B02E0(
    const std::int32_t argument)
{
    upstream_->CommitTransitionFrameAt4B02E0(argument);
}

std::string_view MusicAppreciationProductionSceneServices::
ReadCurrentSceneName() const
{
    return upstream_->ReadCurrentSceneName();
}

std::string_view MusicAppreciationProductionSceneServices::
ReadReturnSourceSceneName() const
{
    return upstream_->ReadReturnSourceSceneName();
}

std::string_view MusicAppreciationProductionSceneServices::
ReadReturnFallbackSceneName() const
{
    return upstream_->ReadReturnFallbackSceneName();
}

bool MusicAppreciationProductionSceneServices::
ReadTitleReturnGateAt4B0740() const
{
    return upstream_->ReadTitleReturnGateAt4B0740();
}

void MusicAppreciationProductionSceneServices::PushActiveSceneAt4A9030(
    const char* const sceneNameCp932)
{
    upstream_->PushActiveSceneAt4A9030(sceneNameCp932);
}

void MusicAppreciationProductionSceneServices::QueuePendingSceneAt47F920(
    const char* const sceneNameCp932)
{
    upstream_->QueuePendingSceneAt47F920(sceneNameCp932);
}

void MusicAppreciationProductionSceneServices::SynchronizeButtonAt4ABB40(
    const MusicAppreciationButtonRef& button)
{
    upstream_->SynchronizeButtonAt4ABB40(button);
}

void MusicAppreciationProductionSceneServices::ReadGraphicsTargetSize(
    std::int32_t& width,
    std::int32_t& height) const
{
    upstream_->ReadGraphicsTargetSize(width, height);
}

MusicAppreciationProductionObjectGraphRuntime::
MusicAppreciationProductionObjectGraphRuntime(
    IMusicAppreciationSceneServices& sceneUpstream,
    IMusicAppreciationButtonServices& buttonUpstream,
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    IMusicAppreciationModeSoundDialogInheritedServices& dialogServices,
    const MusicAppreciationButtonStaticData& staticData)
    : buttonServices_(
          buttonUpstream, surfaceFactory, graphics, dialogServices),
      sceneServices_(sceneUpstream, buttonServices_, staticData),
      scene_(std::make_shared<MusicAppreciationSceneRuntime>(sceneServices_))
{
}

const std::shared_ptr<MusicAppreciationSceneRuntime>&
MusicAppreciationProductionObjectGraphRuntime::Scene() const noexcept
{
    return scene_;
}

MusicAppreciationSceneConstructionServicesRuntime::
MusicAppreciationSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    MusicAppreciationProductionDependencies dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      dependencies_(std::move(dependencies))
{
}

std::shared_ptr<ISceneTask>
MusicAppreciationSceneConstructionServicesRuntime::
CreateMusicAppreciationMode()
{
    auto owner =
        std::make_shared<MusicAppreciationProductionObjectGraphRuntime>(
            dependencies_.sceneUpstream,
            dependencies_.buttonUpstream,
            dependencies_.surfaceFactory,
            dependencies_.graphics,
            dependencies_.dialogServices,
            dependencies_.staticData);
    MusicAppreciationSceneRuntime* const scene = owner->Scene().get();
    if (scene == nullptr)
        throw std::logic_error("Music production owner has no scene");

    return std::shared_ptr<ISceneTask>(std::move(owner), scene);
}
}
