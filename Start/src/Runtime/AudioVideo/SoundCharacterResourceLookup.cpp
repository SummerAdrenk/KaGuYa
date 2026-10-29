#include <Runtime/SoundCharacterGroupConfiguration.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
template<class T> T& Body(const Slot& slot)
{
    auto* body = ResolveEnvironmentObjectReference<T>(slot);
    if (!body) throw std::invalid_argument("sound character query has no payload (safe boundary)");
    return *body;
}
void Normalize(ArchiveRawResourceNameRuntime& owner)
{
    auto& text = owner.Value();
    LowerRuntimeCStringAt481690(text);
    if (text.GetLength() < 0) return;
    const auto* begin = text.GetString();
    const auto* found = _mbsstr(reinterpret_cast<const unsigned char*>(begin),
        reinterpret_cast<const unsigned char*>(":\\"));
    if (!found) return;
    const auto offset = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(found) -
        reinterpret_cast<std::uintptr_t>(begin));
    if (offset < 0) return;
    auto tail = ArchiveResourceNameMidAt420C80(text, offset + 2, text.GetLength() - offset - 2);
    AssignRuntimeCStringAt410220(text, tail.Value());
    tail.ReleaseNormal();
}
struct Match final { const Slot* character = nullptr; const Slot* registration = nullptr; };
Match FirstMatch(SoundCharacterGroupNative* group, RuntimeCString& name, bool requireUsing)
{
    if (!group) throw std::invalid_argument("sound character group has no payload (safe boundary)");
    auto* characters = reinterpret_cast<const unsigned char*>(name.GetString());
    for (auto* character = group->charactersAt08.begin; character != group->charactersAt08.end; ++character)
    {
        if (requireUsing && Body<SoundCharacterNative>(*character).valueAt04 == 0) continue;
        auto* registration = Body<SoundCharacterNative>(*character).parametersAt0C.begin;
        for (; registration != Body<SoundCharacterNative>(*character).parametersAt0C.end; ++registration)
        {
            auto* key = Body<SoundSeExNative>(*registration).fileKeysAt14.begin;
            for (; key != Body<SoundSeExNative>(*registration).fileKeysAt14.end; ++key)
            {
                const auto* prefix = Body<SoundFileKeyNative>(*key).keyAt04.GetString();
                if (!prefix || reinterpret_cast<const ATL::CStringData*>(characters)[-1].nDataLength < 0) continue;
                const auto* found = _mbsstr(characters, reinterpret_cast<const unsigned char*>(prefix));
                characters = reinterpret_cast<const unsigned char*>(name.GetString());
                if (found && found == characters) return {character, registration};
            }
        }
    }
    return {};
}
void Publish(Slot& result, const Slot* source)
{
    result = source ? *source : Slot{};
    if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
}
}
bool IsSoundCharacterAllowedAt46C2D0(SoundCharacterGroupNative* group,
    const char* resourceName, RuntimeCString* master)
{
    auto name = ArchiveResourceNameAt40B530(resourceName);
    Normalize(name);
    const auto match = FirstMatch(group, name.Value(), true);
    bool result = false;
    if (match.registration)
    {
        // 46C4CD赋master后重新取注册正文；回调不应被静音判定提前绕开。
        if (master) AssignRuntimeCStringAt410220(*master, Body<SoundSeExNative>(*match.registration).textAt30);
        result = Body<SoundSeExNative>(*match.registration).valueAt04 == 0;
    }
    name.ReleaseNormal();
    return result;
}
void FindSoundCharacterAt489630(Slot& result, SoundCharacterGroupNative* group, const char* resourceName)
{
    auto name = ArchiveResourceNameAt40B530(resourceName);
    Normalize(name);
    Publish(result, FirstMatch(group, name.Value(), false).character);
    name.ReleaseNormal();
}
void FindSoundRegistrationAt489880(Slot& result, SoundCharacterGroupNative* group, const char* resourceName)
{
    auto name = ArchiveResourceNameAt40B530(resourceName);
    Normalize(name);
    Publish(result, FirstMatch(group, name.Value(), false).registration);
    name.ReleaseNormal();
}
}
