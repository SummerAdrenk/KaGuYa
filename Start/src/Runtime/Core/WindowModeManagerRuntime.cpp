#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootLegacyDisplaySettingsRuntime.h>

#include <bit>
#include <mbstring.h>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// Start.exe:5AE158，42 字节 CP932；保留原文重复措辞以及 438CF0 的 char* 类型。
char monitorFailureAt5AE158[] =
    "\x83\x66\x83\x42\x83\x58\x83\x76\x83\x8C\x83\x43\x90\xDD\x92\xE8"
    "\x82\xCC\x8E\xE6\x93\xBE\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC"
    "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
static_assert(sizeof(monitorFailureAt5AE158) == 43);
static_assert(sizeof(int) == 4 && sizeof(LONG) == 4);

std::int32_t AddDword(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) + static_cast<std::uint32_t>(right));
}

std::int32_t SubDword(std::int32_t left, std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) - static_cast<std::uint32_t>(right));
}

struct MonitorQuery
{
    MonitorQuery() noexcept {}
    ~MonitorQuery() noexcept {}
    std::uint32_t found;
    union { RuntimeCString deviceName; };
    RECT rect;
};
static_assert(sizeof(MonitorQuery) == 0x18);
static_assert(offsetof(MonitorQuery, deviceName) == 4);
static_assert(offsetof(MonitorQuery, rect) == 8);

thread_local IWindowModePlatformApi* monitorPlatform;
struct MonitorPlatformScope final
{
    explicit MonitorPlatformScope(IWindowModePlatformApi& value) noexcept : previous(monitorPlatform)
        { monitorPlatform = &value; }
    ~MonitorPlatformScope() noexcept { monitorPlatform = previous; }
    IWindowModePlatformApi* previous;
};

BOOL CALLBACK SelectMonitorAt417FD0(HMONITOR monitor, HDC, LPRECT, LPARAM data)
{
    auto& query = *reinterpret_cast<MonitorQuery*>(data);
    if (query.found != 0)
        return TRUE;

    if (!monitorPlatform) throw std::logic_error("monitor enumeration callback lacks its platform scope");
    auto& platform = *monitorPlatform;
    MONITORINFOEXA info;
    info.cbSize = sizeof(info);
    if (platform.ReadMonitorInfo(monitor, info) == FALSE)
        return TRUE;

    if (platform.CompareDeviceNames(query.deviceName.GetString(), "") == 0)
    {
        if (info.dwFlags != 1)
            return TRUE;
    }
    else if (platform.CompareDeviceNames(query.deviceName.GetString(), info.szDevice) != 0)
    {
        return TRUE;
    }
    query.rect = info.rcMonitor;
    query.found = 1;
    return TRUE;
}
}

BOOL Win32WindowModePlatformApi::EnumMonitors(MONITORENUMPROC callback, LPARAM data)
{
    return ::EnumDisplayMonitors(nullptr, nullptr, callback, data);
}

BOOL Win32WindowModePlatformApi::ReadMonitorInfo(HMONITOR monitor, MONITORINFOEXA& info)
{
    return ::GetMonitorInfoA(monitor, &info);
}

HMONITOR Win32WindowModePlatformApi::FindWindowMonitor(HWND window, DWORD flags)
{
    return ::MonitorFromWindow(window, flags);
}

int Win32WindowModePlatformApi::CompareDeviceNames(const char* left, const char* right)
{
    return ::_mbscmp(reinterpret_cast<const unsigned char*>(left),
        reinterpret_cast<const unsigned char*>(right));
}

BOOL Win32WindowModePlatformApi::EqualRect(const RECT& left, const RECT& right)
{
    return ::EqualRect(&left, &right);
}

BOOL Win32WindowModePlatformApi::IsEmptyRect(const RECT& rect)
{
    return ::IsRectEmpty(&rect);
}

LONG Win32WindowModePlatformApi::WriteWindowLong(HWND window, int index, LONG value)
{
    return ::SetWindowLongA(window, index, value);
}

BOOL Win32WindowModePlatformApi::AdjustFrameRect(RECT& rect, DWORD style, BOOL menu, DWORD exStyle)
{
    return ::AdjustWindowRectEx(&rect, style, menu, exStyle);
}

BOOL Win32WindowModePlatformApi::PositionWindow(HWND window, HWND after,
    int x, int y, int width, int height, UINT flags)
{
    return ::SetWindowPos(window, after, x, y, width, height, flags);
}

BOOL Win32WindowModePlatformApi::InvalidateWindowRect(HWND window, const RECT* rect, BOOL erase)
{
    return ::InvalidateRect(window, rect, erase);
}

BOOL Win32WindowModePlatformApi::ClearRect(RECT& rect)
{
    return ::SetRectEmpty(&rect);
}

