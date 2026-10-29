#include <Runtime/EnvironmentPreWindowServices.h>
#include <Windows.h>
#include <string>

namespace KaGuYa::Start::Runtime
{
void Win32EnvironmentInitializationDisplay::Show(
    std::string_view text, std::string_view caption, unsigned type)
{
    const std::string textZ(text), captionZ(caption);
    // 477C18 明确 push 0：错误框不是绑定到尚未创建的游戏 HWND。
    static_cast<void>(::MessageBoxA(nullptr, textZ.c_str(), captionZ.c_str(), type));
}

}
