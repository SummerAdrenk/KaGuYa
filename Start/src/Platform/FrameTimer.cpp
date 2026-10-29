#include <Platform/FrameTimer.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Platform
{
MMRESULT Win32TimerPlatformApi::ReadCapabilities(TIMECAPS& value)
{ return timeGetDevCaps(&value, sizeof(value)); }
void Win32TimerPlatformApi::BeginPeriod(UINT period) { timeBeginPeriod(period); }
void Win32TimerPlatformApi::EndPeriod(UINT period) { timeEndPeriod(period); }
DWORD Win32TimerPlatformApi::MultimediaTick() const { return timeGetTime(); }
DWORD Win32TimerPlatformApi::SystemTick() const { return GetTickCount(); }
void Win32TimerPlatformApi::SleepMilliseconds(DWORD milliseconds) { Sleep(milliseconds); }

ProcessTimerSource::ProcessTimerSource(ITimerPlatformApi& api) noexcept
    : localCreator_(MakeNativeTimerCreator()),
      dependencies_{api, Runtime::DefaultEnvironmentRootMemory(), localCreator_}, ownsCreator_(true) {}

ProcessTimerSource::ProcessTimerSource(ITimerPlatformApi& api, NativeTimerCreatorBody0Ch& creator,
    Runtime::IEnvironmentRootMemory& memory) noexcept
    : localCreator_(MakeNativeTimerCreator()), dependencies_{api, memory, creator}, ownsCreator_(false) {}

ProcessTimerSource::~ProcessTimerSource() noexcept
{
    if (ownsCreator_)
    {
        NativeTimerScope scope(dependencies_);
        DeleteNativeTimerCreatorAt43FBF0(localCreator_, 0);
    }
}

bool ProcessTimerSource::MultimediaPeriodEnabled() const noexcept
{
    const auto* source = dependencies_.creator.sourceAt04;
    return source && source->periodEnabledAt04 != 0;
}

UINT ProcessTimerSource::Period() const
{
    const auto* source = dependencies_.creator.sourceAt04;
    if (!source) throw std::logic_error("timer source has not been constructed");
    return source->capabilitiesAt0C.wPeriodMin;
}

FrameTimer::FrameTimer(ProcessTimerSource& timer, ITimerPlatformApi& api)
    : dependencies_(timer.Dependencies())
{
    // 外置服务装配错误不能切换另一时间源；旧签名仅保留类型兼容。
    if (&api != &dependencies_.api) throw std::invalid_argument("FrameTimer API differs from shared creator API");
    NativeTimerScope scope(dependencies_);
    ConstructNativeFpsTimerAt43F780(body_);
    constructed_ = true;
}

FrameTimer::~FrameTimer() noexcept { DestroyAt43F820(); }
void FrameTimer::DestroyAt43F820() noexcept
{
    if (!constructed_) return;
    NativeTimerScope scope(dependencies_);
    DestroyNativeFpsTimerAt43F820(body_);
    constructed_ = false;
}
void FrameTimer::SetFramesPerSecond(std::uint32_t value)
{
    NativeTimerScope scope(dependencies_);
    body_.vptrAt00->setFramesPerSecondAt00(&body_, value);
}
void FrameTimer::ResetTimingAnchor()
{
    NativeTimerScope scope(dependencies_);
    body_.vptrAt00->resetTimingAnchorAt14(&body_);
}
void FrameTimer::ClearAccumulatedProcessingTime()
{
    NativeTimerScope scope(dependencies_);
    body_.vptrAt00->clearAccumulatedAt20(&body_);
}
void FrameTimer::WaitFrame()
{
    NativeTimerScope scope(dependencies_);
    body_.vptrAt00->waitAt18(&body_);
}
}
