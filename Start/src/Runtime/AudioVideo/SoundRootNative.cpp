#include <Runtime/SoundRootNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
SoundRootOwner::SoundRootOwner(IEnvironmentRootLock& lock, IEnvironmentRootMemory& memory,
    IGraphicsModuleNativeWin32Api& win32) noexcept : lock_(lock), memory_(memory), win32_(win32) {}
SoundRootOwner::~SoundRootOwner() noexcept { ReleaseAt5AC690(); }
void SoundRootOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

YtlReferenceSlotRuntime* SoundRootOwner::CaptureAt4183D0()
{
    lock_.Enter();
    struct Unwind final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~Unwind() noexcept { if (active) lock.Leave(); }
    } unwind{lock_};
    if (!published_)
    {
        void* const raw = memory_.AllocateBody(0x0C);
        // 418437..418444只有三DWORD清零；不加载DLL、不创建manager或补偿失败。
        published_ = raw ? ::new(raw) YtlReferenceSlotRuntime : nullptr;
    }
    unwind.active = false;
    LeaveNoexcept();
    return published_; // 418468：解锁后读取publication。
}
void SoundRootOwner::ReleaseAt5AC690() noexcept
{
    const GraphicsModuleNativeScope scope(memory_, win32_);
    // 5FDF48的flags=1，但5FDF30三个unwind项都指向541E55/terminate。
    // Enter、Ref释放、body删除、Leave任一抛出均终止，不补解锁或续清。
    lock_.Enter();
    if (auto* const root = published_)
    {
        ReleaseYtlReference(*root);
        memory_.DeleteBody(root, 0x0C);
        published_ = nullptr;
    }
    LeaveNoexcept();
}
}
