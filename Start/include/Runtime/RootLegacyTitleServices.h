#pragma once
#include <Runtime/LegacyTitleNamePrompt.h>
#include <Runtime/LegacyTitleScene.h>
#include <Runtime/RootTitleSceneNativeSource.h>
#include <Runtime/RootScenePublicNativeServicesRuntime.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>

namespace KaGuYa::Start::Runtime
{
class ISpriteNativeResourceSource;
class SaveDataEnvironmentArchive;
struct SoundManagerNativeBody50h;

struct RootLegacyTitleDependencies final
{
    RootScenePublicNativeDependencies common;
    ParamsRuntimeRootOwner& params;
    RootTitleSceneNativeSource& scripts;
    CgUiButtonNativeServices& buttons;
    IGuiControlResourceLoadServices& pictures;
    ISpriteNativeResourceSource& sprites;
    IArchiveResourceSource& archives;
    ILegacyTitleQuickLoad& quickLoad;
    const LegacyTitleSceneData& data;
    const HINSTANCE& resourceInstance;
    ScriptNamedValueNativeTables namedValues;
    IScrVmCommandErrorSink& errors;
    ILegacyTitleNameApi& nameApi = DefaultLegacyTitleNameApi();
    decltype(&::PostMessageA) postMessage = &::PostMessageA;
};

// 唯一真实root/environment/params的借用适配；不缓存根代际、保存值、声音或Sprite镜像。
// quickLoad为强制依赖，必须由旧异步确认/存档拥有者提供；本类不补同步或假成功实现。
class RootLegacyTitleServices final : public ILegacyTitleSceneHost,
    public ILegacyTitleAudio, public ILegacyTitleMenuHost, public ILegacyTitleNameServices,
    private ILegacyTitleNameRoot, private ICgUiButtonResourceErrorDisplay
{
public:
    explicit RootLegacyTitleServices(RootLegacyTitleDependencies) noexcept;
    LegacyTitleSceneServices& Scene() noexcept { return scene_; }

    SceneManagerView CaptureScenes() override;
    UnSaveDataRuntime& CaptureCommon() override;
    bool Pressed(std::int32_t) override;
    bool Held(std::int32_t) override;
    bool MousePressedEdge(std::int32_t) override;
    std::int32_t ReplayValue() override;
    void CaptureFrame(YtlReferenceSlotRuntime&) override;
    DrawTransitionView CaptureDraw() override;
    std::uint8_t LoadStartupScript(const char*, std::int32_t) override;
    void WriteStartupFlag(std::uint8_t) override;
    void WriteStartupReadyFlag(std::uint8_t) override;
    void WriteLoadEntryFlag(std::uint8_t) override;
    void WriteSpecialMenuFlag(std::uint8_t) override;
    std::int32_t PostWindowMessage(std::uint32_t, std::uint32_t, std::int32_t) override;
    const char* BlackSurfaceName() override;
    bool LoadBlackSurface(YtlReferenceSlotRuntime&, const char*) override;
    std::int32_t ScreenHeight() override;
    std::int32_t ScreenWidth() override;
    void ClearReplay() override;

    std::int32_t ReadBgmVolume() override;
    const char* TitleBgmName() override;
    std::uint8_t PlayBgm(const char*, std::int32_t, std::int32_t) override;
    std::uint8_t ClearSound() override;
    void StopCommonSounds() override;
    const char* BackgroundName() override;
    const char* OverlayName() override;
    bool LoadBackground(YtlReferenceSlotRuntime&, const char*) override;
    bool LoadOverlay(YtlReferenceSlotRuntime&, const char*, int) override;
    std::int32_t UpdateOverlay(const YtlReferenceSlotRuntime&) override;
    RECT OverlayBounds(const YtlReferenceSlotRuntime&) override;
    void BlitOverlay(const YtlReferenceSlotRuntime&, GraphicsSurfaceNativeBody&, const RECT*) override;
    void DrawOverlay(const YtlReferenceSlotRuntime&, const RECT*) override;
    std::int32_t ReadCueVolume() override;
    const char* CueName(LegacyTitleMenuCue) override;
    std::uint8_t PlayCue(const char*, int, int, int) override;

    bool EditableNameBit(unsigned) override;
    bool SkipNamePrompt() override;
    RuntimeCString& SavedGivenName() override;
    RuntimeCString& SavedFamilyName() override;
    const RuntimeCString& DefaultGivenName() override;
    const RuntimeCString& DefaultFamilyName() override;
    void WriteSkipNamePrompt(bool) override;
    ILegacyTitleNameRoot& CaptureRoot() override;
    void WriteInputGate(bool) override;
    HINSTANCE ResourceInstance() override;

private:
    SaveDataEnvironmentArchive& Save();
    SoundManagerNativeBody50h* Sound();
    HWND MainWindow() override;
    const RuntimeCString& Caption() override;
    void ShowAt480A2D(const RuntimeCString&) override;
    RootLegacyTitleDependencies dependencies_;
    // 只固定一次CaptureRoot的环境身份；用于同次caption→HWND，不镜像字段。
    EnvironmentRootStorage* capturedNameEnvironment_ = nullptr;
    LegacyTitleNamePrompt names_;
    LegacyTitleMenuServices menu_;
    LegacyTitleSceneServices scene_;
};
}
