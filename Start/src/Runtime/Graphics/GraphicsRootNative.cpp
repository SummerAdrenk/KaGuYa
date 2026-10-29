#include <Runtime/GraphicsRootNative.h>

#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
GraphicsRootNativeBody1Ch* CreateGraphicsRootNativeAt418349(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    void* const storage = memory.AllocateBody(0x1C);
    if (!storage) return nullptr;
    auto* const root = ::new (storage) GraphicsRootNativeBody1Ch;
    // 41835A..418381：先开始Ref生命期并清三字段，最后才取manager/GetNil。
    // 根没有raw回收守卫；失败保留此前已写前缀，+0C..+17始终不初始化。
    std::construct_at(&root->moduleAt00);
    InitializeEmptyRuntimeCString(root->errorAt18, strings);
    return root;
}

void ReleaseGraphicsModuleAt440620(GraphicsRootNativeBody1Ch& root,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32)
{
    ReleaseGraphicsModuleReferenceAt440620(root.moduleAt00, memory, win32);
}

void ShutdownGraphicsRootAt440660(GraphicsRootNativeBody1Ch& root,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    // 440688只按base门控；base为空时连孤立的control也不释放。
    if (!root.moduleAt00.baseAt00) return;
    auto* const module = ResolveGraphicsModuleReference(root.moduleAt00);
    using ExitSystem = void (__cdecl*)();
    const auto exitSystem = reinterpret_cast<ExitSystem>(
        ResolveGraphicsModuleExportWithCString(module, "ExitSystem", 10, strings, win32));
    exitSystem();
    // 44071F固定root不换代，但ExitSystem后必须重读它的当前control。
    ReleaseYtlReference(root.moduleAt00);
}

const char* GetGraphicsErrorMessageAt440D00(GraphicsRootNativeBody1Ch& root,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32)
{
    if (!root.moduleAt00.baseAt00) return nullptr;
    auto* const module = ResolveGraphicsModuleReference(root.moduleAt00);
    using GetErrorMessage = const char* (__cdecl*)();
    const auto getErrorMessage = reinterpret_cast<GetErrorMessage>(
        ResolveGraphicsModuleExportWithCString(module, "GetErrorMessage", 15, strings, win32));
    return getErrorMessage();
}

const char* ReadGraphicsErrorAt479FE0(const GraphicsRootNativeBody1Ch& root) noexcept
{
    return root.errorAt18.GetString();
}

GraphicsRootOwner::GraphicsRootOwner(IEnvironmentRootLock& lock, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : lock_(lock), memory_(memory), strings_(strings), win32_(win32)
{
}
GraphicsRootOwner::~GraphicsRootOwner() noexcept { ReleaseAt5AADF0(); }
void GraphicsRootOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

GraphicsRootNativeBody1Ch* GraphicsRootOwner::CaptureAt4182F0()
{
    // 41832E此前state=-1；Enter失败不取得解锁资格。
    lock_.Enter();
    struct LockUnwind final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~LockUnwind() noexcept { if (active) lock.Leave(); }
    } unwind{lock_};
    if (!published_) published_ = CreateGraphicsRootNativeAt418349(memory_, strings_);
    // 418398正常Leave处是终止边界，失败不重试Leave；4183A2重新读取发布槽。
    unwind.active = false;
    LeaveNoexcept();
    return published_;
}

void GraphicsRootOwner::ReleaseAt5AADF0() noexcept
{
    const GraphicsModuleNativeScope scope(memory_, win32_);
    // 5E3DE8三个action均为terminate；不设自动解锁或成员重试守卫。
    lock_.Enter();
    auto* const root = published_;
    if (root)
    {
        DestroyRuntimeCStringAt40CD40(&root->errorAt18);
        ReleaseYtlReference(root->moduleAt00);
        // 始终删除5AAE26捕获的root；回调期间槽仍可见，delete成功后才无条件清槽。
        memory_.DeleteBody(root, 0x1C);
        published_ = nullptr;
    }
    lock_.Leave();
}
}
