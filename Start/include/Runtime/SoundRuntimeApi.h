#pragma once
#include <Windows.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
using SoundRuntimeHandle = std::int32_t;
class ISoundRuntimeApi
{
public:
    virtual ~ISoundRuntimeApi() = default;
    virtual SoundRuntimeHandle AddSound(std::uint8_t mode) = 0;
    virtual void RemoveSound(SoundRuntimeHandle) = 0;
    virtual bool OpenSound(SoundRuntimeHandle, const wchar_t*, const wchar_t*, std::int32_t offset, std::int32_t size) = 0;
    virtual void SetSoundVolume(SoundRuntimeHandle, std::int32_t maximum, std::int32_t value) = 0;
    virtual void SetSoundLoop(SoundRuntimeHandle, std::uint8_t loop) = 0;
    virtual bool IsSlotPlaying(SoundRuntimeHandle) = 0;
    virtual void PlaySlot(SoundRuntimeHandle) = 0;
    virtual void StopSlot(SoundRuntimeHandle) = 0;
};
// 启动/退出桥接只借服务；不要求HMODULE、initialized或错误串存储。
class ISoundRuntimeLibrary : public ISoundRuntimeApi
{
public:
    virtual bool Initialize(HWND) = 0;
    virtual void Shutdown() = 0;
    virtual void ActivateAppRestoreIfLoaded(WPARAM) = 0;
    virtual void SetGlobalFocusAt45D28C(std::uint8_t) = 0;
    virtual GUID ReadDefaultDeviceIdAt45D57A() = 0;
    virtual long CalcValueToVolume(std::int32_t maximum, std::int32_t value) const = 0;
};
}
