#include <Runtime/WindowModeNative.h>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class WindowModeControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit WindowModeControl(WindowModeNative2Ch* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<WindowModeNative2Ch*>(payloadAt14);
        if (!body) return;
        // 5B3750+00 -> 492DA0：必须读取当前正文表，不能被final类去虚拟化。
        using Delete = WindowModeNative2Ch* (__thiscall*)(WindowModeNative2Ch*, std::uint32_t);
        const Delete* table;
        std::memcpy(&table, body, sizeof(table));
        table[0](body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 467E00先退5ADD18，再按bit0 delete18h；退役表再次删除走14h。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(WindowModeControl) == 0x18);
}

WindowModeNative2Ch* NewWindowModeAt47791D(IRuntimeStringManagerSource& strings)
{
    void* const storage = ::operator new(0x2C);
    if (!storage) return nullptr;
    struct AllocationUnwind final
    {
        void* storage;
        ~AllocationUnwind() noexcept
        {
            // 599530在展开期间释放2Ch；不能先catch再释放，改变回调的异常状态。
            if (storage) ::operator delete(storage, std::size_t{0x2C});
        }
    } cleanup{storage};
    auto* const body = ::new (storage) WindowModeNative2Ch(strings);
    cleanup.storage = nullptr;
    return body;
}

WindowModeNative2Ch::WindowModeNative2Ch(IRuntimeStringManagerSource& strings)
{
    // 477934..477982：+05..07及viewport不写；GetNil成功后才有CString清理资格。
    modeAt04 = 0;
    InitializeEmptyRuntimeCString(deviceNameAt08, strings);
    struct Unwind final
    {
        RuntimeCString& text;
        bool armed = true;
        ~Unwind() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&text); }
    } unwind{deviceNameAt08};
    savedMonitorRectAt1C = {};
    SetRuntimeCStringAt437680(deviceNameAt08, "", 0);
    unwind.armed = false;
}

WindowModeNative2Ch::~WindowModeNative2Ch() noexcept
{
    // 4646C0的EHFlags=5：Free异常终止，不恢复显示模式，也不调用Graphics。
    DestroyRuntimeCStringAt40CD40(&deviceNameAt08);
}

WindowModeNative2Ch* WindowModeNative2Ch::DeleteAtVslot00(std::uint32_t flags) noexcept
{
    // 4696C0：仅bit0决定是否释放2Ch存储，返回原this值。
    this->~WindowModeNative2Ch();
    if (flags & 1u) ::operator delete(this, std::size_t{0x2C});
    return this;
}

void CreateAndReplaceWindowModeAt47791D(YtlReferenceSlotRuntime& slot,
    IRuntimeStringManagerSource& strings)
{
    auto* const created = NewWindowModeAt47791D(strings);
    ReleaseWindowModeReferenceAt47798B(slot);
    if (!created) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* storage = ::operator new(0x18);
    auto* control = storage ? ::new (storage) WindowModeControl(created) : nullptr;
    slot.controlAt08 = control;
    // 4779F8..477A16：先发布；每步重读当前control，+09..0B保留分配毒值。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x2C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

void ReleaseWindowModeReferenceAt47798B(YtlReferenceSlotRuntime& slot) { ReleaseYtlReference(slot); }
void ReleaseWindowModeReferenceAt464415(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
WindowModeNative2Ch* ResolveWindowModeReference(const YtlReferenceSlotRuntime& slot) noexcept
{ return ResolveEnvironmentObjectReference<WindowModeNative2Ch>(slot); }
}
