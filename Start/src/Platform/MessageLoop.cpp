#include <Platform/MessageLoop.h>

#include <algorithm>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Platform
{
void MessageLoop::AddMessageFilter(IMessageFilter* const filter)
{
    // Start.exe:423990 在扩容失败时跳过该项并继续 WM_CREATE，不向调用方报告失败。
    try
    {
        messageFilters_.push_back(filter);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const std::length_error&)
    {
    }
}

void MessageLoop::AddIdleHandler(IIdleHandler* const handler)
{
    try
    {
        idleHandlers_.push_back(handler);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const std::length_error&)
    {
    }
}

void MessageLoop::RemoveMessageFilter(IMessageFilter* const filter)
{
    const auto found = std::find(messageFilters_.begin(), messageFilters_.end(), filter);
    if (found != messageFilters_.end())
    {
        messageFilters_.erase(found);
    }
}

void MessageLoop::RemoveIdleHandler(IIdleHandler* const handler)
{
    const auto found = std::find(idleHandlers_.begin(), idleHandlers_.end(), handler);
    if (found != idleHandlers_.end())
    {
        idleHandlers_.erase(found);
    }
}

bool MessageLoop::PreTranslateMessage(MSG* const message) const
{
    // Start.exe:42D1D0 从末项向首项调用消息过滤器，首个非零结果终止分派。
    std::size_t index = messageFilters_.size();
    while (index != 0)
    {
        --index;
        IMessageFilter* const filter = messageFilters_[index];
        if (filter != nullptr && filter->PreTranslateMessage(message))
        {
            return true;
        }
    }
    return false;
}

void MessageLoop::OnIdle() const
{
    // Start.exe:424E60 按登记顺序调用全部非空 idle handler，并忽略其返回值。
    for (std::size_t index = 0; index < idleHandlers_.size(); ++index)
    {
        IIdleHandler* const handler = idleHandlers_[index];
        if (handler != nullptr)
        {
            handler->OnIdle();
        }
    }
}

int MessageLoop::Run(const IExitState& exitState)
{
    // Start.exe:43E980 使用 PM_NOREMOVE 探测消息；队列为空时持续推进 idle handler。
    for (;;)
    {
        if (PeekMessageA(&message_, nullptr, 0, 0, PM_NOREMOVE) != FALSE)
        {
            const int getMessageResult = GetMessageA(&message_, nullptr, 0, 0);
            if (getMessageResult == -1)
            {
                // 原实现回到 PeekMessage，而不是把 GetMessage 错误转换为退出码。
                continue;
            }
            if (getMessageResult == 0)
            {
                return static_cast<int>(message_.wParam);
            }
            if (!PreTranslateMessage(&message_))
            {
                TranslateMessage(&message_);
                DispatchMessageA(&message_);
            }
            continue;
        }

        if (exitState.ExitRequested())
        {
            return static_cast<int>(message_.wParam);
        }
        OnIdle();
    }
}
}
