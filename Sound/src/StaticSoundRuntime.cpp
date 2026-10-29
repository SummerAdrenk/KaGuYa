#include "StaticSoundRuntime.h"
#include "SoundBufferRuntime.h"
#include "SoundFileName.h"
#include <Logging/Logger.h>
#include <limits>
#include <utility>

namespace KaGuYa::Sound
{
AudioError IStaticSoundServices::OpenInput(const AudioOpenRequest& request, std::shared_ptr<IAudioDecoder>& input)
{
    std::unique_ptr<IAudioDecoder> opened;
    const auto result = OpenAudioDecoder(request, opened);
    if (result == AudioError::None) input = std::move(opened);
    return result;
}

namespace
{

[[noreturn]] void Fail(const char* operation, HRESULT result)
{
    Logging::WriteFormat(Logging::Level::Error, "Static %s failed: hr=%08lX.", operation,
        static_cast<unsigned long>(result));
    throw 1;
}
}

void StaticSoundRuntime::Initialize()
{
    buffer_ = MakeNullSoundBuffer();
    input_ = MakeNullSoundInput();
    initialized_ = true;
}
StaticSoundRuntime::~StaticSoundRuntime() noexcept(false)
{
    if (initialized_) Clear();
}
void StaticSoundRuntime::Publish(std::shared_ptr<SoundBufferRuntime> next)
{
    // 原8180先发布再退休，旧COM Release回调看到的是新wrapper。
    buffer_.swap(next);
}
DWORD StaticSoundRuntime::Length() const
{
    const auto input = input_;
    const auto length = input->TotalPcmBytes();
    if (length > (std::numeric_limits<DWORD>::max)()) Fail("length overflow", E_INVALIDARG);
    return static_cast<DWORD>(length);
}
bool StaticSoundRuntime::Open(const AudioOpenRequest& request, IStaticSoundServices& services)
{
    try
    {
        // 原706B激活try后才调用初次Clear；handler中的Clear不属于同一恢复范围。
        Clear();
        std::shared_ptr<IAudioDecoder> input;
        const auto result = services.OpenInput(request, input);
        if (result != AudioError::None)
        {
            Logging::WriteFormat(Logging::Level::Error, "Static SoundOpen decoder failed: %ls.", AudioErrorName(result));
            input = MakeNullSoundInput();
        }
        input_ = std::move(input);
        {
            const auto source = input_;
            if (!source || !source->FormatPointer()) throw 1;
        }
        {
            // 原7257的Open级Seek不能由7E8C的Rebuild级Seek替代；忽略普通返回。
            const auto source = input_;
            static_cast<void>(source->SeekToStart());
        }
        Rebuild(services);
        // 原7319在Rebuild成功后才求basename并提交；Create/Lock/读取期间仍是空名。
        name_.assign(SoundFileNamePart(request.logicalPath));
        return true;
    }
    catch (...)
    {
        Clear();
        return false;
    }
}
void StaticSoundRuntime::Rebuild(IStaticSoundServices& services)
{
    Publish(MakeDirectSoundBuffer());
    {
        const auto source = input_;
        const auto destination = buffer_;
        const DWORD firstLength = Length();
        const auto* format = source->FormatPointer();
        DWORD flags = 0x10088;
        if (services.GlobalFocus()) flags |= 0x8000;
        if (services.PanControl()) flags |= 0x40;
        if (services.FrequencyControl()) flags |= 0x20;
        if (services.TruePlayPosition()) flags |= 0x80000;
        const auto result = destination->Create(services.Device(), format, firstLength, flags);
        if (result != DS_OK) Fail("CreateSoundBuffer", result);
    }
    void* pixels = nullptr;
    DWORD actual = 0;
    {
        const auto destination = buffer_;
        auto* const target = destination->Get();
        const DWORD secondLength = Length();
        if (!target) Fail("Lock without COM", E_POINTER);
        auto result = target->Lock(0, secondLength, &pixels, &actual, nullptr, nullptr, 0);
        if (result == DSERR_BUFFERLOST)
        {
            // 原9F90：Restore返回值不决定是否重试；只重试同一组Lock实参一次。
            static_cast<void>(target->Restore());
            result = target->Lock(0, secondLength, &pixels, &actual, nullptr, nullptr, 0);
        }
        if (result != DS_OK) Fail("Lock", result);
    }
    {
        const auto source = input_;
        const auto seek = source->SeekToStart(); // 原7E8C忽略普通返回，不跳过Read。
        if (seek != AudioError::None)
            Logging::WriteFormat(Logging::Level::Error, "Static Seek ordinary return: %ls; continuing original Read path.", AudioErrorName(seek));
    }
    bool safetyRejected = false;
    {
        const auto source = input_;
        const auto read = source->ReadPcm({static_cast<std::byte*>(pixels), actual});
        // 原普通IO/短读返回不决定失败；危险解码负值、非法内存请求仍显式拒绝。
        if (!read && read.error != AudioError::ReadFailed && read.error != AudioError::SeekFailed)
        {
            Logging::WriteFormat(Logging::Level::Error, "Static Read safety rejection: %ls.", AudioErrorName(read.error));
            safetyRejected = true;
        }
        else if (!read)
            Logging::WriteFormat(Logging::Level::Error, "Static Read ordinary return: %ls; continuing Unlock.", AudioErrorName(read.error));
    }
    {
        const auto destination = buffer_;
        auto* const target = destination->Get();
        if (!target) Fail("Unlock without COM", E_POINTER);
        const auto result = target->Unlock(pixels, actual, nullptr, 0);
        if (result != DS_OK) Fail("Unlock", result);
    }
    // 显式安全拒绝沿既有释放锁后失败路径；真实C++输入异常仍会跳过正常Unlock。
    if (safetyRejected) throw 1;
    // 原正常返回不SetVolume/SetPosition/Play，也不清paused/loop；没有PCM缓存。
}
bool StaticSoundRuntime::IsPlaying() const
{
    const auto buffer = buffer_;
    return buffer->State() == 3;
}
void StaticSoundRuntime::Restore(IStaticSoundServices& services)
{
    bool rebuild = false;
    {
        const auto buffer = buffer_;
        if (buffer->State() != 1) return;
        const auto source = input_;
        rebuild = source->FormatPointer() != nullptr;
    }
    if (!rebuild) return;
    // 79F0仅保护Rebuild；门控及catch里的Clear异常不能被外层吞掉。
    try { Rebuild(services); }
    catch (...) { Clear(); }
}
bool StaticSoundRuntime::Play(IStaticSoundServices& services)
{
    paused_ = false;
    { const auto buffer = buffer_; if (buffer->Get()) buffer->Get()->SetCurrentPosition(0); }
    if (IsPlaying())
    {
        { const auto buffer = buffer_; if (buffer->Get()) buffer->Get()->Stop(); }
        { const auto buffer = buffer_; if (buffer->Get()) buffer->Get()->SetCurrentPosition(0); }
    }
    bool failed = true;
    { const auto buffer = buffer_; if (buffer->Get()) failed = buffer->Get()->Play(0, 0, loop_ ? DSBPLAY_LOOPING : 0) != DS_OK; }
    if (failed)
    {
        bool lost;
        { const auto buffer = buffer_; lost = buffer->State() == 1; }
        if (lost) Restore(services); // 原7430失败恢复后不再次Play。
    }
    return !failed;
}
bool StaticSoundRuntime::Pause()
{
    if (paused_)
    {
        paused_ = false;
        const auto buffer = buffer_;
        if (buffer->Get()) buffer->Get()->Play(0, 0, loop_ ? DSBPLAY_LOOPING : 0);
        return true;
    }
    else if (IsPlaying())
    {
        paused_ = true;
        const auto buffer = buffer_;
        if (buffer->Get()) buffer->Get()->Stop();
        return true;
    }
    return false;
}
bool StaticSoundRuntime::Stop()
{
    if (!IsPlaying() && !paused_) return false;
    paused_ = false;
    { const auto buffer = buffer_; if (buffer->Get()) buffer->Get()->Stop(); }
    { const auto buffer = buffer_; if (buffer->Get()) buffer->Get()->SetCurrentPosition(0); }
    return true;
}
void StaticSoundRuntime::Clear()
{
    Stop();
    Publish(MakeNullSoundBuffer());
    std::shared_ptr<IAudioDecoder> next = MakeNullSoundInput();
    input_.swap(next);
    next.reset();
    paused_ = false;
    name_.clear(); // 原8230保留loop。
}
void StaticSoundRuntime::SetVolume(int value)
{
    const auto buffer = buffer_;
    if (buffer->Get()) buffer->Get()->SetVolume(value);
}
int StaticSoundRuntime::GetVolume() const
{
    const auto buffer = buffer_;
    LONG volume = -10000;
    if (!buffer->Get() || buffer->Get()->GetVolume(&volume) != DS_OK) return -10000;
    return volume;
}
const wchar_t* StaticSoundRuntime::FileName() const noexcept
{
    return name_.empty() ? nullptr : name_.c_str();
}
}
