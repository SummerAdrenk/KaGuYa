#include <Runtime/PostWindowLifecycle.h>

namespace KaGuYa::Start::Runtime
{
void PostWindowLifecycle::Initialize(IPostWindowRuntimeServices& services)
{
    // 489D0F..489D70：首次实例确认位于文件系统对象创建之前。
    services.ConfirmFirstInstance();

    // 489D70..48A345：文件管理、启动文件验证、Environment key 与三类持久状态。
    services.CreateFileManager();
    InitializeAfterFileManager(services);
}

void PostWindowLifecycle::InitializeAfterFileManager(IPostWindowRuntimeServices& services)
{
    state_.fileManager = true;
    services.VerifyStartupFileSet();
    services.InstallEnvironmentKeyTable();
    services.LoadSecretEnvironment();
    services.ScanSaveFileCatalog();
    services.LoadGlobalEnvironment();

    // 48A345..48A641：Graphics、DrawManager 与独立的信息显示对象。
    services.InitializeGraphicsLibrary();
    state_.graphicsLibrary = true;
    services.CreateDrawManager();
    state_.drawManager = true;
    services.CreateInformationDisplay();

    // 48A64A..48AADD：八个 owning 图层按注册顺序进入同一个 CLayerList。
    services.CreateLayerWindow();
    services.CreateMessageWindow();
    services.CreateMessageWindowButtons();
    services.CreateTextWindow();
    services.CreateWaitMark();
    services.CreateSelectionWindow();
    services.CreateOpacityWindow();
    services.CreateCountdownWindow();

    // 48AAE2..48AC98：Sound DLL、SoundManager、输入、SceneManager 与 FontManager。
    services.InitializeSoundLibrary();
    state_.soundLibrary = true;
    services.CreateSoundManager();
    state_.soundManager = true;
    services.CreateInputAggregate();
    state_.inputAggregate = true;
    services.CreateSceneManager();
    state_.sceneManager = true;
    services.CreateFontManager();

    // 48AD8A..48AE4A：样本18固定30 FPS，复位字段后只排入初始场景。
    services.ConfigureFrameTimer();
    services.ResetRuntimeState();
    // 48ADFD 只把“タイトルロゴ”交给程序队列；スタート.scr 在 4BADA0 状态循环中延迟装载。
    services.QueueTitleLogoScene();
    services.PostInitialWindowModeCommand();
    state_.initialized = true;
}

void PostWindowLifecycle::Release(IPostWindowRuntimeServices& services)
{
    // 477620 直接检查真实 owning 槽位，不读取“初始化成功”布尔值。
    // 因此即使构造函数在写入根槽后抛出，也必须进入同一条 null-safe 释放链。
    services.ReleaseSceneManager();
    state_.sceneManager = false;
    services.ReleaseInputAggregate();
    state_.inputAggregate = false;
    services.ReleaseSoundManager();
    state_.soundManager = false;
    // 47772A 无条件进入 Sound 包装；包装内部按模块句柄决定是否实际调用 ExitSystem。
    services.ShutdownSoundLibrary();
    state_.soundLibrary = false;
    services.ReleaseDrawManager();
    state_.drawManager = false;
    // 477769 释放 this+0x48/+0x50 的 YTL::ref<Scene::CDirectMovie>。
    // 这是正常播放结束路径之外的退出兜底，必须先于 Graphics 关闭。
    services.ReleaseDirectMovie();
    // 47779C 同样无条件进入 Graphics 包装，由包装检查模块句柄。
    services.ShutdownGraphicsLibrary();
    state_.graphicsLibrary = false;

    // 4777A8..4777C9：Secret/Global 在服务内部按 gate 写回，env.ini 无条件写回。
    services.PersistEnvironmentState();

    services.ReleaseFileManager();
    state_.fileManager = false;
    state_.initialized = false;
}
}
