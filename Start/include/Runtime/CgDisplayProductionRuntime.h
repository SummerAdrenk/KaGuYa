#pragma once

#include <Runtime/ParamsPatternSource.h>

#include <Runtime/CgDisplayButtonGroupSample18GraphicsProvider.h>
#include <Runtime/CgDisplayResourceRuntime.h>
#include <Runtime/CgDisplayWindowRuntime.h>
#include <Runtime/CgOperationObjectGraphRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x499DC0 末尾分别通过 0x478A30 与 0x47B7B0 取得两个共享对象。
// 这两项仍属于公共 GuiDialog 根对象，不能在 CCGdispBtn 内自行构造。
struct CgDisplayProductionDependenciesSample18 final
{
    ICgDisplaySceneServices& sceneUpstream;
    SecretCgArchive& secretCg;
    const ParamsGameSystem& gameSystem;
    ISecretCgParamsRootSource& thumbnailParams;
    const IParamsPatternSource& pattern;
    const IProgramRegistrySlot1& slot1;
    ICgDisplaySpriteResourceLoader& displayResourceLoader;
    ICgDisplayButtonGroupRuntimeServices& buttonGroupServices;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceApi& surfaceApi;
    IGraphicsSurfaceCompositionApi& compositionApi;
    IGraphicsSurfaceCopyApi& copyApi;
    ICgUiButtonRuntimeServices& uiButtonServices;

    std::size_t displaySpriteSlotCount;
    LayerList* displayLayerList;
    ICgDisplayWindowServices& displayWindowServices;
    std::int32_t screenWidth;
    std::int32_t screenHeight;

    ICgOperationButtonsServices& operationButtonServices;
    ICgOperationPopupServices& operationPopupServices;
    ICgOperationDialogServices& operationDialogServices;
    ICgOperationWindowServices& operationWindowServices;
    LayerList* operationRegistrationLayer;
    CgOperationWindowLayerRef operationWindowLayer;
    CgOperationButtonsVisualLayout operationButtonVisualLayout;
    RECT operationButtonSurfaceTemplateRect;
};

// CModeCGdisp 的生产 services：只替换 0x49E3B0 已闭环的三个创建点，
// 场景栈、输入、转场和 registry 等宿主行为仍逐项交给真正 owner。
class CgDisplayProductionSceneServicesSample18 final
    : public ICgDisplaySceneServices
{
public:
    explicit CgDisplayProductionSceneServicesSample18(
        CgDisplayProductionDependenciesSample18 dependencies);

    void ClearCurrentSceneName() override;
    [[nodiscard]] CgDisplayTransitionSourceRef LoadTransitionSource(
        const char* resourceNameCp932) override;
    [[nodiscard]] CgDisplayTransitionSourceRef
        CreateAuxiliaryListSourceAt40C2E0() override;
    [[nodiscard]] bool InitializeAuxiliaryListSourceAt415010(
        const CgDisplayTransitionSourceRef& source) override;
    void InvokeAt4703D0WithNull() override;
    [[nodiscard]] CgDisplayButtonRef CreateCgDisplayButton(
        std::int32_t index) override;
    [[nodiscard]] CgDisplayWindowRef CreateCgDisplayWindow() override;
    [[nodiscard]] CgOperationWindowRef CreateCgOperationWindow() override;
    void QueueTitleAfterConstructionFailureAt47E180() override;

    void WriteRegistrySlot2DwordAt04(std::uint32_t value) override;
    void WriteRegistrySlot2ByteAt09(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt24(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt25(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt26(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt27(std::uint8_t value) override;
    void WriteRootByteAt218(std::uint8_t value) override;
    void WriteRootByteAt219(std::uint8_t value) override;

    [[nodiscard]] bool ShouldAccelerateTransitionAt4AA970() override;
    void RenderTransition(CgDisplayTransitionOperation& operation) override;
    void CommitTransitionFrameAt4B02E0(std::int32_t argument) override;
    void InitializeEntrySubmissionAt4BCCC0() override;

    [[nodiscard]] std::string_view ReadCurrentSceneName() const override;
    [[nodiscard]] std::string_view ReadReturnSourceSceneName() const override;
    [[nodiscard]] std::string_view ReadReturnFallbackSceneName() const override;
    [[nodiscard]] bool ReadTitleReturnGateAt4B0740() const override;
    void PushActiveSceneAt4A9030(const char* sceneNameCp932) override;
    void QueuePendingSceneAt47F920(const char* sceneNameCp932) override;
    [[nodiscard]] std::int32_t ReadExitRouteCountAt47B7B0() const override;

    [[nodiscard]] bool BuildCgSelectionAt4AD780(
        std::int32_t buttonIndex,
        std::int32_t itemIndex,
        CgDisplaySelectionState& state) override;
    void FinalizeCgSelectionAt47E940() override;
    void WriteCgSelectionOwnerByteAt0C(std::uint8_t value) override;
    [[nodiscard]] bool PrepareSelectedCgAt4A84E0(
        const CgDisplaySelectionState& state,
        const CgDisplayWindowRef& displayWindow) override;

    [[nodiscard]] CgDisplayTransitionSourceRef
        CapturePrimarySceneResourceAt4ACDD0() override;
    [[nodiscard]] std::int32_t ReadTransitionSourceValueAt04(
        const CgDisplayTransitionSourceRef& source) const override;
    void SubmitCgDisplayWindowRectToParentDirtyRects(
        const CgDisplayWindowRef& displayWindow) override;
    void CompositeAuxiliaryListSource(
        const CgDisplayTransitionSourceRef& source) override;
    void ClearDirtyRectsAt4AA780() override;

    void InvokeCgOperationCommand4At4BCD70() override;
    [[nodiscard]] bool ReadCgCloseInputAt47F4E0(
        std::int32_t value) const override;
    void InvokeCgOwnerVslot08() override;
    [[nodiscard]] bool ReadRegistrySlot1ByteAt6D() const override;
    void DrawCgGlobalLayerAtVslot18() override;

private:
    CgDisplayProductionDependenciesSample18 dependencies_;
    // CModeCGdisp 才是窗口 owner；services 不能延长 CGOpeWin 生命周期。
    std::weak_ptr<CgOperationWindowRuntime> operationWindow_;
};

// 成员声明顺序保证 services 晚于 Scene 析构；其内部 Operation graph
// 又晚于 CModeCGdisp 释放，保持原 0x49E3B0 的强所有权关系。
class CgDisplayProductionObjectGraphRuntimeSample18 final
{
public:
    explicit CgDisplayProductionObjectGraphRuntimeSample18(
        CgDisplayProductionDependenciesSample18 dependencies);

    [[nodiscard]] const std::shared_ptr<CgDisplaySceneRuntime>&
        Scene() const noexcept;

private:
    CgDisplayProductionSceneServicesSample18 sceneServices_;
    std::shared_ptr<CgDisplaySceneRuntime> scene_;
};

// SceneFactory 的 CG 分支适配层。返回的别名 shared_ptr 对外仍是 ISceneTask，
// 但其控制块持有完整生产对象图，确保场景退出前 services 和子窗口不会先行析构。
class CgDisplaySceneConstructionServicesSample18 final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    CgDisplaySceneConstructionServicesSample18(
        ISceneConstructionServices& upstream,
        CgDisplayProductionDependenciesSample18 dependencies) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateCgDisplayMode() override;

private:
    CgDisplayProductionDependenciesSample18 dependencies_;
};
}
