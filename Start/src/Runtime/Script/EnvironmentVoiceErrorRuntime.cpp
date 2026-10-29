#include <Runtime/EnvironmentVoiceErrorRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

#include <string>

namespace KaGuYa::Start::Runtime
{
EnvironmentVoiceErrorRuntime::EnvironmentVoiceErrorRuntime(EnvironmentStartupRuntime& environment,
    decltype(&::MessageBoxA) messageBox) noexcept
    : environment_(environment), messageBox_(messageBox)
{
}

void EnvironmentVoiceErrorRuntime::ReportVoiceFailure(const std::string_view detail)
{
    // PE 5B5E94..5B5EB2：31字节CP932前缀，含末尾空格；5B5EB3为NUL。
    constexpr char prefix[] = "CmdVoice\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 32);
    // 4D341A -> 4CA280按正文长度连接，不附加换行或按首NUL截断detail。
    std::string message(prefix);
    message.append(detail);
    // 4D3428..4D3436在连接完成后才取Environment+10标题及+2C窗口，忽略返回值。
    // 5F1E9C的try仅覆盖state 1..8；本报告段为state 0，异常须向外传播。
    const auto& errorEnvironment = environment_.CaptureAt4180C0();
    const char* const caption = errorEnvironment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
    static_cast<void>(messageBox_(window, message.c_str(), caption, MB_ICONERROR));
}
}
