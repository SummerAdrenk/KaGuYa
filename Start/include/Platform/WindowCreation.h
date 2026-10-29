#pragma once

#include <Windows.h>

#include <array>
#include <string>

namespace KaGuYa::Start::Runtime { struct SystemDataEnvironmentOwnerInitialization; struct EnvironmentRootStorage; class EnvironmentStartupRuntime; }

namespace KaGuYa::Start::Platform
{
// 413990：原生窗口/对话框共用的监视器工作区及父窗口居中规则。
[[nodiscard]] BOOL CenterWindowAt413990(HWND window, HWND center);

struct ParamsGameProfile
{
    int logicalWidth = 0;
    int logicalHeight = 0;
    std::string gameTitle;
    std::string appendTitle;
};

struct StartExecutableProfile
{
    DWORD configuredStyle = 0;
    DWORD configuredExStyle = 0;
    DWORD fallbackStyle = 0;
    DWORD fallbackExStyle = 0;
    unsigned int targetFramesPerSecond = 0;
    unsigned int iconResourceId = 0;
};

struct WindowCreationPlan
{
    std::string title;
    DWORD style = 0;
    DWORD exStyle = 0;
    int x = 0;
    int y = 0;
    int outerWidth = 0;
    int outerHeight = 0;
    RECT adjustedClientRectangle{};
};

class IWindowGeometryApi
{
public:
    virtual ~IWindowGeometryApi() = default;
    virtual void AdjustFrame(RECT& rectangle, DWORD style, DWORD exStyle) const = 0;
    [[nodiscard]] virtual int PrimaryScreenWidth() const = 0;
    [[nodiscard]] virtual int PrimaryScreenHeight() const = 0;
};

class Win32WindowGeometryApi final : public IWindowGeometryApi
{
public:
    void AdjustFrame(RECT& rectangle, DWORD style, DWORD exStyle) const override;
    [[nodiscard]] int PrimaryScreenWidth() const override;
    [[nodiscard]] int PrimaryScreenHeight() const override;
};

[[nodiscard]] WindowCreationPlan BuildWindowCreationPlan(
    const ParamsGameProfile& gameProfile,
    const StartExecutableProfile& executableProfile,
    const IWindowGeometryApi& geometryApi);

// 434440..434553 从同一 Environment 根读取；不重新解析 params 或安装默认配置。
[[nodiscard]] WindowCreationPlan BuildWindowCreationPlan(
    const Runtime::SystemDataEnvironmentOwnerInitialization& environment,
    const IWindowGeometryApi& geometryApi);
[[nodiscard]] WindowCreationPlan BuildWindowCreationPlan(
    const Runtime::EnvironmentRootStorage& environment,
    const IWindowGeometryApi& geometryApi);
// 434452..434523：正式创建逐次捕获根；上面的固定对象重载仅用于显式输入。
[[nodiscard]] WindowCreationPlan BuildWindowCreationPlan(
    const Runtime::EnvironmentStartupRuntime& environment,
    const IWindowGeometryApi& geometryApi);

struct WindowClassProfile
{
    UINT style = 0;
    WNDPROC windowProcedure = nullptr;
    int classExtraBytes = 0;
    int windowExtraBytes = 0;
    HBRUSH backgroundBrush = nullptr;
    LPCSTR menuResource = nullptr;
    LPCSTR cursorResource = nullptr;
    bool cursorIsSystemResource = false;
    unsigned int iconResourceId = 0;
    std::string explicitClassName;
    std::string superclassName;
};

enum class WindowClassRegistrationKind
{
    NativeControl,
    MainFrame
};

class WindowClassRegistrar final
{
public:
    explicit WindowClassRegistrar(WindowClassProfile profile);
    ~WindowClassRegistrar();

    WindowClassRegistrar(const WindowClassRegistrar&) = delete;
    WindowClassRegistrar& operator=(const WindowClassRegistrar&) = delete;

    [[nodiscard]] ATOM Register(
        HINSTANCE moduleInstance,
        HINSTANCE resourceInstance,
        WNDPROC* superclassProcedure,
        WindowClassRegistrationKind kind = WindowClassRegistrationKind::NativeControl);
    [[nodiscard]] const char* ClassName() const noexcept;

private:
    WindowClassProfile profile_;
    CRITICAL_SECTION registrationLock_{};
    ATOM atom_ = 0;
    WNDPROC superclassProcedure_ = nullptr;
    std::array<char, 13> generatedClassName_{};
};
}
