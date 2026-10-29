#include <Platform/PendingWindowCreation.h>

namespace KaGuYa::Start::Platform
{
static_assert(sizeof(PendingWindowCreateRecord) == 12, "Pending window creation uses the x86 record layout.");

PendingWindowCreateList::PendingWindowCreateList()
{
    InitializeCriticalSection(&lock_);
}

PendingWindowCreateList::~PendingWindowCreateList()
{
    DeleteCriticalSection(&lock_);
}

void PendingWindowCreateList::Push(PendingWindowCreateRecord& record)
{
    EnterCriticalSection(&lock_);
    record.next = head_;
    head_ = &record;
    LeaveCriticalSection(&lock_);
}

void* PendingWindowCreateList::PopForCurrentThread()
{
    EnterCriticalSection(&lock_);
    void* object = nullptr;
    // 416B23..416B2A：仅非空链读取当前线程，首个匹配即可摘链。
    if (head_ != nullptr)
    {
        const DWORD thread = GetCurrentThreadId();
        auto** link = &head_;
        while (*link != nullptr && (*link)->threadId != thread)
            link = &(*link)->next;
        if (*link != nullptr)
        {
            auto* record = *link;
            *link = record->next;
            object = record->object;
        }
    }
    LeaveCriticalSection(&lock_);
    return object;
}

void PendingWindowCreateList::Remove(PendingWindowCreateRecord& record) noexcept
{
    EnterCriticalSection(&lock_);
    auto** link = &head_;
    while (*link != nullptr && *link != &record)
        link = &(*link)->next;
    if (*link != nullptr) *link = record.next;
    LeaveCriticalSection(&lock_);
}

PendingWindowCreateList& PendingWindowCreateRecords()
{
    static PendingWindowCreateList records;
    return records;
}
}
