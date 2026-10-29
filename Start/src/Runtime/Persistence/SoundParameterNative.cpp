#include <Runtime/SoundParameterNative.h>
#include <Runtime/RuntimeCStringSerialize.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredSoundControl final : public YtlReferenceControlRuntime
{
public:
    RetiredSoundControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { if (flags & 1u) ::operator delete(this, std::size_t{0x14}); }
    std::uint32_t strideAt0C, countAt10;
};
static_assert(sizeof(RetiredSoundControl) == 0x14);

template<class T> class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(T* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 493390/4932F0/493300/4932C0：动态第二槽，不直接调用具体类析构。
        auto* body = static_cast<T*>(payloadAt14);
        if (!body) return;
        if constexpr (std::is_same_v<T, SoundFileKeyNative>) body->DeleteAtVslot00(1); // 493200
        else static_cast<IEnvironmentArchiveRuntime*>(body)->DeleteAtVslot04(1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 468550/468490/4684C0/468400：只替换vptr，保留全部20byte字段。
        auto* raw = reinterpret_cast<std::byte*>(this);
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), raw + 4, fields.size());
        ::new (raw) RetiredSoundControl;
        std::memcpy(raw + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(raw, std::size_t{0x18});
    }
};
static_assert(sizeof(Control<SoundSeExNative>) == 0x18);

template<class T> void Adopt(YtlReferenceSlotRuntime& fresh, T* body, IEnvironmentRootMemory& memory)
{
    fresh = {};
    if (!body) return;
    fresh.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) Control<T>(body) : nullptr;
    // 原null控制块将立即非法写入；不将它变成可恢复的空Ref。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    fresh.controlAt08 = control;
    control->countAt10 = 1;
}

template<class T> void AdoptForSettings(YtlReferenceSlotRuntime& fresh, T* body, IEnvironmentRootMemory& memory)
{
    fresh = {};
    if (!body) return;
    fresh.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    // 474A73/472318/472921：配置临时槽先发布control，随后建立vptr和字段。
    fresh.controlAt08 = static_cast<YtlReferenceControlRuntime*>(raw);
    auto* control = raw ? ::new (raw) Control<T>(body) : nullptr;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    control->countAt10 = 1;
}

template<class T> void ReplaceGroup(YtlReferenceSlotRuntime& slot, T* body, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) Control<T>(body) : nullptr;
    slot.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    // 47496D/47214A起每次重读根槽，并非Adopt的局部control发布顺序。
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = sizeof(T);
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

SoundSeNative* InitializeSeInline(void* raw, IRuntimeStringManagerSource& strings)
{
    // 48C7D0..48C833：两个GetNil时分别保持type/se vptr，未写标量不得补零。
    auto* type = ::new (raw) SoundTypeNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(type->keyAt0C, strings);
    auto* se = ::new (raw) SoundSeNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(se->textAt10, strings);
    se->fileKeysAt14 = {};
    se->stringsAt24 = {};
    return se;
}
void InitializeSeAt45F670(void* raw, IRuntimeStringManagerSource& strings) noexcept
{
    // 独立函数5E564C flags5；48C7C0的内联版本没有这一终止边界。
    InitializeSeInline(raw, strings);
}
void DestroySeFields(SoundSeNative& body) noexcept
{
    // 4656A0：字符串表、filekey引用表、两CString，最后才退休vptr。
    DestroyRuntimeCStringVectorAt43A700(body.stringsAt24, DefaultEnvironmentRootMemory());
    DestroyEnvironmentArchiveReferencesAt497080(body.fileKeysAt14);
    DestroyRuntimeCStringAt40CD40(&body.textAt10);
    DestroyRuntimeCStringAt40CD40(&body.keyAt0C);
}
template<class T> T& Required(T* body) noexcept
{
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *body;
}
}

void SoundTypeNative::Serialize(SerializeBuffer& serializer)
{
    SerializeRuntimeCStringAt465ED0(serializer, keyAt0C);
    serializer.SerializeSlot54Byte(valueAt04);
    serializer.SerializeSlot44Dword(valueAt08);
}
void* SoundTypeNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyRuntimeCStringAt40CD40(&keyAt0C);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x10});
    return this;
}
void* SoundSeNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroySeFields(*this);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x30});
    return this;
}
void* SoundSeExNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyRuntimeCStringAt40CD40(&textAt30);
    DestroySeFields(*this);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x34});
    return this;
}
void* SoundCharacterNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    // 4625B0与497080的Ref固定范围清理/对齐delete同构。
    DestroyEnvironmentArchiveReferencesAt497080(parametersAt0C);
    DestroyRuntimeCStringAt40CD40(&keyAt08);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x18});
    return this;
}
SoundTypeNative* CreateSoundTypeAt46FEBD(IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x10);
    if (!raw) return nullptr;
    auto* body = ::new (raw) SoundTypeNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(body->keyAt0C, strings);
    return body;
}
SoundSeNative* CreateSoundSeForFileAt48C7C0(IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x30);
    return raw ? InitializeSeInline(raw, strings) : nullptr;
}
SoundSeNative* CreateSoundSeForSettingsAt474A2C(IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x30);
    if (!raw) return nullptr;
    InitializeSeAt45F670(raw, strings);
    return static_cast<SoundSeNative*>(raw);
}
void AdoptSoundSeAt474A47(YtlReferenceSlotRuntime& fresh, SoundSeNative* body,
    IEnvironmentRootMemory& memory)
{
    AdoptForSettings(fresh, body, memory);
}
void AdoptSoundCharacterAt47230E(YtlReferenceSlotRuntime& fresh, SoundCharacterNative* body,
    IEnvironmentRootMemory& memory) { AdoptForSettings(fresh, body, memory); }
