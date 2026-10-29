#include <Runtime/RootFrameControlRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootFrameControlRuntime::RootFrameControlRuntime(
    Platform::ProcessTimerSource& source, Platform::ITimerPlatformApi& api)
    : timer_(source, api)
{
}

void RootFrameControlRuntime::BeginFrameAt483524() noexcept
{
    // 483524仅写低byte；不能把4778B0的WORD清零套到每帧入口。
    lifecycle_.BeginFrameAt483524();
    framePublished_ = true;
}

void RootFrameControlRuntime::ConfigureTimerAt48AD8A()
{
    timer_.SetFramesPerSecond(30);
}

void RootFrameControlRuntime::InitializeWindowFlagsAt48ADEA() noexcept
{
    // 原WORD 0100h从+1B1写入；不影响+1B0或+1B3。
    lifecycle_.InitializeWindowFlagsAt48ADEA();
    minimizePublished_ = true;
}

std::uint8_t RootFrameControlRuntime::ReadMinimizeByteAt1B2() const
{
    if (!minimizePublished_) throw std::logic_error("root+1B2 read before publication");
    return lifecycle_.minimizeAt1B2;
}

void RootFrameControlRuntime::WriteMinimizeByteAt1B2(std::uint8_t value) noexcept
{
    lifecycle_.minimizeAt1B2 = value;
    minimizePublished_ = true;
}

bool RootFrameControlRuntime::FrameWaitEnabledAt1B0() const
{
    if (!framePublished_) throw std::logic_error("root+1B0 read before first frame publication");
    return lifecycle_.waitAt1B0 != 0;
}

std::uint32_t RootFrameControlRuntime::ReadValueAt214() const
{
    if (!valueAt214_) throw std::logic_error("root+214 read before initialization");
    return *valueAt214_;
}
std::uint32_t& RootFrameControlRuntime::BorrowValueAt214()
{
    if (!valueAt214_) throw std::logic_error("root+214 borrow before initialization");
    return *valueAt214_;
}

bool RootFrameControlRuntime::ExitRequestedAt1BC() const noexcept
{
    return lifecycle_.valueAt1BC != 0;
}

void RootFrameControlRuntime::WaitFrameAt43FA60()
{
    // 普通帧与5071D0各自在调用前读门控；这里不得重置FPS/锚点或再执行一帧。
    timer_.WaitFrame();
}
}
