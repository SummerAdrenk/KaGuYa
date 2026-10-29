#include <Runtime/ParamsRuntimeRoot.h>

#include <memory>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ReleaseRootReference(YtlReferenceSlotRuntime& slot) noexcept
{
    auto* const control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 4123A3/4123DD/412417在payload回调后重取，不删除先前缓存的control。
        if (auto* const current = slot.controlAt08) current->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

IEnvironmentRootLock& ProcessParamsRootLock()
{
    // 601118的独立锁实例；只复用critical-section实现，不借别的根的锁。
    static EnvironmentRootCriticalSection lock;
    return lock;
}
}

ParamsRuntimeRoot::ParamsRuntimeRoot(RawTag) noexcept
{
    std::construct_at(&gameSystemAt04);
    std::construct_at(&patternAt10);
    std::construct_at(&sceneLabelsAt1C);
}

ParamsRuntimeRoot::~ParamsRuntimeRoot() noexcept
{
    // 5E0B18三项均直达541E55；任一阶段抛出即终止，不续清其他Ref。
    ReleaseRootReference(sceneLabelsAt1C.Native());
    ReleaseRootReference(patternAt10.Native());
    ReleaseRootReference(gameSystemAt04.Native());
    ParamsRuntimeRootDiagnostic::Forget(*this);
    // 非虚基类析构仅安装IArchive退休表；union成员不再自动减持。
}

void* ParamsRuntimeRoot::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~ParamsRuntimeRoot();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

ParamsRuntimeRoot* CreateParamsRuntimeRootAt4184D6(IEnvironmentRootMemory& memory)
{
    auto* const block = memory.AllocateBody(ParamsRuntimeRoot::OriginalObjectSize);
    if (!block) return nullptr;
    return ::new (block) ParamsRuntimeRoot(ParamsRuntimeRoot::RawTag{});
}

ParamsRuntimeRootOwner::ParamsRuntimeRootOwner(IEnvironmentRootLock& lock,
    IEnvironmentRootMemory& memory) noexcept : lock_(lock), memory_(memory) {}

ParamsRuntimeRootOwner::ParamsRuntimeRootOwner(DiagnosticTag tag, IEnvironmentRootMemory& memory)
    : ParamsRuntimeRootOwner(tag, ProcessParamsRootLock(), memory) {}

ParamsRuntimeRootOwner::ParamsRuntimeRootOwner(DiagnosticTag, IEnvironmentRootLock& lock,
    IEnvironmentRootMemory& memory) noexcept : lock_(lock), memory_(memory), diagnostic_(std::in_place) {}

ParamsRuntimeRootOwner::~ParamsRuntimeRootOwner() noexcept { ReleaseAt5ABE20(); }
void ParamsRuntimeRootOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

ParamsRuntimeRoot* ParamsRuntimeRootOwner::CaptureAt418480()
{
    lock_.Enter();
    struct LockUnwindCleanup final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~LockUnwindCleanup() noexcept { if (active) lock.Leave(); }
    } cleanup{lock_};
    if (!published_)
    {
        // 不在分配回调后重新检查槽；418530覆盖发布，包括null结果。
        published_ = CreateParamsRuntimeRootAt4184D6(memory_);
        // 显式现代适配没有正文分配或Ref增持；裸Owner不走此分支。
        if (diagnostic_ && published_) diagnostic_->Bind(*published_);
    }
    cleanup.active = false;
    LeaveNoexcept();
    return published_;
}

ParamsRuntimeRoot& ParamsRuntimeRootOwner::GetOrCreateAt418480()
{
    auto* const root = CaptureAt418480();
    // 兼容引用入口的现代安全拒绝；原nullable叶不抛此错误。
    if (!root) throw std::logic_error("418480: Params root allocation returned null (safe boundary)");
    return *root;
}

void ParamsRuntimeRootOwner::ReleaseAt5ABE20() noexcept
{
    lock_.Enter();
    if (published_)
    {
        // 包括flags0后已退休的4-byte基类表，必须经当前虚槽而非直接再调412360。
        auto* const archive = reinterpret_cast<IEnvironmentArchiveRuntime*>(published_);
        archive->DeleteAtVslot04(1);
        // 删除回调期间6198C0仍指向旧根；回调替换也被本次成功清槽覆盖。
        published_ = nullptr;
    }
    lock_.Leave();
}

ParamsRuntimeRootFixture::ParamsRuntimeRootFixture() noexcept
{
    std::construct_at(&root_, ParamsRuntimeRoot::RawTag{});
    diagnostic_.Bind(root_);
}

ParamsRuntimeRootFixture::~ParamsRuntimeRootFixture() noexcept { std::destroy_at(&root_); }

std::optional<double> ParamsRuntimeRoot::Version() const noexcept
{
    const auto* diagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
    if (!diagnostic || !diagnostic->versionWritten) return std::nullopt;
    return versionAt28;
}

std::uint64_t ParamsRuntimeRoot::GameSystemPublicationEpoch() const noexcept
{
    const auto* diagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
    return diagnostic ? diagnostic->gameSystemPublicationEpoch : 0;
}

bool ParamsRuntimeRoot::ReadCompleted() const noexcept
{
    const auto* diagnostic = ParamsRuntimeRootDiagnostic::Find(*this);
    return diagnostic && diagnostic->readCompleted;
}

void ParamsRuntimeRoot::Serialize(SerializeBuffer& input) { SerializeAt434940(input); }
}
