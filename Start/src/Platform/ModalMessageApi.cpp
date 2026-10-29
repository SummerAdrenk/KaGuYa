#include <Platform/ModalMessageApi.h>

namespace KaGuYa::Start::Platform
{
bool Win32ModalMessageApi::PeekMessageNoRemove(MSG& message)
{
    return ::PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE) != FALSE;
}

int Win32ModalMessageApi::GetMessageAt5073B0(MSG& message)
{
    return ::GetMessageA(&message, nullptr, 0, 0);
}

void Win32ModalMessageApi::TranslateMessageAt5073C9(const MSG& message)
{
    static_cast<void>(::TranslateMessage(&message));
}

void Win32ModalMessageApi::DispatchMessageAt5073D3(const MSG& message)
{
    static_cast<void>(::DispatchMessageA(&message));
}
}
