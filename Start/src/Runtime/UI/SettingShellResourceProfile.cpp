#include <Runtime/SettingShellResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const SettingShellResourceProfile& CurrentSettingShellProfile()
{
    const auto* profile = CompiledGameProfile().settingsShell;
    if (!profile) throw std::logic_error("Game settings shell profile is not bound");
    return *profile;
}
}
