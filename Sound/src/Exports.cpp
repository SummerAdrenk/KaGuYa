#include "SoundState.h"

#include <Logging/Logger.h>

#include <Windows.h>

#include <bit>
#include <cstdint>

namespace
{
std::uintptr_t Handle(const int value) noexcept
{
    return static_cast<std::uintptr_t>(static_cast<std::uint32_t>(value));
}
}

extern "C" void __cdecl ActivateAppRestore(const int active)
{
    KaGuYa::Sound::RestoreActiveSounds(active != 0);
}

extern "C" int __cdecl AddSound(const unsigned char mode)
{
    return static_cast<int>(KaGuYa::Sound::AddSoundObject(mode));
}

extern "C" void __cdecl ExitSystem()
{
    KaGuYa::Sound::ShutdownSystem();
}

extern "C" const GUID* __cdecl GetDefaultDeviceID()
{
    return KaGuYa::Sound::GetDefaultPlaybackDevice();
}

extern "C" int __cdecl GetPrimaryVolume(const int maximum)
{
    return KaGuYa::Sound::VolumeToValue(maximum, KaGuYa::Sound::GetPrimaryVolumeDb());
}

extern "C" bool __cdecl InitSystem(const HWND window)
{
    KaGuYa::Logging::Initialize(L"Sound");
    return KaGuYa::Sound::InitializeSystem(window);
}

extern "C" bool __cdecl IsFrequencyControl()
{
    return KaGuYa::Sound::IsFrequencyControl();
}

// 原样本的 ordinal 8 与 9 指向同一 RVA；Sound.def 让 IsPanControl 继续别名到这里。
extern "C" bool __cdecl IsGlobalFocus()
{
    return KaGuYa::Sound::IsGlobalFocus();
}

extern "C" bool __cdecl IsTruePlayPosition()
{
    return KaGuYa::Sound::IsTruePlayPosition();
}

extern "C" int __cdecl RemoveSound(const int handle)
{
    return static_cast<int>(KaGuYa::Sound::RemoveSoundObject(Handle(handle)));
}

extern "C" bool __cdecl ResetDevice(const HWND window)
{
    return KaGuYa::Sound::ResetSystem(window);
}

extern "C" int __cdecl SetFrequencyControl(const unsigned char enabled)
{
    return KaGuYa::Sound::SetFrequencyControl(enabled);
}

extern "C" int __cdecl SetGlobalFocus(const unsigned char enabled)
{
    return KaGuYa::Sound::SetGlobalFocus(enabled);
}

extern "C" int __cdecl SetMinimumVolume(const int volume)
{
    return KaGuYa::Sound::SetMinimumVolume(volume);
}

extern "C" int __cdecl SetPanControl(const unsigned char enabled)
{
    return KaGuYa::Sound::SetPanControl(enabled);
}

#if defined(KAGUYA_SOUND_THREAD_ANSI_ABI)
extern "C" bool __cdecl SetPrimaryBufferFormat(const char* channels,
    const char* sampleRate, const char* bitsPerSample)
{
    return KaGuYa::Sound::SetPrimaryFormatAnsi(channels, sampleRate, bitsPerSample);
}
#else
extern "C" bool __cdecl SetPrimaryBufferFormat(const wchar_t* channels,
    const wchar_t* sampleRate, const wchar_t* bitsPerSample)
{
    return KaGuYa::Sound::SetPrimaryFormat(channels, sampleRate, bitsPerSample);
}
#endif

extern "C" void __cdecl SetPrimaryVolume(const int maximum, const int value)
{
    KaGuYa::Sound::SetPrimaryVolumeDb(KaGuYa::Sound::ValueToVolume(maximum, value));
}

extern "C" int __cdecl SetTruePlayPosition(const unsigned char enabled)
{
    return KaGuYa::Sound::SetTruePlayPosition(enabled);
}

extern "C" int __cdecl SoundCalcValueToVolume(const int maximum, const int value)
{
    return KaGuYa::Sound::ValueToVolume(maximum, value);
}

extern "C" int __cdecl SoundCalcVolumeToValue(const int maximum, const int volume)
{
    return KaGuYa::Sound::VolumeToValue(maximum, volume);
}

extern "C" int __cdecl SoundClose(const int handle)
{
    return static_cast<int>(KaGuYa::Sound::CloseSound(Handle(handle)));
}

#if defined(KAGUYA_SOUND_THREAD_ANSI_ABI)
extern "C" const char* __cdecl SoundGetFileName(const int handle)
{
    return KaGuYa::Sound::GetSoundFileNameAnsi(Handle(handle));
}
#else
extern "C" const wchar_t* __cdecl SoundGetFileName(const int handle)
{
    return KaGuYa::Sound::GetSoundFileName(Handle(handle));
}
#endif

extern "C" int __cdecl SoundGetVolume(const int handle, const int maximum)
{
    return KaGuYa::Sound::GetSoundVolumeValue(Handle(handle), maximum);
}

extern "C" bool __cdecl SoundIsLoop(const int handle)
{
    return KaGuYa::Sound::IsSoundLoop(Handle(handle));
}

extern "C" bool __cdecl SoundIsPlay(const int handle)
{
    return KaGuYa::Sound::IsSoundPlaying(Handle(handle));
}

#if defined(KAGUYA_SOUND_THREAD_ANSI_ABI)
extern "C" bool __cdecl SoundOpen(const int handle, const char* logicalName,
    const char* physicalName, const int offset, const int size)
{
    return KaGuYa::Sound::OpenSoundAnsi(Handle(handle), logicalName, physicalName,
        static_cast<std::uint32_t>(offset), static_cast<std::uint32_t>(size));
}
#else
extern "C" bool __cdecl SoundOpen(const int handle, const wchar_t* logicalName,
    const wchar_t* physicalName, const int offset, const int size)
{
    return KaGuYa::Sound::OpenSound(Handle(handle), logicalName, physicalName,
        static_cast<std::uint32_t>(offset), static_cast<std::uint32_t>(size));
}
#endif

extern "C" int __cdecl SoundPause(const int handle)
{
    return static_cast<int>(KaGuYa::Sound::PauseSound(Handle(handle)));
}

extern "C" int __cdecl SoundPlay(const int handle)
{
    return static_cast<int>(KaGuYa::Sound::PlaySound(Handle(handle)));
}

extern "C" int __cdecl SoundSetLoop(const int handle, const unsigned char loop)
{
    return static_cast<int>(KaGuYa::Sound::SetSoundLoop(Handle(handle), loop));
}

extern "C" int __cdecl SoundSetVolume(const int handle, const int maximum, const int value)
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        KaGuYa::Sound::SetSoundVolume(Handle(handle), maximum, value)));
}

extern "C" int __cdecl SoundStop(const int handle)
{
    return static_cast<int>(KaGuYa::Sound::StopSound(Handle(handle)));
}
