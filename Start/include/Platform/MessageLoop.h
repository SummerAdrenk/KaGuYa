#pragma once

#include <Windows.h>

#include <vector>

namespace KaGuYa::Start::Platform
{
class IMessageFilter
{
public:
    virtual ~IMessageFilter() = default;
    virtual bool PreTranslateMessage(MSG* message) = 0;
};

class IIdleHandler
{
public:
    virtual ~IIdleHandler() = default;
    virtual void OnIdle() = 0;
};

class IExitState
{
public:
    virtual ~IExitState() = default;
    [[nodiscard]] virtual bool ExitRequested() const = 0;
};

class MessageLoop final
{
public:
    void AddMessageFilter(IMessageFilter* filter);
    void AddIdleHandler(IIdleHandler* handler);
    void RemoveMessageFilter(IMessageFilter* filter);
    void RemoveIdleHandler(IIdleHandler* handler);

    [[nodiscard]] bool PreTranslateMessage(MSG* message) const;
    void OnIdle() const;
    int Run(const IExitState& exitState);

private:
    std::vector<IMessageFilter*> messageFilters_;
    std::vector<IIdleHandler*> idleHandlers_;
    MSG message_{};
};
}
