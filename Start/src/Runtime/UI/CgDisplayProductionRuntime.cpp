#include <Runtime/CgDisplayProductionRuntime.h>
#include <Runtime/CgDisplayResourceExecutor.h>
#include <Runtime/CgDisplaySelectionRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
CgDisplayProductionSceneServicesSample18::
CgDisplayProductionSceneServicesSample18(
    CgDisplayProductionDependenciesSample18 dependencies)
    : dependencies_(std::move(dependencies))
{
}

void CgDisplayProductionSceneServicesSample18::ClearCurrentSceneName()
{
    dependencies_.sceneUpstream.ClearCurrentSceneName();
}

CgDisplayTransitionSourceRef
CgDisplayProductionSceneServicesSample18::LoadTransitionSource(
    const char* const resourceNameCp932)
{
    return dependencies_.sceneUpstream.LoadTransitionSource(resourceNameCp932);
}

CgDisplayTransitionSourceRef
CgDisplayProductionSceneServicesSample18::CreateAuxiliaryListSourceAt40C2E0()
{
    return dependencies_.sceneUpstream.CreateAuxiliaryListSourceAt40C2E0();
}

bool CgDisplayProductionSceneServicesSample18::
InitializeAuxiliaryListSourceAt415010(
    const CgDisplayTransitionSourceRef& source)
{
    return dependencies_.sceneUpstream.
        InitializeAuxiliaryListSourceAt415010(source);
}

void CgDisplayProductionSceneServicesSample18::InvokeAt4703D0WithNull()
{
    dependencies_.sceneUpstream.InvokeAt4703D0WithNull();
}

CgDisplayButtonRef
CgDisplayProductionSceneServicesSample18::CreateCgDisplayButton(
    const std::int32_t index)
{
    auto button = CreateCgDisplayButtonGroupSample18ProductionAt499DC0(
        index,
        CgDisplayButtonGroupSample18ProductionDependenciesAt499DC0{
            dependencies_.secretCg,
            dependencies_.thumbnailParams,
            dependencies_.slot1,
            dependencies_.buttonGroupServices,
            dependencies_.surfaceFactory,
            dependencies_.surfaceApi,
            dependencies_.compositionApi,
            dependencies_.copyApi,
            dependencies_.uiButtonServices,
            dependencies_.sharedControls});
    return CgDisplayButtonRef(std::move(button));
}

CgDisplayWindowRef
CgDisplayProductionSceneServicesSample18::CreateCgDisplayWindow()
{
    return std::make_shared<CgDisplayWindowRuntime>(
        dependencies_.displaySpriteSlotCount,
        dependencies_.surfaceApi,
        dependencies_.displayLayerList,
        dependencies_.displayWindowServices,
        dependencies_.compositionApi,
        dependencies_.screenWidth,
        dependencies_.screenHeight);
}

CgOperationWindowRef
CgDisplayProductionSceneServicesSample18::CreateCgOperationWindow()
{
    CgOperationProductionObjectGraphRuntime operationGraph(
        dependencies_.operationRegistrationLayer,
        dependencies_.operationButtonServices,
        dependencies_.operationPopupServices,
        dependencies_.operationDialogServices,
        dependencies_.operationWindowServices,
        dependencies_.operationWindowLayer,
        dependencies_.compositionApi,
        dependencies_.surfaceFactory,
        dependencies_.operationButtonVisualLayout,
        dependencies_.operationButtonSurfaceTemplateRect);
    const auto& window = operationGraph.Graph()->Window();
    operationWindow_ = window;
    return window;
}

