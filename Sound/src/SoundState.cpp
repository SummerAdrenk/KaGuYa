#include "SoundState.h"
#include "SoundAnsiText.h"

#include "AudioDecoder.h"
#include "SoundPollingCursor.h"
#include "StaticSoundRuntime.h"
#include "SoundRegistry.h"
#include "SoundBufferRuntime.h"
#include "SoundRoots.h"
#include "SoundDeviceDrivers.h"
#include "SoundFileName.h"

#include <Logging/Logger.h>

#include <dsound.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <condition_variable>
#include <deque>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace KaGuYa::Sound
{
namespace
{
// 原1000C479：公开轮询对象不申请通知接口，不能套内部事件流的0x100位。
constexpr DWORD PollingBufferFlags = 0x00010088u;
constexpr DWORD GlobalFocusFlag = 0x00008000u;
constexpr DWORD PanControlFlag = 0x00000040u;
constexpr DWORD FrequencyControlFlag = 0x00000020u;
constexpr DWORD TruePlayPositionFlag = 0x00080000u;
constexpr std::size_t WorkerCount = 3;

int DivideVolumeAt10004436(const int minimum, const int maximum)
{
    // 原 CDQ/IDIV 的除零是处理器异常，不能改成 C++ 异常、返回值或有 UB 的除法。
    // 本项目只构建 x86；不为其他架构猜测等价的异常现场。
#if !defined(_M_IX86)
#error Sound volume conversion requires the original x86 IDIV semantics.
#endif
    int quotient;
    __asm
    {
        mov eax, minimum
        cdq
        mov ecx, maximum
        idiv ecx
        mov quotient, eax
    }
    return quotient;
}

IStaticSoundServices& StaticServices();
void QueueStreamingTask(SoundObject* sound);
}

class SoundObject final
{
public:
    explicit SoundObject(const bool streaming) : streaming_(streaming)
    {
        if (!streaming_) staticSound_.Initialize();
        else
        {
            buffer_ = MakeNullSoundBuffer();
            decoder_ = MakeNullSoundInput();
        }
    }

    ~SoundObject() noexcept
    {
        if (streaming_) ClearPolling();
    }

    bool Open(const AudioOpenRequest& request)
    {
        if (!streaming_)
        {
            return staticSound_.Open(request, StaticServices());
        }
        // B730的初次Clear在try内；失败续接的Clear在catch外层保护范围之外。
        try
        {
            ClearPolling();
            std::shared_ptr<IAudioDecoder> input;
            const auto error = StaticServices().OpenInput(request, input);
            if (error != AudioError::None)
            {
                Logging::WriteFormat(Logging::Level::Error, "Polling input factory: %ls.", AudioErrorName(error));
                input = MakeNullSoundInput();
            }
            PublishInput(std::move(input));
            {
                const auto source = CaptureInput();
                const auto* format = source->FormatPointer();
                if (!format) throw 1;
                polling_.bytesPerSecond = format->nAvgBytesPerSec;
            }
            {
                const auto source = CaptureInput();
                const auto length = source->TotalPcmBytes();
                if (length > (std::numeric_limits<std::uint32_t>::max)())
                {
                    Logging::Write(Logging::Level::Error, "Polling input length exceeds original DWORD domain.");
                    throw 1;
                }
                polling_.totalPcmBytes = static_cast<std::uint32_t>(length);
            }
            { const auto source = CaptureInput(); static_cast<void>(source->SeekToStart()); }
            fileName_.assign(SoundFileNamePart(request.logicalPath));
            return true;
        }
        catch (...)
        {
            ClearPolling();
            return false;
        }
    }

    std::uintptr_t Close()
    {
        if (!streaming_) { staticSound_.Clear(); return reinterpret_cast<std::uintptr_t>(this); }
        ClearPolling();
        return 0;
    }

    std::uintptr_t Play()
    {
        if (!streaming_) return staticSound_.Play(StaticServices()) ? 2u : 0u;
        try
        {
            // BF89先查IsPlay，再读paused；不得仅按旧logicalPlaying决定。
            if (IsPlaying() || paused_) Stop();
            PrepareStream();
            HRESULT result;
            {
                const auto buffer = CaptureBuffer();
                if (!buffer->Get()) throw 1;
                result = buffer->Get()->Play(0, 0, DSBPLAY_LOOPING);
            }
            if (result != DS_OK) throw 1;
            QueueStreamingTask(this);
        }
        catch (...)
        {
            // BF50本地handler只Stop；失败重建不直接Clear成空输入。
            Stop();
        }
        return 0;
    }

    std::uintptr_t Pause()
    {
        if (!streaming_) return staticSound_.Pause() ? 2u : 0u;
        std::scoped_lock lock(mutex_);
        if (!paused_)
        {
            if (!IsPlaying()) return 0;
            paused_ = true;
            const auto buffer = CaptureBuffer();
            if (buffer->Get()) static_cast<void>(buffer->Get()->Stop());
        }
        else
        {
            paused_ = false;
            const auto buffer = CaptureBuffer();
            if (buffer->Get()) static_cast<void>(buffer->Get()->Play(0, 0, DSBPLAY_LOOPING));
        }
        // BC04/BC34先改paused再调用COM，不依据普通HRESULT回滚字段。
        return 0;
    }

    std::uintptr_t Stop()
    {
        if (!streaming_) return staticSound_.Stop() ? 2u : 0u;
        StopStreamingTaskAndWait();
        std::scoped_lock lock(mutex_);
        StopStreamPlaybackLocked();
        return 0;
    }

    bool IsPlaying()
    {
        if (!streaming_) return staticSound_.IsPlaying();
        const auto buffer = CaptureBuffer();
        return buffer->State() == 3;
    }

    std::uintptr_t SetLoop(const std::uint8_t loop) noexcept
    {
        if (!streaming_) { staticSound_.SetLoop(loop); return loop; }
        std::scoped_lock lock(mutex_);
        loop_ = loop;
        // 原 DLL 的 StreamSoundPolling 虚函数会更新循环状态，但导出返回 0；
        // StaticSound 保留并返回完整 byte，不能在导出边界提前归一化为 bool。
        return streaming_ ? 0u : loop;
    }

    bool IsLoop() const noexcept
    {
        if (!streaming_) return staticSound_.IsLoop();
        return loop_.load(std::memory_order_relaxed);
    }

    std::uintptr_t SetVolume(const int volume)
    {
        if (!streaming_) { staticSound_.SetVolume(volume); return 2; }
        std::scoped_lock lock(mutex_);
        volumeDb_ = volume;
        const auto buffer = CaptureBuffer();
        if (buffer->Get()) static_cast<void>(buffer->Get()->SetVolume(volumeDb_));
        // 与 SetLoop 相同，轮询流对象的原虚函数返回 0，但音量已经写入底层缓冲。
        return streaming_ ? 0u : 2u;
    }

    int GetVolume() const
    {
        if (!streaming_) return staticSound_.GetVolume();
        std::scoped_lock lock(mutex_);
        return volumeDb_;
    }

    const wchar_t* GetFileName() const noexcept
    {
        if (!streaming_) return staticSound_.FileName();
        std::scoped_lock lock(mutex_);
        return fileName_.empty() ? nullptr : fileName_.c_str();
    }

    void Restore()
    {
        if (!streaming_) { staticSound_.Restore(StaticServices()); return; }
        int state;
        {
            std::scoped_lock lock(mutex_);
            const auto buffer = CaptureBuffer();
            state = buffer->State();
        }
        bool hasFormat;
        { const auto source = CaptureInput(); hasFormat = source->FormatPointer() != nullptr; }
        if (state != 1 || !hasFormat) return;
        // C192重新查当前状态；持续lost只Stop，不重建，也不旧COM.Restore。
        const bool wasPlaying = IsPlaying();
        Stop();
        if (wasPlaying)
        {
            try { Play(); }
            catch (...) { ClearPolling(); }
        }
    }

    bool IsStreaming() const noexcept
    {
        return streaming_;
    }

    std::uint64_t PrepareStreamingTask()
    {
        std::unique_lock lock(taskMutex_);
        if (taskState_ == TaskState::Running)
        {
            taskStopRequested_ = true;
            taskCondition_.notify_all();
            taskCondition_.wait(lock, [this] { return taskState_ == TaskState::Finished; });
        }

        // 1760不取消已有state0项；代次仅用于退休安全屏障，不用于普通Start去重。
        taskStopRequested_ = false;
        taskState_ = TaskState::Queued;
        return taskGeneration_;
    }

    void StopStreamingTaskAndWait() noexcept
    {
        if (!streaming_) return;
        std::unique_lock lock(taskMutex_);
        if (taskState_ != TaskState::Running)
        {
            return;
        }

        taskStopRequested_ = true;
        taskCondition_.notify_all();
        taskCondition_.wait(lock, [this] { return taskState_ == TaskState::Finished; });
    }

    void RetireStreamingTasks() noexcept
    {
        if (!streaming_) return;
        std::unique_lock lock(taskMutex_);
        // 正常Stop不取消state0。只有根真正退休时额外阻止未开始项，避免原裸Q悬空。
        taskRetiring_ = true;
        ++taskGeneration_;
        taskStopRequested_ = true;
        taskCondition_.notify_all();
        taskCondition_.wait(lock, [this] { return activeRuns_ == 0; });
        taskState_ = TaskState::Finished;
    }

    bool BeginStreamingTask(const std::uint64_t generation) noexcept
    {
        std::scoped_lock lock(taskMutex_);
        if (taskGeneration_ != generation || taskRetiring_)
        {
            return false;
        }
        taskState_ = TaskState::Running;
        ++activeRuns_;
        return true;
    }

    void RunStreamingTask(const std::uint64_t generation)
    {
        while (true)
        {
            {
                std::scoped_lock lock(taskMutex_);
                if (taskGeneration_ != generation || taskStopRequested_)
                {
                    return;
                }
            }

            const StreamPollResult result = PollStream();
            if (result == StreamPollResult::Complete)
            {
                return;
            }
            if (result == StreamPollResult::ContinueImmediately)
            {
                continue;
            }

            std::unique_lock lock(taskMutex_);
            taskCondition_.wait_for(lock, std::chrono::milliseconds(20), [this, generation]
            {
                return taskGeneration_ != generation || taskStopRequested_;
            });
        }
    }

    void FinishStreamingTask(const std::uint64_t generation) noexcept
    {
        std::scoped_lock lock(taskMutex_);
        (void)generation;
        --activeRuns_;
        taskState_ = TaskState::Finished;
        taskCondition_.notify_all();
    }

private:
    enum class StreamPollResult
    {
        ContinueImmediately,
        Wait,
        Complete
    };

    enum class TaskState
    {
        Idle,
        Queued,
        Running,
        Finished
    };

    StreamPollResult PollStream()
    {
        std::unique_lock lock(mutex_);
        if (!buffer_->Get())
        {
            // 原Null.GetPosition不写输出而Run仍读取栈值；明确拒绝该未初始化危险域。
            Logging::Write(Logging::Level::Error, "Polling Run without live COM position output: rejected.");
            return StreamPollResult::Complete;
        }
        DWORD playCursor = 0;
        DWORD writeCursor = 0;
        if (buffer_->Get()->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
        {
            // DE8F之后不判断返回，仍消费已写游标；未写输出时使用安全初始化的0。
            Logging::Write(Logging::Level::Warning, "Polling position query failed; consuming written cursor output.");
        }
        if (polling_.AdvancePlay(playCursor))
        {
            // 1000DF72..1000E074：先停/归零/回卷，再解锁读取Loop；短暂false是合法观察。
            StopStreamPlaybackLocked();
            lock.unlock();
            if (!IsLoop()) return StreamPollResult::Complete;
            PrepareStream();
            lock.lock();
            const auto buffer = CaptureBuffer();
            lock.unlock();
            // 原1000E0A4循环重启不检查Play的普通返回；公开Play的失败合同另行保留。
            static_cast<void>(buffer->Get()->Play(0, 0, DSBPLAY_LOOPING));
            return StreamPollResult::ContinueImmediately;
        }
        // 原1000E12D在播放累计之后再次读取play/write，不能复用先前样本。
        playCursor = 0;
        writeCursor = 0;
        if (buffer_->Get()->GetCurrentPosition(&playCursor, &writeCursor) != DS_OK)
        {
            Logging::Write(Logging::Level::Warning, "Polling second position query failed; consuming written cursor output.");
        }
        std::uint32_t requested = 0;
        if (!polling_.PlanRefill(playCursor, writeCursor, requested))
            return StreamPollResult::Wait;
        const auto nextFill = polling_.nextFill;
        lock.unlock();
        if (!FillStreamRange(nextFill, requested))
        {
            lock.lock();
            StopStreamPlaybackLocked();
            return StreamPollResult::Complete;
        }
        return StreamPollResult::ContinueImmediately;
    }
    std::shared_ptr<SoundBufferRuntime> CaptureBuffer() const
    {
        std::scoped_lock lock(bufferReferenceMutex_);
        return buffer_;
    }
    std::shared_ptr<IAudioDecoder> CaptureInput() const
    {
        std::scoped_lock lock(bufferReferenceMutex_);
        return decoder_;
    }
    void PublishBuffer(std::shared_ptr<SoundBufferRuntime> next)
    {
        // E6A0先发布，再在引用临界区外退休旧wrapper，不直接交换COM指针。
        { std::scoped_lock lock(bufferReferenceMutex_); buffer_.swap(next); }
    }
    void PublishInput(std::shared_ptr<IAudioDecoder> next)
    {
        { std::scoped_lock lock(bufferReferenceMutex_); decoder_.swap(next); }
    }
    void StopStreamPlaybackLocked()
    {
        { const auto buffer = CaptureBuffer(); if (buffer->Get()) static_cast<void>(buffer->Get()->Stop()); }
        { const auto buffer = CaptureBuffer(); if (buffer->Get()) static_cast<void>(buffer->Get()->SetCurrentPosition(0)); }
        { const auto source = CaptureInput(); static_cast<void>(source->SeekToStart()); }
        polling_.played = 0;
        paused_ = false;
    }
    bool CreateStreamingBufferLocked()
    {
        PublishBuffer(MakeDirectSoundBuffer());
        const auto source = CaptureInput();
        const auto buffer = CaptureBuffer();
        const auto* format = source->FormatPointer();
        auto& services = StaticServices();
        DWORD flags = PollingBufferFlags;
        if (services.GlobalFocus()) flags |= GlobalFocusFlag;
        if (services.PanControl()) flags |= PanControlFlag;
        if (services.FrequencyControl()) flags |= FrequencyControlFlag;
        if (services.TruePlayPosition()) flags |= TruePlayPositionFlag;
        auto* const device = services.Device();
        const auto result = buffer->Create(device, format, bufferBytes_, flags);
        if (result != DS_OK)
        {
            Logging::WriteFormat(Logging::Level::Error,
                "Polling buffer Create failed: hr=%08lX flags=%08lX bytes=%lu.",
                static_cast<unsigned long>(result), flags, bufferBytes_);
            return false;
        }
        // C84D..C8A8只对流式Create清整个新环；格式来自本次Create的实际输入引用。
        void* initial = nullptr;
        DWORD actual = bufferBytes_;
        // 原04 1000C86A/03 1000C8A9首次清环直接调COM，lost也立即失败；
        // 后续fill仍走带Restore重试的wrapper，不能把两个调用点混成同一策略。
        if (buffer->Get()->Lock(0, bufferBytes_, &initial, &actual, nullptr, nullptr, 0) != DS_OK) return false;
        std::memset(initial, format->wBitsPerSample == 8 ? 0x80 : 0, actual);
        return buffer->Get()->Unlock(initial, actual, nullptr, 0) == DS_OK;
    }

    void PrepareStream()
    {
        std::unique_lock lock(mutex_);
        { const auto source = CaptureInput(); if (!source->FormatPointer()) throw 1; }
        const auto bytes = static_cast<std::uint64_t>(polling_.bytesPerSecond) * 4u;
        if (bytes > (std::numeric_limits<DWORD>::max)())
        {
            Logging::Write(Logging::Level::Error, "Polling ring size overflows original DWORD domain.");
            throw 1;
        }
        bufferBytes_ = static_cast<DWORD>(bytes);
        {
            const auto source = CaptureInput();
            const auto* format = source->FormatPointer();
            if (!format || !format->nBlockAlign)
            {
                Logging::Write(Logging::Level::Error, "Polling format/block alignment is invalid.");
                throw 1;
            }
            bufferBytes_ -= bufferBytes_ % format->nBlockAlign;
        }
        polling_.bufferBytes = bufferBytes_;
        // CB40不提前Stop旧buffer；发布新wrapper即退休旧owner，然后才实际Create。
        if (!CreateStreamingBufferLocked()) throw 1;
        initialFillBytes_ = ((bufferBytes_ * 7u) >> 3u) & 0x1FFFFFFCu;
        polling_.ResetForBuffer();
        { const auto buffer = CaptureBuffer(); static_cast<void>(buffer->Get()->SetCurrentPosition(0)); }
        lock.unlock();
        if (!FillStreamRange(0, initialFillBytes_)) throw 1;
        lock.lock();
        { const auto buffer = CaptureBuffer(); static_cast<void>(buffer->Get()->SetVolume(volumeDb_)); }
    }

    bool FillStreamRange(const DWORD offset, const DWORD bytes)
    {
        std::scoped_lock lock(mutex_);
        void* first = nullptr;
        void* second = nullptr;
        DWORD firstBytes = 0, secondBytes = 0;
        {
            const auto buffer = CaptureBuffer();
            if (buffer->Lock(offset, bytes, &first, &firstBytes, &second, &secondBytes, 0) != DS_OK)
                return false;
        }
        std::uint32_t firstRead = 0, secondRead = 0;
        const bool firstOk = DecodeStreamBlockLocked(
            std::span<std::byte>(static_cast<std::byte*>(first), firstBytes), firstRead);
        const bool secondOk = firstOk && (secondBytes == 0 || DecodeStreamBlockLocked(
            std::span<std::byte>(static_cast<std::byte*>(second), secondBytes), secondRead));
        HRESULT result;
        {
            const auto buffer = CaptureBuffer();
            result = buffer->Get()->Unlock(first, firstBytes, second, secondBytes);
        }
        if (!firstOk || !secondOk || result != DS_OK) return false;
        polling_.CommitRead(bytes, firstRead + secondRead);
        return true;
    }

    bool DecodeStreamBlockLocked(std::span<std::byte> destination, std::uint32_t& actual)
    {
        AudioReadResult read;
        bool ordinaryWaveFailure = false;
        {
            const auto source = CaptureInput();
            read = source->ReadPcm(destination);
            // 6470普通ReadFile失败返回0；D9C0将它当短读，不得跳过静音/second/Commit。
            ordinaryWaveFailure = !read && read.error == AudioError::ReadFailed && read.bytesRead == 0 &&
                read.state != AudioDecodeState::Failed && source->Kind() == AudioInputKind::WavePcm;
        }
        if ((!read && !ordinaryWaveFailure) || read.bytesRead > destination.size()) return false;
        if (ordinaryWaveFailure)
            Logging::Write(Logging::Level::Warning,
                "Polling Wave ordinary read failure: zero-byte return, silence fill and continuation.");
        actual = static_cast<std::uint32_t>(read.bytesRead);
        if (read.bytesRead < destination.size())
        {
            const auto source = CaptureInput();
            const auto* format = source->FormatPointer();
            if (!format)
            {
                Logging::Write(Logging::Level::Error, "Polling short read has no format: rejected.");
                return false;
            }
            std::memset(destination.data() + read.bytesRead, format->wBitsPerSample == 8 ? 0x80 : 0,
                destination.size() - read.bytesRead);
        }
        return true;
    }

    void ClearPolling()
    {
        // E8C0仅当前IsPlay为真才Stop；暂停不构成条件，不能统一清零字段。
        if (IsPlaying()) Stop();
        std::scoped_lock lock(mutex_);
        PublishBuffer(MakeNullSoundBuffer());
        PublishInput(MakeNullSoundInput());
        fileName_.clear();
    }

    mutable std::mutex mutex_;
    mutable std::mutex bufferReferenceMutex_;
    const bool streaming_ = false;
    std::atomic<std::uint8_t> loop_{0};
    bool paused_ = false;
    int volumeDb_ = 0;
    DWORD bufferBytes_ = 0;
    DWORD initialFillBytes_ = 0;
    SoundPollingCursor polling_;
    std::mutex taskMutex_;
    std::condition_variable taskCondition_;
    TaskState taskState_ = TaskState::Idle;
    bool taskStopRequested_ = true;
    bool taskRetiring_ = false;
    std::uint32_t activeRuns_ = 0;
    std::uint64_t taskGeneration_ = 0;
    std::shared_ptr<SoundBufferRuntime> buffer_;
    std::shared_ptr<IAudioDecoder> decoder_;
    StaticSoundRuntime staticSound_;
    std::wstring fileName_;
};

namespace
{
struct StreamingTask
{
    std::shared_ptr<SoundObject> sound;
    std::uint64_t generation = 0;
};

struct Runtime
{
    ~Runtime();
    SoundColdMutex soundGate, parameterGate, deviceEnumGate;
    std::atomic<SoundSystemRoot*> soundRoot{nullptr};
    std::atomic<SoundParameterRoot*> parameters{nullptr};
    std::atomic<SoundDefaultDeviceRoot*> defaultDeviceRoot{nullptr};
    std::mutex taskQueueMutex;
    std::condition_variable taskQueueCondition;
    std::deque<StreamingTask> taskQueue;
    std::vector<std::thread> workers;
    bool stopWorkers = true;
};

Runtime& GetRuntime()
{
    static Runtime runtime;
    return runtime;
}

SoundSystemRoot& GetSoundRoot()
{
    auto& runtime = GetRuntime();
    auto* root = runtime.soundRoot.load(std::memory_order_acquire);
    if (!root)
    {
        SoundColdLock lock(runtime.soundGate);
        root = runtime.soundRoot.load(std::memory_order_relaxed);
        if (!root)
        {
            // 1D28/1D5F：先实际根20，再哨兵16，构造完整后发布。
            root = new SoundSystemRoot;
            runtime.soundRoot.store(root, std::memory_order_release);
        }
    }
    return *root;
}

SoundParameterRoot& GetParameters()
{
    auto& runtime = GetRuntime();
    auto* parameters = runtime.parameters.load(std::memory_order_acquire);
    if (!parameters)
    {
        SoundColdLock lock(runtime.parameterGate);
        parameters = runtime.parameters.load(std::memory_order_relaxed);
        if (!parameters)
        {
            // 不复制原早发布未构造对象/分配失败遗留锁的危险域。
            parameters = new SoundParameterRoot;
            runtime.parameters.store(parameters, std::memory_order_release);
        }
    }
    return *parameters;
}

class RuntimeStaticServices final : public IStaticSoundServices
{
public:
    bool GlobalFocus() override { return KaGuYa::Sound::IsGlobalFocus(); }
    bool PanControl() override { return KaGuYa::Sound::IsPanControl(); }
    bool FrequencyControl() override { return KaGuYa::Sound::IsFrequencyControl(); }
    bool TruePlayPosition() override { return KaGuYa::Sound::IsTruePlayPosition(); }
    IDirectSound8* Device() override
    {
        return GetSoundRoot().device;
    }
};
IStaticSoundServices& StaticServices()
{
    static RuntimeStaticServices services;
    return services;
}

SoundObject* Resolve(const std::uintptr_t handle)
{
    auto& root = GetSoundRoot();
    const auto* found = root.sounds.Find(handle);
    return found == root.sounds.End() ? nullptr : found->sound.get();
}

void WorkerLoop() noexcept
{
    auto& runtime = GetRuntime();
    while (true)
    {
        StreamingTask task;
        bool running = false;
        {
            std::unique_lock lock(runtime.taskQueueMutex);
            runtime.taskQueueCondition.wait(lock, [&runtime]
            {
                return runtime.stopWorkers || !runtime.taskQueue.empty();
            });
            if (runtime.stopWorkers)
            {
                return;
            }
            task = std::move(runtime.taskQueue.front());
            runtime.taskQueue.pop_front();
            // 出队到标记Running之间不能让退休侧漏掉一份已取走的引用。
            running = task.sound != nullptr && task.sound->BeginStreamingTask(task.generation);
            if (!running) task.sound.reset();
        }

        if (running)
        {
            // F870仅捕获Run本体；异常不补Stop/Clear，但必须完成统一的AfterRun。
            try { task.sound->RunStreamingTask(task.generation); }
            catch (...) { Logging::Write(Logging::Level::Error, "Polling Run ended with a C++ exception."); }
            auto* const sound = task.sound.get();
            // 注册节点在Stop等待完成前仍保有owner；先归还worker引用，最后通知。
            task.sound.reset();
            sound->FinishStreamingTask(task.generation);
        }
    }
}

void QueueStreamingTask(SoundObject* sound)
{
    auto& runtime = GetRuntime();
    const std::uint64_t generation = sound->PrepareStreamingTask();
    std::shared_ptr<SoundObject> owner;
    {
        auto& root = GetSoundRoot();
        const auto* node = root.sounds.Find(reinterpret_cast<std::uintptr_t>(sound));
        if (node == root.sounds.End())
            throw std::runtime_error("Streaming sound is no longer registered");
        owner = node->sound;
    }
    {
        std::scoped_lock lock(runtime.taskQueueMutex);
        // 1001005D：零线程池拒绝入队；首次Reset成功后可正常到达这条分支。
        if (runtime.workers.empty())
        {
            Logging::Write(Logging::Level::Error, "Sound pool enqueue: thread list is empty.");
            throw 1;
        }
        runtime.taskQueue.push_back(StreamingTask{std::move(owner), generation});
    }
    runtime.taskQueueCondition.notify_all();
}

void RetireSoundNode(Runtime& runtime, SoundRegistry::Node* node) noexcept
{
    if (node->sound->IsStreaming())
    {
        node->sound->RetireStreamingTasks();
        std::scoped_lock lock(runtime.taskQueueMutex);
        std::erase_if(runtime.taskQueue, [node](const StreamingTask& task)
        {
            return task.sound.get() == node->sound.get();
        });
    }
    SoundRegistry::Destroy(node);
}

void ClearSoundObjects(Runtime& runtime, SoundSystemRoot& root) noexcept
{
    auto* node = root.sounds.DetachAll();
    // 1000F000先公开为空，再按旧next顺序逐节点退owner/释放节点。
    while (node != root.sounds.End())
    {
        auto* const next = node->next;
        RetireSoundNode(runtime, node);
        node = next;
    }
}

void StopWorkerThreads(Runtime& runtime) noexcept
{
    // F296..F29C：零线程直接返回，不写shutdown、不锁池、不广播或清队。
    if (runtime.workers.empty()) return;
    {
        std::scoped_lock lock(runtime.taskQueueMutex);
        runtime.stopWorkers = true;
    }
    runtime.taskQueueCondition.notify_all();
    for (auto& worker : runtime.workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    runtime.workers.clear();
    std::scoped_lock lock(runtime.taskQueueMutex);
    runtime.taskQueue.clear();
}

void ReleaseDeviceObjects(SoundSystemRoot& root) noexcept
{
    // FC20：Release返回后才清槽；primary在device之前。
    if (root.primary) { root.primary->Release(); root.primary = nullptr; }
    if (root.device) { root.device->Release(); root.device = nullptr; }
}

Runtime::~Runtime()
{
    auto* root = soundRoot.load(std::memory_order_relaxed);
    if (root) { ClearSoundObjects(*this, *root); ReleaseDeviceObjects(*root); }
    StopWorkerThreads(*this);
    delete root;
    delete parameters.load(std::memory_order_relaxed);
    // 原57A20在模块退出时独立加锁并退枚举根；ExitSystem/Reset不触碰它。
    SoundColdLock lock(deviceEnumGate);
    delete defaultDeviceRoot.load(std::memory_order_relaxed);
    defaultDeviceRoot.store(nullptr, std::memory_order_release);
}

void StartWorkers(Runtime& runtime)
{
    {
        std::scoped_lock lock(runtime.taskQueueMutex);
        runtime.stopWorkers = false;
    }
    for (std::size_t index = 0; index < WorkerCount; ++index)
    {
        (void)index;
        // 1000FA00 每次成功 InitSystem 都向线程容器追加三个 worker。
        runtime.workers.emplace_back(WorkerLoop);
    }
}

bool CreateDeviceForRoot(SoundSystemRoot& root, HWND window)
{
    try
    {
        if (root.device)
        {
            Logging::Write(Logging::Level::Error, "DirectSoundCreate8: device already exists; clearing original root state.");
            throw 1;
        }
        const auto created = DirectSoundCreate8(nullptr, &root.device, nullptr);
        if (FAILED(created))
        {
            Logging::WriteFormat(Logging::Level::Error, "DirectSoundCreate8 failed: %08lX.",
                static_cast<unsigned long>(created));
            throw 1;
        }
        if (!root.device)
        {
            Logging::Write(Logging::Level::Error, "DirectSoundCreate8 succeeded without a device: rejected.");
            throw 1;
        }
        // FCAC..FCBB不提前拒绝空HWND，实际COM决定普通返回。
        const auto cooperative = root.device->SetCooperativeLevel(window, DSSCL_PRIORITY);
        if (FAILED(cooperative))
        {
            Logging::WriteFormat(Logging::Level::Error, "SetCooperativeLevel failed: %08lX.",
                static_cast<unsigned long>(cooperative));
            throw 1;
        }
        return true;
    }
    catch (...)
    {
        // FCE0的清理在原try之外，不能吞掉清理异常或恢复旧device。
    }
    ClearSoundObjects(GetRuntime(), root);
    ReleaseDeviceObjects(root);
    return false;
}

}

SoundSystemRoot::~SoundSystemRoot() noexcept
{
    ClearSoundObjects(GetRuntime(), *this);
    ReleaseDeviceObjects(*this);
    // 随后的SoundRegistry成员析构再次清空并释放哨兵，对应ECA0末段。
}

bool InitializeSystem(const HWND window)
{
    auto& root = GetSoundRoot();
    if (!CreateDeviceForRoot(root, window)) return false;
    // 1EA4在设备helper的catch之外；追加worker失败保留已提交设备/线程前缀。
    StartWorkers(GetRuntime());
    Logging::Write(Logging::Level::Info, "InitSystem: DirectSound8 initialized; appended three polling workers.");
    return true;
}

void ShutdownSystem()
{
    auto& root = GetSoundRoot();
    auto& runtime = GetRuntime();
    ClearSoundObjects(runtime, root);
    ReleaseDeviceObjects(root);
    StopWorkerThreads(runtime);
}

bool ResetSystem(const HWND window)
{
    auto& root = GetSoundRoot();
    ClearSoundObjects(GetRuntime(), root);
    ReleaseDeviceObjects(root);
    return CreateDeviceForRoot(root, window);
}

void RestoreActiveSounds(const bool active)
{
    if (!active)
    {
        return;
    }
    auto& root = GetSoundRoot();
    // 10003230/1000F1F0：借当前节点对象；调用之后才读取next，不做快照/额外增持。
    for (auto* node = root.sounds.Begin(); node != root.sounds.End(); node = node->next)
        node->sound->Restore();
}

const GUID* GetDefaultPlaybackDevice()
{
    auto& runtime = GetRuntime();
    auto* root = runtime.defaultDeviceRoot.load(std::memory_order_acquire);
    if (root) return root->Query();
    {
        SoundColdLock lock(runtime.deviceEnumGate);
        root = runtime.defaultDeviceRoot.load(std::memory_order_relaxed);
        if (!root)
        {
            root = new SoundDefaultDeviceRoot;
            runtime.defaultDeviceRoot.store(root, std::memory_order_release);
        }
    }
    // 原34E3仅冷路径激活terminate展开资格；不能把冷构造或热查询也标成noexcept。
    return [](SoundDefaultDeviceRoot* value) noexcept { return value->Query(); }(root);
}

namespace
{
WORD PrimaryChannels(const wchar_t* value) noexcept
{
    if (std::wcscmp(value, L"stereo") == 0) return 2;
    if (std::wcscmp(value, L"mono") == 0) return 1;
    return 0;
}
DWORD PrimaryRate(const wchar_t* value) noexcept
{
    if (std::wcscmp(value, L"11.025kHz") == 0) return 11025;
    if (std::wcscmp(value, L"22.05kHz") == 0) return 22050;
    if (std::wcscmp(value, L"44.1kHz") == 0) return 44100;
    return 0;
}
WORD PrimaryBits(const wchar_t* value) noexcept
{
    if (std::wcscmp(value, L"8bit") == 0) return 8;
    if (std::wcscmp(value, L"16bit") == 0) return 16;
    return 0;
}

bool ApplyPrimaryFormat(WORD channelCount, DWORD samplesPerSecond, WORD bitCount)
{
    // 原234E只有三字段全部有效才取声音根；无效请求不冷建根/哨兵。
    auto& root = GetSoundRoot();
    // 1000F110的catch覆盖建primary、SetFormat与逐声源恢复；3150没有这个catch。
    try
    {
        if (root.device == nullptr)
        {
            return false;
        }
        if (root.primary == nullptr)
        {
            DSBUFFERDESC description{};
            description.dwSize = sizeof(description);
            description.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
            // 1000F162把this+8本身交给COM；失败返回也不撤销已经写入的槽。
            if (FAILED(root.device->CreateSoundBuffer(&description, &root.primary, nullptr)))
            {
                return false;
            }
        }
        WAVEFORMATEX format{};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = channelCount;
        format.nSamplesPerSec = samplesPerSecond;
        format.wBitsPerSample = bitCount;
        format.nBlockAlign = static_cast<WORD>(channelCount * bitCount / 8u);
        format.nAvgBytesPerSec = samplesPerSecond * format.nBlockAlign;
        if (!root.primary)
        {
            Logging::Write(Logging::Level::Error, "Primary Create succeeded without a buffer: rejected.");
            return false;
        }
        if (FAILED(root.primary->SetFormat(&format)))
        {
            return false;
        }
        // 1000F1E6..1000F1F8：格式设置成功后依次恢复所有已登记声音对象。
        RestoreActiveSounds(true);
        return true;
    }
    catch (...)
    {
        Logging::Write(Logging::Level::Error, "SetPrimaryBufferFormat: original local catch after format/restore failure.");
        return false;
    }
}
}

bool SetPrimaryFormat(const wchar_t* const channels, const wchar_t* const sampleRate,
    const wchar_t* const bitsPerSample)
{
    if (!channels || !sampleRate || !bitsPerSample) return false;
    const WORD channelCount = PrimaryChannels(channels);
    if (!channelCount) return false;
    const DWORD samplesPerSecond = PrimaryRate(sampleRate);
    if (!samplesPerSecond) return false;
    const WORD bitCount = PrimaryBits(bitsPerSample);
    return bitCount ? ApplyPrimaryFormat(channelCount, samplesPerSecond, bitCount) : false;
}

bool SetPrimaryFormatAnsi(const char* channels, const char* sampleRate, const char* bitsPerSample)
{
    WORD channelCount;
    DWORD samplesPerSecond;
    WORD bitCount;
    // 原03 100020D0：逐字段构造/验证/销毁。前项无效时不读取后项，也不冷建声音根。
    {
        SoundAnsiText value(channels);
        channelCount = PrimaryChannels(value.GetString());
        if (!channelCount) return false;
    }
    {
        SoundAnsiText value(sampleRate);
        samplesPerSecond = PrimaryRate(value.GetString());
        if (!samplesPerSecond) return false;
    }
    {
        SoundAnsiText value(bitsPerSample);
        bitCount = PrimaryBits(value.GetString());
        if (!bitCount) return false;
    }
    return ApplyPrimaryFormat(channelCount, samplesPerSecond, bitCount);
}

void SetPrimaryVolumeDb(const int volume)
{
    auto& root = GetSoundRoot();
    if (root.primary != nullptr)
    {
        root.primary->SetVolume(volume);
    }
}

int GetPrimaryVolumeDb()
{
    auto& root = GetSoundRoot();
    LONG volume = DSBVOLUME_MIN;
    if (root.primary == nullptr || root.primary->GetVolume(&volume) != DS_OK)
    {
        return DSBVOLUME_MIN;
    }
    return volume;
}

int SetMinimumVolume(const int volume)
{
    auto& parameters = GetParameters();
    if (volume < -10000)
    {
        parameters.minimumVolume.store(-10000, std::memory_order_relaxed);
        return volume;
    }
    const int bounded = volume > 0 ? 0 : volume;
    parameters.minimumVolume.store(bounded, std::memory_order_relaxed);
    return bounded;
}
int MinimumVolume() { return GetParameters().minimumVolume.load(std::memory_order_relaxed); }
int SetGlobalFocus(const std::uint8_t value) { return static_cast<int>(GetParameters().SetFocus(value)); }
int SetPanControl(const std::uint8_t value) { return static_cast<int>(GetParameters().SetPan(value)); }
int SetFrequencyControl(const std::uint8_t value) { return static_cast<int>(GetParameters().SetFrequency(value)); }
int SetTruePlayPosition(const std::uint8_t value) { return static_cast<int>(GetParameters().SetPosition(value)); }
bool IsGlobalFocus() { return GetParameters().Focus() != 0; }
bool IsPanControl() { return GetParameters().Pan() != 0; }
bool IsFrequencyControl() { return GetParameters().Frequency() != 0; }
bool IsTruePlayPosition() { return GetParameters().Position() != 0; }

int ValueToVolume(const int maximum, const int value)
{
    // 4373先取参数根；特殊值也必须经过冷根创建，热路径不取全局锁。
    auto& parameters = GetParameters();
    if (value == 0) return -10000;
    if (value == maximum) return 0;
    const int minimum = parameters.minimumVolume.load(std::memory_order_relaxed);
    const auto quotient = static_cast<std::uint32_t>(DivideVolumeAt10004436(minimum, maximum));
    const auto difference = static_cast<std::uint32_t>(maximum) - static_cast<std::uint32_t>(value);
    return std::bit_cast<std::int32_t>(quotient * difference);
}

namespace
{
int ConvertVolumeWithParameters(SoundParameterRoot& parameters, const int maximum, const int volume)
{
    if (volume == -10000) return 0;
    if (volume == 0) return maximum;
    const int divisor = DivideVolumeAt10004436(parameters.minimumVolume.load(std::memory_order_relaxed), maximum);
    return DivideVolumeAt10004436(volume, divisor);
}
}
int VolumeToValue(const int maximum, const int volume)
{
    // 4473在-10000/0分支之前取根；两次除法仍保持原硬件IDIV异常。
    return ConvertVolumeWithParameters(GetParameters(), maximum, volume);
}

std::uintptr_t AddSoundObject(const std::uint8_t mode)
{
    if (mode > 1) return 0;
    auto& root = GetSoundRoot();
    try
    {
        if (!root.device) throw 1;
        auto sound = std::make_shared<SoundObject>(mode == 1);
        const auto handle = reinterpret_cast<std::uintptr_t>(sound.get());
        root.sounds.Append(sound);
        return handle;
    }
    catch (...) { return 0; }
}

std::uintptr_t RemoveSoundObject(const std::uintptr_t handle)
{
    auto& root = GetSoundRoot();
    if (root.sounds.Empty()) return reinterpret_cast<std::uintptr_t>(root.sounds.End());
    auto* node = root.sounds.Find(handle);
    if (node == root.sounds.End()) return handle;
    root.sounds.Unlink(node);
    RetireSoundNode(GetRuntime(), node);
    return 1;
}

bool OpenSound(const std::uintptr_t handle, const wchar_t* const logicalName,
    const wchar_t* const physicalName, const std::uint32_t offset, const std::uint32_t size)
{
    const auto sound = Resolve(handle);
    if (!sound) return false;
    // 原3674/3684在对象Open/Clear之前持有两份名称；允许传入自身GetFileName的借用指针。
    AudioOpenRequest request{};
    if (logicalName) request.logicalPath = logicalName;
    if (physicalName) request.physicalPath = physicalName;
    request.offset = offset;
    request.size = size;
    return sound->Open(request);
}

bool OpenSoundAnsi(std::uintptr_t handle, const char* logicalName, const char* physicalName,
    std::uint32_t offset, std::uint32_t size)
{
    auto* const found = Resolve(handle);
    if (!found) return false;
    // 原03 1000337C..100033B8：登记成功后依次转两份名字，覆盖整个子Open调用。
    const SoundAnsiOpenNames names(logicalName, physicalName);
    AudioOpenRequest request{};
    request.logicalPath = names.LogicalName();
    if (names.PhysicalName()) request.physicalPath = names.PhysicalName();
    request.physicalPathSpecified = names.PhysicalName() != nullptr;
    request.offset = offset;
    request.size = size;
    return found->Open(request);
}

std::uintptr_t CloseSound(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    if (sound == nullptr)
    {
        return handle;
    }
    return sound->Close();
}

std::uintptr_t PlaySound(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    if (sound == nullptr)
    {
        return handle;
    }
    return sound->Play();
}

std::uintptr_t PauseSound(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    return sound == nullptr ? handle : sound->Pause();
}

std::uintptr_t StopSound(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    if (sound == nullptr)
    {
        return handle;
    }
    return sound->Stop();
}

bool IsSoundPlaying(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    return sound != nullptr && sound->IsPlaying();
}

std::uintptr_t SetSoundLoop(const std::uintptr_t handle, const std::uint8_t loop)
{
    const auto sound = Resolve(handle);
    return sound == nullptr ? handle : sound->SetLoop(loop);
}

bool IsSoundLoop(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    return sound != nullptr && sound->IsLoop();
}

std::uintptr_t SetSoundVolume(const std::uintptr_t handle, const int maximum, const int value)
{
    auto& root = GetSoundRoot();
    const auto* node = root.sounds.Find(handle);
    if (node == root.sounds.End())
        return reinterpret_cast<std::uintptr_t>(root.sounds.End());
    auto* const sound = node->sound.get();
    const int volume = ValueToVolume(maximum, value);
    return sound->SetVolume(volume);
}

int GetSoundVolumeValue(const std::uintptr_t handle, const int maximum)
{
    auto* const sound = Resolve(handle);
    if (!sound) return 0; // 4141未命中，不创建参数根。
    auto& parameters = GetParameters(); // 4154在GetVolume虚调用之前；minimum字段在其后读。
    const int volume = sound->GetVolume();
    return ConvertVolumeWithParameters(parameters, maximum, volume);
}

const wchar_t* GetSoundFileName(const std::uintptr_t handle)
{
    const auto sound = Resolve(handle);
    return sound == nullptr ? nullptr : sound->GetFileName();
}

const char* GetSoundFileNameAnsi(std::uintptr_t handle)
{
    if (!GetSoundFileName(handle)) return nullptr;
    // 原03 100041BB..100041D4把已退役的栈/堆转换缓冲返回给调用者。
    // 无有效寿命可复刻：明确拒绝，不返回悬空指针，也不偷偷增加持久缓存合同。
    Logging::Write(Logging::Level::Error, "ANSI SoundGetFileName: original returns retired storage; safely rejected.");
    SetLastError(ERROR_NOT_SUPPORTED);
    return nullptr;
}
}
