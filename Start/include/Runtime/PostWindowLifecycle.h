#pragma once

namespace KaGuYa::Start::Runtime
{
class IPostWindowRuntimeServices
{
public:
    virtual ~IPostWindowRuntimeServices() = default;

    virtual void ConfirmFirstInstance() = 0;
    virtual void CreateFileManager() = 0;
    virtual void VerifyStartupFileSet() = 0;
    virtual void InstallEnvironmentKeyTable() = 0;
    virtual void LoadSecretEnvironment() = 0;
    virtual void ScanSaveFileCatalog() = 0;
    virtual void LoadGlobalEnvironment() = 0;

    virtual void InitializeGraphicsLibrary() = 0;
    virtual void CreateDrawManager() = 0;
    virtual void CreateInformationDisplay() = 0;
    virtual void CreateLayerWindow() = 0;
    virtual void CreateMessageWindow() = 0;
    virtual void CreateMessageWindowButtons() = 0;
    virtual void CreateTextWindow() = 0;
    virtual void CreateWaitMark() = 0;
    virtual void CreateSelectionWindow() = 0;
    virtual void CreateOpacityWindow() = 0;
    virtual void CreateCountdownWindow() = 0;

    virtual void InitializeSoundLibrary() = 0;
    virtual void CreateSoundManager() = 0;
    virtual void CreateInputAggregate() = 0;
    virtual void CreateSceneManager() = 0;
    virtual void CreateFontManager() = 0;
    virtual void ConfigureFrameTimer() = 0;
    virtual void ResetRuntimeState() = 0;
    virtual void QueueTitleLogoScene() = 0;
    virtual void PostInitialWindowModeCommand() = 0;

    // 48AE4A / 59AB87：root 结束时解除本轮 progress 引用；未建立时为空操作。
    // 460730 的 EHFlags=5 含 noexcept 位，释放异常不得转成初始化 false。
    virtual void ReleaseInitializationProgress() noexcept = 0;

    virtual void ReleaseSceneManager() noexcept = 0;
    virtual void ReleaseInputAggregate() noexcept = 0;
    virtual void ReleaseSoundManager() noexcept = 0;
    virtual void ShutdownSoundLibrary() = 0;
    virtual void ReleaseDrawManager() = 0;
    virtual void ReleaseDirectMovie() = 0;
    virtual void ShutdownGraphicsLibrary() = 0;
    virtual void PersistEnvironmentState() = 0;
    virtual void ReleaseFileManager() = 0;
};

struct PostWindowLifecycleState final
{
    bool fileManager = false;
    bool graphicsLibrary = false;
    bool drawManager = false;
    bool soundLibrary = false;
    bool soundManager = false;
    bool inputAggregate = false;
    bool sceneManager = false;
    bool initialized = false;
};

class PostWindowLifecycle final
{
public:
    // 对应 489C90 的逐调用时序；异常保留，由 ApplicationStagesRuntime 的 root 边界
    // 释放本轮 progress 并 catch-all 返回 false，不在这里回滚已发布的子系统。
    void Initialize(IPostWindowRuntimeServices& services);
    // 原489E56之后：progress已取得caller清理资格，manager已完成安装。
    void InitializeAfterFileManager(IPostWindowRuntimeServices& services);

    // 对应 Start.exe:477620。服务必须按真实 owning 槽位执行 null-safe 释放。
    // 434691已退出初始化catch的state3..4；关闭/持久化异常外传，保留已完成前缀。
    void Release(IPostWindowRuntimeServices& services);

    [[nodiscard]] const PostWindowLifecycleState& State() const noexcept
    {
        return state_;
    }

private:
    PostWindowLifecycleState state_{};
};
}
