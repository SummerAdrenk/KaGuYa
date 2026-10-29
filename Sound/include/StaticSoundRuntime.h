#pragma once
#include "AudioDecoder.h"
#include <dsound.h>
#include <memory>
#include <string>

namespace KaGuYa::Sound
{
class IStaticSoundServices
{
public:
    virtual ~IStaticSoundServices() = default;
    virtual bool GlobalFocus() = 0;
    virtual bool PanControl() = 0;
    virtual bool FrequencyControl() = 0;
    virtual bool TruePlayPosition() = 0;
    virtual IDirectSound8* Device() = 0;
    virtual AudioError OpenInput(const AudioOpenRequest& request, std::shared_ptr<IAudioDecoder>& input);
};

class SoundBufferRuntime;

// 原StaticSound的独立输入/缓冲归属；不混用Polling的音量缓存及恢复策略。
class StaticSoundRuntime final
{
public:
    StaticSoundRuntime() = default;
    StaticSoundRuntime(const StaticSoundRuntime&) = delete;
    StaticSoundRuntime& operator=(const StaticSoundRuntime&) = delete;
    void Initialize();
    ~StaticSoundRuntime() noexcept(false);
    bool Open(const AudioOpenRequest& request, IStaticSoundServices& services);
    void Clear();
    bool Play(IStaticSoundServices& services);
    bool Pause();
    bool Stop();
    bool IsPlaying() const;
    void SetLoop(std::uint8_t value) noexcept { loop_ = value; }
    bool IsLoop() const noexcept { return loop_ != 0; }
    void SetVolume(int value);
    int GetVolume() const;
    const wchar_t* FileName() const noexcept;
    void Restore(IStaticSoundServices& services);
private:
    void Rebuild(IStaticSoundServices& services);
    void Publish(std::shared_ptr<SoundBufferRuntime> next);
    DWORD Length() const;
    std::shared_ptr<SoundBufferRuntime> buffer_;
    std::shared_ptr<IAudioDecoder> input_;
    std::wstring name_;
    bool initialized_ = false;
    bool paused_ = false;
    std::uint8_t loop_ = 0;
};
}