RECT& QueryMonitorRectAt417480(RECT& output, const char* deviceName,
    IWindowModePlatformApi& platform)
{
    output = {};
    MonitorQuery query;
    InitializeEmptyRuntimeCString(query.deviceName);
    struct StringUnwind final
    {
        RuntimeCString& value;
        bool armed = true;
        ~StringUnwind() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    } unwind{query.deviceName};
    query.rect = {};
    query.found = 0;
    const char* const name = deviceName ? deviceName : "";
    SetRuntimeCStringAt437680(query.deviceName, name, static_cast<int>(std::strlen(name)));
    MonitorPlatformScope scope(platform);
    const BOOL enumerated = platform.EnumMonitors(
        SelectMonitorAt417FD0, reinterpret_cast<LPARAM>(&query));
    if (enumerated != FALSE && query.found != 0)
        output = query.rect;
    // 417550先撤清理资格；正常Free异常外传，展开时不二次减持。
    unwind.armed = false;
    DestroyRuntimeCStringAt40CD40(&query.deviceName);
    return output;
}

WindowModeManagerRuntime::WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics,
    const HWND& window, const Platform::ParamsGameProfile& profile,
    IWindowModePlatformApi& platform)
    : services_{&graphics, &platform, &window, &profile}, body_(NewWindowModeAt47791D())
{
}

WindowModeManagerRuntime::WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics,
    const EnvironmentStartupRuntime& environment, IWindowModePlatformApi& platform)
    : services_{&graphics, &platform, nullptr, nullptr, &environment}, body_(NewWindowModeAt47791D())
{
}

WindowModeManagerRuntime::WindowModeManagerRuntime(IGraphicsWindowModeApi& graphics,
    EnvironmentRootOwner& environment, IWindowModePlatformApi& platform)
    : services_{&graphics, &platform, nullptr, nullptr, nullptr, &environment}, body_(NewWindowModeAt47791D())
{
}

IGraphicsWindowModeApi& WindowModeView::Graphics() const
{
    if (!services_.graphics) throw std::logic_error("window mode lacks Graphics services");
    return *services_.graphics;
}

IWindowModePlatformApi& WindowModeView::PlatformApi() const
{
    if (!services_.platform) throw std::logic_error("window mode lacks platform services");
    return *services_.platform;
}

HWND WindowModeView::ReadWindowAtEnvironment2C() const
{
    if (services_.nativeEnvironment)
        return reinterpret_cast<HWND>(services_.nativeEnvironment->CaptureAt4180C0()->windowAt2C);
    if (services_.environment) return reinterpret_cast<HWND>(services_.environment->CaptureAt4180C0().windowAt2C);
    if (!services_.window) throw std::logic_error("window mode lacks window source");
    return *services_.window;
}

std::int32_t WindowModeView::ReadWidthAtEnvironment18() const
{
    if (services_.nativeEnvironment)
        return std::bit_cast<std::int32_t>(services_.nativeEnvironment->CaptureAt4180C0()->widthAt18);
    if (services_.environment) return std::bit_cast<std::int32_t>(services_.environment->WindowFields().widthAt18);
    if (!services_.profile) throw std::logic_error("window mode lacks width source");
    return services_.profile->logicalWidth;
}

std::int32_t WindowModeView::ReadHeightAtEnvironment1C() const
{
    if (services_.nativeEnvironment)
        return std::bit_cast<std::int32_t>(services_.nativeEnvironment->CaptureAt4180C0()->heightAt1C);
    if (services_.environment) return std::bit_cast<std::int32_t>(services_.environment->WindowFields().heightAt1C);
    if (!services_.profile) throw std::logic_error("window mode lacks height source");
    return services_.profile->logicalHeight;
}

void WindowModeView::UpdateLayoutAt437D80(UINT additionalFlags)
{
    auto& platform_ = PlatformApi();
    auto& graphics_ = Graphics();
    RECT monitor{};
    QueryMonitorRectAt417480(monitor,
        body_->deviceNameAt08.GetLength() == 0 ? nullptr : body_->deviceNameAt08.GetString(), platform_);
    const int height = SubDword(monitor.bottom, monitor.top);
    const int width = SubDword(monitor.right, monitor.left);
    const int* const resized = graphics_.ResizeScreenIfLoaded(width, height, 1);
    if (resized != nullptr)
    {
        // 437DEB..437DFF：在 SetWindowPos 可能同步重入之前发布四个 DWORD。
        body_->viewportAt0C[0] = resized[0];
        body_->viewportAt0C[1] = resized[1];
        body_->viewportAt0C[2] = resized[2];
        body_->viewportAt0C[3] = resized[3];
    }
    else
    {
        // 437DD6/437DE1分别重取Environment+18/+1C，不取Params或缓存profile。
        body_->viewportAt0C[1] = 0;
        body_->viewportAt0C[0] = 0;
        body_->viewportAt0C[2] = ReadWidthAtEnvironment18();
        body_->viewportAt0C[3] = ReadHeightAtEnvironment1C();
    }
    if (viewportPublished_) *viewportPublished_ = true;
    // 437E02/437E1E晚于Resize及viewport提交，不能复用438B71的局部HWND。
    static_cast<void>(platform_.PositionWindow(ReadWindowAtEnvironment2C(), HWND_NOTOPMOST,
        monitor.left, monitor.top, width, height, additionalFlags | 0x40u));
    body_->savedMonitorRectAt1C = monitor;
    graphics_.ResetRendererIfLoaded();
}