void AdoptSoundSeExAt472916(YtlReferenceSlotRuntime& fresh, SoundSeExNative* body,
    IEnvironmentRootMemory& memory) { AdoptForSettings(fresh, body, memory); }
static void InitializeCharacterAt45F5F0(void* raw, IRuntimeStringManagerSource& strings) noexcept
{
    // 独立构造的FuncInfo5E76F8 flags5；只建立CString/vector，不填写两个开关。
    auto* body = ::new (raw) SoundCharacterNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(body->keyAt08, strings);
    body->parametersAt0C = {};
}
SoundCharacterNative* CreateSoundCharacterForSettingsAt4722D1(IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x18);
    if (!raw) return nullptr;
    InitializeCharacterAt45F5F0(raw, strings);
    return static_cast<SoundCharacterNative*>(raw);
}
SoundSeExNative* CreateSoundSeExForSettingsAt4728B3(IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    // 4728BF/4728CA/4728E7与48C321..48C345相同；控制块发布另用配置入口。
    return CreateSoundSeExForFileAt48C310(strings, memory);
}
SoundSeExNative* CreateSoundSeExForFileAt48C310(IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x34);
    if (!raw) return nullptr;
    InitializeSeAt45F670(raw, strings);
    auto* body = ::new (raw) SoundSeExNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(body->textAt30, strings);
    return body;
}
SoundCharacterNative* CreateSoundCharacterForFileAt48C570(IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x18);
    if (!raw) return nullptr;
    auto* body = ::new (raw) SoundCharacterNative(SoundNativeShellTag{});
    InitializeEmptyRuntimeCString(body->keyAt08, strings);
    body->parametersAt0C = {};
    return body;
}
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& slot, SoundTypeNative* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& slot, SoundSeNative* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& slot, SoundSeExNative* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& slot, SoundCharacterNative* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void AdoptSoundNativeReference(YtlReferenceSlotRuntime& slot, SoundFileKeyNative* body, IEnvironmentRootMemory& memory)
{ Adopt(slot, body, memory); }
void ReplaceSoundGroupReference(YtlReferenceSlotRuntime& slot, SoundTypeGroupNative* body, IEnvironmentRootMemory& memory)
{ ReplaceGroup(slot, body, memory); }
void ReplaceSoundGroupReference(YtlReferenceSlotRuntime& slot, SoundCharacterGroupNative* body, IEnvironmentRootMemory& memory)
{ ReplaceGroup(slot, body, memory); }

SoundNativeTemporary::~SoundNativeTemporary() noexcept
{ if (slot_) ReleaseYtlReference(*slot_); }
void SoundNativeTemporary::Finish(YtlReferenceControlRuntime* cached) noexcept
{
    auto* slot = slot_;
    slot_ = nullptr;
    if (!cached) return;
    if (--cached->referenceCountAt04 == 0)
    {
        if (cached->owningAt08) cached->DestroyPayloadAtVslot00();
        cached->DeleteControlAtVslot04(1);
    }
    slot->controlAt08 = nullptr;
    slot->baseAt00 = 0;
}

void SoundCharacterNative::Serialize(SerializeBuffer& serializer)
{ SerializeWithSources(serializer, DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()); }

void SoundCharacterNative::SerializeWithSources(SerializeBuffer& serializer,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    serializer.SerializeSlot54Byte(valueAt04);
    SerializeRuntimeCStringAt465ED0(serializer, keyAt08);
    serializer.SerializeSlot54Byte(valueAt05);
    if (serializer.IsWriting())
    {
        const auto bytes = reinterpret_cast<std::uintptr_t>(parametersAt0C.end) -
            reinterpret_cast<std::uintptr_t>(parametersAt0C.begin);
        auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
        serializer.SerializeSlot58Dword(count);
        for (auto* slot = parametersAt0C.begin; slot != parametersAt0C.end; ++slot)
            Required(ResolveEnvironmentObjectReference<SoundSeExNative>(*slot)).Serialize(serializer);
        return;
    }
    // 48C2D1先清旧元素但保留容量，然后才读count；负数不进入循环。
    ClearEnvironmentArchiveReferencesAt498312(parametersAt0C);
    std::uint32_t rawCount;
    serializer.SerializeSlot58Dword(rawCount);
    const auto count = std::bit_cast<std::int32_t>(rawCount);
    for (std::int32_t index = 0; index < count; ++index)
    {
        auto* body = CreateSoundSeExForFileAt48C310(strings, memory);
        YtlReferenceSlotRuntime local;
        AdoptSoundNativeReference(local, body, memory);
        auto* cached = local.controlAt08;
        SoundNativeTemporary unwind(local); // state1在body/control完成后才取得资格。
        Required(body).Serialize(serializer);
        YtlReferenceSlotRuntime copied;
        copied.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        copied.biasAt04 = 0;
        copied.controlAt08 = cached;
        if (cached) ++cached->referenceCountAt04;
        SoundNativeTemporary copiedUnwind(copied);
        // 4562C0与454310逐项核对同构；fast分支的source为bias=0的本地Ref。
        AppendEnvironmentArchiveReferenceAt49852D(parametersAt0C, copied, memory);
        copiedUnwind.Finish(copied.controlAt08);
        unwind.Finish(cached);
    }
}
}
