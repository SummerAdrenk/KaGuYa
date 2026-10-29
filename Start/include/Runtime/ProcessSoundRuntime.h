#pragma once
#include <Runtime/SoundRootNativeInitialize.h>

namespace KaGuYa::Start::Runtime
{
class NativeSoundRuntimeLibrary;
// 仅取稳定owner/API；不创建Ref12、不装载DLL，不增加独立HMODULE拥有。
SoundRootOwner& ProcessSoundRootOwner() noexcept;
SoundModuleNativeApi& ProcessSoundModuleNativeApi() noexcept;
NativeSoundRuntimeLibrary& ProcessSoundRuntimeLibrary() noexcept;
YtlReferenceSlotRuntime* CaptureProcessSoundRootAt4183D0();
bool InitializeProcessSoundAt53DFC0(HWND window);
void ShutdownProcessSoundAt53DF20();
}
