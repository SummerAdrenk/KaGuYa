#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <Runtime/LegacyDisplaySettingsPersistence.h>
#include <Runtime/LegacyDisplaySettingsWin32.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Logging/Logger.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void FailedOwnership(LegacyDisplayOwnershipResult result)
{
    if (result == LegacyDisplayOwnershipResult::MissingSavedMode)
        throw std::runtime_error("Legacy display saved mode is not in its device catalogue");
    if (result == LegacyDisplayOwnershipResult::ApplyFailed)
        throw std::runtime_error("Legacy display exclusive mode application failed");
    if (result == LegacyDisplayOwnershipResult::RestoreFailed)
        throw std::runtime_error("Legacy display desktop restore failed; ownership retained");
}
}
struct RootLegacyDisplaySettingsRuntime::State final : ILegacyDisplaySettingsSaveSource, ILegacyDisplaySettingsHost
{
    explicit State(RootLegacyDisplaySettingsRuntime& value)
        : outer(value), store(*this), devices(value.display_,store), ui(value.resources_,window)
    {
        root = outer.root_.Published();
        environment = outer.environment_.RootSource().Published();
        if (!root || !environment) throw std::logic_error("Legacy display initialization requires published Root/Env");
        save = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*environment,1));
        mode = ResolveWindowModeReference(root->WindowModeSlotAt0C());
        if (!save || !mode) throw std::logic_error("Legacy display initialization requires real SaveData/WindowMode");
    }
    bool BindingsAlive() const noexcept
    {
        // 先核发布身份，再读正文。这里绝不Ensure/Capture一个退役后的新Root/Env。
        return outer.root_.Published() == root && outer.environment_.RootSource().Published() == environment &&
            ResolveWindowModeReference(root->WindowModeSlotAt0C()) == mode &&
            ResolveEnvironmentArchiveAt417990(*environment,1) == save;
    }
    void RequireBindings() const
    {
        if (quiesced || !BindingsAlive())
            throw std::logic_error("Legacy display Root/Env/SaveData/WindowMode binding expired");
    }
    SaveDataEnvironmentArchive& CurrentSaveData() override
    {
        RequireBindings();
        auto& current = outer.environment_.Archives().SaveData();
        if (&current != save) throw std::logic_error("Legacy display SaveData identity changed");
        return current;
    }
    LegacyDisplaySettingsContext CaptureDisplayContext() override
    { RequireBindings(); return devices.CaptureDisplayContext(); }
    void RefreshGameViewport(std::uint32_t scaling) override
    {
        RequireBindings();
        devices.RefreshViewport(root->WindowModeAt0C(),outer.graphics_,outer.platform_,window,scaling);
    }
    void SetMinimizeGate(bool enabled) override
    { RequireBindings(); root->Frame().WriteMinimizeByteAt1B2(enabled ? 1 : 0); }
    void PostDisplayCommand(WORD command) override
    {
        RequireBindings();
        static_cast<void>(outer.windows_.PostMessageA(window,WM_COMMAND,command,0));
    }
    RootLegacyDisplaySettingsRuntime& outer;
    RootRuntimePayload* root = nullptr;
    EnvironmentRootStorage* environment = nullptr;
    SaveDataEnvironmentArchive* save = nullptr;
    WindowModeNative2Ch* mode = nullptr;
    HWND window = nullptr; // UI借用稳定槽；不借临时HWND或已退役frame栈。
    LegacyDisplaySettingsSaveData store;
    LegacyDisplayDeviceOwner devices;
    Win32LegacyDisplaySettingsUi ui;
    std::unique_ptr<LegacyDisplaySettingsRuntime> receiver;
    bool windowReady = false, quiescing = false, quiesced = false;
};
struct RootLegacyDisplaySettingsRuntime::Operation final
{
    explicit Operation(RootLegacyDisplaySettingsRuntime& value) noexcept : outer(value) { ++outer.dispatchDepth_; }
    ~Operation() noexcept { outer.LeaveDispatch(); }
    RootLegacyDisplaySettingsRuntime& outer;
};

