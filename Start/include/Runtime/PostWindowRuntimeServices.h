#pragma once

#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/PostWindowLifecycle.h>
#include <Runtime/GuiModalDialogState.h>
#include <variant>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
class RootRuntimeOwner;
class IDrawManagerNativeRootSource;
struct PostWindowNativeLayerServices;
struct LayerWindowNativeServices;
struct DrawLayersNativeWindowServices;

// 调用方显式选择原生/既有受控构造域；无“依赖缺失就转旧实现”的分支。
class SceneManagerConstructionAt48ABF6 final
{
public:
    SceneManagerConstructionAt48ABF6(SceneManagerNativeServices& native) noexcept:source_(&native){}
    SceneManagerConstructionAt48ABF6(ISceneConstructionServices& scenes,ISceneManagerPublicDialogFactory& dialogs) noexcept
        :source_(Controlled{&scenes,&dialogs}){}
    void Create(EnvironmentPostWindowStartupRuntime& post) const
    {
        std::visit([&](const auto& source)
        {
            if constexpr(std::is_same_v<std::decay_t<decltype(source)>,Controlled>)
                post.CreateSceneManager(*source.scenes,*source.dialogs);
            else post.CreateSceneManager(*source);
        },source_);
    }
private:
    struct Controlled{ISceneConstructionServices* scenes;ISceneManagerPublicDialogFactory* dialogs;};
    std::variant<Controlled,SceneManagerNativeServices*> source_;
};
class InformationDisplayConstructionAt48A64A final
{
public:
    InformationDisplayConstructionAt48A64A(SystemInformationDisplayNativeServices& services) noexcept:source_(&services){}
    InformationDisplayConstructionAt48A64A(const GuiModalDialogDependencies& modal,ISystemMenuDialogServices& menu) noexcept
        :source_(Controlled{&modal,&menu}){}
    void Create(EnvironmentPostWindowStartupRuntime& post) const
    {
        std::visit([&](const auto& source)
        {
            if constexpr(std::is_same_v<std::decay_t<decltype(source)>,Controlled>)
                post.CreateControlledInformationDisplay(*source.modal,*source.menu);
            else post.CreateInformationDisplay(*source);
        },source_);
    }
private:
    struct Controlled{const GuiModalDialogDependencies* modal;ISystemMenuDialogServices* menu;};
    std::variant<Controlled,SystemInformationDisplayNativeServices*> source_;
};

// 全部依赖显式给出，不允许未接通的工厂靠空指针或空操作进入489C90。
// payload及下列对象必须长于服务和它们创建的图层/场景；payload换代后须重新装配。
// 只借用同一根，不复制Environment、Input或保存任何owning槽的镜像。
struct PostWindowRuntimeDependencies final
{
    RootRuntimePayload& root;
    NativeGraphicsRuntimeLibrary& graphics;
    ICrtRandomSource& drawRandom;
    InformationDisplayConstructionAt48A64A information;
    // root对应489C90固定EDI；每层构造前/后另经drawSource捕获当前+18。
    IDrawManagerNativeRootSource& drawSource;
    PostWindowNativeLayerServices& layers;
    LayerWindowNativeServices& layerRuntime;
    DrawLayersNativeWindowServices& windowEffects;
    DrawGuiNativeServices& gui;
    SceneManagerConstructionAt48ABF6 scenes;
    IWindowModeCommandPlatformApi& windowCommands;
    RootRuntimeOwner& owner; // 48ADFD当场Ensure/取当前Root，不能沿用上面的固定receiver。
    const HINSTANCE& resourceInstance; // 原61A6A0；模态建窗前才读取，不存一份值。
};

// 489C90/477620的生产转发，时序和异常边界仍由PostWindowLifecycle/ApplicationStages掌管。
// 构造只绑定依赖，不提前创建图层、输入、模式或场景，也不隐式加载DLL。
class PostWindowRuntimeServices final : public IPostWindowRuntimeServices
{
public:
    explicit PostWindowRuntimeServices(PostWindowRuntimeDependencies dependencies) noexcept;
    void ConfirmFirstInstance() override;
    void CreateFileManager() override;
    void VerifyStartupFileSet() override;
    void InstallEnvironmentKeyTable() override;
    void LoadSecretEnvironment() override;
    void ScanSaveFileCatalog() override;
    void LoadGlobalEnvironment() override;
    void InitializeGraphicsLibrary() override;
    void CreateDrawManager() override;
    void CreateInformationDisplay() override;
    void CreateLayerWindow() override;
    void CreateMessageWindow() override;
    void CreateMessageWindowButtons() override;
    void CreateTextWindow() override;
    void CreateWaitMark() override;
    void CreateSelectionWindow() override;
    void CreateOpacityWindow() override;
    void CreateCountdownWindow() override;
    void InitializeSoundLibrary() override;
    void CreateSoundManager() override;
    void CreateInputAggregate() override;
    void CreateSceneManager() override;
    void CreateFontManager() override;
    void ConfigureFrameTimer() override;
    void ResetRuntimeState() override;
    void QueueTitleLogoScene() override;
    void PostInitialWindowModeCommand() override;
    void ReleaseInitializationProgress() noexcept override;
    void ReleaseSceneManager() noexcept override;
    void ReleaseInputAggregate() noexcept override;
    void ReleaseSoundManager() noexcept override;
    void ShutdownSoundLibrary() override;
    void ReleaseDrawManager() override;
    void ReleaseDirectMovie() override;
    void ShutdownGraphicsLibrary() override;
    void PersistEnvironmentState() override;
    void ReleaseFileManager() override;

private:
    EnvironmentPostWindowStartupRuntime& Post() const noexcept;
    PostWindowRuntimeDependencies dependencies_;
};
}
