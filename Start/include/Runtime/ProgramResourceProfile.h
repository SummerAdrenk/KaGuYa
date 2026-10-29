#pragma once
#include <Runtime/ProgramDialogResourceData.h>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class ProgramModeSelection : std::uint8_t
{
    None,
    FixedSingle,
    NamedParameter,
};

// The mode table contains game data and observable policies, never native layouts.
struct ProgramResourceProfile final
{
    ProgramModeSelection selection;
    const char* modeVariable;
    std::span<const ProgramModeProfile> modes;
};

const ProgramResourceProfile& NoProgramResourceProfile() noexcept;
const ProgramResourceProfile& CurrentProgramResourceProfile();
const ProgramModeProfile& SelectProgramMode(const ProgramResourceProfile&, std::int32_t value);
}
