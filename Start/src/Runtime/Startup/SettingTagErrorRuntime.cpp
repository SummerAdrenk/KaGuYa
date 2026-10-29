#include <Runtime/SettingTagErrorRuntime.h>

namespace KaGuYa::Start::Runtime
{
SettingTagErrorOwner::SettingTagErrorOwner(IEnvironmentRootLock& lock,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : lock_(lock), memory_(memory), strings_(strings)
{
}

SettingTagErrorOwner::~SettingTagErrorOwner() noexcept { ReleaseAt5AB440(); }

void SettingTagErrorOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

RuntimeCString* SettingTagErrorOwner::CaptureAt47BCF0()
{
    // 47BD2E成功之后才取得解锁资格；分配/取nil失败只解锁，不补正文回滚。
    lock_.Enter();
    struct LockCleanup final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~LockCleanup() noexcept { if (active) lock.Leave(); }
    } cleanup{lock_};
    if (!published_)
    {
        auto* const body = static_cast<RuntimeCString*>(memory_.AllocateBody(sizeof(RuntimeCString)));
        if (body) InitializeEmptyRuntimeCString(*body, strings_);
        published_ = body;
    }
    // 47BD80撤销自动解锁资格；普通Leave失败终止，不能再次Leave。
    cleanup.active = false;
    LeaveNoexcept();
    return published_; // 47BD8A在解锁回调之后重新取61A4F0。
}

void SettingTagErrorOwner::ReleaseAt5AB440() noexcept
{
    // 5ECC54的两个状态均terminate；Enter/Free/Delete失败不补解锁或清槽。
    lock_.Enter();
    if (auto* const body = published_)
    {
        DestroyRuntimeCStringAt40CD40(body);
        memory_.DeleteBody(body, sizeof(RuntimeCString));
        published_ = nullptr;
    }
    lock_.Leave();
}

SettingTagErrorOwner& ProcessSettingTagErrorOwner()
{
    static EnvironmentRootCriticalSection lock;
    static SettingTagErrorOwner owner(lock);
    return owner;
}
}
