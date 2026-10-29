#include <Runtime/ScriptNativeBody.h>

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IScriptNativeEntryContext* activeEntries = nullptr;

template<unsigned Opcode>
std::int32_t __fastcall Entry(ScriptNativeBody* self, void*)
{
    // fastcall的第二寄存器形参未使用，实际无栈参数，与原ECX=this/retn一致。
    if constexpr (Opcode == 28) return 0;
    else
    {
        if (!activeEntries) throw std::logic_error("CScript entry requires its external call context");
        return activeEntries->Invoke(*self, Opcode);
    }
}

ScriptNativeBody* __fastcall DeleteBody(ScriptNativeBody* self, void*, std::uint32_t flags);
const ScriptNativeVtable methods{
    reinterpret_cast<decltype(ScriptNativeVtable::deletingDestructor)>(&DeleteBody)};

void RegisterEntries(ScriptNativeBody& body) noexcept
{
    // 4C8DE9..4C8EC2按此顺序写29个直接代码地址；不把handler对象地址放进正文。
#define KAGUYA_SCRIPT_ENTRY(n) body.entriesAt2C[n] = reinterpret_cast<ScriptNativeEntry>(&Entry<n>)
    KAGUYA_SCRIPT_ENTRY(0); KAGUYA_SCRIPT_ENTRY(1); KAGUYA_SCRIPT_ENTRY(2);
    KAGUYA_SCRIPT_ENTRY(3); KAGUYA_SCRIPT_ENTRY(4); KAGUYA_SCRIPT_ENTRY(5);
    KAGUYA_SCRIPT_ENTRY(6); KAGUYA_SCRIPT_ENTRY(7); KAGUYA_SCRIPT_ENTRY(8);
    KAGUYA_SCRIPT_ENTRY(9); KAGUYA_SCRIPT_ENTRY(10); KAGUYA_SCRIPT_ENTRY(27);
    KAGUYA_SCRIPT_ENTRY(11); KAGUYA_SCRIPT_ENTRY(12); KAGUYA_SCRIPT_ENTRY(13);
    KAGUYA_SCRIPT_ENTRY(14); KAGUYA_SCRIPT_ENTRY(15); KAGUYA_SCRIPT_ENTRY(16);
    KAGUYA_SCRIPT_ENTRY(17); KAGUYA_SCRIPT_ENTRY(18); KAGUYA_SCRIPT_ENTRY(19);
    KAGUYA_SCRIPT_ENTRY(26); KAGUYA_SCRIPT_ENTRY(20); KAGUYA_SCRIPT_ENTRY(21);
    KAGUYA_SCRIPT_ENTRY(22); KAGUYA_SCRIPT_ENTRY(23); KAGUYA_SCRIPT_ENTRY(24);
    KAGUYA_SCRIPT_ENTRY(25); KAGUYA_SCRIPT_ENTRY(28);
#undef KAGUYA_SCRIPT_ENTRY
}

ScriptNativeBody* __fastcall DeleteBody(ScriptNativeBody* self, void*, std::uint32_t flags)
{
    self->~ScriptNativeBody();
    if (flags & 1u) ::operator delete(self, std::size_t{0xCC});
    return self;
}

class ScriptControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ScriptControl(ScriptNativeBody* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492D20读正文当前vptr，不静态调用本版本析构函数。
        auto* body = static_cast<ScriptNativeBody*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructor(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ScriptControl) == 0x18);

struct ConstructionUnwind final
{
    ScriptNativeBody& body;
    bool parser = false;
    bool active = true;
    ~ConstructionUnwind() noexcept
    {
        if (!active) return;
        if (parser) body.parserAtA0.~ScrContainer();
        ReleaseYtlReference(body.waitAt18);
        ReleaseYtlReference(body.effectAt0C);
    }
};
}

ScriptNativeBody::ScriptNativeBody(PrefixOnlyTag, IRuntimeStringManagerSource& strings)
    : vptrAt00(&methods)
{
    // 两Ref的默认构造只清+0C..23；其余raw数值/字节/padding保持分配块原内容。
    ConstructionUnwind unwind{*this};
    ::new (static_cast<void*>(&parserAtA0)) ScrContainer(strings);
    unwind.parser = true;
    RegisterEntries(*this);
    unwind.active = false;
}

ScriptNativeBody::ScriptNativeBody(ParamsRuntimeRootOwner& params, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
    : vptrAt00(&methods)
{
    // 5F1800展开不调用完整删除析构，也不重新写vptr；避免委托构造的隐式析构改动失败前缀。
    ConstructionUnwind unwind{*this};
    ::new (static_cast<void*>(&parserAtA0)) ScrContainer(strings);
    unwind.parser = true;
    RegisterEntries(*this);
    CreateScriptEffectAt4C8ECC(effectAt0C, strings, memory);
    InitializeScriptEffectAndWaitAt4C8FA1(effectAt0C, waitAt18, params, memory);
    unwind.active = false;
}

ScriptNativeBody::~ScriptNativeBody() noexcept
{
    vptrAt00 = &methods;
    parserAtA0.~ScrContainer();
    ReleaseYtlReference(waitAt18);
    ReleaseYtlReference(effectAt0C);
}

ScriptNativeEntryScope::ScriptNativeEntryScope(IScriptNativeEntryContext& context) noexcept
    : previous_(activeEntries) { activeEntries = &context; }
ScriptNativeEntryScope::~ScriptNativeEntryScope() noexcept { activeEntries = previous_; }

ScriptNativeBody* GetScriptAt47CD00(YtlReferenceSlotRuntime& slot, ParamsRuntimeRootOwner& params,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    if (!slot.baseAt00)
    {
        void* raw = memory.AllocateBody(0xCC);
        ScriptNativeBody* body = nullptr;
        try { if (raw) body = ::new (raw) ScriptNativeBody(params, memory, strings); }
        catch (...)
        {
            // 599CA0只覆盖正文构造；旧control释放或新control分配抛出不回滚新正文。
            struct Cleanup final
            {
                IEnvironmentRootMemory& memory;
                void* raw;
                ~Cleanup() noexcept { memory.DeleteBody(raw, 0xCC); }
            } cleanup{memory, raw};
            throw;
        }
        ReleaseYtlReference(slot);
        if (body)
        {
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
            slot.biasAt04 = 0;
            slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
            void* controlRaw = memory.AllocateBody(0x18);
            auto* control = controlRaw ? ::new (controlRaw) ScriptControl(body) : nullptr;
            slot.controlAt08 = control;
            if (!control) _invalid_parameter_noinfo_noreturn();
            control->referenceCountAt04 = 1;
            slot.controlAt08->owningAt08 = 1;
            static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0xCC;
            static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
        }
    }
    return ResolveEnvironmentObjectReference<ScriptNativeBody>(slot);
}
}
