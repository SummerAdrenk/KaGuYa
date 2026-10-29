#include <Runtime/NativeSoundRuntimeLibrary.h>
#include <Runtime/ProcessSoundRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
NativeSoundRuntimeLibrary::NativeSoundRuntimeLibrary() noexcept
    : NativeSoundRuntimeLibrary(ProcessSoundRootOwner(), DefaultEnvironmentRootMemory(),
        DefaultRuntimeStringManagerSource(), DefaultGraphicsModuleNativeWin32Api(), DefaultSoundInitializationPathsNativeSource()) {}
NativeSoundRuntimeLibrary::NativeSoundRuntimeLibrary(SoundRootOwner& root, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32,
    ISoundInitializationPathsNativeSource& paths) noexcept
    : root_(root), memory_(memory), strings_(strings), win32_(win32), paths_(paths), api_(root, strings, win32) {}
bool NativeSoundRuntimeLibrary::Initialize(HWND window)
{ return InitializeSoundRootNativeAt53DFC0(*root_.CaptureAt4183D0(), window, memory_, strings_, win32_, paths_); }
bool NativeSoundRuntimeLibrary::Initialize(HWND window, ISoundInitializationPathsNativeSource& paths)
{ return InitializeSoundRootNativeAt53DFC0(*root_.CaptureAt4183D0(), window, memory_, strings_, win32_, paths); }
void NativeSoundRuntimeLibrary::Shutdown()
{ ShutdownSoundRootNativeAt53DF20(*root_.CaptureAt4183D0(), memory_, strings_, win32_); }
void NativeSoundRuntimeLibrary::ActivateAppRestoreIfLoaded(WPARAM active) { api_.ActivateAppRestoreAt42103E(active); }
void NativeSoundRuntimeLibrary::SetGlobalFocusAt45D28C(std::uint8_t enabled) { api_.SetGlobalFocusAt45D1F4(enabled); }
GUID NativeSoundRuntimeLibrary::ReadDefaultDeviceIdAt45D57A()
{
    GUID value;
    std::memcpy(&value, api_.DefaultDeviceAt45D4E2(), sizeof(value)); // 原45D581直接复制16字节，无空GUID兜底。
    return value;
}
long NativeSoundRuntimeLibrary::CalcValueToVolume(std::int32_t maximum, std::int32_t value) const
{ return api_.CalcValueToVolumeAt4A0E3A(maximum, value); }
SoundRuntimeHandle NativeSoundRuntimeLibrary::AddSound(std::uint8_t mode) { return api_.AddAt412AB0(mode, 0); }
void NativeSoundRuntimeLibrary::RemoveSound(SoundRuntimeHandle handle)
{
    // 外显SDK接口；18号Start未发现RemoveSound调用，manager析构不进入此方法。
    auto* module = ResolveGraphicsModuleReference(*root_.CaptureAt4183D0());
    auto name = ArchiveResourceNameAt40B530("RemoveSound", strings_);
    auto function = reinterpret_cast<void (__cdecl*)(std::int32_t)>(win32_.GetProcAddress(module->moduleAt04, name.Value().GetString()));
    name.ReleaseNormal();
    function(handle);
}
bool NativeSoundRuntimeLibrary::OpenSound(SoundRuntimeHandle handle, const wchar_t* logical, const wchar_t* physical,
    std::int32_t offset, std::int32_t size)
{ return api_.OpenCapturedAt42BD8F(*api_.CaptureAt42BD45(), handle, logical, physical, offset, size); }
void NativeSoundRuntimeLibrary::SetSoundVolume(SoundRuntimeHandle handle, std::int32_t maximum, std::int32_t value)
{ api_.SetVolume(handle, maximum, value); }
void NativeSoundRuntimeLibrary::SetSoundLoop(SoundRuntimeHandle handle, std::uint8_t loop) { api_.SetLoopAt42BEBF(handle, loop); }
bool NativeSoundRuntimeLibrary::IsSlotPlaying(SoundRuntimeHandle handle) { return api_.IsPlaying(handle); }
void NativeSoundRuntimeLibrary::PlaySlot(SoundRuntimeHandle handle) { api_.PlayHandleAt438090(handle); }
void NativeSoundRuntimeLibrary::StopSlot(SoundRuntimeHandle handle) { api_.StopAt4385F1(handle); }
}
