#pragma once

#include <Platform/WindowCreation.h>
#include <Runtime/GraphicsWindowModeApi.h>
#include <Runtime/WindowModeNative.h>

#include <array>
#include <cstdint>
#include <optional>
#include <memory>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class EnvironmentRootOwner;
// 仅隔离真实系统调用；monitor 选择、模式分支和字段提交顺序在生产算法中实现。
class IWindowModePlatformApi
{
public:
    virtual ~IWindowModePlatformApi() = default;
    virtual BOOL EnumMonitors(MONITORENUMPROC callback, LPARAM data) = 0;
    virtual BOOL ReadMonitorInfo(HMONITOR monitor, MONITORINFOEXA& info) = 0;
    virtual HMONITOR FindWindowMonitor(HWND window, DWORD flags) = 0;
    virtual int CompareDeviceNames(const char* left, const char* right) = 0;
    virtual BOOL EqualRect(const RECT& left, const RECT& right) = 0;
    virtual BOOL IsEmptyRect(const RECT& rect) = 0;
    virtual LONG WriteWindowLong(HWND window, int index, LONG value) = 0;
    virtual BOOL AdjustFrameRect(RECT& rect, DWORD style, BOOL menu, DWORD exStyle) = 0;
    virtual BOOL PositionWindow(HWND window, HWND after, int x, int y,
        int width, int height, UINT flags) = 0;
    virtual BOOL InvalidateWindowRect(HWND window, const RECT* rect, BOOL erase) = 0;
    virtual BOOL ClearRect(RECT& rect) = 0;
};

class Win32WindowModePlatformApi final : public IWindowModePlatformApi
{
public:
    BOOL EnumMonitors(MONITORENUMPROC callback, LPARAM data) override;
    BOOL ReadMonitorInfo(HMONITOR monitor, MONITORINFOEXA& info) override;
    HMONITOR FindWindowMonitor(HWND window, DWORD flags) override;
    int CompareDeviceNames(const char* left, const char* right) override;
    BOOL EqualRect(const RECT& left, const RECT& right) override;
    BOOL IsEmptyRect(const RECT& rect) override;
    LONG WriteWindowLong(HWND window, int index, LONG value) override;
    BOOL AdjustFrameRect(RECT& rect, DWORD style, BOOL menu, DWORD exStyle) override;
    BOOL PositionWindow(HWND window, HWND after, int x, int y,
        int width, int height, UINT flags) override;
    BOOL InvalidateWindowRect(HWND window, const RECT* rect, BOOL erase) override;
    BOOL ClearRect(RECT& rect) override;
};

// 417480 先清输出；417FD0 按原 _mbscmp/flags==1 规则选择 rcMonitor。
RECT& QueryMonitorRectAt417480(RECT& output, const char* deviceName,
    IWindowModePlatformApi& platform);

// 仅借用服务，不保存正文或拥有引用。按原取值点读取Environment。
struct WindowModeRuntimeServices final
{
    IGraphicsWindowModeApi* graphics = nullptr;
    IWindowModePlatformApi* platform = nullptr;
    const HWND* window = nullptr;
    const Platform::ParamsGameProfile* profile = nullptr;
    const EnvironmentStartupRuntime* environment = nullptr;
    EnvironmentRootOwner* nativeEnvironment = nullptr;
};

// 按值复制仅固定本次receiver，不增持Ref；内层重入不改写外层receiver。
class WindowModeView
{
public:
    explicit WindowModeView(WindowModeNative2Ch& body,
        WindowModeRuntimeServices services = {}, bool* published = nullptr) noexcept
        : services_(services), body_(&body), viewportPublished_(published) {}

