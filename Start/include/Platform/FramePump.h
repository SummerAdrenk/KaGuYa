#pragma once

#include <Platform/MessageLoop.h>

namespace KaGuYa::Start::Platform
{
class IFrameRuntime
{
public:
    virtual ~IFrameRuntime() = default;
    [[nodiscard]] virtual bool TickFrame() = 0;
    [[nodiscard]] virtual bool ExitRequested() const = 0;
};

class FramePump final : public IIdleHandler
{
public:
    FramePump(HWND window, IFrameRuntime& runtime) noexcept;
    void OnIdle() override;

private:
    HWND window_ = nullptr;
    IFrameRuntime& runtime_;
};
}
