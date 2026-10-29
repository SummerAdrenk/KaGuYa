#pragma once
#include <Runtime/SoundRootNative.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentPathRuntime;
class ISoundInitializationPathsNativeSource
{
public:
    virtual ~ISoundInitializationPathsNativeSource() = default;
    virtual EnvironmentPathRuntime* CapturePathsAt4180C0() = 0;
};
ISoundInitializationPathsNativeSource& DefaultSoundInitializationPathsNativeSource() noexcept;

// 原receiver就是singleton中的Ref12；不另持HMODULE/initialized/error镜像。
bool InitializeSoundRootNativeAt53DFC0(YtlReferenceSlotRuntime&, HWND,
    IEnvironmentRootMemory&, IRuntimeStringManagerSource&, IGraphicsModuleNativeWin32Api&,
    ISoundInitializationPathsNativeSource&);
void ShutdownSoundRootNativeAt53DF20(YtlReferenceSlotRuntime&, IEnvironmentRootMemory&,
    IRuntimeStringManagerSource&, IGraphicsModuleNativeWin32Api&);
}
