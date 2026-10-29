#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/CategorizedSoundEnvironmentAdapter.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootCriticalSection::EnvironmentRootCriticalSection()
{
    // 401000/601164：原 Environment 使用独立锁，不借用6198C4运行根的锁。
    ::InitializeCriticalSection(&section_);
}

EnvironmentRootCriticalSection::~EnvironmentRootCriticalSection() noexcept
{
    ::DeleteCriticalSection(&section_);
}

void EnvironmentRootCriticalSection::Enter()
{
    ::EnterCriticalSection(&section_);
    const DWORD thread = ::GetCurrentThreadId();
    ++depth_;
    thread_.store(thread, std::memory_order_relaxed);
}

void EnvironmentRootCriticalSection::Leave()
{
    if (--depth_ == 0) thread_.store(0xFFFFFFFFu, std::memory_order_relaxed);
    ::LeaveCriticalSection(&section_);
}

bool EnvironmentRootCriticalSection::HeldByCurrentThreadAt4076D0() const noexcept
{
    return thread_.load(std::memory_order_relaxed) == ::GetCurrentThreadId();
}

EnvironmentRootStorage* CreateEnvironmentRootAt41811B(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    auto* const root = static_cast<EnvironmentRootStorage*>(memory.AllocateBody(sizeof(EnvironmentRootStorage)));
    if (!root) return nullptr; // 41812C/4181BC：保留原空返回叶，不改写成分配异常。
    // 聚合正文由operator new提供存储；逐个开始非隐式生命周期成员，不能用整个对象的构造回滚。
    // 两次 Capture 均在原 state0 内；第二次失败时第一字符串和正文不自动回收。
    // 418143/41815D直接调用原始GetNil槽；ATL构造的throw()会截断原本可展开到锁的异常。
    InitializeEmptyRuntimeCString(root->titleAt10, strings);
    InitializeEmptyRuntimeCString(root->appendTitleAt14, strings);
    root->archivesAt30 = {nullptr, nullptr, nullptr};
    root->errorsAt3C = {nullptr, nullptr, nullptr};
    root->pathsAt48 = {};
    root->versionAt54 = {};
    return root;
}

void InitializeEnvironmentWindowFieldsAt4982AA(EnvironmentRootStorage& root,
    HMODULE module, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    root.moduleAt00 = module;
    root.previousAt04 = previousInstance;
    root.commandLineAt08 = commandLine;
    root.showCommandAt0C = showCommand;
    SetRuntimeCStringAt437680(root.titleAt10, "", 0);
    SetRuntimeCStringAt437680(root.appendTitleAt14, "", 0);
    root.widthAt18 = 640;
    root.heightAt1C = 480;
    root.styleAt20 = 0x02CB0000;
    root.exStyleAt24 = 0x200;
    // 4982F7 -> 4982FE跳过+28，重入也不能抹掉fullscreen style。
    root.windowAt2C = 0;
}

EnvironmentRootOwner::EnvironmentRootOwner(IEnvironmentRootLock& lock,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : lock_(lock), memory_(memory), strings_(strings)
{
}

EnvironmentRootOwner::~EnvironmentRootOwner() noexcept { ReleaseAt5AB7D0(); }

void EnvironmentRootOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

EnvironmentRootStorage* EnvironmentRootOwner::CaptureAt4180C0()
{
    // 4180FE处state=-1：Enter抛出时不能调用Leave。
    lock_.Enter();
    struct LockUnwindCleanup
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~LockUnwindCleanup() noexcept { if (active) lock.Leave(); }
    } cleanup{lock_};
    // 5926C0在异常展开中调用40E780，不是在catch里解锁；回调应仍可观察未捕获异常。
    // 此守卫只释放锁，不补原本不存在的正文/字符串回滚。
    if (!published_) published_ = CreateEnvironmentRootAt41811B(memory_, strings_);
    // 4181CE先撤销自动解锁资格，Leave异常terminate，不重试。
    cleanup.active = false;
    LeaveNoexcept();
    return published_;
}

void EnvironmentRootOwner::ReleaseAt5AB7D0() noexcept
{
    lock_.Enter();
    ResetCategorizedSoundEnvironment();
    if (published_)
    {
        // 删除回调期间仍可见旧6198C8；删除完成后才清槽。
        DeleteEnvironmentRootAt4976E0(*published_, 1, memory_);
        published_ = nullptr;
    }
    lock_.Leave();
}
void EnvironmentRootOwner::BindCategorizedSoundEnvironment(SaveDataEnvironmentArchive& save,
    std::unique_ptr<CategorizedSoundEnvironmentAdapter> adapter)
{
    if(!adapter)throw std::invalid_argument("Categorized env adapter is null");
    categorizedSound_=std::move(adapter);categorizedSave_=&save;
}
void EnvironmentRootOwner::ResetCategorizedSoundEnvironment() noexcept
{
    categorizedSave_=nullptr;categorizedSound_.reset();
}
CategorizedSoundEnvironmentAdapter& EnvironmentRootOwner::RequireCategorizedSoundEnvironment(
    const SaveDataEnvironmentArchive& save) const
{
    if(categorizedSave_!=&save||!categorizedSound_)
        throw std::logic_error("Categorized env metadata is not bound to the selected save archive");
    return *categorizedSound_;
}
}
