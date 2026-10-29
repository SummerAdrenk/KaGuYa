#pragma once
#include <Platform/InputViews.h>

#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/LoadAfterSelectionScene.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/DrawTransitionView.h>
#include <Runtime/SceneManagerView.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class DrawManagerRuntime;
class IGraphicsRuntimeCalls;
class ReadScriptEffectSurfaceRuntime;
class SaveDataEnvironmentArchive;
class SceneManagerRuntime;
class ScrParamsResetRuntime;
class ScrVmValueTable;
class ITextWindowSoundSource;
class TextWindowFormatRuntime;
class TextWindowTextServicesRuntime;
class UnSaveDataRuntime;
class RootRuntimeOwner;
class IGraphicsTransitApi;

// 恢复每个调用点独立取根/选槽；层与转场分别捕获，不缓存跨回调的manager。
class ILoadAfterDrawSource:public IDrawLayersViewSource
{
public:
    virtual DrawTransitionView CaptureTransitionAfter413BE0()=0;
};
class RootLoadAfterDrawSource final:public ILoadAfterDrawSource
{
public:
    RootLoadAfterDrawSource(RootRuntimeOwner& root,IGraphicsTransitApi& graphics) noexcept:root_(root),graphics_(graphics){}
    DrawLayersView CaptureDrawLayersAt413BE0() override;
    DrawTransitionView CaptureTransitionAfter413BE0() override;
private:
    RootRuntimeOwner& root_;IGraphicsTransitApi& graphics_;
};
// 显式固定图适配，仅旧构造入口使用；不在原生路径失败后换入。
class FixedLoadAfterDrawSource final:public ILoadAfterDrawSource
{
public:
    explicit FixedLoadAfterDrawSource(DrawManagerRuntime* manager) noexcept:manager_(manager){}
    DrawLayersView CaptureDrawLayersAt413BE0() override;
    DrawTransitionView CaptureTransitionAfter413BE0() override;
private:
    DrawManagerRuntime* manager_;
};
struct LoadAfterSelectionCoreDependencies final
{
    IGraphicsRuntimeCalls& graphics;
    ReadScriptEffectSurfaceRuntime& resources;
    SaveDataEnvironmentArchive& save;
    UnSaveDataRuntime& unsave;
    ITextWindowSoundSource& sound;
    SceneManagerView scenes;
    ScrParamsResetRuntime& paramsReset;
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    std::uint8_t& rootByteAt218;
    std::uint8_t& rootByteAt219;
    std::uint32_t& rootServiceDwordAt10;
};
struct LoadAfterDrawBindings final
{
    ILoadAfterDrawSource& source;
    DrawTextNativeRebuildServices* nativeText;
    DrawTextControlledRebuildServices* controlledText;
    PatternLayerNativeRenderServices* patterns;
    TextWindowLayerNativeServices* resetText;
};

// 全部借用同一根的现存对象；三个 scalar 是根/根+3C 的字段，不在各场景复制。
struct LoadAfterSelectionDependencies final
{
    DrawManagerRuntime& drawing;
    IGraphicsRuntimeCalls& graphics;
    ReadScriptEffectSurfaceRuntime& resources;
    SaveDataEnvironmentArchive& save;
    UnSaveDataRuntime& unsave;
    ITextWindowSoundSource& sound;
    SceneManagerView scenes;
    TextWindowFormatRuntime& textFormat;
    TextWindowTextServicesRuntime& textLayout;
    ScrVmValueTable& values;
    ScrParamsResetRuntime& paramsReset;
    Platform::MouseView mouse;
    Platform::KeyboardView keyboard;
    std::uint8_t& rootByteAt218;
    std::uint8_t& rootByteAt219;
    std::uint32_t& rootServiceDwordAt10;
};

// 4B6380 / 47E180 的实际宿主：每次从 owning 槽重新取窗口，调用同一 CTransit。
class LoadAfterSelectionServicesRuntime final : public ILoadAfterSelectionServices
{
public:
    // 原生装配不要求创建一个无用的现代Draw/Text对象，只借实际来源及外部业务服务。
    LoadAfterSelectionServicesRuntime(LoadAfterSelectionCoreDependencies,LoadAfterDrawBindings) noexcept;
    explicit LoadAfterSelectionServicesRuntime(LoadAfterSelectionDependencies dependencies) noexcept;
    LoadAfterSelectionServicesRuntime(const LoadAfterSelectionServicesRuntime&)=delete;
    LoadAfterSelectionServicesRuntime& operator=(const LoadAfterSelectionServicesRuntime&)=delete;
    LoadAfterTransitionSourceRef LoadTransitionSource(const char* resourceNameCp932) override;
    std::uint32_t ReadRegistrySlot2DwordAt04() const override;
    void WriteRegistrySlot2DwordAt04(std::uint32_t value) override;
    void WriteRootByteAt218(std::uint8_t value) override;
    void WriteRootByteAt219(std::uint8_t value) override;
    std::uint32_t ReadTransitionSpeedIndexAt70() const override;
    bool ReadTransitionAccelerationGateAt6F() const override;
    bool PollTransitionAccelerationAtSlot5() override;
    void RenderTransition(LoadAfterTransitionOperation& operation) override;
    void CommitTransitionFrame(std::int32_t argument) override;
    void WriteRootServiceDwordAt10(std::uint32_t value) override;
    void StopSoundManagerEntryNamedAt10() override;
    void ClearSoundManagerEntries() override;
    void StopRootSoundEntryNamedByRegistrySlot2At30() override;
    void WriteSceneMarkerAt38(std::int32_t value) override;
    void SynchronizeRestoredGroup0BeforeGroup3() override;
    void RestoreEnvironmentGroup3(std::int32_t argument) override;
    void SynchronizeRestoredGroup0AfterGroup3() override;
    void SynchronizeRestoredRootOwners() override;
    LoadAfterTransitionSourceRef CaptureScreenBackup() override;
    void ApplyPostRestoreScreenState(std::int32_t argument) override;
    void ClearCurrentSceneName() override;
    void ClearScenes() noexcept override;
    void PushActiveScene(const char* sceneNameCp932) override;
    void QueueScene(const char* sceneNameCp932) override;
    void ResetScriptForDefaultRoute() override;
    void ResetOwnerGroup3ForDefaultRoute() override;

private:
    LoadAfterSelectionCoreDependencies dependencies_;
    GameGuiInputStateRuntime input_;
    FixedLoadAfterDrawSource fixedDrawing_;
    ILoadAfterDrawSource* drawing_;
    std::optional<DrawTextControlledRebuildServices> controlledText_;
    DrawTextNativeRebuildServices* nativeText_=nullptr;
    DrawTextControlledRebuildServices* text_=nullptr;
    PatternLayerNativeRenderServices* patterns_=nullptr;
    TextWindowLayerNativeServices* resetText_=nullptr;
};
}