void CgDisplayProductionSceneServicesSample18::
QueueTitleAfterConstructionFailureAt47E180()
{
    dependencies_.sceneUpstream.QueueTitleAfterConstructionFailureAt47E180();
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2DwordAt04(
    const std::uint32_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2DwordAt04(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2ByteAt09(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2ByteAt09(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2ByteAt24(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2ByteAt24(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2ByteAt25(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2ByteAt25(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2ByteAt26(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2ByteAt26(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRegistrySlot2ByteAt27(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRegistrySlot2ByteAt27(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRootByteAt218(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRootByteAt218(value);
}

void CgDisplayProductionSceneServicesSample18::WriteRootByteAt219(
    const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteRootByteAt219(value);
}

bool CgDisplayProductionSceneServicesSample18::
ShouldAccelerateTransitionAt4AA970()
{
    return dependencies_.sceneUpstream.ShouldAccelerateTransitionAt4AA970();
}

void CgDisplayProductionSceneServicesSample18::RenderTransition(
    CgDisplayTransitionOperation& operation)
{
    dependencies_.sceneUpstream.RenderTransition(operation);
}

void CgDisplayProductionSceneServicesSample18::CommitTransitionFrameAt4B02E0(
    const std::int32_t argument)
{
    dependencies_.sceneUpstream.CommitTransitionFrameAt4B02E0(argument);
}

void CgDisplayProductionSceneServicesSample18::
InitializeEntrySubmissionAt4BCCC0()
{
    dependencies_.sceneUpstream.InitializeEntrySubmissionAt4BCCC0();
}

std::string_view
CgDisplayProductionSceneServicesSample18::ReadCurrentSceneName() const
{
    return dependencies_.sceneUpstream.ReadCurrentSceneName();
}

std::string_view
CgDisplayProductionSceneServicesSample18::ReadReturnSourceSceneName() const
{
    return dependencies_.sceneUpstream.ReadReturnSourceSceneName();
}

std::string_view
CgDisplayProductionSceneServicesSample18::ReadReturnFallbackSceneName() const
{
    return dependencies_.sceneUpstream.ReadReturnFallbackSceneName();
}

bool CgDisplayProductionSceneServicesSample18::
ReadTitleReturnGateAt4B0740() const
{
    return dependencies_.sceneUpstream.ReadTitleReturnGateAt4B0740();
}

void CgDisplayProductionSceneServicesSample18::PushActiveSceneAt4A9030(
    const char* const sceneNameCp932)
{
    dependencies_.sceneUpstream.PushActiveSceneAt4A9030(sceneNameCp932);
}

void CgDisplayProductionSceneServicesSample18::QueuePendingSceneAt47F920(
    const char* const sceneNameCp932)
{
    dependencies_.sceneUpstream.QueuePendingSceneAt47F920(sceneNameCp932);
}

std::int32_t
CgDisplayProductionSceneServicesSample18::ReadExitRouteCountAt47B7B0() const
{
    return dependencies_.sceneUpstream.ReadExitRouteCountAt47B7B0();
}

bool CgDisplayProductionSceneServicesSample18::BuildCgSelectionAt4AD780(
    const std::int32_t buttonIndex,
    const std::int32_t itemIndex,
    CgDisplaySelectionState& state)
{
    return BuildCgDisplaySelectionAt4AD780And4B74A0(
        dependencies_.secretCg,
        dependencies_.gameSystem,
        buttonIndex,
        itemIndex,
        state);
}

void CgDisplayProductionSceneServicesSample18::FinalizeCgSelectionAt47E940()
{
    dependencies_.sceneUpstream.FinalizeCgSelectionAt47E940();
}

void CgDisplayProductionSceneServicesSample18::
WriteCgSelectionOwnerByteAt0C(const std::uint8_t value)
{
    dependencies_.sceneUpstream.WriteCgSelectionOwnerByteAt0C(value);
}

bool CgDisplayProductionSceneServicesSample18::PrepareSelectedCgAt4A84E0(
    const CgDisplaySelectionState& state,
    const CgDisplayWindowRef& displayWindow)
{
    // display 只借 Scene 的实际成员槽，各原 getter 点才解析，不提前增持或判空。
    // operation/Dialog 的既存现代装配仍保留，不据此声称原 CG 根拥有链已经闭合。
    const auto concreteOperation = operationWindow_.lock();
    if (!concreteOperation || !concreteOperation->DialogAt24())
    {
        return false;
    }

    CgDisplayResourceRuntime target(
        displayWindow,
        *concreteOperation,
        dependencies_.displayResourceLoader,
        *concreteOperation->DialogAt24());
    return ExecuteCgDisplayResourceSelectionAt4A84E0(
        dependencies_.pattern,
        state,
        target);
}

CgDisplayTransitionSourceRef
CgDisplayProductionSceneServicesSample18::
CapturePrimarySceneResourceAt4ACDD0()
{
    return dependencies_.sceneUpstream.
        CapturePrimarySceneResourceAt4ACDD0();
}

std::int32_t CgDisplayProductionSceneServicesSample18::
ReadTransitionSourceValueAt04(
    const CgDisplayTransitionSourceRef& source) const
{
    return dependencies_.sceneUpstream.ReadTransitionSourceValueAt04(source);
}

void CgDisplayProductionSceneServicesSample18::
SubmitCgDisplayWindowRectToParentDirtyRects(
    const CgDisplayWindowRef& displayWindow)
{
    dependencies_.sceneUpstream.
        SubmitCgDisplayWindowRectToParentDirtyRects(displayWindow);
}

void CgDisplayProductionSceneServicesSample18::CompositeAuxiliaryListSource(
    const CgDisplayTransitionSourceRef& source)
{
    dependencies_.sceneUpstream.CompositeAuxiliaryListSource(source);
}

void CgDisplayProductionSceneServicesSample18::ClearDirtyRectsAt4AA780()
{
    dependencies_.sceneUpstream.ClearDirtyRectsAt4AA780();
}

void CgDisplayProductionSceneServicesSample18::
InvokeCgOperationCommand4At4BCD70()
{
    dependencies_.sceneUpstream.InvokeCgOperationCommand4At4BCD70();
}

bool CgDisplayProductionSceneServicesSample18::ReadCgCloseInputAt47F4E0(
    const std::int32_t value) const
{
    return dependencies_.sceneUpstream.ReadCgCloseInputAt47F4E0(value);
}

void CgDisplayProductionSceneServicesSample18::InvokeCgOwnerVslot08()
{
    dependencies_.sceneUpstream.InvokeCgOwnerVslot08();
}

bool CgDisplayProductionSceneServicesSample18::
ReadRegistrySlot1ByteAt6D() const
{
    return dependencies_.sceneUpstream.ReadRegistrySlot1ByteAt6D();
}

void CgDisplayProductionSceneServicesSample18::DrawCgGlobalLayerAtVslot18()
{
    dependencies_.sceneUpstream.DrawCgGlobalLayerAtVslot18();
}

CgDisplayProductionObjectGraphRuntimeSample18::
CgDisplayProductionObjectGraphRuntimeSample18(
    CgDisplayProductionDependenciesSample18 dependencies)
    : sceneServices_(std::move(dependencies)),
      scene_(std::make_shared<CgDisplaySceneRuntime>(sceneServices_))
{
}

const std::shared_ptr<CgDisplaySceneRuntime>&
CgDisplayProductionObjectGraphRuntimeSample18::Scene() const noexcept
{
    return scene_;
}

CgDisplaySceneConstructionServicesSample18::
CgDisplaySceneConstructionServicesSample18(
    ISceneConstructionServices& upstream,
    CgDisplayProductionDependenciesSample18 dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      dependencies_(std::move(dependencies))
{
}

std::shared_ptr<ISceneTask>
CgDisplaySceneConstructionServicesSample18::CreateCgDisplayMode()
{
    auto owner =
        std::make_shared<CgDisplayProductionObjectGraphRuntimeSample18>(
            dependencies_);
    CgDisplaySceneRuntime* const scene = owner->Scene().get();
    if (scene == nullptr)
        throw std::logic_error("CG production owner has no scene");

    return std::shared_ptr<ISceneTask>(std::move(owner), scene);
}
}
