#include <Runtime/ProgramResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const ProgramResourceProfile& NoProgramResourceProfile() noexcept
{
    static const ProgramResourceProfile value{ProgramModeSelection::None, nullptr, {}};
    return value;
}
const ProgramResourceProfile& CurrentProgramResourceProfile()
{
    const auto* profile = CompiledGameProfile().programResources;
    if (!profile) throw std::logic_error("ProgramResourceProfile is not bound");
    return *profile;
}
const ProgramModeProfile& SelectProgramMode(const ProgramResourceProfile& profile, std::int32_t value)
{
    switch (profile.selection)
    {
    case ProgramModeSelection::None:
        throw std::invalid_argument("Program scene is absent from the selected game");
    case ProgramModeSelection::FixedSingle:
        if (profile.modes.size() != 1 || profile.modeVariable)
            throw std::logic_error("Invalid fixed Program profile");
        return profile.modes.front();
    case ProgramModeSelection::NamedParameter:
        if (!profile.modeVariable || profile.modes.empty())
            throw std::logic_error("Invalid named Program profile");
        for (const auto& mode : profile.modes)
            if (mode.scriptValue == value) return mode;
        throw std::invalid_argument("Unsupported Program mode for the selected game");
    }
    throw std::logic_error("Invalid Program mode selection policy");
}
}
