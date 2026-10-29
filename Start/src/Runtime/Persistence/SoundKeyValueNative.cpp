#include <Runtime/SoundKeyValueNative.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/RuntimeCStringVector.h>
#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { if (flags & 1u) ::operator delete(this, std::size_t{0x14}); }
    std::uint32_t stride, count;
};
template<class T> class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(T* value) noexcept : EnvironmentObjectReferenceControl(value, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (payloadAt14) static_cast<IEnvironmentArchiveRuntime*>(payloadAt14)->DeleteAtVslot04(1); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 467F20及同族退休保留全部非vptr字段，删除标志使用18h实际大小。
        auto* raw = reinterpret_cast<std::byte*>(this);
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), raw + 4, fields.size());
        ::new (raw) RetiredControl;
        std::memcpy(raw + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(raw, std::size_t{0x18});
    }
};
template<class T> void Adopt(Slot& fresh, T* value, IEnvironmentRootMemory& memory)
{
    fresh = {};
    if (!value) return;
    fresh.baseAt00 = reinterpret_cast<std::uintptr_t>(value);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) Control<T>(value) : nullptr;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    fresh.controlAt08 = control;
    control->countAt10 = 1;
}
void* LegacyValue(const Slot& slot)
{
    auto* value = ResolveEnvironmentObjectReference<void>(slot);
    if (!value) _invalid_parameter_noinfo_noreturn(); // 原48BD4F无空记录回退。
    return value;
}
void SerializeLegacyFactoryValue(void* value, std::uint32_t kind, SerializeBuffer& serializer)
{
    // 仅明确的旧宿主factory协议；native写支始终动态slot00，不按此kind选类型。
    if (kind == 0 || kind == 1) static_cast<SoundBasicValueNative*>(value)->Serialize(serializer);
    else if (kind == 2) static_cast<SoundEffectValueNative*>(value)->Serialize(serializer);
    else throw 0;
}
std::int32_t Count(const EnvironmentVectorStorage<Slot>& table) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(table.end) - reinterpret_cast<std::uintptr_t>(table.begin))) / 12;
}
void DestroyChild(SoundChildValueNative& value) noexcept
{
    DestroyRuntimeCStringVectorAt43A700(value.stringsAt28, DefaultEnvironmentRootMemory());
    DestroyRuntimeCStringVectorAt43A700(value.stringsAt18, DefaultEnvironmentRootMemory());
    DestroyRuntimeCStringAt40CD40(&value.textAt14);
    DestroyRuntimeCStringAt40CD40(&value.textAt10);
    value.IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
}
class ChildTemporary final
{
public:
    explicit ChildTemporary(IRuntimeStringManagerSource& strings)
    {
        auto* child = ::new (storage_.data()) SoundChildValueNative(SoundNativeShellTag{});
        // 48C133/48C14D两次nil全部成功后才有state0；第二次失败不自动释放第一个。
        InitializeEmptyRuntimeCString(child->textAt10, strings);
        InitializeEmptyRuntimeCString(child->textAt14, strings);
        child->stringsAt18 = {}; child->stringsAt28 = {};
    }
    ~ChildTemporary() noexcept { if (active_) DestroyChild(Get()); }
    SoundChildValueNative& Get() noexcept { return *reinterpret_cast<SoundChildValueNative*>(storage_.data()); }
    void Finish() noexcept { active_ = false; DestroyChild(Get()); }
private:
    alignas(SoundChildValueNative) std::array<std::byte, sizeof(SoundChildValueNative)> storage_;
    bool active_ = true;
};
}
void SoundBasicValueNative::Serialize(SerializeBuffer& serializer)
{
    serializer.SerializeSlot54Byte(mutedAt04);
    serializer.SerializeSlot44Dword(volumeAt08);
}
void* SoundBasicValueNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x0C});
    return this;
}
void* SoundBgmValueNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x10});
    return this;
}
void SoundChildValueNative::Serialize(SerializeBuffer& serializer)
{
    SoundBasicValueNative::Serialize(serializer);
    serializer.SerializeSlot54Byte(valueAt0C);
}
void* SoundChildValueNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyChild(*this);
    if (flags & 1u) ::operator delete(this, std::size_t{0x34});
    return this;
}
void SoundEffectValueNative::Serialize(SerializeBuffer& serializer)
{ SerializeWithSources(serializer, DefaultRuntimeStringManagerSource()); }
void SoundEffectValueNative::SerializeWithSources(SerializeBuffer& serializer, IRuntimeStringManagerSource& strings)
{
    SoundBasicValueNative::Serialize(serializer);
    auto count = serializer.IsWriting() ? static_cast<std::uint32_t>(Count(childrenAt0C)) : 0u;
    serializer.SerializeSlot58Dword(count);
    if (serializer.IsWriting())
    {
        if (count)
            for (auto* child = childrenAt0C.begin; child != childrenAt0C.end; ++child)
                static_cast<SoundChildValueNative*>(LegacyValue(*child))->Serialize(serializer);
        return;
    }
    // 48C0E6按索引合并现有项，不清表、不增长；多余项构造完整34h栈临时后消费并丢弃。
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
    {
        if (index < Count(childrenAt0C))
            static_cast<SoundChildValueNative*>(LegacyValue(childrenAt0C.begin[index]))->Serialize(serializer);
        else
        {
            ChildTemporary temporary(strings);
            temporary.Get().Serialize(serializer);
            temporary.Finish();
        }
    }
}
void* SoundEffectValueNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyRuntimeCStringVectorAt43A700(stringsAt1C, DefaultEnvironmentRootMemory());
    DestroyEnvironmentArchiveReferencesAt497080(childrenAt0C);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x28});
    return this;
}
void ConstructSoundValueReferenceAt48BE18(Slot& fresh, std::uint32_t kind, IEnvironmentRootMemory& memory)
{
    if (kind == 0)
    {
        auto* raw = memory.AllocateBody(0x0C);
        Adopt(fresh, raw ? ::new (raw) SoundBasicValueNative(SoundNativeShellTag{}) : nullptr, memory);
    }
    else if (kind == 1)
    {
        auto* raw = memory.AllocateBody(0x10);
        Adopt(fresh, raw ? ::new (raw) SoundBgmValueNative(SoundNativeShellTag{}) : nullptr, memory);
    }
    else if (kind == 2)
    {
        auto* raw = memory.AllocateBody(0x28);
        auto* value = raw ? ::new (raw) SoundEffectValueNative(SoundNativeShellTag{}) : nullptr;
        if (value) { value->childrenAt0C = {}; value->stringsAt1C = {}; }
        Adopt(fresh, value, memory);
    }
    else throw 0; // 48BF8F：未知类型抛int 0，不读未知payload，也不改变当前对象。
}
void AdoptSoundChildValueReference(Slot& fresh, SoundChildValueNative* value, IEnvironmentRootMemory& memory)
{ Adopt(fresh, value, memory); }
void SoundKeyValueNative::Serialize(SerializeBuffer& serializer)
{ SerializeWithMemory(serializer, DefaultEnvironmentRootMemory()); }
void SoundKeyValueNative::SerializeWithMemory(SerializeBuffer& serializer, IEnvironmentRootMemory& memory)
{ SerializeWithLegacyValue(serializer, memory, SerializeLegacyFactoryValue); }
void SoundKeyValueNative::SerializeWithLegacyValue(SerializeBuffer& serializer, IEnvironmentRootMemory& memory,
    LegacyValueSerialize serializeValue)
{
    if (serializer.IsWriting())
    {
        serializer.SerializeSlot58Dword(kindAt08);
        serializeValue(LegacyValue(valueAt0C), kindAt08, serializer);
        return;
    }
    std::uint32_t kind;
    serializer.SerializeSlot58Dword(kind);
    if (kind == kindAt08) { serializeValue(LegacyValue(valueAt0C), kindAt08, serializer); return; }
    Slot temporary;
    SoundNativeTemporary unwind(temporary); // 48BDFF在分配前即取得局部Ref清理资格。
    ConstructSoundValueReferenceAt48BE18(temporary, kind, memory);
    auto* cached = temporary.controlAt08;
    SerializeLegacyFactoryValue(LegacyValue(temporary), kind, serializer);
    unwind.Finish(cached);
}
void* SoundKeyValueNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    ReleaseYtlReference(valueAt0C);
    DestroyRuntimeCStringAt40CD40(&keyAt04);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x18});
    return this;
}
std::int32_t AppendSoundKeyValueAt46A650(EnvironmentVectorStorage<Slot>& table,
    const char* key, std::uint32_t kind, const Slot& value, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x18);
    auto* body = raw ? ::new (raw) SoundKeyValueNative(SoundNativeShellTag{}) : nullptr;
    if (body)
    {
        InitializeEmptyRuntimeCString(body->keyAt04, strings);
        body->valueAt0C = {};
    }
    Slot local;
    Adopt(local, body, memory);
    auto* cached = local.controlAt08;
    SoundNativeTemporary unwind(local);
    if (!body) _invalid_parameter_noinfo_noreturn();
    SetRuntimeCStringAt437680(body->keyAt04, key, key ? static_cast<int>(std::strlen(key)) : 0);
    body->kindAt08 = kind;
    if (&body->valueAt0C != &value)
    {
        ReleaseYtlReference(body->valueAt0C);
        body->valueAt0C = value;
        if (body->valueAt0C.controlAt08) ++body->valueAt0C.controlAt08->referenceCountAt04;
    }
    Slot copied{local.baseAt00, 0, cached};
    if (cached) ++cached->referenceCountAt04;
    SoundNativeTemporary copiedUnwind(copied);
    // 4547F0与454310使用相同的12byte Ref增持、1.5倍扩容和32byte对齐。
    AppendEnvironmentArchiveReferenceAt49852D(table, copied, memory);
    copiedUnwind.Finish(copied.controlAt08);
    const auto count = Count(table);
    unwind.Finish(cached);
    return count;
}
SoundKeyValueTableNative::~SoundKeyValueTableNative() noexcept
{ DestroyEnvironmentArchiveReferencesAt497080(storage_); }
std::int32_t SoundKeyValueTableNative::AddAt46A650(const char* key, std::uint32_t kind, const Slot& value)
{ return AppendSoundKeyValueAt46A650(storage_, key, kind, value); }
}
