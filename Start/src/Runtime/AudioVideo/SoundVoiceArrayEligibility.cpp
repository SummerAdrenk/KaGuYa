#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
bool EnvironmentSoundParamsRuntime::IsVoiceArrayAllowedAt46C830(
    const UnSaveDataStringArrayReference& names) const
{ return archive_.VoiceArrayAllowedAt60(names); }

bool EnvironmentSoundParamsArchive::VoiceArrayAllowedAt60(const UnSaveDataStringArrayReference& names)
{
    // +60先查总音声静音，再查组开关；不是单个A8的Master资格查询。
    const auto* voice = typeParametersAt20.FindAt47B0C0(namesAt10.voiceAt0C.GetString());
    if (!voice || voice->valueAt04 != 0) return false;
    if (!seByCharsAt38.Native().baseAt00) return false; // 46C8E7 -> 46C8A8。
    const auto* group = seByCharsAt38.Get();
    if (!group) throw std::invalid_argument("sound character group has no payload (safe boundary)");
    if (group->valueAt04 == 0 || group->valueAt05 == 0) return false;
    // 46C991..46CB61的内联查找与46C2D0一致，不输出Master；首匹配静音只试下一声音。
    for (std::int32_t index = 0;
        index < std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(names.Size())); ++index)
    {
        // 46C958先借CString字符，再解组Ref；不另分配std::string，也不无符号化count。
        const char* characters = names.At(static_cast<std::uint32_t>(index)).GetString();
        auto* currentGroup = seByCharsAt38.Get();
        if (IsSoundCharacterAllowedAt46C2D0(currentGroup, characters, nullptr))
            return true;
    }
    return false;
}
}
