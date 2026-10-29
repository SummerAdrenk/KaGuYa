#include <Runtime/SoundParameterNative.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <mbstring.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
template<class T> T& Resolve(const Slot& slot) noexcept
{
    auto* object = ResolveEnvironmentObjectReference<T>(slot);
    if (!object) _invalid_parameter_noinfo_noreturn();
    return *object;
}
bool SameKey(const char* left, const char* right)
{
    // 三处原函数均检查Str2，而不是对两个字符串作空串兜底。
    if (!right) throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(left),
        reinterpret_cast<const unsigned char*>(right)) == 0;
}
template<class T> void WriteList(SerializeBuffer& serializer, EnvironmentVectorStorage<Slot>& list)
{
    const auto bytes = reinterpret_cast<std::uintptr_t>(list.end) - reinterpret_cast<std::uintptr_t>(list.begin);
    auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    serializer.SerializeSlot58Dword(count);
    for (auto* slot = list.begin; slot != list.end; ++slot)
        Resolve<T>(*slot).Serialize(serializer);
}
class TypeGroupConstruction final
{
public:
    explicit TypeGroupConstruction(SoundTypeGroupNative& body) noexcept : body_(body) {}
    ~TypeGroupConstruction() noexcept
    {
        if (!active) return;
        if (vectorConstructed) DestroyEnvironmentArchiveReferencesAt497080(body_.parametersAt08);
        if (stringConstructed) DestroyRuntimeCStringAt40CD40(&body_.masterAt04);
        body_.IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    }
    bool stringConstructed = false, vectorConstructed = false, active = true;
private:
    SoundTypeGroupNative& body_;
};
}

SoundTypeGroupNative* InitializeSoundTypeGroupAt474877(void* raw, const char* master,
    IRuntimeStringManagerSource& strings)
{
    auto* body = ::new (raw) SoundTypeGroupNative(SoundNativeShellTag{});
    TypeGroupConstruction unwind(*body);
    InitializeEmptyRuntimeCString(body->masterAt04, strings);
    unwind.stringConstructed = true;
    body->parametersAt08 = {};
    unwind.vectorConstructed = true;
    SetRuntimeCStringAt437680(body->masterAt04, master, master ? static_cast<int>(std::strlen(master)) : 0);
    unwind.active = false;
    return body;
}
SoundCharacterGroupNative* CreateSoundCharacterGroupAt47209F(IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x14);
    if (!raw) return nullptr;
    auto* body = ::new (raw) SoundCharacterGroupNative(SoundNativeShellTag{});
    body->charactersAt08 = {}; // 4720B5..C3未写04/05或padding。
    return body;
}
void* SoundTypeGroupNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(parametersAt08);
    DestroyRuntimeCStringAt40CD40(&masterAt04);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    return this;
}
void* SoundCharacterGroupNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(charactersAt08);
    IEnvironmentArchiveRuntime::DeleteAtVslot04(0);
    if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    return this;
}
void SoundTypeGroupNative::Serialize(SerializeBuffer& serializer)
{ SerializeWithSources(serializer, DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()); }
void SoundCharacterGroupNative::Serialize(SerializeBuffer& serializer)
{ SerializeWithSources(serializer, DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()); }

void SoundTypeGroupNative::SerializeWithSources(SerializeBuffer& serializer,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    if (serializer.IsWriting()) { WriteList<SoundSeNative>(serializer, parametersAt08); return; }
    std::uint32_t bits;
    serializer.SerializeSlot58Dword(bits);
    const auto count = std::bit_cast<std::int32_t>(bits);
    for (std::int32_t index = 0; index < count; ++index)
    {
        auto* body = CreateSoundSeForFileAt48C7C0(strings, memory);
        Slot local;
        AdoptSoundNativeReference(local, body, memory);
        auto* cached = local.controlAt08;
        SoundNativeTemporary unwind(local);
        Resolve<SoundSeNative>(local).Serialize(serializer);
        for (auto* target = parametersAt08.begin; target != parametersAt08.end; ++target)
        {
            if (!SameKey(body->keyAt0C.GetString(), Resolve<SoundSeNative>(*target).keyAt0C.GetString())) continue;
            Resolve<SoundSeNative>(*target).valueAt04 = body->valueAt04;
            Resolve<SoundSeNative>(*target).valueAt08 = body->valueAt08;
            break;
        }
        unwind.Finish(cached);
    }
}

void SoundCharacterGroupNative::SerializeWithSources(SerializeBuffer& serializer,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    serializer.SerializeSlot54Byte(valueAt05); // Enabled(+04)不在文件中，也不清已有配置表。
    if (serializer.IsWriting()) { WriteList<SoundCharacterNative>(serializer, charactersAt08); return; }
    std::uint32_t bits;
    serializer.SerializeSlot58Dword(bits);
    const auto count = std::bit_cast<std::int32_t>(bits);
    for (std::int32_t index = 0; index < count; ++index)
    {
        auto* body = CreateSoundCharacterForFileAt48C570(strings, memory);
        Slot local;
        AdoptSoundNativeReference(local, body, memory);
        auto* cached = local.controlAt08;
        SoundNativeTemporary unwind(local);
        if (!body) _invalid_parameter_noinfo_noreturn();
        body->SerializeWithSources(serializer, strings, memory);
        for (auto* target = charactersAt08.begin; target != charactersAt08.end; ++target)
        {
            if (!SameKey(body->keyAt08.GetString(), Resolve<SoundCharacterNative>(*target).keyAt08.GetString())) continue;
            body->MergeIntoAt48EE20(*target);
            break;
        }
        unwind.Finish(cached);
    }
}

void SoundCharacterNative::MergeIntoAt48EE20(Slot& destination) const
{
    Resolve<SoundCharacterNative>(destination).valueAt04 = valueAt04;
    Resolve<SoundCharacterNative>(destination).valueAt05 = valueAt05;
    for (auto* incoming = parametersAt0C.begin; incoming != parametersAt0C.end; ++incoming)
    {
        auto* target = Resolve<SoundCharacterNative>(destination).parametersAt0C.begin;
        for (; target != Resolve<SoundCharacterNative>(destination).parametersAt0C.end; ++target)
        {
            const auto* key = Resolve<SoundSeExNative>(*incoming).keyAt0C.GetString();
            if (!SameKey(Resolve<SoundSeExNative>(*target).keyAt0C.GetString(), key)) continue;
            Resolve<SoundSeExNative>(*target).valueAt04 = Resolve<SoundSeExNative>(*incoming).valueAt04;
            Resolve<SoundSeExNative>(*target).valueAt08 = Resolve<SoundSeExNative>(*incoming).valueAt08;
            // 48EFB0→48EFB3→48EFB6：此处更新后直接递增incoming(edi)，不是target(esi)。
            break;
        }
    }
}
}
