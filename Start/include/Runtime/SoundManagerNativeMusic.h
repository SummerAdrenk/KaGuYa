#pragma once
#include <Runtime/SoundManagerNativePlayback.h>

namespace KaGuYa::Start::Runtime
{
class SoundManagerNativeMusic final
{
public:
    SoundManagerNativeMusic(SoundModuleNativeApi&, ISoundManagerNativeOpen&) noexcept;
    bool IsAbsolutePlayingAt419370(SoundManagerNativeBody50h&, std::uint32_t slot);
    bool StopAbsoluteAt4383B0(SoundManagerNativeBody50h&, std::uint32_t slot);
    void StopMusicAt4C3500(SoundManagerNativeBody50h&, RuntimeCString& environmentName);
    bool PlayMusicAt4BCCC0(SoundManagerNativeBody50h&, RuntimeCString& environmentName,
        const char* capturedName, std::int32_t value, std::int32_t loop);
    void StopEffectsAt413270(SoundManagerNativeBody50h&, EnvironmentVectorStorage<RuntimeCString>* mappings);
    bool HasSlotsAt4D5B1A(SoundManagerNativeBody50h&);
    bool ApplyActiveAt4D5B28(SoundManagerNativeBody50h&, const char* name,
        std::int32_t value, std::int32_t loop);
    void StopCurrentAt4D5A39(SoundManagerNativeBody50h&);
private:
    SoundModuleNativeApi& sound_;
    ISoundManagerNativeOpen& open_;
};
}
