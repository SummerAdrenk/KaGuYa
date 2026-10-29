#include <Runtime/DemoTransitionRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add(const std::int32_t first, const std::int32_t second) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second));
}
}

DemoTransitionRuntime::DemoTransitionRuntime(const DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen) noexcept
    : manager_(manager), screen_(screen)
{
}

DemoTransitionState& DemoTransitionRuntime::State() noexcept { return state_; }
const DemoTransitionState& DemoTransitionRuntime::State() const noexcept { return state_; }
std::int32_t DemoTransitionRuntime::Step() const noexcept { return step_; }
void DemoTransitionRuntime::SetStep(const std::int32_t value) noexcept { step_ = value; }
void DemoTransitionRuntime::Reset() noexcept { state_ = {}; }

bool DemoTransitionRuntime::Advance()
{
    // 4C38A0：先保存完成判定，再绘制、推进、登记失效区域；即使完成仍绘制这一帧。
    // 4C38AB在Render之前选定两条代码路径，回调改opcode不能改变本帧加减方向。
    const bool universal = state_.opcode == 1;
    const bool completed = universal ? state_.current < state_.target
        : state_.opcode == -1 || state_.current >= state_.target;
    Render();
    if (universal)
        state_.current = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(state_.current) - static_cast<std::uint32_t>(step_));
    else
    {
        state_.current = Add(state_.current, step_);
        if (state_.current >= state_.target)
            state_.current = state_.target;
    }
    // 4B02E0 只调用 InvalidatedRect，不是 RenderSceen/Present。
    screen_.InvalidatedRectIfLoaded(nullptr);
    return completed;
}

void DemoTransitionRuntime::Render()
{
    static_cast<void>(manager_.RenderTransitionAt4C39E0(state_));
}
}
