#pragma once
#include <Platform/InputViews.h>

#include <Runtime/TitleDialogRuntime.h>
#include <Runtime/TitleDialogChildObjectGraphRuntime.h>
#include <Runtime/TitleDialogVisualNotificationsRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/GraphicsTransitionSourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct TitleDialogProductionDependencies final
{
    ICgOperationSurfaceFactory& surfaces;
    IGraphicsSurfaceCompositionApi& graphics;
    IGraphicsSurfacePixelApi& pixels;
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    ITitleDialogExternalServices& external;
    ITitleDialogHoverSoundServices& sound;
    std::int32_t screenWidth;
    std::int32_t screenHeight;
};

// 样本 18 的 4F4530/4FE690：完整 TitleDialog owner，父 Dialog 固定为空。
// 根对象仅提供窗口、registry、声音、转场提交等真正的外部服务。
class TitleDialogProductionRuntime final : public ITitleDialogRuntime,
    private ITitleDialogServices, private ITitleDialogVisualServices,
    private ICgOperationButtonInputServices
{
public:
    explicit TitleDialogProductionRuntime(TitleDialogProductionDependencies dependencies);
    ~TitleDialogProductionRuntime();
    TitleDialogProductionRuntime(const TitleDialogProductionRuntime&) = delete;
    TitleDialogProductionRuntime& operator=(const TitleDialogProductionRuntime&) = delete;
    void InvokeVslot44() override;
    bool Run() override;
    std::uint32_t SelectionResult() const override;
    void RedrawAllAt530E60();
    const TitleDialogChildObjectGraphRuntime& Children() const noexcept { return *children_; }
    const TitleDialogInputRuntime& Input() const noexcept { return input_; }
    const TitleDialogRuntime& State() const noexcept { return *state_; }

private:
    void ReleaseAt4FE690() noexcept;
    void ClearClickedEventsAt532200() override;
    void WriteDialogEnabledAt538660(std::int32_t) override;
    void ResetInputAt510840() override;
    void ResetChildrenAt522E90() override;
    void UpdateGuiDialogAt518C30() override;
    std::int32_t PollClickedEventAt50FDB0(std::int32_t) override;
    ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() override;
    void DrawChildrenAt5230B0(const RECT&) override;
    void CompositeChildrenAt5231C0(const LoadAfterTransitionSourceRef&, const RECT*) override;
    void SetActiveControlAtVslot30(ICgOperationDialogEventTarget&) override;
    bool ReadInputAtVslot6C(std::int32_t) override;
    bool ReadInputAtVslot70(std::int32_t) override;
    POINT ReadPointerAtVslot74() override;
    bool ReadPressAtVslot80() override;
    bool ReadReleaseAtVslot84() override;
    std::uint32_t ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle, std::int32_t, std::int32_t) override;
    bool ReadPointerMovedAtVslot7C() override;
    void SubmitChildRectAt50FC80(const RECT&) override;
    const char* ReadHoverSoundNameAt61A65C() override;
    std::int32_t ReadEffectVolumeAtVslot24() override;
    void PlayEffectAt4BCD70(const char*, std::int32_t, std::int32_t, std::int32_t) override;
    void InvalidatedRect(const RECT&) override;
    bool ReadInputAt4C3430() override;
    bool ReadInputAt4AA970() override;
    void RenderTransitionAt449DD0(LoadAfterTransitionOperation&) override;
    void CommitTransitionFrameAt4B02E0(std::int32_t) override;
    void WriteRegistrySlot2DwordAt04(std::uint32_t) override;
    std::int32_t ReadRegistrySlot1BgmVolumeAtVslot14() override;
    const char* ReadTitleBgmNameAt61A654() override;
    void WriteRegistrySlot2BgmNameAt2C(const char*) override;
    void PlayBgmAt42BFE0(const char*, std::int32_t, std::int32_t) override;
    void ClearSoundManagerAt4384D0() override;
    void StopSoundNamedByRegistrySlot2At30() override;
    bool ReadInputAt47F6D0() override;
    bool ReadSceneManagerAt485CE0() override;
    void PlayConfirmSoundAt4BCD70() override;
    std::uint32_t ReadObjectAt50A620DwordAt1C() override;
    std::uint8_t ReadRegistrySlot1ByteAt54() override;
    std::shared_ptr<IModalDialogRuntimeTargetReference> CreateConfirmationAt4E4340() override;
    void PrepareModalAt4AD230() override;
    std::int32_t RunModalAt5071D0(IModalDialogRuntimeTargetReference&) override;
    void WriteCurrentSceneName(const char*) override;
    void PostCloseMessage() override;

    TitleDialogProductionDependencies dependencies_;
    TitleDialogInputRuntime input_;
    CgOperationDialogRenderHostRuntime render_;
    std::unique_ptr<IGuiInputStateRuntime> inputState_;
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirty_;
    std::unique_ptr<TitleDialogChildObjectGraphRuntime> children_;
    std::shared_ptr<GraphicsTransitionSourceRuntime> black_;
    std::shared_ptr<GraphicsTransitionSourceRuntime> canvas_;
    std::vector<GuiDialogNavigationGroupRef> navigation_;
    std::unique_ptr<TitleDialogVisualNotificationsRuntime> notifications_;
    std::unique_ptr<TitleDialogRuntime> state_;
};
}
