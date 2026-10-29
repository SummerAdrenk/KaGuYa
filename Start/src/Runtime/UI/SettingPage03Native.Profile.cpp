#include <Runtime/SettingPage03ResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const SettingPage03ResourceProfile& CurrentSettingPage03Profile()
{
    const auto* profile = CompiledGameProfile().soundPage;
    if (!profile) throw std::logic_error("Game sound page profile is not bound");
    return *profile;
}
const SettingSoundMasterRow& SettingSoundMasterRowFor(SettingSoundMaster role)
{
    for (const auto& row : CurrentSettingPage03Profile().masters)
        if (row.role == role) return row;
    throw std::logic_error("Sound page master role is missing");
}
}
