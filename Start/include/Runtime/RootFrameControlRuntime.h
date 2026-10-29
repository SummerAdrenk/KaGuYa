#pragma once

#include <Platform/FrameTimer.h>
#include <Runtime/PreWindowInitialization.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 根的控制/计时子对象，不是完整413BE0 singleton；不拥有Graphics或输入的第二份实例。
class RootFrameControlRuntime final
{
public:
    RootFrameControlRuntime(Platform::ProcessTimerSource& source, Platform::ITimerPlatformApi& api);
    RootFrameControlRuntime(const RootFrameControlRuntime&) = delete;
    RootFrameControlRuntime& operator=(const RootFrameControlRuntime&) = delete;

    [[nodiscard]] PreWindowLifecycleState& Lifecycle() noexcept { return lifecycle_; }
    [[nodiscard]] const PreWindowLifecycleState& Lifecycle() const noexcept { return lifecycle_; }
    [[nodiscard]] Platform::FrameTimer& TimerAt6C() noexcept { return timer_; }
    [[nodiscard]] const Platform::FrameTimer& TimerAt6C() const noexcept { return timer_; }

    void BeginFrameAt483524() noexcept;
    void ConfigureTimerAt48AD8A();
    void InitializeWindowFlagsAt48ADEA() noexcept;
    [[nodiscard]] bool MinimizeByteAt1B2Published() const noexcept { return minimizePublished_; }
    [[nodiscard]] std::uint8_t ReadMinimizeByteAt1B2() const;
    // 4295EF/429704及42F883/42F913的独立byte写入同样发布此字段。
    void WriteMinimizeByteAt1B2(std::uint8_t value) noexcept;
    [[nodiscard]] std::uint32_t ReadValueAt214() const;
    [[nodiscard]] std::uint32_t& BorrowValueAt214();
    void WriteValueAt214(std::uint32_t value) noexcept { valueAt214_ = value; }
    [[nodiscard]] bool FrameWaitEnabledAt1B0() const;
    // 可在装配时借用稳定地址；接收者仍须遵守原启动/帧入口先写后读的前提。
    [[nodiscard]] std::uint8_t& FrameWaitByteAt1B0() noexcept { return lifecycle_.waitAt1B0; }
    [[nodiscard]] std::uint8_t& PopupByteAt1B1() noexcept { return lifecycle_.popupAt1B1; }
    // 旧固定引用入口不证明已发生写入；首次生产发布必须走上面的显式写入或48ADEA。
    [[nodiscard]] std::uint8_t& MinimizeByteAt1B2() noexcept { return lifecycle_.minimizeAt1B2; }
    [[nodiscard]] bool ExitRequestedAt1BC() const noexcept;
    void WaitFrameAt43FA60();

private:
    Platform::FrameTimer timer_;
    PreWindowLifecycleState lifecycle_{PreWindowLifecycleState::NativeConstructionAt413BE0Tag{}};
    // 有效位在原20字节控制存储之外；正文的帧byte不再另存一份。
    bool framePublished_ = false;
    bool minimizePublished_ = false;
    // 暂按原偏移命名；48ADF3明确写零，但不能据此猜测业务用途。
    std::optional<std::uint32_t> valueAt214_;
};
}
