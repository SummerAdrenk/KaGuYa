#pragma once

#include <Windows.h>

namespace KaGuYa::Start::Platform
{
// 原ATL创建记录是窗口对象内的三个DWORD，不是CreateWindow调用栈上的临时节点。
struct PendingWindowCreateRecord final
{
    void* object = nullptr;
    DWORD threadId = 0;
    PendingWindowCreateRecord* next = nullptr;
};

class PendingWindowCreateList final
{
public:
    PendingWindowCreateList();
    ~PendingWindowCreateList();
    PendingWindowCreateList(const PendingWindowCreateList&) = delete;
    PendingWindowCreateList& operator=(const PendingWindowCreateList&) = delete;

    void Push(PendingWindowCreateRecord& record);
    [[nodiscard]] void* PopForCurrentThread();
    // 只在现代宿主销毁时移除未消费记录，防止失败创建留下悬空链；不是成功创建分支。
    void Remove(PendingWindowCreateRecord& record) noexcept;

private:
    CRITICAL_SECTION lock_{};
    PendingWindowCreateRecord* head_ = nullptr;
};

[[nodiscard]] PendingWindowCreateList& PendingWindowCreateRecords();
}
