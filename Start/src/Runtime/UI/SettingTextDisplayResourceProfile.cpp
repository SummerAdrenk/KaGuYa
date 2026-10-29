#include <Runtime/SettingTextDisplayResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const SettingTextDisplayResourceProfile& CurrentSettingTextDisplayProfile()
{
    const auto* profile = CompiledGameProfile().settingsTextDisplay;
    if (!profile) throw std::logic_error("Game text/display settings profile is not bound");
    return *profile;
}
}
