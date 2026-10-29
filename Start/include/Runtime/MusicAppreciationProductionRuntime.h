#pragma once

#include <Runtime/MusicAppreciationButtonRuntime.h>
#include <Runtime/MusicAppreciationModeSoundDialogRuntime.h>
#include <Runtime/MusicAppreciationSceneRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
struct MusicAppreciationProductionDependencies final
{
    IMusicAppreciationSceneServices& sceneUpstream;
    IMusicAppreciationButtonServices& buttonUpstream;
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceCompositionApi& graphics;
    IMusicAppreciationModeSoundDialogInheritedServices& dialogServices;
    const MusicAppreciationButtonStaticData& staticData;
};

// 保留 CSoundBtn 其余宿主依赖，只把 0x4E3900 替换为已经审计的生产对象图。
class MusicAppreciationProductionButtonServices final
    : public IMusicAppreciationButtonServices
{
public:
    MusicAppreciationProductionButtonServices(
        IMusicAppreciationButtonServices& upstream,
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        IMusicAppreciationModeSoundDialogInheritedServices& dialogServices) noexcept;

    [[nodiscard]] std::unique_ptr<IMusicAppreciationButtonDirtyBatch>
        CreateDirtyBatchAt4A2E60() override;
    [[nodiscard]] IMusicAppreciationButtonNavigationState&
        NavigationStateAt4A2E60() noexcept override;
    [[nodiscard]] std::shared_ptr<IMusicAppreciationModeSoundDialog>
        CreateModeSoundDialogAt4E3900(
            const MusicAppreciationButtonStaticData& staticData) override;
    [[nodiscard]] std::shared_ptr<IMusicAppreciationGuiChildren>
        AcquireGuiChildrenAt47B7B0() override;
    [[nodiscard]] RECT ReadGraphicsTargetRectAt4180C0() const override;
    [[nodiscard]] std::int32_t PollBaseSelectionAt4AA200() override;
    void ClearSoundMappingAt437680() override;
    void StopCurrentSoundAt4383B0() override;
    void BindAndPlayAt4BCCC0(std::string_view resourceName) override;
    void EmitExitSoundAt4BCD70() override;
    [[nodiscard]] std::string_view ReadExtensionAt45DA40(
        std::string_view resourceName) const override;
    void SubmitInvalidatedRect(const RECT& rect) override;

private:
    IMusicAppreciationButtonServices* upstream_ = nullptr;
    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    IMusicAppreciationModeSoundDialogInheritedServices* dialogServices_ = nullptr;
};

// 保留 CModeSound 的场景宿主，只把 0x4A1C70 的 CSoundBtn 创建接到生产实现。
class MusicAppreciationProductionSceneServices final
    : public IMusicAppreciationSceneServices
{
public:
    MusicAppreciationProductionSceneServices(
        IMusicAppreciationSceneServices& upstream,
        IMusicAppreciationButtonServices& buttonServices,
        const MusicAppreciationButtonStaticData& staticData) noexcept;

    [[nodiscard]] MusicAppreciationTransitionSourceRef LoadTransitionSource(
        const char* resourceNameCp932) override;
    void PrepareSceneControllerAt4703D0(std::int32_t mode) override;
    [[nodiscard]] MusicAppreciationButtonRef
        CreateMusicAppreciationButton() override;
    [[nodiscard]] MusicAppreciationTransitionSourceRef
        CapturePrimarySceneResourceAt4ACDD0() override;
    void StopMusicEntryAt4AA890(std::int32_t index) override;
    void ResetMusicEntriesAt4AA780() override;
    void ClearCurrentSceneName() override;
    void WriteRegistrySlot2DwordAt04(std::uint32_t value) override;
    void WriteRegistrySlot2ByteAt09(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt24(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt25(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt26(std::uint8_t value) override;
    void ClearRegistrySlot2StringAt2C() override;
    void InvokeSoundProcedureAt4383B0() override;
    void WriteRootByteAt218(std::uint8_t value) override;
    void WriteRootByteAt219(std::uint8_t value) override;
    [[nodiscard]] bool ShouldAccelerateTransitionAt4AA970() override;
    void RenderTransition(MusicAppreciationTransitionOperation& operation) override;
    void CommitTransitionFrameAt4B02E0(std::int32_t argument) override;
    [[nodiscard]] std::string_view ReadCurrentSceneName() const override;
    [[nodiscard]] std::string_view ReadReturnSourceSceneName() const override;
    [[nodiscard]] std::string_view ReadReturnFallbackSceneName() const override;
    [[nodiscard]] bool ReadTitleReturnGateAt4B0740() const override;
    void PushActiveSceneAt4A9030(const char* sceneNameCp932) override;
    void QueuePendingSceneAt47F920(const char* sceneNameCp932) override;
    void SynchronizeButtonAt4ABB40(
        const MusicAppreciationButtonRef& button) override;
    void ReadGraphicsTargetSize(
        std::int32_t& width,
        std::int32_t& height) const override;

private:
    IMusicAppreciationSceneServices* upstream_ = nullptr;
    IMusicAppreciationButtonServices* buttonServices_ = nullptr;
    const MusicAppreciationButtonStaticData* staticData_ = nullptr;
};

// CModeSound 的生产强所有权边界。成员声明顺序保证服务先于场景构造，
// 场景先于两层服务析构。
class MusicAppreciationProductionObjectGraphRuntime final
{
public:
    MusicAppreciationProductionObjectGraphRuntime(
        IMusicAppreciationSceneServices& sceneUpstream,
        IMusicAppreciationButtonServices& buttonUpstream,
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        IMusicAppreciationModeSoundDialogInheritedServices& dialogServices,
        const MusicAppreciationButtonStaticData& staticData);

    [[nodiscard]] const std::shared_ptr<MusicAppreciationSceneRuntime>&
        Scene() const noexcept;

private:
    MusicAppreciationProductionButtonServices buttonServices_;
    MusicAppreciationProductionSceneServices sceneServices_;
    std::shared_ptr<MusicAppreciationSceneRuntime> scene_;
};

// SceneFactory 的音乐鉴赏分支适配层。别名 shared_ptr 将场景接口和完整
// CModeSound 对象图绑定到同一个控制块，保持原 owner 的析构边界。
class MusicAppreciationSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    MusicAppreciationSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        MusicAppreciationProductionDependencies dependencies) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateMusicAppreciationMode() override;

private:
    MusicAppreciationProductionDependencies dependencies_;
};
}
