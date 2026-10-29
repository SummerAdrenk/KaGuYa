#include <Platform/WindowCreation.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RuntimeCStringResult.h>

#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <utility>

namespace KaGuYa::Start::Platform
{
void Win32WindowGeometryApi::AdjustFrame(
    RECT& rectangle,
    const DWORD style,
    const DWORD exStyle) const
{
    AdjustWindowRectEx(&rectangle, style, FALSE, exStyle);
}

int Win32WindowGeometryApi::PrimaryScreenWidth() const
{
    return GetSystemMetrics(SM_CXSCREEN);
}

int Win32WindowGeometryApi::PrimaryScreenHeight() const
{
    return GetSystemMetrics(SM_CYSCREEN);
}

namespace
{
int SignedBits(const std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
int Add(const int a, const int b) noexcept
{ return SignedBits(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
int Subtract(const int a, const int b) noexcept
{ return SignedBits(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }

std::string_view WindowTitleBytes(const std::string& value) noexcept { return value; }
std::string_view WindowTitleBytes(const Runtime::RuntimeCString& value) noexcept
{ return Runtime::RuntimeStringView(value); }

template<class Dimension, class Style, class Text>
WindowCreationPlan BuildPlan(const Dimension& width, const Dimension& height,
    const Text& title, const Text& appendTitle,
    const Style& style, const Style& exStyle, DWORD zeroStyle, DWORD zeroExStyle,
    const IWindowGeometryApi& geometryApi)
{
    // 434448/43444E 各查询一次，且早于读取尺寸、AdjustWindowRectEx 和标题拼接。
    const int screenWidth = geometryApi.PrimaryScreenWidth();
    const int screenHeight = geometryApi.PrimaryScreenHeight();
    const int clientHeight = SignedBits(static_cast<std::uint32_t>(height));
    const int clientWidth = SignedBits(static_cast<std::uint32_t>(width));
    RECT rectangle{0, 0, clientWidth, clientHeight};
    geometryApi.AdjustFrame(rectangle, style, exStyle);

    WindowCreationPlan plan;
    plan.adjustedClientRectangle = rectangle;
    plan.outerWidth = Subtract(rectangle.right, rectangle.left);
    plan.outerHeight = Subtract(rectangle.bottom, rectangle.top);
    // 434493..4344C3 是 32 位环绕算术，除二向零截断，再偏移整个 RECT。
    const int dx = Add(Subtract(rectangle.left, rectangle.right), screenWidth) / 2;
    const int dy = screenHeight > plan.outerHeight
        ? Add(Subtract(rectangle.top, rectangle.bottom), screenHeight) / 2
        : Subtract(0, rectangle.top);
    plan.x = Add(rectangle.left, dx);
    plan.y = Add(rectangle.top, dy);

    // 4344C9..43451B：几何 API 返回后才读取根标题；保留 CString 的显式字节长度。
    plan.title = WindowTitleBytes(title);
    if (!WindowTitleBytes(appendTitle).empty())
    {
        plan.title.append("\x81\x40", 2);
        plan.title.append(WindowTitleBytes(appendTitle));
    }
    // 原版重新读取 style；零值替代仅影响创建参数，不回写根或调整后的 RECT。
    plan.exStyle = exStyle != 0 ? exStyle : zeroExStyle;
    plan.style = style != 0 ? style : zeroStyle;
    return plan;
}
}

BOOL CenterWindowAt413990(HWND window, HWND center)
{
    const LONG style = GetWindowLongA(window, GWL_STYLE);
    if (center == nullptr)
        center = (style & WS_CHILD) != 0 ? GetParent(window) : GetWindow(window, GW_OWNER);

    RECT rectangle{};
    GetWindowRect(window, &rectangle);
    RECT bounds{};
    RECT target{};
    if ((style & WS_CHILD) == 0)
    {
        if (center != nullptr &&
            (GetWindowLongA(center, GWL_STYLE) & (WS_VISIBLE | WS_MINIMIZE)) != WS_VISIBLE)
            center = nullptr;
        const HMONITOR monitor = MonitorFromWindow(
            center != nullptr ? center : window, MONITOR_DEFAULTTONEAREST);
        if (monitor == nullptr) return FALSE;
        MONITORINFO info{};
        info.cbSize = sizeof(info);
        if (!GetMonitorInfoA(monitor, &info)) return FALSE;
        bounds = info.rcWork;
        if (center == nullptr) target = bounds;
        else GetWindowRect(center, &target);
    }
    else
    {
        const HWND parent = GetParent(window);
        GetClientRect(parent, &bounds);
        GetClientRect(center, &target);
        MapWindowPoints(center, parent, reinterpret_cast<POINT*>(&target), 2);
    }

    const LONG width = Subtract(rectangle.right, rectangle.left);
    const LONG height = Subtract(rectangle.bottom, rectangle.top);
    LONG x = Subtract(Add(target.right, target.left) / 2, width / 2);
    LONG y = Subtract(Add(target.bottom, target.top) / 2, height / 2);
    if (Add(x, width) > bounds.right) x = Subtract(bounds.right, width);
    if (x < bounds.left) x = bounds.left;
    if (Add(y, height) > bounds.bottom) y = Subtract(bounds.bottom, height);
    if (y < bounds.top) y = bounds.top;
    return SetWindowPos(window, nullptr, x, y, -1, -1,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

WindowCreationPlan BuildWindowCreationPlan(const ParamsGameProfile& gameProfile,
    const StartExecutableProfile& executableProfile, const IWindowGeometryApi& geometryApi)
{
    return BuildPlan(gameProfile.logicalWidth, gameProfile.logicalHeight,
        gameProfile.gameTitle, gameProfile.appendTitle, executableProfile.configuredStyle,
        executableProfile.configuredExStyle, executableProfile.fallbackStyle,
        executableProfile.fallbackExStyle, geometryApi);
}

WindowCreationPlan BuildWindowCreationPlan(
    const Runtime::SystemDataEnvironmentOwnerInitialization& environment,
    const IWindowGeometryApi& geometryApi)
{
    return BuildPlan(environment.widthAt18, environment.heightAt1C,
        environment.stringAt10, environment.stringAt14, environment.valueAt20,
        environment.valueAt24, 0x06CF0000u, 0x00040100u, geometryApi);
}

WindowCreationPlan BuildWindowCreationPlan(const Runtime::EnvironmentRootStorage& environment,
    const IWindowGeometryApi& geometryApi)
{
    return BuildPlan(environment.widthAt18, environment.heightAt1C,
        environment.titleAt10, environment.appendTitleAt14, environment.styleAt20,
        environment.exStyleAt24, 0x06CF0000u, 0x00040100u, geometryApi);
}

WindowCreationPlan BuildWindowCreationPlan(const Runtime::EnvironmentStartupRuntime& environment,
    const IWindowGeometryApi& geometryApi)
{
    const int screenWidth = geometryApi.PrimaryScreenWidth();
    const int screenHeight = geometryApi.PrimaryScreenHeight();
    const int height = SignedBits(environment.CaptureAt4180C0().heightAt1C);
    const int width = SignedBits(environment.CaptureAt4180C0().widthAt18);
    RECT rectangle{0, 0, width, height};
    const DWORD adjustExStyle = environment.CaptureAt4180C0().exStyleAt24;
    const DWORD adjustStyle = environment.CaptureAt4180C0().styleAt20;
    geometryApi.AdjustFrame(rectangle, adjustStyle, adjustExStyle);
    WindowCreationPlan plan;
    plan.adjustedClientRectangle = rectangle;
    plan.outerWidth = Subtract(rectangle.right, rectangle.left);
    plan.outerHeight = Subtract(rectangle.bottom, rectangle.top);
    const int dx = Add(Subtract(rectangle.left, rectangle.right), screenWidth) / 2;
    const int dy = screenHeight > plan.outerHeight
        ? Add(Subtract(rectangle.top, rectangle.bottom), screenHeight) / 2
        : Subtract(0, rectangle.top);
    plan.x = Add(rectangle.left, dx);
    plan.y = Add(rectangle.top, dy);

    // 4344C9..4344E3先Clone真实CString；append判空与实际追加分别重取根。
    Runtime::RuntimeCStringResult title([&](Runtime::RuntimeCString* out) {
        const auto* chars = environment.CaptureAt4180C0().titleAt10.GetString();
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1;
        const auto* copied = reinterpret_cast<const char*>(Runtime::CloneRuntimeCStringDataAt414540(header) + 1);
        std::memcpy(out, &copied, sizeof(copied));
    });
    if (environment.CaptureAt4180C0().appendTitleAt14.GetLength() != 0)
    {
        Runtime::AppendRuntimeCStringAt413550(title.Value(), "\x81\x40", 2);
        const auto& append = environment.CaptureAt4180C0().appendTitleAt14;
        Runtime::AppendRuntimeCStringAt413550(title.Value(), append.GetString(), append.GetLength());
    }
    const DWORD exStyle = environment.CaptureAt4180C0().exStyleAt24;
    const DWORD style = environment.CaptureAt4180C0().styleAt20;
    plan.exStyle = exStyle != 0 ? exStyle : 0x00040100u;
    plan.style = style != 0 ? style : 0x06CF0000u;
    // 返回给现有WindowHost的值计划；完整主窗对象/ATL创建EH仍由外层审计核销。
    plan.title = Runtime::RuntimeStringView(title.Value());
    return plan;
}

WindowClassRegistrar::WindowClassRegistrar(WindowClassProfile profile)
    : profile_(std::move(profile))
{
    InitializeCriticalSection(&registrationLock_);
}

WindowClassRegistrar::~WindowClassRegistrar()
{
    DeleteCriticalSection(&registrationLock_);
}

ATOM WindowClassRegistrar::Register(
    const HINSTANCE moduleInstance,
    const HINSTANCE resourceInstance,
    WNDPROC* const superclassProcedure,
    const WindowClassRegistrationKind kind)
{
    if (atom_ == 0)
    {
        EnterCriticalSection(&registrationLock_);
        if (atom_ == 0)
        {
            WNDCLASSEXA windowClass{};
            windowClass.cbSize = sizeof(windowClass);
            windowClass.style = profile_.style;
            windowClass.lpfnWndProc = profile_.windowProcedure;
            windowClass.cbClsExtra = profile_.classExtraBytes;
            windowClass.cbWndExtra = profile_.windowExtraBytes;
            windowClass.hbrBackground = profile_.backgroundBrush;
            windowClass.lpszMenuName = profile_.menuResource;

            const char* className = profile_.explicitClassName.empty()
                ? nullptr
                : profile_.explicitClassName.c_str();

            if (!profile_.superclassName.empty())
            {
                const WNDPROC replacementProcedure = windowClass.lpfnWndProc;
                const char* const replacementName = className;
                WNDCLASSEXA superclass{};
                superclass.cbSize = sizeof(superclass);
                // 431757/43176A 主窗先模块后系统；451E01..451E21 控件的顺序相反。
                const HINSTANCE first = kind == WindowClassRegistrationKind::MainFrame ? moduleInstance : nullptr;
                const HINSTANCE second = kind == WindowClassRegistrationKind::MainFrame ? nullptr : moduleInstance;
                if (GetClassInfoExA(first, profile_.superclassName.c_str(), &superclass) == FALSE &&
                    GetClassInfoExA(second, profile_.superclassName.c_str(), &superclass) == FALSE)
                {
                    LeaveCriticalSection(&registrationLock_);
                    return 0;
                }

                windowClass = superclass;
                superclassProcedure_ = superclass.lpfnWndProc;
                windowClass.lpfnWndProc = replacementProcedure;
                className = replacementName;
            }
            else
            {
                const HINSTANCE cursorInstance = profile_.cursorIsSystemResource
                    ? nullptr
                    : (kind == WindowClassRegistrationKind::MainFrame ? moduleInstance : resourceInstance);
                windowClass.hCursor = LoadCursorA(cursorInstance, profile_.cursorResource);
            }

            windowClass.style &= ~CS_GLOBALCLASS;
            windowClass.hInstance = moduleInstance;
            if (className == nullptr)
            {
                static_assert(sizeof(void*) == 4, "Start_RS.exe 仅支持原版 x86 窗口类名格式");
                std::snprintf(
                    generatedClassName_.data(),
                    generatedClassName_.size(),
                    "ATL:%p",
                    static_cast<void*>(this));
                className = generatedClassName_.data();
            }
            windowClass.lpszClassName = className;

            WNDCLASSEXA existingClass{};
            existingClass.cbSize = sizeof(existingClass);
            // 451ECE..451ED8直接保存AX，不能把已有类的返回值规范化为1。
            atom_ = static_cast<ATOM>(
                GetClassInfoExA(moduleInstance, className, &existingClass));
            if (atom_ == 0)
            {
                if (profile_.iconResourceId != 0)
                {
                    const int iconHeight = GetSystemMetrics(SM_CYICON);
                    const int iconWidth = GetSystemMetrics(SM_CXICON);
                    windowClass.hIcon = static_cast<HICON>(LoadImageA(
                        resourceInstance,
                        MAKEINTRESOURCEA(profile_.iconResourceId),
                        IMAGE_ICON,
                        iconWidth,
                        iconHeight,
                        LR_DEFAULTCOLOR));
                    const int smallIconHeight = GetSystemMetrics(SM_CYSMICON);
                    const int smallIconWidth = GetSystemMetrics(SM_CXSMICON);
                    windowClass.hIconSm = static_cast<HICON>(LoadImageA(
                        resourceInstance,
                        MAKEINTRESOURCEA(profile_.iconResourceId),
                        IMAGE_ICON,
                        smallIconWidth,
                        smallIconHeight,
                        LR_DEFAULTCOLOR));
                }
                atom_ = RegisterClassExA(&windowClass);
            }
        }
        LeaveCriticalSection(&registrationLock_);
    }

    if (!profile_.superclassName.empty() && superclassProcedure != nullptr)
    {
        *superclassProcedure = superclassProcedure_;
    }
    return atom_;
}

const char* WindowClassRegistrar::ClassName() const noexcept
{
    if (!profile_.explicitClassName.empty())
    {
        return profile_.explicitClassName.c_str();
    }
    return generatedClassName_.data();
}
}
