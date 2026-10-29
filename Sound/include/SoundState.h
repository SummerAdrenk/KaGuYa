#pragma once

#include <Windows.h>

#include <cstdint>

namespace KaGuYa::Sound
{
bool InitializeSystem(HWND window);
void ShutdownSystem();
bool ResetSystem(HWND window);
void RestoreActiveSounds(bool active);

const GUID* GetDefaultPlaybackDevice();
bool SetPrimaryFormat(const wchar_t* channels, const wchar_t* sampleRate,
    const wchar_t* bitsPerSample);
bool SetPrimaryFormatAnsi(const char* channels, const char* sampleRate, const char* bitsPerSample);
void SetPrimaryVolumeDb(int volume);
int GetPrimaryVolumeDb();

int SetMinimumVolume(int volume);
int MinimumVolume();
int SetGlobalFocus(std::uint8_t value);
int SetPanControl(std::uint8_t value);
int SetFrequencyControl(std::uint8_t value);
int SetTruePlayPosition(std::uint8_t value);
bool IsGlobalFocus();
bool IsPanControl();
bool IsFrequencyControl();
bool IsTruePlayPosition();

int ValueToVolume(int maximum, int value);
int VolumeToValue(int maximum, int volume);

std::uintptr_t AddSoundObject(std::uint8_t mode);
std::uintptr_t RemoveSoundObject(std::uintptr_t handle);
bool OpenSound(std::uintptr_t handle, const wchar_t* logicalName, const wchar_t* physicalName,
    std::uint32_t offset, std::uint32_t size);
bool OpenSoundAnsi(std::uintptr_t handle, const char* logicalName, const char* physicalName,
    std::uint32_t offset, std::uint32_t size);
std::uintptr_t CloseSound(std::uintptr_t handle);
std::uintptr_t PlaySound(std::uintptr_t handle);
std::uintptr_t PauseSound(std::uintptr_t handle);
std::uintptr_t StopSound(std::uintptr_t handle);
bool IsSoundPlaying(std::uintptr_t handle);
std::uintptr_t SetSoundLoop(std::uintptr_t handle, std::uint8_t loop);
bool IsSoundLoop(std::uintptr_t handle);
std::uintptr_t SetSoundVolume(std::uintptr_t handle, int maximum, int value);
int GetSoundVolumeValue(std::uintptr_t handle, int maximum);
const wchar_t* GetSoundFileName(std::uintptr_t handle);
const char* GetSoundFileNameAnsi(std::uintptr_t handle);
}
