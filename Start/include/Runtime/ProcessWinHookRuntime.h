#pragma once
#include <Runtime/WinHookNative.h>
namespace KaGuYa::Start::Runtime
{
WinHookRootNativeOwner& ProcessWinHookRootNativeOwner() noexcept;
WinHookNativeServices& ProcessWinHookNativeServices() noexcept;
NativeWindowHookMessageSource& ProcessWindowHookMessageSource() noexcept;
WinHookNativeBody30h* CaptureProcessWinHookAt450E20();
}
