#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundResourcePathMapping.h>
#include <bit>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T& NonNull(T* value)
{ if (!value) _invalid_parameter_noinfo_noreturn(); return *value; }
template<class T> T& Required(T* value) { if (!value) throw 1; return *value; }
std::int32_t Count(const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& values) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(values.end) - reinterpret_cast<std::uintptr_t>(values.begin))) / 12;
}
const YtlReferenceSlotRuntime& At(const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& values, std::int32_t index)
{
    // 原47DD28/47897A无下标检查；非法存储明确拒绝，不伪造空项或钳位到首项。
    if (index < 0 || index >= Count(values)) _invalid_parameter_noinfo_noreturn();
    return values.begin[index];
}
SoundTypeNative* Type(EnvironmentSoundParamsArchive& owner, const RuntimeCString& key)
{ return const_cast<SoundTypeNative*>(owner.typeParametersAt20.FindAt47B0C0(key.GetString())); }
SoundCharacterNative* Character(EnvironmentSoundParamsArchive& owner, std::int32_t index)
{
    return ResolveEnvironmentObjectReference<SoundCharacterNative>(At(
        NonNull(owner.seByCharsAt38.Get()).charactersAt08, index));
}
RuntimeCString* Share(RuntimeCString* out, const RuntimeCString& source)
{
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())) - 1;
    auto* shared = CloneRuntimeCStringDataAt414540(header);
    const auto characters = reinterpret_cast<std::uintptr_t>(shared) + sizeof(ATL::CStringData);
    std::memcpy(out, &characters, sizeof(characters));
    return out;
}
template<class T> std::uint8_t* ByteAddress(T* body, std::uint32_t offset) noexcept
{
    // 46CC50/478AB0/4787E0的ADD即使正文为空也返回4/5，不擅自规范化为nullptr。
    return reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body) + offset);
}
}
void* EnvironmentSoundParamsArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~EnvironmentSoundParamsArchive();
    if (flags & 1u) ::operator delete(this, std::size_t{0x44});
    return this;
}
bool EnvironmentSoundParamsArchive::HasMusicAt0C() { return Type(*this, namesAt10.musicAt00) != nullptr; }
std::uint8_t* EnvironmentSoundParamsArchive::MusicMuteAt10() { return &Required(Type(*this, namesAt10.musicAt00)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::MusicVolumeAt14() { return &Required(Type(*this, namesAt10.musicAt00)).valueAt08; }
bool EnvironmentSoundParamsArchive::MusicActiveAt18()
{ auto* type = Type(*this, namesAt10.musicAt00); return type && type->valueAt04 == 0; }
bool EnvironmentSoundParamsArchive::HasNormalEffectAt1C() { return Type(*this, namesAt10.normalEffectAt04) != nullptr; }
std::uint8_t* EnvironmentSoundParamsArchive::NormalEffectMuteAt20() { return &Required(Type(*this, namesAt10.normalEffectAt04)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::NormalEffectVolumeAt24() { return &Required(Type(*this, namesAt10.normalEffectAt04)).valueAt08; }
bool EnvironmentSoundParamsArchive::HasLoopEffectAt28() { return Type(*this, namesAt10.loopEffectAt08) != nullptr; }
std::uint8_t* EnvironmentSoundParamsArchive::LoopEffectMuteAt2C() { return &Required(Type(*this, namesAt10.loopEffectAt08)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::LoopEffectVolumeAt30() { return &Required(Type(*this, namesAt10.loopEffectAt08)).valueAt08; }
bool EnvironmentSoundParamsArchive::HasTypeGroupAt34() { return seByTypeAt2C.Native().baseAt00 != 0; }
RuntimeCString* EnvironmentSoundParamsArchive::TypeMasterAt38(RuntimeCString* out)
{ return Share(out, NonNull(seByTypeAt2C.Get()).masterAt04); }
std::int32_t EnvironmentSoundParamsArchive::TypeCountAt3C()
{ return HasTypeGroupAt34() ? Count(NonNull(seByTypeAt2C.Get()).parametersAt08) : 0; }
SoundSeNative* EnvironmentSoundParamsArchive::TypeAt40(std::int32_t index)
{ return ResolveEnvironmentObjectReference<SoundSeNative>(At(NonNull(seByTypeAt2C.Get()).parametersAt08, index)); }
RuntimeCString* EnvironmentSoundParamsArchive::TypeNameAt44(RuntimeCString* out, std::int32_t index)
{ return Share(out, Required(TypeAt40(index)).keyAt0C); }
std::uint8_t* EnvironmentSoundParamsArchive::TypeMuteAt48(std::int32_t index)
{ return &Required(TypeAt40(index)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::TypeVolumeAt4C(std::int32_t index)
{ return &Required(TypeAt40(index)).valueAt08; }
std::uint32_t* EnvironmentSoundParamsArchive::TypeStateAt50(std::int32_t index)
{ auto* value = TypeAt40(index); return value ? &value->stateAt20 : nullptr; }
EnvironmentVectorStorage<RuntimeCString>* EnvironmentSoundParamsArchive::TypeFilesAt54(std::int32_t index)
{ auto* value = TypeAt40(index); return value ? &value->stringsAt24 : nullptr; }
bool EnvironmentSoundParamsArchive::HasVoiceAt64() { return Type(*this, namesAt10.voiceAt0C) != nullptr; }
std::uint8_t* EnvironmentSoundParamsArchive::VoiceMuteAt68() { return &Required(Type(*this, namesAt10.voiceAt0C)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::VoiceVolumeAt6C() { return &Required(Type(*this, namesAt10.voiceAt0C)).valueAt08; }
bool EnvironmentSoundParamsArchive::CharacterGroupEnabledAt70()
{ return seByCharsAt38.Native().baseAt00 && NonNull(seByCharsAt38.Get()).valueAt04 != 0; }
std::uint8_t* EnvironmentSoundParamsArchive::CharacterGroupUsingAt74() { return ByteAddress(seByCharsAt38.Get(), 5); }
std::int32_t EnvironmentSoundParamsArchive::CharacterCountAt78()
{ return CharacterGroupEnabledAt70() ? Count(NonNull(seByCharsAt38.Get()).charactersAt08) : 0; }
std::uint8_t* EnvironmentSoundParamsArchive::CharacterUsingAt7C(std::int32_t index) { return ByteAddress(Character(*this, index), 4); }
RuntimeCString* EnvironmentSoundParamsArchive::CharacterNameAt80(RuntimeCString* out, std::int32_t index)
{ return Share(out, NonNull(Character(*this, index)).keyAt08); }
std::uint8_t* BorrowSoundCharacterDisplayAt4787E0(EnvironmentSoundParamsArchive& owner,std::int32_t index) noexcept
{
    const auto read=[]<class T>(const T& value) noexcept{return *static_cast<const volatile T*>(&value);};
    const auto resolve=[&](const YtlReferenceSlotRuntime& slot,std::uintptr_t base) noexcept
    {
        if(!base)return std::uintptr_t{0};
        auto* control=static_cast<EnvironmentObjectReferenceControl*>(read(slot.controlAt08));
        const auto stride=control?read(control->strideAt0C):0u;
        return base+read(slot.biasAt04)*stride;
    };
    const auto& groupReference=owner.seByCharsAt38.Native();
    const auto groupBase=read(groupReference.baseAt00);
    auto* group=reinterpret_cast<SoundCharacterGroupNative*>(resolve(groupReference,groupBase));
    // 原47880B只取begin；空组在8读取故障，不改成invalid-parameter。
    const auto begin=reinterpret_cast<std::uintptr_t>(read(group->charactersAt08.begin));
    const auto& slot=*reinterpret_cast<const YtlReferenceSlotRuntime*>(begin+std::uint32_t(index)*12u);
    const auto base=read(slot.baseAt00);
    return reinterpret_cast<std::uint8_t*>(resolve(slot,base)+5u);
}
std::uint8_t* EnvironmentSoundParamsArchive::CharacterDisplayAt84(std::int32_t index)
{return BorrowSoundCharacterDisplayAt4787E0(*this,index);}
SoundSeExNative* EnvironmentSoundParamsArchive::RegistrationAt88(std::int32_t index, const char* name)
{
    auto& character = NonNull(Character(*this, index));
    for (auto* item = character.parametersAt0C.begin; item != character.parametersAt0C.end; ++item)
    {
        auto* body = ResolveEnvironmentObjectReference<SoundSeExNative>(*item);
        if (!name) throw ATL::CAtlException(E_FAIL); // 4789D0：空表不检查name。
        if (_mbscmp(reinterpret_cast<const unsigned char*>(NonNull(body).keyAt0C.GetString()),
            reinterpret_cast<const unsigned char*>(name)) == 0)
            return ResolveEnvironmentObjectReference<SoundSeExNative>(*item);
    }
    return nullptr;
}
std::uint32_t* EnvironmentSoundParamsArchive::RegistrationVolumeAt8C(std::int32_t index, const char* name)
{ return &Required(RegistrationAt88(index, name)).valueAt08; }
std::uint8_t* EnvironmentSoundParamsArchive::RegistrationMuteAt90(std::int32_t index, const char* name)
{ return &Required(RegistrationAt88(index, name)).valueAt04; }
std::uint32_t* EnvironmentSoundParamsArchive::RegistrationStateAt94(std::int32_t index, const char* name)
{ auto* value = RegistrationAt88(index, name); return value ? &value->stateAt20 : nullptr; }
EnvironmentVectorStorage<RuntimeCString>* EnvironmentSoundParamsArchive::RegistrationFilesAt98(std::int32_t index, const char* name)
{ auto* value = RegistrationAt88(index, name); return value ? &value->stringsAt24 : nullptr; }
RuntimeCString* EnvironmentSoundParamsArchive::RegistrationMasterAt9C(RuntimeCString* out, std::int32_t index, const char* name)
{ return Share(out, Required(RegistrationAt88(index, name)).textAt30); }
RuntimeCString* EnvironmentSoundParamsArchive::MapResourceAtA0(RuntimeCString* out, const char* name)
{ ConstructSoundResourcePathAt476A80(out, name, seByCharsAt38.Native(), seByTypeAt2C.Native()); return out; }
RuntimeCString* EnvironmentSoundParamsArchive::ResourceCharacterNameAtB0(RuntimeCString* out, const char* name)
{ ConstructSoundCharacterNameAt478B60(out, seByCharsAt38.Native(), name); return out; }
bool EnvironmentSoundParamsArchive::MatchesVoiceRelationAtB4(const char* relation, const char* name)
{
    YtlReferenceSlotRuntime selected;
    FindSoundCharacterAt489630(selected, seByCharsAt38.Get(), name);
    auto* cached = selected.controlAt08;
    SoundNativeTemporary cleanup(selected);
    bool result = false;
    if (selected.baseAt00)
    {
        if (!relation) throw ATL::CAtlException(E_FAIL);
        result = _mbscmp(reinterpret_cast<const unsigned char*>(
            NonNull(ResolveEnvironmentObjectReference<SoundCharacterNative>(selected)).keyAt08.GetString()),
            reinterpret_cast<const unsigned char*>(relation)) == 0;
    }
    cleanup.Finish(cached);
    return result;
}
}
