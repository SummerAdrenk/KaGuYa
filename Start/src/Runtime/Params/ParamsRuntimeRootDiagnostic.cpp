#include <Runtime/ParamsRuntimeRootDiagnostic.h>

#include <Windows.h>
#include <atomic>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
SRWLOCK bindingLock = SRWLOCK_INIT;
ParamsRuntimeRootDiagnostic* bindings = nullptr;
std::atomic<std::uint64_t> nextEpoch{1};
}

ParamsRuntimeRootDiagnostic::ParamsRuntimeRootDiagnostic() noexcept
{
    ::AcquireSRWLockExclusive(&bindingLock);
    next_ = bindings;
    bindings = this;
    ::ReleaseSRWLockExclusive(&bindingLock);
}

ParamsRuntimeRootDiagnostic::~ParamsRuntimeRootDiagnostic() noexcept
{
    ::AcquireSRWLockExclusive(&bindingLock);
    auto** link = &bindings;
    while (*link != this) link = &(*link)->next_;
    *link = next_;
    ::ReleaseSRWLockExclusive(&bindingLock);
}

void ParamsRuntimeRootDiagnostic::Bind(ParamsRuntimeRoot& root) noexcept
{
    ::AcquireSRWLockExclusive(&bindingLock);
    for (auto* entry = bindings; entry; entry = entry->next_)
    {
        if (entry != this && entry->root_ == &root)
        {
            ::ReleaseSRWLockExclusive(&bindingLock);
            _invalid_parameter_noinfo_noreturn();
        }
    }
    if (root_ != &root)
    {
        root_ = &root;
        versionWritten = false;
        readCompleted = false;
        header = {};
        rawTripleCountMetadata = 0;
        InvalidateGameSystem();
    }
    ::ReleaseSRWLockExclusive(&bindingLock);
}

void ParamsRuntimeRootDiagnostic::Unbind() noexcept
{
    ::AcquireSRWLockExclusive(&bindingLock);
    root_ = nullptr;
    versionWritten = false;
    readCompleted = false;
    InvalidateGameSystem();
    ::ReleaseSRWLockExclusive(&bindingLock);
}

void ParamsRuntimeRootDiagnostic::BeginRead() noexcept
{
    versionWritten = true;
    readCompleted = false;
    header = {};
}

void ParamsRuntimeRootDiagnostic::InvalidateGameSystem() noexcept
{
    gameSystemPublicationEpoch = nextEpoch.fetch_add(1, std::memory_order_relaxed);
}

ParamsRuntimeRootDiagnostic* ParamsRuntimeRootDiagnostic::Find(const ParamsRuntimeRoot& root) noexcept
{
    ::AcquireSRWLockShared(&bindingLock);
    auto* entry = bindings;
    while (entry && entry->root_ != &root) entry = entry->next_;
    ::ReleaseSRWLockShared(&bindingLock);
    return entry;
}

void ParamsRuntimeRootDiagnostic::Forget(const ParamsRuntimeRoot& root) noexcept
{
    ::AcquireSRWLockExclusive(&bindingLock);
    for (auto* entry = bindings; entry; entry = entry->next_)
    {
        if (entry->root_ != &root) continue;
        entry->root_ = nullptr;
        entry->versionWritten = false;
        entry->readCompleted = false;
        entry->InvalidateGameSystem();
        break;
    }
    ::ReleaseSRWLockExclusive(&bindingLock);
}
}