RootLegacyDisplaySettingsRuntime::RootLegacyDisplaySettingsRuntime(LegacyDisplaySettingsFamily family,
    RootRuntimeOwner& root, EnvironmentStartupRuntime& environment, IGraphicsWindowModeApi& graphics,
    IGraphicsPresentNativeApi& present, ILegacyDisplayDeviceApi& display, IWindowModePlatformApi& platform,
    IMainFrameNativeWindowApi& windows, const HINSTANCE& resources) noexcept
    : family_(family),root_(root),environment_(environment),graphics_(graphics),present_(present),display_(display),
      platform_(platform),windows_(windows),resources_(resources) {}
RootLegacyDisplaySettingsRuntime::~RootLegacyDisplaySettingsRuntime()
{
    // 正式Process服务使用显式长期存储。不得在这里借Root/Env补Cancel。
    // 错误调用者销毁尚有独占责任的服务是契约错误，不能静默丢失owner。
    if (HasOutstandingRestore() || (state_ && state_->receiver && state_->receiver->Active()))
    {
        ::OutputDebugStringA("Legacy display runtime destroyed with outstanding desktop restore ownership\n");
        std::terminate();
    }
}
void RootLegacyDisplaySettingsRuntime::RememberFailure(const char* stage, std::exception_ptr error) noexcept
{
    if (!failure_) failure_ = error;
    Logging::WriteFormat(Logging::Level::Error,"Legacy display failure at %s; first exception retained.",stage);
    // Release文件日志可关闭；原生诊断仍保留，随后在C++ Poll/FinishRun重抛。
    ::OutputDebugStringA("Legacy display failure: ");
    ::OutputDebugStringA(stage);
    ::OutputDebugStringA("; exception/restore ownership retained\n");
}
RootLegacyDisplaySettingsRuntime::State& RootLegacyDisplaySettingsRuntime::RequireState()
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled || !state_)
        throw std::logic_error("Enabled legacy display session was not initialized");
    state_->RequireBindings();
    if (!state_->devices.Initialized()) throw std::logic_error("Legacy display directory initialization failed");
    return *state_;
}
void RootLegacyDisplaySettingsRuntime::AfterPreWindow()
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled) return;
    try
    {
        if (state_) throw std::logic_error("Legacy display run cannot reuse a previous session");
        state_ = std::make_unique<State>(*this);
        auto& s = *state_;
        // pre原序：登记/读取env -> 安装WindowMode。此时正好可按真实记录初始化目录。
        s.devices.Initialize(s.environment->widthAt18,s.environment->heightAt1C);
        s.receiver = std::make_unique<LegacyDisplaySettingsRuntime>(family_,s,s.store,display_,s.ui,present_);
    }
    catch (...) { RememberFailure("after-pre-window",std::current_exception()); throw; }
}
void RootLegacyDisplaySettingsRuntime::AfterPostWindow()
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled) return;
    try
    {
        auto& s = RequireState();
        s.window = reinterpret_cast<HWND>(s.environment->windowAt2C);
        if (!s.window) throw std::logic_error("Legacy display post-window has no MainFrame HWND");
        s.windowReady = true;
        // 全屏仍消费原PostInitial的800B；首次窗口态也经过同一个owner，不能漏初始化。
        if (s.CurrentSaveData().bytesAt04[1] != 0)
            FailedOwnership(s.devices.SetWindowMode(s.root->WindowModeAt0C(),graphics_,platform_,s.window,false,
                std::bit_cast<LONG>(s.environment->styleAt20),std::bit_cast<LONG>(s.environment->exStyleAt24)));
    }
    catch (...) { RememberFailure("after-post-window",std::current_exception()); throw; }
}
bool RootLegacyDisplaySettingsRuntime::Dispatch(UINT message,WPARAM w,LPARAM l,LRESULT& result) noexcept
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled || message != WM_COMMAND ||
        LOWORD(w) < 0x801D || LOWORD(w) > 0x801F) return false;
    result = 0;
    if (state_ && (state_->quiescing || state_->quiesced)) return true;
    Operation operation(*this);
    try
    {
        auto& s = RequireState();
        if (!s.windowReady) throw std::logic_error("Legacy display command arrived before post-window completion");
        static_cast<void>(s.receiver->Dispatch(0,message,w,l,result));
    }
    catch (...) { RememberFailure("WM_COMMAND 801D/801E/801F",std::current_exception()); }
    return true;
}
void RootLegacyDisplaySettingsRuntime::LeaveDispatch() noexcept
{
    --dispatchDepth_;
    if (dispatchDepth_) return;
    if (forcedRelease_) { forcedRelease_ = false; Quiesce(); }
    if (const auto window = deferredClose_)
    {
        deferredClose_ = nullptr;
        try
        {
            if (!windows_.PostMessageA(window,WM_CLOSE,0,0))
                throw std::runtime_error("Cannot repost deferred legacy MainFrame close");
        }
        catch (...) { RememberFailure("deferred-close",std::current_exception()); }
    }
}
bool RootLegacyDisplaySettingsRuntime::PreTranslate(MSG& message)
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled) return false;
    if (failure_) std::rethrow_exception(failure_);
    if (!state_ || state_->quiesced || state_->quiescing) return false;
    Operation operation(*this);
    return state_->ui.PreTranslate(message);
}
bool RootLegacyDisplaySettingsRuntime::Poll()
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled) return false;
    if (failure_) std::rethrow_exception(failure_);
    if (!state_) throw std::logic_error("Legacy idle has no initialized session");
    if (state_->quiesced || state_->quiescing) return true;
    Operation operation(*this);
    state_->RequireBindings();
    return state_->receiver->Poll();
}
BYTE RootLegacyDisplaySettingsRuntime::SetWindowMode(WindowModeView view,BYTE requested,LONG style,LONG ex) noexcept
{
    if (state_ && (state_->quiesced || state_->quiescing)) return 0;
    Operation operation(*this);
    try
    {
        auto& s = RequireState();
        if (&view.NativeBody() != s.mode) throw std::logic_error("Legacy mode entry used a different WindowMode body");
        if (!s.windowReady) throw std::logic_error("Legacy mode entry precedes post-window completion");
        if (requested > 1) throw std::invalid_argument("Legacy mode entry requires window/fullscreen byte");
        FailedOwnership(s.devices.SetWindowMode(view,graphics_,platform_,s.window,requested != 0,style,ex));
        return 1;
    }
    catch (...) { RememberFailure("central-window-mode",std::current_exception()); return 0; }
}
void RootLegacyDisplaySettingsRuntime::Activate(HWND window,bool active,IMainFrameNativeWindowApi& windows,
    IMainFrameNativeModules& modules) noexcept
{
    // 创建窗口期间尚未发布Env HWND，此时没有exclusive申请；不能重建或捕获新根。
    if (!state_ || !state_->windowReady || state_->quiesced || state_->quiescing) return;
    Operation operation(*this);
    try
    {
        auto& s = RequireState();
        // 保留原短路次序：窗口态不查iconic/gate；激活分支不读失焦gate。
        if (!s.mode->modeAt04) return;
        if (!active && !s.root->Frame().ReadMinimizeByteAt1B2()) return;
        const auto action = DecideLegacyDisplayActivation(true,active,windows.IsIconic(window) != FALSE,true);
        if (action == LegacyDisplayActivationAction::None) return;
        if (action == LegacyDisplayActivationAction::MinimizeThenRelease)
        {
            static_cast<void>(windows.CloseWindow(window));
            const auto result = s.devices.SetExclusive(false);
            modules.ResetGraphicsAt432B40();
            FailedOwnership(result);
        }
        else
        {
            FailedOwnership(s.devices.SetExclusive(true));
            const auto current = s.devices.ReadSelectedCurrentMode();
            WINDOWPLACEMENT placement{}; placement.length = sizeof(placement);
            static_cast<void>(windows.GetWindowPlacement(window,&placement));
            placement.showCmd = SW_SHOWNORMAL;
            placement.rcNormalPosition = {current.dmPosition.x,current.dmPosition.y,
                current.dmPosition.x + static_cast<LONG>(current.dmPelsWidth),
                current.dmPosition.y + static_cast<LONG>(current.dmPelsHeight)};
            s.mode->savedMonitorRectAt1C = placement.rcNormalPosition;
            static_cast<void>(windows.SetWindowPlacement(window,&placement));
            modules.ResetGraphicsAt432B40();
        }
    }
    catch (...) { RememberFailure("WM_ACTIVATEAPP",std::current_exception()); }
}
void RootLegacyDisplaySettingsRuntime::Quiesce() noexcept
{
    if (!state_ || state_->quiesced || state_->quiescing) return;
    if (dispatchDepth_) { forcedRelease_ = true; return; }
    auto& s = *state_;
    s.quiescing = true;
    try
    {
        if (s.receiver && s.receiver->Active())
        {
            s.RequireBindings();
            s.receiver->RequestClose(false);
            static_cast<void>(s.receiver->Poll());
            if (s.receiver->LastCloseResult() == LegacyDisplayCloseResult::RestoreFailed)
                throw std::runtime_error("Legacy settings cancellation could not restore saved game mode");
        }
    }
    catch (...) { RememberFailure("cancel-before-release",std::current_exception()); }
    // Cancel失败也必须终止receiver。其析构只Close UI，不再访问Store/Host。
    s.receiver.reset();
    if (s.BindingsAlive()) s.root->Frame().WriteMinimizeByteAt1B2(1);
    s.quiesced = true;
    try { FailedOwnership(s.devices.Shutdown()); }
    catch (...) { RememberFailure("desktop-restore-before-release",std::current_exception()); }
    s.quiescing = false;
}
bool RootLegacyDisplaySettingsRuntime::BeforeClose(HWND window) noexcept
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled) return true;
    if (state_ && state_->quiescing) return false;
    if (dispatchDepth_) { deferredClose_ = window; return false; }
    Quiesce();
    return true;
}
void RootLegacyDisplaySettingsRuntime::BeforeWindowRelease() noexcept
{ if (family_ != LegacyDisplaySettingsFamily::Disabled) Quiesce(); }
void RootLegacyDisplaySettingsRuntime::BeforeSubsystemRelease() noexcept
{ if (family_ != LegacyDisplaySettingsFamily::Disabled) Quiesce(); }
void RootLegacyDisplaySettingsRuntime::FinishRun()
{
    // 此时原frame栈/WindowMode可能已退役；这里只核收据，绝不补Cancel/Close/Root读取。
    if (state_ && !state_->quiesced)
    {
        try { throw std::logic_error("Legacy display missed its pre-release boundary; late cancellation forbidden"); }
        catch (...) { RememberFailure("finish-run-boundary",std::current_exception()); }
    }
    if (failure_) std::rethrow_exception(failure_);
    if (HasOutstandingRestore()) throw std::runtime_error("Legacy display still owns an unrestored desktop");
}
void RootLegacyDisplaySettingsRuntime::ObserveRunUnwind() noexcept
{
    if (state_ && !state_->quiesced)
    {
        try { throw std::logic_error("Legacy display escaped without a live-dependency shutdown boundary"); }
        catch (...) { RememberFailure("run-unwind-boundary",std::current_exception()); }
    }
}
bool RootLegacyDisplaySettingsRuntime::HasOutstandingRestore() const noexcept
{ return state_ && state_->devices.ActiveIndex().has_value(); }
LegacyDisplayOwnershipResult RootLegacyDisplaySettingsRuntime::RetryDesktopRestore()
{
    if (!state_) return LegacyDisplayOwnershipResult::NoChange;
    if (!state_->quiesced) throw std::logic_error("Quiesce legacy settings before retrying desktop restore");
    // 此调用即使发生在Env退出后也不借Root/SaveData；失败时设备owner保持原责任。
    return state_->devices.Shutdown();
}
}
