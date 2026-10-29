#include <Runtime/ParamsGameSystemReference.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredGameSystemControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredGameSystemControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退休后的删除尺寸为14h，不再调用正文析构。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredGameSystemControl) == 0x18);

class GameSystemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit GameSystemControl(ParamsGameSystem* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5AE2BC -> 43CAD0：缓存payload，40F370完成后才删同一正文。
        // 原入口无整体flags5；此处全局sized delete仍受v143自身noexcept声明约束。
        auto* const body = static_cast<ParamsGameSystem*>(payloadAt14);
        if (!body) return;
        std::destroy_at(body);
        ::operator delete(body, std::size_t{0x11C});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 411740：flags=0也只退化vptr，保留+04..+17；首次删除传18h。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredGameSystemControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(GameSystemControl) == 0x18);

struct ConstructionUnwind final
{
    IEnvironmentRootMemory& memory;
    void* block;
    bool armed = true;
    ~ConstructionUnwind() noexcept
    {
        // 434940 state0 -> 593910：只删原始11Ch，不析构未完成正文或已成功字符串前缀。
        if (armed) memory.DeleteBody(block, 0x11C);
    }
};
}

void RebuildParamsGameSystemAt434C60(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ReleaseYtlReference(slot);
    auto* const storage = memory.AllocateBody(0x11C);
    ConstructionUnwind unwind{memory, storage};
    auto* const body = storage ? ::new (storage) ParamsGameSystem(ParamsGameSystem::RawAt40C410{}, strings) : nullptr;
    auto* const rebound = slot.controlAt08; // 434CBB先捕获，434CBE才撤销state0。
    unwind.armed = false;
    if (rebound) rebound->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* const controlStorage = memory.AllocateBody(0x18);
    auto* const control = controlStorage ? ::new (controlStorage) GameSystemControl(body) : nullptr;
    slot.controlAt08 = control;
    // 安全差异：原434CFB会写null+4；保留null发布结果后明确抛错，不回滚已发布正文。
    if (!control) throw std::bad_alloc{};
    control->referenceCountAt04 = 1; // 原写申请返回值；后面三次写各自重新取当前slot。
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x11C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

ParamsGameSystem* ResolveParamsGameSystem(const YtlReferenceSlotRuntime& slot) noexcept
{
    return ResolveEnvironmentObjectReference<ParamsGameSystem>(slot);
}

ParamsGameSystemReference::~ParamsGameSystemReference() noexcept { reset(); }
ParamsGameSystemReference::ParamsGameSystemReference(const ParamsGameSystemReference& other) noexcept
    : slot_(other.slot_)
{
    if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04;
}
ParamsGameSystemReference& ParamsGameSystemReference::operator=(const ParamsGameSystemReference& other)
{
    if (this != &other)
    {
        ParamsGameSystemReference retained(other);
        *this = std::move(retained);
    }
    return *this;
}
ParamsGameSystemReference::ParamsGameSystemReference(ParamsGameSystemReference&& other) noexcept
    : slot_(std::exchange(other.slot_, YtlReferenceSlotRuntime{})) {}
ParamsGameSystemReference& ParamsGameSystemReference::operator=(ParamsGameSystemReference&& other)
{
    if (this != &other)
    {
        reset();
        slot_ = std::exchange(other.slot_, YtlReferenceSlotRuntime{});
    }
    return *this;
}
void ParamsGameSystemReference::reset() { ReleaseYtlReference(slot_); }

void ParamsGameSystemReference::ReleaseCached(YtlReferenceControlRuntime* captured) noexcept
{
    // 495E10 state50 -> terminate；payload回调后仍用缓存控制块，不晚取slot.control。
    if (captured && --captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
    // 正常完成后撤本C++临时的析构资格，不对回调可能改入的另一个control再减持。
    slot_.controlAt08 = nullptr;
    slot_.baseAt00 = 0;
}

ParamsGameSystemReference ParamsGameSystemReference::CreateFixture()
{
    // 工具事务和434940的失败残留不同，明确独立，不在生产reader上复用。
    auto body = std::make_unique<ParamsGameSystem>();
    void* const storage = ::operator new(sizeof(GameSystemControl));
    if (!storage) throw std::bad_alloc{};
    auto* control = ::new (storage) GameSystemControl(body.get());
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x11C;
    control->countAt10 = 1;
    ParamsGameSystemReference result;
    result.slot_ = {reinterpret_cast<std::uintptr_t>(body.release()), 0, control};
    return result;
}
}
