#pragma once

#include <Platform/NativeTimerRuntime.h>

namespace KaGuYa::Start::Platform
{
class ITimerPlatformApi
{
public:
    virtual ~ITimerPlatformApi() = default;
    virtual MMRESULT ReadCapabilities(TIMECAPS& capabilities) = 0;
    virtual void BeginPeriod(UINT period) = 0;
    virtual void EndPeriod(UINT period) = 0;
    [[nodiscard]] virtual DWORD MultimediaTick() const = 0;
    [[nodiscard]] virtual DWORD SystemTick() const = 0;
    virtual void SleepMilliseconds(DWORD milliseconds) = 0;
};

class Win32TimerPlatformApi final : public ITimerPlatformApi
{
public:
    MMRESULT ReadCapabilities(TIMECAPS& capabilities) override;
    void BeginPeriod(UINT period) override;
    void EndPeriod(UINT period) override;
    [[nodiscard]] DWORD MultimediaTick() const override;
    [[nodiscard]] DWORD SystemTick() const override;
    void SleepMilliseconds(DWORD milliseconds) override;
};

// 正文外宿主：所有Timer借同一creator，不拥有每实例的startTick。
// 原14h时钟在第一个CTimer构造时建立、最后一个CTimer析构时释放。
class ProcessTimerSource final
{
public:
    // 显式独立宿主/测试scope；不是正式进程singleton的替代布局。
    explicit ProcessTimerSource(ITimerPlatformApi& api) noexcept;
    // 正式composition绑定ProcessNativeTimerCreator；该creator及API/内存须活过所有借用。
    ProcessTimerSource(ITimerPlatformApi& api, NativeTimerCreatorBody0Ch& creator,
        Runtime::IEnvironmentRootMemory& memory) noexcept;
    ~ProcessTimerSource() noexcept;
    ProcessTimerSource(const ProcessTimerSource&) = delete;
    ProcessTimerSource& operator=(const ProcessTimerSource&) = delete;

    [[nodiscard]] NativeTimerDependencies& Dependencies() noexcept { return dependencies_; }
    [[nodiscard]] const NativeTimerCreatorBody0Ch& Creator() const noexcept { return dependencies_.creator; }
    [[nodiscard]] bool MultimediaPeriodEnabled() const noexcept;
    [[nodiscard]] UINT Period() const;

private:
    NativeTimerCreatorBody0Ch localCreator_;
    NativeTimerDependencies dependencies_;
    bool ownsCreator_;
};

// 现代生命周期外壳，唯一计时状态是原140h；不能将外壳自身当作原根+6C正文。
class FrameTimer final
{
public:
    FrameTimer(ProcessTimerSource& timer, ITimerPlatformApi& api);
    ~FrameTimer() noexcept;
    FrameTimer(const FrameTimer&) = delete;
    FrameTimer& operator=(const FrameTimer&) = delete;

    void SetFramesPerSecond(std::uint32_t framesPerSecond);
    void ResetTimingAnchor();
    void ClearAccumulatedProcessingTime();
    void WaitFrame();
    // 4642A9在CParams之后、Font之前显式销毁；外壳随后析构不能再次减count。
    void DestroyAt43F820() noexcept;

    [[nodiscard]] NativeFpsTimerBody140h& NativeBody() noexcept { return body_; }
    [[nodiscard]] const NativeFpsTimerBody140h& NativeBody() const noexcept { return body_; }
    [[nodiscard]] NativeTimerDependencies& Dependencies() noexcept { return dependencies_; }
    [[nodiscard]] std::uint32_t FramesPerSecond() const noexcept { return body_.framesPerSecondAt04; }
    [[nodiscard]] std::uint32_t FixedPeriod() const noexcept { return body_.fixedPeriodAt08; }
    [[nodiscard]] std::uint32_t FractionAccumulator() const noexcept { return body_.fractionAt10; }
    [[nodiscard]] DWORD ScheduledBase() const noexcept { return body_.scheduledBaseAt0C; }
    [[nodiscard]] std::uint32_t FrameCount() const noexcept { return body_.frameCountAt120; }
    [[nodiscard]] std::uint32_t HistoryIndex() const noexcept { return body_.historyIndexAt11C; }
    [[nodiscard]] bool SevereLate() const noexcept { return body_.severeLateAt124 != 0; }
    [[nodiscard]] std::uint32_t PreviousLateCount() const noexcept { return body_.previousLateCountAt128; }
    [[nodiscard]] std::uint32_t CurrentLateCount() const noexcept { return body_.currentLateCountAt12C; }
    [[nodiscard]] std::uint32_t AccumulatedProcessingTime() const noexcept { return body_.accumulatedAt14; }
    [[nodiscard]] DWORD Timestamp(std::size_t index) const noexcept { return body_.timestampsAt1C[index]; }
    [[nodiscard]] DWORD ProcessingTime(std::size_t index) const noexcept { return body_.processingTimesAt9C[index]; }

private:
    NativeTimerDependencies& dependencies_;
    NativeFpsTimerBody140h body_;
    bool constructed_ = false;
};
}
