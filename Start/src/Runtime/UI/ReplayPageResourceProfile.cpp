#include <Runtime/ReplayPageResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
const ReplayPageResourceProfile& CurrentReplayPageProfile()
{
    const auto* profile = CompiledGameProfile().replayPage;
    if (!profile) throw std::logic_error("Replay page resource profile is not bound");
    return *profile;
}
}