    void UpdateLayoutAt437D80(UINT additionalFlags);
    void SynchronizeSavedNormalPositionAt42116C();
    [[nodiscard]] std::uint8_t SetModeAt438AD0(std::uint8_t request, LONG style, LONG exStyle);
    [[nodiscard]] std::uint8_t ModeAt04() const noexcept { return body_->modeAt04; }
    [[nodiscard]] std::string_view DeviceNameAt08() const noexcept { return RuntimeStringView(body_->deviceNameAt08); }
    // 原生receiver直接读取原正文；仅显式现代fixture保留未发布检查，不补零原viewport。
    [[nodiscard]] std::optional<WindowModeViewport> ViewportAt0C() const noexcept
        { return !viewportPublished_ || *viewportPublished_ ? std::optional(body_->viewportAt0C) : std::nullopt; }
    [[nodiscard]] const WindowModeViewport& RequireViewportAt0C() const
    {
        if (viewportPublished_ && !*viewportPublished_) throw std::bad_optional_access{};
        return body_->viewportAt0C;
    }
    [[nodiscard]] const RECT& SavedMonitorRectAt1C() const noexcept { return body_->savedMonitorRectAt1C; }
    [[nodiscard]] WindowModeNative2Ch& NativeBody() const noexcept { return *body_; }

private:
    [[nodiscard]] HWND ReadWindowAtEnvironment2C() const;
    [[nodiscard]] std::int32_t ReadWidthAtEnvironment18() const;
    [[nodiscard]] std::int32_t ReadHeightAtEnvironment1C() const;
    [[nodiscard]] IGraphicsWindowModeApi& Graphics() const;
    [[nodiscard]] IWindowModePlatformApi& PlatformApi() const;
    WindowModeRuntimeServices services_;
    WindowModeNative2Ch* body_;
    bool* viewportPublished_;
};

// 显式现代独立owner；生产根只持原Ref，不在根中存本facade。
class WindowModeManagerRuntime final
{
public:
    // 固定载体入口：逐次读取所借用的HWND/profile，但不随Environment换代自动切换来源。
    // 所有借用与graphics/platform均须长于本对象。
    WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics, const HWND& window,
        const Platform::ParamsGameProfile& profile, IWindowModePlatformApi& platform);
    // 477934..477982构造不读环境；后续操作按原4180C0取值点读取实际Environment字段。
    WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics,
        const EnvironmentStartupRuntime& environment, IWindowModePlatformApi& platform);
    WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics,
        EnvironmentRootOwner& environment, IWindowModePlatformApi& platform);
    // 4646C0/4696C0：仅释放内存；不进行模式恢复、Resize 或 RenderReset。
    ~WindowModeManagerRuntime() noexcept = default;
    WindowModeManagerRuntime(const WindowModeManagerRuntime&) = delete;
    WindowModeManagerRuntime& operator=(const WindowModeManagerRuntime&) = delete;

    [[nodiscard]] WindowModeView Borrow() const noexcept { return WindowModeView(*body_, services_, &viewportPublished_); }
    void UpdateLayoutAt437D80(UINT additionalFlags) { Borrow().UpdateLayoutAt437D80(additionalFlags); }
    // 42116C..4211B6：caller 已检查模式/激活/iconic；此处不处理 placement 或外层 Reset。
    void SynchronizeSavedNormalPositionAt42116C() { Borrow().SynchronizeSavedNormalPositionAt42116C(); }
    // 438AD0直接消费调用方已取出的style/exStyle；环境重入不改写本次实参。
    [[nodiscard]] std::uint8_t SetModeAt438AD0(
        std::uint8_t request, LONG style, LONG exStyle) { return Borrow().SetModeAt438AD0(request, style, exStyle); }

    [[nodiscard]] std::uint8_t ModeAt04() const noexcept { return body_->modeAt04; }
    [[nodiscard]] std::string_view DeviceNameAt08() const noexcept { return RuntimeStringView(body_->deviceNameAt08); }
    // 477934..477982 未写 viewport；未发布时不能冒充零矩形。
    [[nodiscard]] std::optional<WindowModeViewport> ViewportAt0C() const noexcept
        { return viewportPublished_ ? std::optional(body_->viewportAt0C) : std::nullopt; }
    [[nodiscard]] const WindowModeViewport& RequireViewportAt0C() const
    {
        if (!viewportPublished_) throw std::bad_optional_access{};
        return body_->viewportAt0C;
    }
    [[nodiscard]] const RECT& SavedMonitorRectAt1C() const noexcept { return body_->savedMonitorRectAt1C; }
    [[nodiscard]] WindowModeNative2Ch& NativeBody() noexcept { return *body_; }
    [[nodiscard]] const WindowModeNative2Ch& NativeBody() const noexcept { return *body_; }

private:
    WindowModeRuntimeServices services_;
    struct NativeDelete final
    {
        void operator()(WindowModeNative2Ch* body) const noexcept
            { if (body) body->DeleteAtVslot00(1); }
    };
    std::unique_ptr<WindowModeNative2Ch, NativeDelete> body_;
    // 仅现代安全观测，不能把它与viewport一起塞进原正文或写成默认矩形。
    mutable bool viewportPublished_ = false;
};
}
