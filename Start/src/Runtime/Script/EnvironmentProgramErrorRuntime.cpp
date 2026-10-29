#include <Runtime/EnvironmentProgramErrorRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>

#include <string>

namespace KaGuYa::Start::Runtime
{
EnvironmentProgramErrorRuntime::EnvironmentProgramErrorRuntime(EnvironmentStartupRuntime& environment,
    decltype(&::MessageBoxA) messageBox) noexcept
    : environment_(environment), messageBox_(messageBox)
{
}

void EnvironmentProgramErrorRuntime::ReportProgramFailure(const std::string_view detail)
{
    // PE 5B6040..5B6060：33字节CP932前缀，含末尾空格；不是格式模板，不附加换行。
    constexpr char prefix[] = "CmdProgram\x8F\x88\x97\x9D\x82\xAA\x8E\xB8\x94\x73"
        "\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : ";
    static_assert(sizeof(prefix) == 34);
    // 4CA319 -> 4145E0按CString记录长度连接。string_view里的内嵌NUL不改变拷贝长度。
    std::string message(prefix);
    message.append(detail);
    // 4D0731..4D073F在连接完成之后才借当前标题raw和窗口；异常不在报告内吞掉。
    const auto& errorEnvironment = environment_.CaptureAt4180C0();
    const char* const caption = errorEnvironment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
    static_cast<void>(messageBox_(window, message.c_str(), caption, MB_ICONERROR));
}
}
