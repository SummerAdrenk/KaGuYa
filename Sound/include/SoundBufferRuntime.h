#pragma once
#include "AudioDecoder.h"
#include <dsound.h>
#include <memory>

namespace KaGuYa::Sound
{
// 原Static/Polling共用底层buffer；上层播放/恢复策略保持独立。
class SoundBufferRuntime
{
public:
    virtual ~SoundBufferRuntime() noexcept(false) = default;
    virtual IDirectSoundBuffer* Get() const noexcept = 0;
    virtual int State() const = 0;
    virtual HRESULT Create(IDirectSound8* device, const WAVEFORMATEX* format,
        DWORD length, DWORD flags) = 0;
    HRESULT Lock(DWORD offset, DWORD length, void** first, DWORD* firstLength,
        void** second, DWORD* secondLength, DWORD flags);
};
std::shared_ptr<SoundBufferRuntime> MakeNullSoundBuffer();
std::shared_ptr<SoundBufferRuntime> MakeDirectSoundBuffer();
std::shared_ptr<IAudioDecoder> MakeNullSoundInput();
}
