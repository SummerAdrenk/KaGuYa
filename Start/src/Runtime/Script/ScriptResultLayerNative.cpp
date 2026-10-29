#include <Runtime/ScriptResultLayerNative.h>
#include <Runtime/ScrContainer.h>

#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ScriptResultLayerNativeBody34h;
using Slot = YtlReferenceSlotRuntime;

class ResultLayerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ResultLayerControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // PE 5B5C5C.slot00=4D7500：固定当前payload，filter销毁后仍删该34h。
        auto* const body = static_cast<Body*>(payloadAt14);
        if (!body) return;
        DestroyScriptResultLayerNative(*body);
        ::operator delete(body, std::size_t{0x34});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 4CA950：flags0也退5ADD18；首次flags1删18h，退役后基表删14h。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(ResultLayerControl) == 0x18);

ResultLayerControl* NewControl(Body* body)
{
    void* const raw = ::operator new(std::size_t{0x18});
    return raw ? ::new (raw) ResultLayerControl(body) : nullptr;
}
template<class T> T Read(const void* address) noexcept
{
    T value;
    std::memcpy(&value, address, sizeof(value));
    return value;
}
const std::uint8_t* At(const ScrContainer& container, std::uint32_t offset) noexcept
{
    const auto& slot = container.bytesAt04.RawSlot();
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<const std::uint8_t*>(base + (slot.biasAt04 + offset) * stride);
}
const std::uint8_t* RecordAt(const ScrContainer& container, std::uint32_t offset) noexcept
{ return At(container, container.parsedEndAt1C + offset); }

void InstallForReader(Slot& out, Body* body)
{
    // 4D37F5：new34回调后只删当前control，不减持，不清槽；与默认构造不是同一发布次序。
    if (out.controlAt08) out.controlAt08->DeleteControlAtVslot04(1);
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const control = NewControl(body);
    out.controlAt08 = control;
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x34;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}
}

Body* ConstructScriptResultLayerNativeAt4CB222(Body& body) noexcept
{
    body.valueAt00 = 0xFFFFFFFFu;
    body.valueAt08 = 0;
    body.valueAt0C = 0;
    body.valueAt10 = 0;
    static_cast<void>(ConstructPatternFilterNativeAt45B1CB(&body.filterAt14));
    return &body;
}
Body* NewScriptResultLayerNativeAt4CB208()
{
    void* const raw = ::operator new(std::size_t{0x34});
    // 默认构造没有可抛外调；不凭空添加原不存在的裸块EH owner。
    return raw ? ConstructScriptResultLayerNativeAt4CB222(*::new (raw) Body) : nullptr;
}
void ConstructScriptResultLayerReferenceAt4CB27C(Slot& out, Body* completed)
{
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    if (!completed) return;
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(completed);
    auto* const control = NewControl(completed);
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x34;
    out.controlAt08 = control;
    control->countAt10 = 1;
}
void DestroyScriptResultLayerNative(Body& body) noexcept
{ DestroyPatternFilterNativeAt462FC0(body.filterAt14); }
Body* ResolveScriptResultLayerReferenceAt4CA1C0(const Slot& slot) noexcept
{ return ResolveEnvironmentObjectReference<Body>(slot); }
void ReleaseScriptResultLayerReferenceAt4D71D0(Slot& slot)
{ ReleaseYtlReference(slot); }
void ReleaseScriptResultLayerReferenceAt4C93A0(Slot& slot) noexcept
{
    // 5F1710 FuncInfo.flags=5；EH清当前Ref，释放再抛即终止。
    ReleaseYtlReference(slot);
}

Slot* ReadScriptResultLayerNativeAt4D3670(const ScrContainer& container, Slot& out,
    std::int32_t layerIndex, std::int32_t entryIndex)
{
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    struct Unwind final
    {
        Slot& slot;
        bool armed = true;
        ~Unwind() noexcept { if (armed) ReleaseScriptResultLayerReferenceAt4C93A0(slot); }
    } unwind{out};
    const auto finish = [&]() noexcept -> Slot* { unwind.armed = false; return &out; };
    if (layerIndex < 0) return finish();

    auto offset = Read<std::uint32_t>(At(container,
        container.layerOffsetAt18 + static_cast<std::uint32_t>(layerIndex) * 4u));
    const auto count = Read<std::uint8_t>(RecordAt(container, offset));
    if (entryIndex >= static_cast<std::int32_t>(count)) return finish();
    ++offset;
    std::int32_t index = 0;
    for (; index < count; ++index)
    {
        if (index == entryIndex) break;
        offset += Read<std::uint16_t>(RecordAt(container, offset));
    }
    // 负entry必走完上述扫描；不以现代边界判定替换原访问序列。
    if (index == count) return finish();

    auto* const body = NewScriptResultLayerNativeAt4CB208();
    InstallForReader(out, body);
    offset += 2u;
    // 每字段先选输入地址，再解当前out receiver，再读值/推进/写入。
    // 字段写或allocator重入可改输入、out和parsedEnd，不能整条预读或批量提交。
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::uint32_t>(source); offset += 4u; target->valueAt00 = value;
    }
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::uint8_t>(source); ++offset; target->valueAt04 = value;
    }
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::int16_t>(source); offset += 2u; target->valueAt08 = value;
    }
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::int16_t>(source); offset += 2u; target->valueAt0C = value;
    }
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::uint8_t>(source); ++offset; target->valueAt10 = value;
    }
    {
        const auto* source = RecordAt(container, offset);
        auto* target = ResolveScriptResultLayerReferenceAt4CA1C0(out);
        const auto value = Read<std::int8_t>(source); ++offset;
        target->filterAt14.modeAt04 = static_cast<std::uint32_t>(static_cast<std::int32_t>(value));
    }
    auto extras = Read<std::uint8_t>(RecordAt(container, offset));
    ++offset;
    while (extras)
    {
        const auto* const value = RecordAt(container, offset);
        auto& arguments = ResolveScriptResultLayerReferenceAt4CA1C0(out)->filterAt14.argumentsAt08;
        auto* const position = arguments.end;
        if (arguments.capacity != position)
        {
            *position = Read<std::uint32_t>(value);
            arguments.end = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(arguments.end) + 4u);
        }
        else static_cast<void>(InsertScriptResultLayerExtraAt4447B0(arguments, position,
            reinterpret_cast<const std::uint32_t*>(value)));
        offset += 4u;
        --extras;
    }
    return finish();
}
}