void WindowModeView::SynchronizeSavedNormalPositionAt42116C()
{
    auto& platform_ = PlatformApi();
    RECT monitor{};
    QueryMonitorRectAt417480(monitor,
        body_->deviceNameAt08.GetLength() == 0 ? nullptr : body_->deviceNameAt08.GetString(), platform_);
    if (platform_.EqualRect(body_->savedMonitorRectAt1C, monitor) != FALSE)
        return;
    if (platform_.IsEmptyRect(monitor) != FALSE)
        SetRuntimeCStringAt437680(body_->deviceNameAt08, "", 0);
    // 4211AD 的清名已提交；4211B6 必须重新查询，不能复用首次 RECT 或回滚清名。
    UpdateLayoutAt437D80(0x20u);
}

std::uint8_t WindowModeView::SetModeAt438AD0(
    std::uint8_t request, LONG style, LONG exStyle)
{
    // 所有原ABI菜单/命令统一进入正文外owner；必须先于same-mode早退。
    if (auto* legacy=BoundRootLegacyDisplaySettings()) return legacy->SetWindowMode(*this,request,style,exStyle);
    if (body_->modeAt04 == request)
        return 1;

    auto& platform_ = PlatformApi();
    auto& graphics_ = Graphics();

    if (request != 0)
    {
        SetRuntimeCStringAt437680(body_->deviceNameAt08, "", 0);
        const HMONITOR monitor = platform_.FindWindowMonitor(ReadWindowAtEnvironment2C(), 0);
        if (monitor != nullptr)
        {
            MONITORINFOEXA info;
            info.cbSize = sizeof(info);
            if (platform_.ReadMonitorInfo(monitor, info) != FALSE)
                SetRuntimeCStringAt437680(body_->deviceNameAt08, info.szDevice, static_cast<int>(std::strlen(info.szDevice)));
        }
    }

    // 438B56：按当前旧 byte 翻转；非规范 request 不能提前归一化为 bool。
    body_->modeAt04 = body_->modeAt04 == 0 ? 1 : 0;
    // 438B09与438B61是两次环境读取；438B71以后两次style写及438C9F复用同一HWND。
    const HWND window = ReadWindowAtEnvironment2C();
    static_cast<void>(platform_.WriteWindowLong(window, GWL_STYLE, style));
    static_cast<void>(platform_.WriteWindowLong(window, GWL_EXSTYLE, exStyle));
    if (request != 0)
    {
        UpdateLayoutAt437D80(0x20u);
        return 1;
    }

    RECT monitor{};
    QueryMonitorRectAt417480(monitor,
        body_->deviceNameAt08.GetLength() == 0 ? nullptr : body_->deviceNameAt08.GetString(), platform_);
    if (platform_.IsEmptyRect(monitor) != FALSE)
    {
        QueryMonitorRectAt417480(monitor, nullptr, platform_);
        if (platform_.IsEmptyRect(monitor) != FALSE)
            throw static_cast<char*>(monitorFailureAt5AE158);
    }

    // 438BDF/438BE7先高后宽；Resize重入之后438C0C/438C17再次先宽后高。
    const int resizeHeight = ReadHeightAtEnvironment1C();
    const int resizeWidth = ReadWidthAtEnvironment18();
    static_cast<void>(graphics_.ResizeScreenIfLoaded(resizeWidth, resizeHeight, -1));
    RECT adjusted{};
    adjusted.right = ReadWidthAtEnvironment18();
    adjusted.bottom = ReadHeightAtEnvironment1C();
    static_cast<void>(platform_.AdjustFrameRect(adjusted,
        static_cast<DWORD>(style), FALSE, static_cast<DWORD>(exStyle)));

    const int monitorWidth = SubDword(monitor.right, monitor.left);
    const int width = SubDword(adjusted.right, adjusted.left);
    const int height = SubDword(adjusted.bottom, adjusted.top);
    const int monitorHeight = SubDword(monitor.bottom, monitor.top);
    // 438C4D/438C63 的 CDQ/SUB/SAR 等价于先 DWORD SUB，再有符号向零除二。
    int x = AddDword(SubDword(monitorWidth, width) / 2, monitor.left);
    int y = AddDword(SubDword(monitorHeight, height) / 2, monitor.top);
    // 438C6D/438C85在AdjustWindowRectEx之后重新读取；style/exStyle始终是原调用实参。
    if (monitorWidth <= ReadWidthAtEnvironment18())
        x = monitor.left;
    if (monitorHeight <= ReadHeightAtEnvironment1C())
        y = monitor.top;

    // 438C9F 仍使用 438B71 缓存的 HWND；437D80 则独立重读当前 HWND。
    static_cast<void>(platform_.PositionWindow(window, HWND_NOTOPMOST,
        x, y, width, height, 0x60u));
    static_cast<void>(platform_.InvalidateWindowRect(nullptr, nullptr, FALSE));
    graphics_.ResetRendererIfLoaded();
    static_cast<void>(platform_.ClearRect(body_->savedMonitorRectAt1C));
    return 1;
}
}
