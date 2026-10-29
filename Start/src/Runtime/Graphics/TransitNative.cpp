#include <Runtime/TransitNative.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IEnvironmentRootMemory* memoryAtCall = nullptr;

IEnvironmentRootMemory& Memory() noexcept
{
    // 原全局CRT new/delete为默认服务；显式scope只借用同一调用的测试/宿主分配器。
    // 不依正文地址登记owner，也不把服务指针塞入原正文或control。
    return memoryAtCall ? *memoryAtCall : DefaultEnvironmentRootMemory();
}

TransitRuntime* __fastcall DeleteBody(TransitRuntime* self, void*, std::uint32_t flags)
{
    return DeleteTransitNativeAt448C80(self, flags, Memory());
}
const TransitNativeVtable methods{
    reinterpret_cast<decltype(TransitNativeVtable::deletingDestructorAt00)>(&DeleteBody)};

class RetiredTransitControl final : public YtlReferenceControlRuntime
{
public:
    RetiredTransitControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); std::terminate(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 412460：退役到5ADD18后再删除，其传入尺寸是14h。
        if (flags & 1u) Memory().DeleteBody(this, 0x14);
    }
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
};
static_assert(sizeof(RetiredTransitControl) == 0x14);

class TransitControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit TransitControl(TransitRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5B3108 -> 492D90：payload固定一次，但正文虚表必须取当前值。
        auto* const body = static_cast<TransitRuntime*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 467DD0：仅改vptr，再按bit0删除18h。placement期间保全未写入的毒值。
        std::array<std::byte, 20> fields;
        auto* const raw = reinterpret_cast<std::byte*>(this);
        std::memcpy(fields.data(), raw + 4, fields.size());
        ::new (static_cast<void*>(this)) RetiredTransitControl;
        std::memcpy(raw + 4, fields.data(), fields.size());
        if (flags & 1u) Memory().DeleteBody(raw, 0x18);
    }
};
static_assert(sizeof(TransitControl) == 0x18);
}

TransitNativeMemoryScope::TransitNativeMemoryScope(IEnvironmentRootMemory& memory) noexcept
    : previous_(memoryAtCall) { memoryAtCall = &memory; }
TransitNativeMemoryScope::~TransitNativeMemoryScope() noexcept { memoryAtCall = previous_; }

const TransitNativeVtable& TransitNativeMethods() noexcept { return methods; }

void DestroyTransitNativeAt448C70(TransitRuntime& body) noexcept
{
    // 原destroy-only只复位表，不清函数槽/random，也不结束借用服务的生命周期。
    body.vptrAt00 = &methods;
}

TransitRuntime* DeleteTransitNativeAt448C80(TransitRuntime* body, std::uint32_t flags,
    IEnvironmentRootMemory& memory)
{
    DestroyTransitNativeAt448C70(*body);
    if (flags & 1u) memory.DeleteBody(body, 0x1F8);
    return body;
}

TransitRuntime* NewTransitNativeAt458819(ICrtRandomSource& random, IEnvironmentRootMemory& memory)
{
    void* const storage = memory.AllocateBody(0x1F8);
    if (!storage) return nullptr;
    struct Unwind final
    {
        IEnvironmentRootMemory& memory;
        void* storage;
        ~Unwind() noexcept { if (storage) memory.DeleteBody(storage, 0x1F8); }
    } unwind{memory, storage};
    // 596370：仅构造抛出时删裸块；已消耗的rand和已写前缀不回滚。
    auto* const body = ::new (storage) TransitRuntime(random);
    unwind.storage = nullptr;
    return body;
}

void InstallTransitReferenceAt45883E(YtlReferenceSlotRuntime& slot, TransitRuntime* body,
    IEnvironmentRootMemory& memory)
{
    TransitNativeMemoryScope scope(memory);
    ReleaseYtlReference(slot);
    if (!body) return;
    // 45886E已xor ecx,ecx；45887B不是第二次读取slot，更不能额外删除回调晚填对象。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* const storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new (storage) TransitControl(body) : nullptr;
    slot.controlAt08 = control;
    // 4588AE..4588CC：先发布再分宽度写入，每次重读当前control；+09..0B不写。
    // 分配返回null的原非法边界仍是null+4写AV，不能清base或自动回收新正文。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x1F8;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

TransitRuntime* ResolveTransitReference(const YtlReferenceSlotRuntime& slot) noexcept
{
    return ResolveEnvironmentObjectReference<TransitRuntime>(slot);
}

void ReleaseTransitReferenceAt458841(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    TransitNativeMemoryScope scope(memory);
    ReleaseYtlReference(slot);
}

void ReleaseTransitReferenceAt4603B0(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory) noexcept
{
    // 4603B0的EHFlags=5：正常释放的异常不得逃出构析展开边界。
    TransitNativeMemoryScope scope(memory);
    ReleaseYtlReference(slot);
}
}
