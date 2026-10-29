#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootCriticalSectionNative.h>

namespace KaGuYa::Start::Runtime
{
void RootRuntimeOwner::Enter()
{
    if (borrowedLock_) { borrowedLock_->Enter(); return; }
    ::EnterCriticalSection(&lock_);
    const DWORD thread = ::GetCurrentThreadId(); // 原40769D在depth递增之前取线程。
    ++lockDepth_;
    lockThread_.store(thread, std::memory_order_relaxed);
}
void RootRuntimeOwner::Leave() noexcept
{
    if (borrowedLock_) { borrowedLock_->Leave(); return; }
    if (--lockDepth_ == 0) lockThread_.store(0xFFFFFFFFu, std::memory_order_relaxed);
    ::LeaveCriticalSection(&lock_);
}
RootRuntimeOwner::CriticalLock::CriticalLock(RootRuntimeOwner& owner) : owner_(owner) { owner_.Enter(); }
RootRuntimeOwner::CriticalLock::~CriticalLock() noexcept { owner_.Leave(); }
RootRuntimeOwner::CriticalLock RootRuntimeOwner::AcquireCriticalLock() { return CriticalLock(*this); }
bool RootRuntimeOwner::LockHeldByCurrentThreadAt4076D0() const noexcept
{
    if (borrowedLock_) return borrowedLock_->HeldByCurrentThreadAt4076D0();
    return lockThread_.load(std::memory_order_relaxed) == ::GetCurrentThreadId();
}
}
