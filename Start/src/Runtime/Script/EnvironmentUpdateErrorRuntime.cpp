#include <Runtime/EnvironmentUpdateErrorRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

#include <string>

namespace KaGuYa::Start::Runtime
{
EnvironmentUpdateErrorRuntime::EnvironmentUpdateErrorRuntime(EnvironmentStartupRuntime& environment,
    decltype(&::MessageBoxA) messageBox) noexcept
    : environment_(environment), messageBox_(messageBox)
{
}

void EnvironmentUpdateErrorRuntime::ReportUpdateFailure(const std::string_view detail)
{
    // PE 5B5E04..5B5E23：32字节CP932前缀，含末尾空格；5B5E24为NUL。
    constexpr char prefix[] = "CmdUpdate\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 33);
    // 4D2F29 -> 4CA280 按正文长度连接，不附加换行，不按首NUL截断detail。
    std::string message(prefix);
    message.append(detail);
    // 4D2F37..4D2F45：连接完成后才读实际 Environment+10/+2C；报告异常外传。
    const auto& errorEnvironment = environment_.CaptureAt4180C0();
    const char* const caption = errorEnvironment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
    static_cast<void>(messageBox_(window, message.c_str(), caption, MB_ICONERROR));
}
}
