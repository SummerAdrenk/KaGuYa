#pragma once

#include <Runtime/ScrVmYieldDispatcher.h>

#include <memory>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class IProgramAction
{
public:
    virtual ~IProgramAction() = default;
};

using ProgramActionHandle = std::shared_ptr<IProgramAction>;

class IProgramActionFactory
{
public:
    virtual ~IProgramActionFactory() = default;
    [[nodiscard]] virtual ProgramActionHandle Create(
        std::string_view cp932ActionName) = 0;
};

class IProgramActionQueue
{
public:
    virtual ~IProgramActionQueue() = default;
    virtual void Push(ProgramActionHandle action) = 0;
};

// 对应 0x4A9030：先由动作工厂按固定 CP932 名称创建对象，再提交到宿主 +0x10 队列。
class ProgramFixedActionRuntime final
{
public:
    ProgramFixedActionRuntime(
        IProgramActionFactory& factory,
        IProgramActionQueue& queue) noexcept;

    void Execute(ScrVmFixedAction action);
    [[nodiscard]] static std::string_view ActionName(ScrVmFixedAction action);

private:
    IProgramActionFactory* factory_ = nullptr;
    IProgramActionQueue* queue_ = nullptr;
};
}
