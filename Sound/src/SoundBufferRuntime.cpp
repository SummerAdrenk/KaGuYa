#include "SoundBufferRuntime.h"
#include <Logging/Logger.h>

namespace KaGuYa::Sound
{
namespace
{
class NullBuffer final : public SoundBufferRuntime
{
public:
    IDirectSoundBuffer* Get() const noexcept override { return nullptr; }
    int State() const override { return -1; }
    HRESULT Create(IDirectSound8*, const WAVEFORMATEX*, DWORD, DWORD) override { return E_FAIL; }
};
class DirectBuffer final : public SoundBufferRuntime
{
public:
    ~DirectBuffer() noexcept(false) override { if (buffer_) buffer_->Release(); }
    IDirectSoundBuffer* Get() const noexcept override { return buffer_; }
    int State() const override
    {
        if (!buffer_) return 0;
        DWORD status = 0;
        const auto result = buffer_->GetStatus(&status);
        // 原AB40使用已写status而非HRESULT；未写输出的原栈垃圾域不复制。
        if (result != DS_OK)
            Logging::WriteFormat(Logging::Level::Error, "Sound GetStatus hr=%08lX status=%08lX.",
                static_cast<unsigned long>(result), status);
        if (status & DSBSTATUS_BUFFERLOST) return 1;
        return status & DSBSTATUS_PLAYING ? 3 : 2;
    }
    HRESULT Create(IDirectSound8* device, const WAVEFORMATEX* format, DWORD length, DWORD flags) override
    {
        if (!device || !format) return E_POINTER;
        DSBUFFERDESC description{};
        description.dwSize = sizeof(description);
        description.dwFlags = flags;
        description.dwBufferBytes = length;
        description.lpwfxFormat = const_cast<WAVEFORMATEX*>(format);
        // C430直接写已发布wrapper的COM槽；没有Buffer8 QI或成功后才发布。
        const auto result = device->CreateSoundBuffer(&description, &buffer_, nullptr);
        return result == DS_OK && !buffer_ ? E_POINTER : result;
    }
private:
    IDirectSoundBuffer* buffer_ = nullptr;
};
static_assert(sizeof(DirectBuffer) == 8);
static_assert(sizeof(NullBuffer) == 4);

class NullInput final : public IAudioDecoder
{
public:
    AudioInputKind Kind() const noexcept override { return AudioInputKind::None; }
    const WAVEFORMATEX& Format() const noexcept override { static const WAVEFORMATEX empty{}; return empty; }
    const WAVEFORMATEX* FormatPointer() const noexcept override { return nullptr; }
    std::uint64_t TotalPcmBytes() const noexcept override { return 0; }
    std::uint64_t PositionPcmBytes() const noexcept override { return 0; }
    AudioDecodeState State() const noexcept override { return AudioDecodeState::EndOfStream; }
    AudioError LastError() const noexcept override { return AudioError::None; }
    AudioReadResult ReadPcm(std::span<std::byte>) noexcept override
    { return {0, AudioDecodeState::EndOfStream, AudioError::None}; }
    AudioError SeekToStart() noexcept override { return AudioError::None; }
};
static_assert(sizeof(NullInput) == 4);
}

HRESULT SoundBufferRuntime::Lock(const DWORD offset, const DWORD length, void** first,
    DWORD* firstLength, void** second, DWORD* secondLength, const DWORD flags)
{
    auto* const buffer = Get();
    if (!buffer)
    {
        Logging::Write(Logging::Level::Error, "Sound buffer Lock without COM: rejected.");
        return E_POINTER;
    }
    auto result = buffer->Lock(offset, length, first, firstLength, second, secondLength, flags);
    if (result == DSERR_BUFFERLOST)
    {
        // 10009F90/1000A000忽略Restore普通返回，原参数只重试一次Lock。
        static_cast<void>(buffer->Restore());
        result = buffer->Lock(offset, length, first, firstLength, second, secondLength, flags);
    }
    return result;
}
std::shared_ptr<SoundBufferRuntime> MakeNullSoundBuffer() { return std::make_shared<NullBuffer>(); }
std::shared_ptr<SoundBufferRuntime> MakeDirectSoundBuffer() { return std::make_shared<DirectBuffer>(); }
std::shared_ptr<IAudioDecoder> MakeNullSoundInput() { return std::make_shared<NullInput>(); }
}
