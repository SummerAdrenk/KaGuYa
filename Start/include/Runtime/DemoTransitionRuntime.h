#pragma once

#include <Runtime/DrawManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
// CDemoSeq 的 operation 可达字段。源是 CSurface 对象，其余图像槽是 DLL 句柄。
using DemoTransitionState = TransitOperation;

// 4C38A0保留Demo自身推进语义，绘制借用manager唯一的完整CTransit。
class DemoTransitionRuntime final
{
public:
    DemoTransitionRuntime(const DrawManagerRuntime& manager, IGraphicsScreenUpdateApi& screen) noexcept;
    [[nodiscard]] DemoTransitionState& State() noexcept;
    [[nodiscard]] const DemoTransitionState& State() const noexcept;
    [[nodiscard]] std::int32_t Step() const noexcept;
    void SetStep(std::int32_t value) noexcept;
    // 4C57F0 只重置 operation，不重置 CDemoSeq+4 的 step。
    void Reset() noexcept;
    [[nodiscard]] bool Advance();

private:
    void Render();
    const DrawManagerRuntime& manager_;
    IGraphicsScreenUpdateApi& screen_;
    DemoTransitionState state_;
    std::int32_t step_ = 0;
};
}
