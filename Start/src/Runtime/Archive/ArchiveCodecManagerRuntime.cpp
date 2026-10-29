#include <Runtime/ArchiveCodecManagerRuntime.h>

#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
ArchiveCodecManagerRuntime::ArchiveCodecManagerRuntime() noexcept {}

ArchiveCodecManagerRuntime::~ArchiveCodecManagerRuntime() noexcept
{
    // 43DAE5..43DBCC：每槽payload返回后重取control；无control不清base/bias。
    ReleaseYtlReference(blockConvertAt24);
    ReleaseYtlReference(lzssAt18);
    ReleaseYtlReference(bmrAt0C);
    ReleaseYtlReference(huffmanAt00);
}

ArchiveCodecManagerOwner::ArchiveCodecManagerOwner(IEnvironmentRootLock& lock,
    IEnvironmentRootMemory& memory) noexcept : lock_(lock), memory_(memory) {}

ArchiveCodecManagerOwner::~ArchiveCodecManagerOwner() noexcept { ReleaseAt5AA640(); }

void ArchiveCodecManagerOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

ArchiveCodecManagerRuntime* ArchiveCodecManagerOwner::CaptureAt445910()
{
    lock_.Enter();
    struct LockUnwind final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~LockUnwind() noexcept { if (active) lock.Leave(); }
    } cleanup{lock_};
    if (!published_)
    {
        void* const storage = memory_.AllocateBody(sizeof(ArchiveCodecManagerRuntime));
        // 4459CE：不在分配回调后重验槽；创建结果包括null都覆盖回调的发布。
        published_ = storage ? ::new (storage) ArchiveCodecManagerRuntime : nullptr;
    }
    // 4459DD切state1；正常Leave失败直接终止，不再走state0解锁。
    cleanup.active = false;
    LeaveNoexcept();
    return published_;
}

void ArchiveCodecManagerOwner::ReleaseAt5AA640() noexcept
{
    // 5AA66D在Enter之前已有终止资格，没有异常解锁动作。
    lock_.Enter();
    if (auto* const body = published_)
    {
        std::destroy_at(body);
        memory_.DeleteBody(body, sizeof(ArchiveCodecManagerRuntime));
        // 5AA692：正常删除之后才清槽，覆盖析构/删除回调中的替换。
        published_ = nullptr;
    }
    lock_.Leave();
}
}
